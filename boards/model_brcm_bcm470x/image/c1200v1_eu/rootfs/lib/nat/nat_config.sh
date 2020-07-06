# Copyight(c) 2011-2013 Shenzhen TP-LINK Technologies Co.Ltd.
# file     nat_config.sh
# bief    
# autho   Guo Dongxian
# vesion  1.0.0
# date     26Feb14
# history   arg 1.0.0, 26Feb14, Guo Dongxian, Ceate the file

NAT_ALL_NETWORK=

NAT_FILTER_CHAINS="zone_lan_natfd_vpn zone_lan_natfd_vs zone_lan_natfd_dmz forwarding_rule_pt "\
"zone_wan_natfd_vs zone_wan_natfd_dmz"

NAT_RULE_CHAINS="prerouting_rule"
NAT_DMZ_CHAINS="prerouting_wan_dmz prerouting_lan_dmz"

nat_config_default() {
    local zones
    local ifaces
    
    config_get zones "nat" "zones"

    NAT_ZONES=$zones

    [ -n "$zones" ] && {
        for z in $zones; do
            [ "$z" == "$NAT_ZONE_LAN" ] && continue
            list_contains NAT_WAN_ZONES $z && continue
            append NAT_WAN_ZONES $z
        done
    }

    config_get NAT_ALL_NETWORK "nat" "network"

    for ifc in $NAT_ALL_NETWORK; do
        local i=${ifc#*-}        
        append ifaces $i
    done

    uci_set_state nat core ifaces "$ifaces"

    # Prerouting nat rule chain order
    config_get NAT_NORDER "nat" "norder"

    export "nat_norder"="$NAT_NORDER" 
    export "nat_dmz_chains"="$NAT_DMZ_CHAINS"
    export "nat_rule_chains"="$NAT_RULE_CHAINS"
    export "nat_filter_chains"="$NAT_FILTER_CHAINS"
}

nat_config_nw_exist() {
    [ -z "$NAT_ALL_NETWORK" ] && return 1

    local nw="${1}-${2}"
    local all_nw="$NAT_ALL_NETWORK"

    list_contains all_nw $nw && return 0

    return 1
}

# Read information of the remote management form config file
# and create nat rule of remote management 
nat_config_http_rule() {
    local rules=
    local mod="$1"

    fw flush 4 n "prerouting_rule_${mod}"
    config_get en "remote" "enable"
    [ "$en" != "off" ] && {
        local params=
        config_get http_port "remote" port
        config_get http_ip "remote" ipaddr

        if [ "$en" == "all" ]; then
            append params "255.255.255.255"
        else
            append params "$http_ip"
        fi
        append params "$http_port"

        rules=$(nat_http_param_to_rule "$params")
        [ -z "$rules" ] && return 0

        fw add 4 n "prerouting_rule_${mod}" "DNAT" "$" { $rules }

        return 0
    }

    return 1
}

# Read information of the ftp server form usbshare config file
# and create nat rule of ftp server
nat_config_ftp_rule() {
    local ftp_ctl_port=
    local mod="$1"

    config_get ftp_en "global" ftp
    [ "$ftp_en" == "on" ] && {
        config_get ftpex_en "global" ftpex
        [ "$ftpex_en" == "on" ] && {
            config_get ftp_ctl_port "global" ftpex_port
            local rules=$(nat_ftp_param_to_rule $ftp_ctl_port)
            [ -z "$rules" ] && return
        }
    }

    fw flush 4 n "prerouting_rule_${mod}"
    return
}

nat_config_init_mods_rules() {
    # Remote management
    nat_config_http_rule http
    # Ftp Server
    nat_config_ftp_rule ftp
}


