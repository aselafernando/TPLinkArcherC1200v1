# Copyright(c) 2011-2013 Shenzhen TP-LINK Technologies Co.Ltd.
#
# /usr/lib/dynamic_dns/dynamic_dns_dyndns.sh
# Written by Guo Dongxian<guodongxian@tp-link.com.cn>, April 2014
#
# This script is used to parse dyndns server return code and convert
# to TP New UI status code

. /usr/lib/ddns/dynamic_dns_log.sh

DYNDNS_RETCODE_GOOD=8001
DYNDNS_RETCODE_ERR=8002
DYNDNS_RETCODE_UNKNOWN=8003

DYNDNS_RETCODE_NOCHG=9000
DYNDNS_RETCODE_NOHOST=9001      # The hostname specified does not exist
DYNDNS_RETCODE_NOTFQDN=9002     # The hostname specified is not a fully-qualified domain name
DYNDNS_RETCODE_NOT_YOURS=9003   # The hostname specified exists, but not under the username specified
DYNDNS_RETCODE_ABUSE=9004       # The hostname specified is blocked for update abuse
DYNDNS_RETCODE_NUMHOST=9005     # Too many or too few hosts found

DYNDNS_RETCODE_BADAUTH=9101     # The username or password specified are incorrect

# An option available only to credited users was specified, but the user is
# not a credited user. If mulitple hosts were specified, only a single !donator
# will be returned
DYNDNS_RETCODE_NOT_DONATOR=9102
# The sycodestem parameter given is not valid. Valid system parameters:dyndns,statdns and custom
DYNDNS_RETCODE_BADSYS=9201      
# The user agent that was sent has been blocked for not flollowing these specifications or no user agent was specified
DYNDNS_RETCODE_BADAGENT=9202

DYNDNS_RETCODE_DNSERR=9301      # DNS error encountered
DYNDNS_RETCODE_911=9302         # There is a serious problem on our side, such as a database or DNS server failure 

# Dyndns state
DYNDNS_STATE_RUN_OK=0     # ok
DYNDNS_STATE_STOP=1       # stop
DYNDNS_STATE_CONNECTING=2 # connecting
DYNDNS_STATE_BADAUTH=3    # badauth
DYNDNS_STATE_BADHOST=4    # badhost     

DYNDNS_HTTP_ERR_PATH=/tmp/dyndns.error
DYNDNS_NSLOOKUP_TMO=10

dyndns_retrieve_prog()
{
    # MIPS:"ez-update-3.0.10 mips-mips-linux-gnu [daemon] (by Angus Mackay)"
    # ARM:?
    local user_agent=
    
    if [ -z "$user_agent" ]; then
        dyndns_prog="/usr/bin/curl"
    else
        dyndns_prog="/usr/bin/curl -U ${user_agent}"
    fi
    
    echo "$dyndns_prog"
}

dyndns_state_init()
{
    uci_set_state ddns dyndns "" ddns_state
    uci_set_state ddns dyndns state $DYNDNS_STATE_CONNECTING
}

dyndns_update()
{
    local prog="$1"
    local url="$2"
    
    dyndns_result=1
    
    if [ -n "$prog" -a -n "$url" ]
    then
        request=$( ${prog} -v -0 -A tplink -H "host: members.dyndns.org" -H "Accept:" "${url} " 2>$DYNDNS_HTTP_ERR_PATH )
        if [ $? -gt 0 ]; then
            dyndns_result=1
        else
            dyndns_result=0        
        fi
    fi
    
    echo $dyndns_result
}

dyndns_parse_retcode()
{
    local service_id="$1"
    local http_res=$(cat $DYNDNS_HTTP_ERR_PATH|grep -o "curl")

    dyndns_code=''
    if [ -n "$http_res" -a "$http_res" == "curl" ]; then
        # Only check 4xx http error code
        local http_code=$(cat $DYNDNS_HTTP_ERR_PATH|grep -o "4[0-9][0-9]")
        case $http_code in
            401) dyndns_code=$DYNDNS_RETCODE_BADAUTH ;;
            404) 
                dyndns_code=$DYNDNS_RETCODE_ERR
                ddns_syslog $DDNS_LOG_WNG_RES_NOT_FOUND "dyndns"
            ;;
            *) 
                dyndns_code=$DYNDNS_RETCODE_UNKNOWN
                ddns_syslog $DDNS_LOG_WNG_SVR_RESET "dyndns"
            ;;
        esac
    else
        local result=$(cat /var/run/dynamic_dns/${service_id}.retcode|awk -F ' ' '{print $1}')
        case $result in
            good)       dyndns_code=$DYNDNS_RETCODE_GOOD ;;
            nochg)      dyndns_code=$DYNDNS_RETCODE_NOCHG ;;
            nohost)     dyndns_code=$DYNDNS_RETCODE_NOHOST ;;
            notfqdn)    dyndns_code=$DYNDNS_RETCODE_NOTFQDN ;;
            !yours)     dyndns_code=$DYNDNS_RETCODE_NOT_YOURS ;;
            numhost)    dyndns_code=$DYNDNS_RETCODE_NUMHOST ;;
            abuse)      dyndns_code=$DYNDNS_RETCODE_ABUSE ;;
            badauth)    dyndns_code=$DYNDNS_RETCODE_BADAUTH ;;
            !donator)   dyndns_code=$DYNDNS_RETCODE_NOT_DONATOR ;;
            badsys)     dyndns_code=$DYNDNS_RETCODE_BADSYS ;;
            badagent)   dyndns_code=$DYNDNS_RETCODE_BADAGENT ;;
            dnserr)     dyndns_code=$DYNDNS_RETCODE_DNSERR ;;
            911)        dyndns_code=$DYNDNS_RETCODE_911 ;;
            *)          dyndns_code=$DYNDNS_RETCODE_ERR ;;
        esac
        ddns_syslog $DDNS_LOG_INF_RETCODE "dyndns" $result
    fi
    
    # After parsing, then remove the temp http response code file
    rm -f $DYNDNS_HTTP_ERR_PATH

    echo "$dyndns_code"
}

dyndns_set_state()
{
    local code="$1"

    [ -z "$code" ] && {
        uci_toggle_state ddns dyndns state $DYNDNS_STATE_STOP
        ddns_syslog $DDNS_LOG_NTC_LOGIN_FAIL "dyndns"
        return    
    }

    if [ $code -eq $DYNDNS_RETCODE_GOOD -o $code -eq $DYNDNS_RETCODE_NOCHG ]; then
        uci_toggle_state ddns dyndns state $DYNDNS_STATE_RUN_OK
        ddns_syslog $DDNS_LOG_NTC_LOGIN_SUCC "dyndns"
    elif [ $code -eq $DYNDNS_RETCODE_UNKNOWN ]; then
        uci_toggle_state ddns dyndns state $DYNDNS_STATE_CONNECTING
        ddns_syslog $DDNS_LOG_INF_CONNING "dyndns"
    elif [ $code -eq $DYNDNS_RETCODE_BADAUTH ]; then
        uci_toggle_state ddns dyndns state $DYNDNS_STATE_BADAUTH
        ddns_syslog $DDNS_LOG_NTC_ACC_INCORRECT "dyndns"
    elif [ $code -ge $DYNDNS_RETCODE_NOHOST -a $code -le $DYNDNS_RETCODE_NOT_YOURS ]; then
        uci_toggle_state ddns dyndns state $DYNDNS_STATE_BADHOST
        ddns_syslog $DDNS_LOG_NTC_DOMAIN_FAIL "dyndns"
    else
        uci_toggle_state ddns dyndns state $DYNDNS_STATE_STOP 
        ddns_syslog $DDNS_LOG_NTC_LOGIN_FAIL "dyndns"   
    fi
    
    return    
}

dyndns_check()
{
    local s=$(uci_get_state ddns dyndns state)
    
    config_get domain "dyndns" domain 
    [ -n "$s" ] && {
        if [ $s -eq $DYNDNS_STATE_CONNECTING ]; then
            network_get_dnsserver dns "$1"
            for d in $dns; do
                [ -n "$d" ] && {
                    local ns=$(dnslookup -t $DYNDNS_NSLOOKUP_TMO $domain $d|head -n 1|grep -o "$ip_regex")
                    [ "$ns" == "$2" ] && {
                        # When finding server can update ip address, then changing state: connection->succeed 
                        uci_toggle_state ddns dyndns state $DYNDNS_STATE_RUN_OK  
                        ddns_syslog $DDNS_LOG_NTC_LOGIN_SUCC "dyndns"                  
                        return 0
                    }           
                }
            done        
        fi
    }

    return 1
}

dyndns_retry_interval()
{
    dyndns_retry_int=
    local s=$(uci_get_state ddns dyndns state)

    local chk_int chk_unit chk_int_sec
    config_get chk_int "dyndns" check_interval
    config_get chk_unit "dyndns" check_unit

    [ "$chk_int" != "never" ] && {
        case "$check_unit" in
	        "days" )
		        chk_int_sec=$(($chk_int*60*60*24))
		    ;;
	        "hours" )
		        chk_int_sec=$(($chk_int*60*60))
		    ;;
	        "minutes" )
		        chk_int_sec=$(($chk_int*60))
		    ;;
	        "seconds" )
		        chk_int_sec=$chk_int
		    ;;
	        * )
		        #default is seconds
		        chk_int_sec=$chk_int
		    ;;
        esac
    }
        
    if [ "$s" -eq $DYNDNS_STATE_BADHOST ]; then
        dyndns_retry_int=${chk_int_sec:-"never"}
    elif [ "$s" -eq $DYNDNS_STATE_BADAUTH ]; then
        dyndns_retry_int=${chk_int_sec:-"never"}
    elif [ "$s" -eq $DYNDNS_STATE_STOP ]; then
        dyndns_retry_int=$((60*1)) # One minutes
    else
        dyndns_retry_int='' # Default, read it from config
    fi

    echo "$dyndns_retry_int"
}

