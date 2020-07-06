######### Switch APIs for BCM53125  #########

#Registers
local PAGE_VLAN=0x34
local REG_VLAN_PTAG=0x10	# REG_VLAN_PTAG0 - REG_VLAN_PTAG8: 0x10 - 0x18

#Registers
local PAGE_VTBL=0x05
local REG_VTBL_ENTRY=0x83
local REG_VTBL_INDEX=0x81
local REG_VTBL_ACCESS=0x80

#Registers
local PAGE_GPHY=0x00

#Switch ports & vlans, saved in /etc/config/switch?
local LAN_PORTS="1 2 3 4"
local WAN_PORTS="0"
local CPU_PORTS="8 5"
local MAX_VLAN="4095"

log () {
	#echo "$@" >> /dev/console
	echo "$@" >> /dev/null
}

et_d () {
	log "et $@"
	et $@
}

setup_port_vlan() {	#	<port> <vid>
	local port=$1
	local vid=$2

	et_d robowr $PAGE_VLAN $(($REG_VLAN_PTAG + (2 * $port))) $vid
}

setup_member_vlan() {	#	<vid> <memberMap> <untagMap>
	local map=$((($3 << 9) | $2))	#bit 0-8:	member group bits
									#bit 9-17:	untag bits
	et_d robowr $PAGE_VTBL $REG_VTBL_ENTRY $map
	
	local vid=$1
	et_d robowr $PAGE_VTBL $REG_VTBL_INDEX $vid
	
	local access=$(((0x1<<7)|0x0))	#bit 7:		start command
									#bit 0-1:	write
	et_d robowr $PAGE_VTBL $REG_VTBL_ACCESS $access
}

clear_vlan_table() {
	local access=$(((0x1<<7)|0x02))	#bit 7:		start command
									#bit 0-1:	10--->clear-table
	et_d robowr $PAGE_VTBL $REG_VTBL_ACCESS $access
}

clear_all_vlans() {
	local vid
	local port
	
	for port in $LAN_PORTS
	do
		setup_port_vlan $port 0		# set all ports's default vlan is 0
	done

	clear_vlan_table
}

setup_switch_vlan() {	#	<vid> <port>{u} <port>{u}...
	#switch : 4 LAN ports, 1 WAN port, 2 CPU port

	local vid="$1"
	local port
	local flag
	
	local memberMap=0	#6 bits, ports
	local untagMap=0	#6 bits, ports
	
	shift
	log "ports=$@"
	while [ -n "$1" ]
	do

		$(echo "$1" | grep -Eq "[0-8][\*ut]?") || { 
			log "SWITCH: the format of port is incorrect!"
			exit 1; 
		}

		port=${1:0:1}
		flag=${1:1}

		memberMap=$(($memberMap | (0x1 << $port)))

		# non-cpu port should be untaged
		if $(echo "$CPU_PORTS" | grep -Eq "$port")
		then
			# flag "u" tell untag
			$(echo "$flag" | grep -Eq "u") && {
				setup_port_vlan $port $vid
				untagMap=$(($untagMap | (0x1 << $port)))
			}
		else
			setup_port_vlan $port $vid
			# flag "t" tell tag
			$(echo "$flag" | grep -Eq "t") || {
				untagMap=$(($untagMap | (0x1 << $port)))
			}
		fi

		shift
	done

	setup_member_vlan $vid $memberMap $untagMap
}

link_down_phy_port () {	# <port>
	local port=$1
	local regs=$(($PAGE_GPHY + $port))
	local oldval=$(et phyrd $regs 0x00)
	local newval=$(($oldval | (1 << 11)))	#low power bit.

	# et robowr <port_register> <offset> <val> <len>
	et_d phywr $regs 0x00 $newval
}

link_up_phy_port () {	# <port>
	local port=$1
	local regs=$(($PAGE_GPHY + $port))
	local oldval=$(et phyrd $regs 0x00)
	local newval=$(($oldval & (0xFFFF - (1 << 11)))) #low power bit.

	# et robowr <port_register> <offset> <val> <len>
	et_d phywr $regs 0x00 $newval
}

link_down_all_ports () {
	for port in $LAN_PORTS $WAN_PORTS
	do
		link_down_phy_port $port
	done
	
	log "switch ports is linked down!"
}

link_up_all_ports () {
	for port in $LAN_PORTS $WAN_PORTS
	do
		link_up_phy_port $port
	done
	
	log "switch ports is linked up!"
}

link_down_lan_ports () {
	for port in $LAN_PORTS
	do
		link_down_phy_port $port
	done
	
	log "switch ports is linked down!"
}

link_up_lan_ports () {
	for port in $LAN_PORTS
	do
		link_up_phy_port $port
	done
	
	log "switch ports is linked up!"
}

set_port_duplex () {	#	<port> <rate> <duplex> <auto>
	local port=$1
	local rate=$2
	local duplex=$3
	local auto=$4

	local regs=$(($PAGE_GPHY + $port))
	local oldval=$(et phyrd $regs 0x00)	# MII Control Register
	local newval=$oldval

	local oldadv=$(et phyrd $regs 0x04)	# Advertisement Register for 10/100 H/F
	local newadv=$oldadv

	local oldadv2=$(et phyrd $regs 0x09)	# Control & Advertisement Register for 1000 H/F
	local newadv2=$oldadv2
	
	case $auto in
		"on")
			local power_down=$(($newval & (1 << 11)))
			newval=$(($newval | (1 << 15)))	# Reset the Switch Phy Port to Default
			#MII Control
			et_d phywr $regs 0x00 $newval
			[ $power_down == 0 ] || link_down_phy_port $port
			return
			;;
	esac

	
	newadv=$(($newadv & (0xFFFF - (1 << 8) - (1 << 7) - (1 << 6) - (1 << 5))))
	newadv2=$(($newadv2 & (0xFFFF - (1 << 8) - (1 << 9))))
	
	case $rate in
		"1000")
			case $duplex in
				"full")
					newadv2=$(($newadv2 | (1 << 9)))
					;;
				"half")
					newadv2=$(($newadv2 | (1 << 8)))
					;;
				*)
					return
					;;
			esac
			;;
		"100")
			case $duplex in
				"full")
					newadv=$(($newadv | (1 << 8)))
					;;
				"half")
					newadv=$(($newadv | (1 << 7)))
					;;
				*)
					return
					;;
			esac
			;;
		"10")
			case $duplex in
				"full")
					newadv=$(($newadv | (1 << 6)))
					;;
				"half")
					newadv=$(($newadv | (1 << 5)))
					;;
				*)
					return
					;;
			esac
			;;
		*)
			return
			;;
	esac
	
	#Write Advertisement
	et_d phywr $regs 0x04 $newadv
	et_d phywr $regs 0x09 $newadv2

	# Restart Auto-Negotiation
	newval=$(($newval | (1 << 9) | (1 << 12)))	
	
	#MII Control
	et_d phywr $regs 0x00 $newval	# Write to Control
}

