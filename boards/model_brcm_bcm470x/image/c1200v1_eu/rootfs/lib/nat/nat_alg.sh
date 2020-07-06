# Copyight(c) 2011-2013 Shenzhen TP-LINK Technologies Co.Ltd.
# file     nat_alg.sh
# bief    
# autho   Guo Dongxian
# vesion  1.0.0
# date     13Mar14
# history   ag 1.0.0, 26Feb14, Guo Dongxian, Ceate the file

ALG_FILTER_CHAINS=
nat_config_load_alg() {
	nat_config_get_section "$1" nat_alg { \
		string ftp on \
		string tftp on \
        string h323 on \
        string rtsp on \
        string pptp on \
        string l2tp on \
        string ipsec on \
	} || return
    
    nat__do_vpn_passthrough() {
        local vpn=$1
        local enable=$2
        local setup_mods=$3
        
        local pptp_mod
        [ "$vpn" == "pptp" ] && {                                
            for m in $setup_mods; do
                list_contains pptp_mod $m && continue
                [ "$m" == "nf_nat_pptp" ] && append pptp_mod $m
                [ "$m" == "nf_conntrack_pptp" ] && append pptp_mod $m
                [ "$m" == "nf_conntrack_proto_gre" ] && append pptp_mod $m                        
            done
        }        
    
        local vpn_upper=$(echo "$vpn"|tr '[a-z]' '[A-Z]')
        [ "$enable" == "off" ] && {
            local ifname=$(uci_get_state nat env ${NAT_ZONE_LAN}_if)
            [ -z "$ifname" ] && return
            case $vpn in
                pptp)
                    fw add 4 f ${ALG_FILTER_CHAINS} DROP ^ \
                        { -p tcp -m tcp --dport 1723 }
              
                    [ -n "$pptp_mod" ] && {
                        for mod in nf_nat_pptp nf_conntrack_pptp nf_conntrack_proto_gre; do
                            list_contains pptp_mod $mod && {
                                rmmod ${mod}
                            }
                        done
                    }
                ;;
                l2tp)
                    fw add 4 f ${ALG_FILTER_CHAINS} DROP ^ \
                        { -p udp -m udp --dport 1701 }
                ;;
                ipsec)
                    fw add 4 f ${ALG_FILTER_CHAINS} DROP ^ \
                        { -p udp -m udp --dport 500 }
                ;;
                *)
                ;;
            esac
            # We have to flush all conntrack, fo make vpn passthrough close at once
            echo f >/proc/net/nf_conntrack 2>/dev/null
            nat_syslog 22 "$vpn_upper"
            return
        }
        
        [ "$vpn" == "pptp" ] && { 
            for mod in nf_conntrack_proto_gre nf_conntrack_pptp nf_nat_pptp; do
                list_contains pptp_mod $mod && continue
                insmod ${mod}
            done
        }
        nat_syslog 21 "$vpn_upper"
    }

    nat__do_alg() {
        local alg=$1
        local enable=$2
        local setup_mods=$3

        local alg_mod
        for m in $setup_mods; do
            list_contains alg_mod $m && continue
            [ "$m" == "nf_nat_${alg}" ] && append alg_mod $m
            [ "$m" == "nf_conntrack_${alg}" ] && append alg_mod $m
        done
        
        local upper_alg=$(echo "$alg"|tr '[a-z]' '[A-Z]')
        [ "$enable" == "on" ] && {
            for mod in nf_conntrack_${alg} nf_nat_${alg}; do
                list_contains alg_mod $mod && continue
                insmod ${mod}                                               
            done   
            nat_syslog 21 "$upper_alg"
            return     
        }
        
        [ -z "$alg_mod" ] && return
        for mod in nf_nat_${alg} nf_conntrack_${alg}; do
            list_contains alg_mod $mod && {
                [ "$mod" == "nf_conntrack_h323" ] && continue
                rmmod ${mod}
            }    
        done
        nat_syslog 22 "$upper_alg"
    }
    
    [ -n "$nat_alg_ftp" ] && nat__do_alg ftp $nat_alg_ftp "$2"
    [ -n "$nat_alg_tftp" ] && nat__do_alg tftp $nat_alg_tftp "$2"
    [ -n "$nat_alg_h323" ] && nat__do_alg h323 $nat_alg_h323 "$2"
    [ -n "$nat_alg_rtsp" ] && nat__do_alg rtsp $nat_alg_rtsp "$2" 
    [ -n "$nat_alg_pptp" ] && nat__do_vpn_passthrough pptp $nat_alg_pptp "$2"    
    [ -n "$nat_alg_l2tp" ] && nat__do_vpn_passthrough l2tp $nat_alg_l2tp "$2"
    [ -n "$nat_alg_ipsec" ] && nat__do_vpn_passthrough ipsec $nat_alg_ipsec "$2"

}

nat_alg_load() {
    local setup_mods=$(lsmod|grep -E '^nf_(nat|conntrack)_*'|awk -F ' ' '{print $1}')
    nat_config_once nat_config_load_alg $1 "$setup_mods"
}

nat_alg_run() {
    [ -n "$nat_filter_chains" ] && {
        for fc in $nat_filter_chains; do
            local alg=$(echo "$fc"|grep 'vpn$')
            [ -n "$alg" ] && {
                append ALG_FILTER_CHAINS $fc
                fw flush 4 f $fc
                break
            }
        done
    }
    nat_alg_load nat_alg
    unset ALG_FILTER_CHAINS
}

