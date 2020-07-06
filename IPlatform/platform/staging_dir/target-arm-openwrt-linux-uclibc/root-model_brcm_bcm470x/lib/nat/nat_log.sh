# Copyright(c) 2011-2013 Shenzhen TP-LINK Technologies Co.Ltd.
#
# /lib/nat/nat_log.sh
# Written by Guo Dongxian<guodongxian@tp-link.com.cn>, July 2014
#
# This script is used to make tp-link log of nat

NAT_MOD_ID=211

# 1, DBG, "Flush conntrack"
# 2, DBG, "Port triggering:trigger match %1 and trigger relate %2"

# 21, INF, "%1 ALG enabled"
# 22, INF, "%1 ALG disabled"
# 23, INF, "DMZ enabled"
# 24, INF, "DMZ disabled"

# 51, NTC, "Create NAT chain succeeded"
# 52, NTC, "Destroy NAT chain succeeded"
# 53, NTC, "The address %1 will be DMZ host"
# 54, NTC, "Virtual server created succeeded[ex-port:%1 ip:%2 in-port:%3 protocol:%4]"
# 55, NTC, "Port trigger created succeeded[trigger-port:%1 trigger-protocol:%2 ex-port:%3 ex-protocol:%4]"

# 81, WNG, "The host ip address is not in the lan subnet[%1]"

# 101, ERR, "Port triggering external port over five groups"

# common log
# 501, INF 
# 502, INF 
# 503, INF

# nat_syslog log_id log_param
nat_syslog()
{
    local log_id=$1
    shift
    logx -p $$ $NAT_MOD_ID $log_id "$@"
}

