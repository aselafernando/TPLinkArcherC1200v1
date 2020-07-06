#!/bin/sh
# Copyright(c) 2011-2013 Shenzhen TP-LINK Technologies Co.Ltd.
#
# /usr/lib/dnsproxy/dnsproxy_deamon.sh
# Written by Guo Dongxian<guodongxian@tp-link.com.cn>, April 2014
#
# The main of dnsproxy, which checks primary dns or secondary dns whether ok or not
# Create nat rule according to dns server
# version 1.0.1

. /usr/lib/dnsproxy/dnsproxy_api.sh

lock /var/run/dnsproxy.lock 

dnsproxy_config_append dnsproxy
dnsproxy_config_append network

! dnsproxy_enable && return 0

dnsp_retry_times=$(dnsproxy_get_retry_times)

[ -z "$dnsp_retry_times" ] && {
    dnsp_retry_times=3 # Default retry three times
}

dnsp_check_interval=$(dnsproxy_get_check_int)

[ -z "$dnsp_check_interval" ] && {
    dnsp_check_interval=30 # Default check interval 30 seconds
}

dnsp_query_timeout=$(dnsproxy_get_timeout)

[ -z "$dnsp_query_timeout" ] && {
    dnsp_query_timeout=5 # Default query timeout each time is 5 seconds
}

dnsp_domains=$(dnsproxy_get_domains)

if [ -d /var/run/dnsproxy ]
then
    if [ -e "/var/run/dnsproxy/dnsproxy.pid" ]
    then
	    pid=$(cat /var/run/dnsproxy/dnsproxy.pid)
	    match=$(ps | grep "^[\t ]*$pid")

	    if [ -n "$match" ]
	    then
		    kill $pid
	    fi
        rm -f /var/run/dnsproxy/dnsproxy.*
    fi
else
    mkdir /var/run/dnsproxy
fi
echo $$ > /var/run/dnsproxy/dnsproxy.pid

lock -u /var/run/dnsproxy.lock

ip_regex="[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}"
while [ true ]
do
    local dns_status=0
    local effect_dns=

    dnsp_servers=$(dnsproxy_get_dns_servers)
    [ -z "$dnsp_servers" ] && {
        # If getting dns servers failed, then tried to get
        # dns servers from wan
        network_get_dnsserver dnsp_servers "wan"
    }

    for domain in $dnsp_domains; do
        for server in $dnsp_servers; do
            local retries=0
            while [ $retries -lt $dnsp_retry_times ]
            do
                retries=$((retries + 1))
                local result=$(dnslookup -t $dnsp_query_timeout $domain $server)
                [ -z "$result" ] && continue
                
                local parse=$(echo "$result"|awk 'NR==1'|grep -o "$ip_regex")
                [ -n "$parse" ] && {
                    dns_status=1
                    break
                }
            done
            [ $dns_status -eq 1 ] && {
                effect_dns=$server
                break
            }
        done
        [ $dns_status -eq 1 ] && break
    done

    # After checking dns server, if both of primary and secondary dns are bad,
    # we get the primary dns as default
    [ -z "$effect_dns" ] && {
        effect_dns=$(echo "$dnsp_servers"|awk -F ' ' '{print $1}')
        [ -z "$effect_dns" ] && {
            effect_dns=0.0.0.0        
        }
    }

    network_get_ipaddr lan_ip "lan"
    [ -z "$lan_ip" ] && {
        lan_ip=0.0.0.0
    }

    recorded_dns=
	[ -e "/var/run/dnsproxy/dnsproxy.server" ] && recorded_dns=$(cat /var/run/dnsproxy/dnsproxy.server)

    recorded_lan=
    [ -e "/var/run/dnsproxy/dnsproxy.lan" ] && recorded_lan=$(cat /var/run/dnsproxy/dnsproxy.lan)
    
    # When dns server be changed or the ipaddr of lan be changed, then changed nat rule
    [ "$effect_dns" != "$recorded_dns" -o "$lan_ip" != "$recorded_lan" ] && {
        # Delete the old dnsproxy nat rule first
        nat del dns
        nat add dns { "$lan_ip" "$effect_dns" }

        echo "$effect_dns" > "/var/run/dnsproxy/dnsproxy.server"
        echo "$lan_ip" > "/var/run/dnsproxy/dnsproxy.lan"
    }

    sleep $dnsp_check_interval
done

config_clear

return 0

