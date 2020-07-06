# Copyright(c) 2011-2013 Shenzhen TP-LINK Technologies Co.Ltd.
# file     nat.sh
# brief    
# author   Guo Dongxian
# version  1.0.0
# date     25Mar14
# history  arg 1.0.0, 25Mar14, Guo Dongxian, Create the file

FW_LIBDIR=/lib/firewall

. $FW_LIBDIR/fw.sh
. /lib/functions.sh

nat() {
    local act=$1
    local mod=$2
    local tgt=$3
    local pos=$4

    local zones
    local mode
    local flag=0

    config_load nat
    local norder=
    local nzorder=
    config_get norder "nat" "norder"
    list_contains norder $mod && {
        mode=r
    }

    list_contains nzorder "nat" "nzorder"
    list_contains nzorder $mod && {
        mode=z    
    }

    [ -n "$mode" ] && {
        shift 4
        [ -n "$1" -a "$1" == "{" ] && {
            flag=1
        }
        
        local rule
        [ "$act" == "add" -a "$flag" -eq 1 ] && {
            shift
            while [ "$1" != "}" ]; do
                [ $# -eq 0 ] && return 1
                append rule $1
                shift
            done
        } 
        
        case $mode in 
            r) 
                local chain="prerouting_rule_$mod"
                nat_do_cmd $act $chain $tgt $pos "$rule"
            ;;
            z)
                config_get zones "nat" "zones"
                for zone in $zones; do
                    [ "$zone" != "lan" ] && {
                        local chain="prerouting_$zone_$mod"
                        nat_do_cmd $act $chain $tgt $pos "$rule"                    
                    }
                done
            ;;
            *);;
        esac
    }

    config_clear

    return 0
}

#<action> <chain> <target> <position> <rule>
nat_do_cmd() {
    local act=$1
    
    case $act in
        add)
            local dup=$(fw list 4 n $2 | grep -E "($5)")
            [ -z "$dup" ] && { 
                fw add 4 n $2 $3 $4 { "$5" }
            } 
        ;;
        del) fw flush 4 n $2 ;;
        *) ;;
    esac

    return
}

