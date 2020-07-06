#!/bin/sh
# Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.
#
# Details : The real auto detection script.
# Author  : Ye Qianchuan <yeqianchuan@tp-link.net>
# Version : 1.0
# Date    : 25 Mar, 2014

. /lib/functions/network.sh

# Get wan interface.
network_get_physdev IFC wan

DNS_FILE=/tmp/autodetect-dns
DHCP_SCRIPT="/lib/autodetect/dhcp.script"
DHCP_PIDFILE="/var/run/udhcpc-${IFC}:${DHCP_ALIAS}.pid"

DHCP_IFC=${IFC}:${DHCP_ALIAS}
PPPOE_IFC=${IFC}:${PPPOE_ALIAS}

internet_ok=1
dhcp_ok=1
pppoe_ok=1

record() {
    echo $@ >$RESULT_FILE
}

record_clean_and_exit() {
    record $@
    rm -f $DNS_FILE
    [ -f "$DHCP_PIDFILE" ] && kill $(cat "$DHCP_PIDFILE")
    exit 0
}

wan_get_proto() {
    local ifc=wan
    ubus list | grep -q network.interface.internet && ifc=internet

    local proto
    network_get_proto proto $ifc && echo $proto || echo none
}

# Check physical connection.
network_get_link physlink $IFC
[ -z "$physlink" ] && record_clean_and_exit "none"
[ "$physlink" = 0 ] && record_clean_and_exit "unplugged"

# Check the internet status.
online-test "$CHECK_URL" "$DNS_FIRST_TIMEOUT" && \
    record_clean_and_exit "$(wan_get_proto)"

# Update estimated time
record time $(((EST_TIME-INTERNET_TIMEOUT)*1000))

# Check DHCP.
udhcpc \
    -p "$DHCP_PIDFILE" \
    -s "$DHCP_SCRIPT" \
    -i $DHCP_IFC \
    -t $DHCP_TIMEOUT -T $DHCP_RETRIES \
    -n -R >/dev/null &
# This is the foreground udhcpc pid.
DHCP_PID=$!

# Check PPPOE.
pppoe-discovery \
    -I $PPPOE_IFC \
    -t $PPPOE_TIMEOUT -T $PPPOE_RETRIES >/dev/null &
PPPOE_PID=$!

# Check the DHCP status and
# check the internet status with DNS server set.

if wait $DHCP_PID; then
    # Update estimated time
    record time $((DNS_TIMEOUT*1000))

    dnslookup -t $DNS_TIMEOUT "$CHECK_URL" $(cat "$DNS_FILE") >/dev/null && \
    record_clean_and_exit "dhcp"
fi

# Check the PPPOE status.
wait $PPPOE_PID && \
    record_clean_and_exit "pppoe" || record_clean_and_exit "dhcp"
