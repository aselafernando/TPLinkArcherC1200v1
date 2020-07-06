# Copyright (C) 2011-2014 TP-LINK

. /lib/functions.sh

BOARD_TYPE=brcm
# SET OF LAN PHY PORT: LAN1->1 LAN2->2 LAN3->3 LAN4->4
LAN_PHY_PORT_SET="1 2 3 4"
# SET OF WAN PHY PORT: WAN->5
WAN_PHY_PORT_SET="0"
# Default LAN/WAN CPU PORT: Format->PortID:Pvid
WAN_DFT_CPU="5:4094"
LAN_DFT_CPU="8:1"
# The number of GMAC
GMAC_NUM=2
# LAN use vlan tag
LAN_TAGED=1
# WAN use vlan tag
WAN_TAGED=1

KMOD_PATH=/lib/modules/iplatform

select_vid()
{
	local used="$1"
	local vid=1

	for id in `seq 4094`; do
		! list_contains used $id && {
			vid=$id
			break		
		}
	done
	
	echo $vid
}

# $1: vid
# $2: used vid
is_used_vid()
{
	local vid=$1
	local used=$2
	if list_contains used $vid; then
		echo 1;
	else 
		echo 0;
	fi
}

PAGE_VLAN=0x34
REG_VLAN_WRITE=0x08
REG_VLAN_READ=0x0c
REG_VLAN_ACCESS=0x06

PAGE_VTBL=0x5
REG_VTBL_ENTRY_5395=0x83
REG_VTBL_INDX_5395=0x81
REG_VTBL_ACCESS_5395=0x80
VLAN_WRITE_CMD=0x80
DEFAULT_MEMBER=0x
SWITCH_PORT_NUMBER=9
VLAN_MAXVID=4095
REG_VLAN_PTAG0=0x10
REG_VLAN_PTAG1=0x12
REG_VLAN_PTAG2=0x14
REG_VLAN_PTAG3=0x16
REG_VLAN_PTAG4=0x18
REG_VLAN_PTAG5=0x1a
REG_VLAN_PTAG6=0x1c
REG_VLAN_PTAG7=0x1e
REG_VLAN_PTAG8=0x20
WAN_BIT=0
MII_BIT=5

# $1: vid
create_vlan_entry()
{
	if [ $BOARD_TYPE = "qca" ]; then
		ssdk_sh vlan entry create $1
	else
		# Delete the old entry.
		et robowr $PAGE_VTBL $REG_VTBL_ENTRY_5395 0 
		et robowr $PAGE_VTBL $REG_VTBL_INDX_5395 $1
		et robowr $PAGE_VTBL $REG_VTBL_ACCESS_5395 0x80
	fi
}

# $1 base number
# $2 power
power()
{
	local base=$1
	local power=$2
	local val=1
	[ "$base" = "0" ] && {
	echo 1
	return
	}
	for i in $(seq $power)
	do
		val=$(($val * $base))
	done
	echo $val
}

#power 2 3
#power 0 0
#power 0 1
#power 1 0

set_bit()
{
	local old_val=$1
	local bit=$2
	local bit_val=
	bit_val=$(power 2 $bit)
	local new_val=$(($old_val | $bit_val))
	printf "0x%x" $new_val
}

clear_bit()
{
	[ $# != 2 ] && return
	local old_val=$1
	local bit=$2
	local bit_val=
	bit_val=$(power 2 $bit)
	local nor_bit_val=$((0xffffffff ^ $bit_val))
	local new_val=$(($old_val & $nor_bit_val))
	printf "0x%x" $new_val
}

clear_bit 0

# $1: vid
delete_vlan_entry()
{
	local vid=$1
	if [ $BOARD_TYPE = "qca" ];then
		ssdk_sh vlan entry del $vid 
	else
		et robowr $PAGE_VTBL $REG_VTBL_ENTRY_5395 0 
		et robowr $PAGE_VTBL $REG_VTBL_INDX_5395 $vid
		et robowr $PAGE_VTBL $REG_VTBL_ACCESS_5395 0x80
	fi
}

add_vlan_member()
{
	[ $# -lt 2 ] && return
	local vid=$1
	local port=$2

	if [ $BOARD_TYPE = "qca" ]; then
		case "$3" in
			t) ssdk_sh vlan member add $vid $port tagged ;;
			*) ssdk_sh vlan member add $vid $port untagged ;;
		esac
	else
		et robowr $PAGE_VTBL $REG_VTBL_INDX_5395 $vid
		et robowr $PAGE_VTBL $REG_VTBL_ACCESS_5395 0x81 
		local old=$(et robord $PAGE_VTBL $REG_VTBL_ENTRY_5395)
		local new=
		case "$3" in
			t) new=$(set_bit $old $port);;
			*) new=$(set_bit $old $port)
			new=$(set_bit $new $(expr $port + 9));;
		esac
		et robowr $PAGE_VTBL $REG_VTBL_ENTRY_5395 $new
		et robowr $PAGE_VTBL $REG_VTBL_INDX_5395 $vid
		et robowr $PAGE_VTBL $REG_VTBL_ACCESS_5395 0x80
	fi
	return
}

# $1: vid
# $2: port
del_vlan_member()
{
	[ $# != 2 ] && return
	local vid=$1
	local port=$2
	if [ $BOARD_TYPE = "qca" ]; then
		ssdk_sh vlan member del $1 $2
	else
		echo "del_vlan_member vid($vid) port($port)"
		et robowr $PAGE_VTBL $REG_VTBL_INDX_5395 $vid
		et robowr $PAGE_VTBL $REG_VTBL_ACCESS_5395 0x81 
		local old=$(et robord $PAGE_VTBL $REG_VTBL_ENTRY_5395)
		[ -n "$old" ] && old=0
		local new=$(clear_bit $old $port)
		et robowr $PAGE_VTBL $REG_VTBL_ENTRY_5395 $new
		et robowr $PAGE_VTBL $REG_VTBL_INDX_5395 $vid
		et robowr $PAGE_VTBL $REG_VTBL_ACCESS_5395 0x80
	fi
}

# $1: port
# $2: vid
set_pvid()
{
	[ $# -ne 2 ] && return
	local port=$1
	local vid=$2
	if [ $BOARD_TYPE = "qca" ]; then
		ssdk_sh portVlan defaultCVid set $1 $2
	else
		local port_reg=REG_VLAN_PTAG$port
		local port_reg_val=$(eval echo \$$port_reg)
		local vid_hex=$(printf "0x%x" $vid)
		echo "et robowr $PAGE_VLAN $port_reg_val $vid_hex"
		et robowr $PAGE_VLAN $port_reg_val $vid
	fi
}

# $1: ports- 1 2 3 4
# $2: vid
set_port_member_vid() 
{
	local ports=$1
	local vid=$2

	for port in $ports; do
		local port_id=${port%:*}
		set_pvid $port_id $vid
	done
}

#$1: dev name
#$2: vid
create_vif()
{
	[ $# -ne 2 ] && return
	vconfig add $1 $2
}

destroy_vif()
{
	echo "vconfig rem $1"
	vconfig rem $1
}

up_iface()
{
	local dev=$1
	#local vid=${1#*.}
	#local dev=${1%%.*}

	#[ -n "$vid" ] && dev=$1

	ifconfig $dev up
}

down_iface()
{
	local dev=$1
	local vid=${1#*.}
	local dev=${1%%.*}

	[ -n "$vid" ] && dev=$1
	ifconfig $dev down
}

# create virtual device and corresponding vlan entry.
# $1: father device, likes eth0, eth1, etc
# $2: vid
# $3: port "1 2"
# $4: output tag ? t/* 
# $5: cpu port number
# $6: cpu port output tag ? t/* 
create_vdevice()
{
	[ $# != 6 ] && return
	local father_device=$1
	local vid=$2
	local port=$3
	local output_tag=$4
	local cpu_port=$5
	local cpu_tag=$6

	echo "create vdevice vid=$vid device=$father_device port=$port cpu_port=$cpu_port tag=$output_tag cpu_tag=$cpu_tag"
	create_vlan_entry $vid
	for p in $port;do
		add_vlan_member $vid $p $output_tag
		set_pvid $port $vid
	done
	add_vlan_member $vid $cpu_port $cpu_tag
	[ "$cpu_tag" = "t" ] && {
	create_vif $father_device $vid
	up_iface "$father_device.$vid"
	}
}

# destroy virtual device and corresponding vlan entry.
# $1: father device, likes eth0, eth1, etc
# $2: vid
destroy_vdevice()
{
	local father_device=$1
	local vid=$2
	down_iface "$father_device.$vid"
	destroy_vif "$father_device.$vid"
	delete_vlan_entry $1
}

set_8021q_prio()
{
	echo "vconfig set_egress_map $1 $2 $3"
	vconfig set_egress_map $1 $2 $3
}

create_br()
{
	brctl addbr $1
}

destroy_br()
{
	brctl delbr $1
}

is_dev_existen()
{
	local tmp=$(cat /proc/net/dev | grep "$1")
	if [ -n "$tmp" ];then
		echo 1
	else
		echo 0
	fi
}

# $1: dev_name
# $2: br_name
is_in_br()
{
	local tmp=$(brctl show "$2" | grep "$1$")
	if [ -n "$tmp" ];then
		echo 1
	else
		echo 0
	fi
}

add_br_member()
{
	local vid=${2#*.}
	local dev=${2%%.*}

	[ -n "$vid" ] && dev=$2
	brctl addif $1 $dev
}

# $1: br_name
# $2: dev_name
del_br_member()
{
	[ $# != 2 ] && return;
	local vid=${2#*.}
	local dev=${2%%.*}

	if [ $(is_in_br $2 $1) -eq "1" ]; then
		echo "del $2 from $1"
		[ -n "$vid" ] && dev=$2
		brctl delif $1 $dev
	fi
}

flush_fdb()
{
	if [ $BOARD_TYPE = "qca" ]; then
		ssdk_sh fdb entry flush 0
	else
		echo "flush_fdb in brcm"
	fi
}

