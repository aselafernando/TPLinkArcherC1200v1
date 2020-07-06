# Copyright (C) 2011-2014 TP-LINK

iptv_load_network()
{
    config_load network
}

iptv_lan_set_ifname()
{
    # $1 virtual network interfaces set
    lua /lib/iptv/iptv_config.lua set network lan ifname "$1"
}

iptv_set_bridge_type()
{
    # $1 lan/wan
    lua /lib/iptv/iptv_config.lua set network $1 type bridge
}

iptv_set_unbridge_type()
{
    # $1 lan/wan
    lua /lib/iptv/iptv_config.lua set network $1 type ""
}

iptv_wanv6_set_ifname()
{
    # $1 the value of ifname
    lua /lib/iptv/iptv_config.lua set network wanv6 ifname "$1"
    lua /lib/iptv/iptv_config.lua set protocol staticv6 ifname "$1"
    lua /lib/iptv/iptv_config.lua set protocol dhcpv6 ifname "$1"
    lua /lib/iptv/iptv_config.lua set protocol pppoev6 ifname "$1"
    lua /lib/iptv/iptv_config.lua set protocol 6to4 ifname "$1"
    lua /lib/iptv/iptv_config.lua set protocol dslite ifname "$1"
}

iptv_internet_set_ifname()
{
    # $1 the value of ifname
    lua /lib/iptv/iptv_config.lua set network internet ifname "$1"
}

iptv_wan_set_ifname()
{
    # If wan, lan interfaces are based on the same device, we need to sync the 
    # wan device name before setting the wan interface ifname.
    local lan_ifname=`lua /lib/iptv/iptv_config.lua get_lan_ifname`
    local wan_ifname=`lua /lib/iptv/iptv_config.lua get_wan_ifname`
    local lan_dev=${lan_ifname%%.*}
    local wan_dev=${wan_ifname%%.*}

    if [ "${lan_dev}" = "${wan_dev}" ]; then
	lua /lib/iptv/iptv_config.lua set network device name "$1"
    fi
    lua /lib/iptv/iptv_config.lua set network wan ifname "$1"
}

iptv_set_device_attr()
{
    lua /lib/iptv/iptv_config.lua set network device $1 "$2"
}

iptv_igmp_snooping_set()
{
    lua /lib/iptv/iptv_config.lua set network $1 igmp_snooping "$2"
}

