#!/bin/sh
. /lib/switch/config.sh
. /lib/config/uci.sh
###### Kcodes's USB Printer Discovery ########

PACKAGE="printer"
TYPE="ptinfo"

save_printer_info()
{
	local manufacturer="$1"
	local product="$2"
	
	[ -e "/etc/config/$PACKAGE" ] || touch "/etc/config/$PACKAGE"
	
	uci_add $PACKAGE $TYPE
	uci_set $PACKAGE $CONFIG_SECTION manufacturer $manufacturer
	uci_set $PACKAGE $CONFIG_SECTION product $product
	uci_commit
}

remv_printer_info()
{
	uci_remove $PACKAGE $1
}

scan_printers() {
	local scan_path="/proc/bus/usb/devices"
	local manufacturer
	local product
	
	#First, delete all old printer info
	config_load printer
	config_foreach remv_printer_info ptinfo
	uci_commit
	
	#Second, scan
	local scan_file="/tmp/scan_file"
	cat $scan_path > $scan_file
	while read myline
	do
		local manu=$(echo "$myline" | grep "S:  Manufacturer=")
		if [ "x$manu" != "x" ]
		then
			manufacturer=$(echo "$manu" | sed 's/^S:  Manufacturer=\(.*\)$/\1/g')
		fi
		
		local prod=$(echo "$myline" | grep "S:  Product=")
		if [ "x$prod" != "x" ]
		then
			product=$(echo "$prod" | sed 's/^S:  Product=\(.*\)$/\1/g')
		fi

		#Last, save new printer info
		local print=$(echo "$myline" | grep '^I:\* If#.*Cls=07.*$')
		local noprint=$(echo "$myline" | grep '^I:\* If#.*Cls=.*$')
		if [ "x$print" != "x" ]
		then
			save_printer_info $manufacturer $product
		elif [ "x$noprint" != "x" ]
		then
			manufacturer=""
			product=""
		fi
			
	done < $scan_file
}