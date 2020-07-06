#!/bin/sh

append DRIVERS "brcmwifi"

WL=/usr/sbin/wl
NVRAM=/usr/sbin/nvram
WLCONF=/usr/sbin/wlconf

STDOUT=/dev/null
#DEBUG=1
[ -n "${DEBUG}" ] && STDOUT="/dev/console"

nvram() {
	[ -n "${DEBUG}" ] && echo nvram "$@" >$STDOUT
	$NVRAM "$@"
}

wl() {
	[ -n "${DEBUG}" ] && echo wl "$@" >$STDOUT
	$WL "$@"
}

wlconf() {
	[ -n "${DEBUG}" ] && echo wlconf "$@" > $STDOUT
	$WLCONF "$@"
}

kill_nas() {
	echo "kill_nas" >$STDOUT
}

# parse the /etc/config/wireless and make sure there are no configuration incompatibilities.
scan_brcmwifi() {
	echo "=====>>>>> scan_brcmwifi" >$STDOUT
}

# bring up the wifi device and create application specific configuration files.
enable_brcmwifi() {
	echo "=====>>>>> enable_brcmwifi" >$STDOUT
}

# bring down the wifi device and all its virtual interfaces
disable_brcmwifi() {
	echo "=====>>>>> disable_brcmwifi" >$STDOUT
}

# looks for interfaces that are usable with the driver
detect_brcmwifi() {
	echo "=====>>>>> detect_brcmwifi" >$STDOUT
}
