# Copyright(c) 2011-2013 Shenzhen TP-LINK Technologies Co.Ltd.
#
# /usr/lib/dynamic_dns/dynamic_dns_log.sh
# Written by Guo Dongxian<guodongxian@tp-link.com.cn>, June 2014
#
# This script is used to make tp-link log of dynamic dns

DYNAMIC_DNS_MOD_ID=214
# MSG(1, DBG, "Dynamic dns %1 registered ip: %2")
DDNS_LOG_DBG_REG_IP=1
# MSG(2, INF, "Dynamic dns %1 login start")
DDNS_LOG_INF_LOGIN_START=2
# MSG(3, INF, "Dynamic dns %1 connecting")
DDNS_LOG_INF_CONNING=3
# MSG(4, INF, "Dynamic dns %1 return code %2")
DDNS_LOG_INF_RETCODE=4
# MSG(4, NTC, "Dynamic dns %1 login succeed")
DDNS_LOG_NTC_LOGIN_SUCC=5
# MSG(5, NTC, "Dynamic dns %1 not launching")
DDNS_LOG_NTC_LOGIN_FAIL=6
# MSG(6, NTC, "Dynamic dns %1 incorrect username or password")
DDNS_LOG_NTC_ACC_INCORRECT=7
# MSG(7, NTC, "Dynamic dns %1 domain name error")
DDNS_LOG_NTC_DOMAIN_FAIL=8
# MSG(9,  WNG, "Dynamic dns server reset %1 login")
DDNS_LOG_WNG_SVR_RESET=9
# MSG(9,  WNG, "Dynamic dns %1 can not find the server resource")
DDNS_LOG_WNG_RES_NOT_FOUND=10
# MSG(11, WNG, "Dynamic dns get external ip from server[%1] failed")
DDNS_LOG_WNG_GET_EXIP_FAIL=11
# MSG(11, ERR, "Dynamic dns %1 get system ip address failed")
DDNS_LOG_ERR_GET_SYSIP_ERR=12
# MSG(12, CRT, "Dynamic dns %1 create many instances")
DDNS_LOG_CRT_MANY_INST=13

# Common log from common.h
# MSG(501, INF, "Initializing succeeded")
DDNS_LOG_INF_INIT_SUCC=501
# MSG(504, INF, "Service start")
DDNS_LOG_INF_SERVICE_START=504
# MSG(505, INF, "Service stop")
DDNS_LOG_INF_SERVICE_STOP=505

# ddns_syslog log_id log_param
ddns_syslog()
{
    local log_id=$1
    shift
    logx -p $$ $DYNAMIC_DNS_MOD_ID $log_id "$@"
}

