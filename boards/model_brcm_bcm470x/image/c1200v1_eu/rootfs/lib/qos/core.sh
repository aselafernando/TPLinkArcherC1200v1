# Copyright (C) 2009-2010 OpenWrt.org

PC_LIBDIR=${PC_LIBDIR:-/lib/qos}

include /lib/network
# check firewall
fw_is_loaded() {
    local bool=$(uci_get_state firewall.core.loaded)
    return $((! ${bool:-0}))
}


fw_init() {
    [ -z "$PC_INITIALIZED" ] || return 0

    . $PC_LIBDIR/config.sh

    # export the qos config
    fw_config_append qos

    for file in $PC_LIBDIR/core_*.sh; do
        . $file
    done
    
    PC_INITIALIZED=1
    return 0
}

fw_start() {
    
    # make sure firewall is loaded
    fw_is_loaded || {
        echo "firewall is not loaded" >&2
        exit 1
    }

    # check the hook and chains

    # init
    fw_init

    # ready to load rules from uci config
    echo "loading qos"
    fw_load_qos

    syslog $LOG_INF_SERVICE_START
    
}

fw_stop() {

    # make sure firewall is loaded
    fw_is_loaded || {
        echo "firewall is not loaded" >&2
        exit 1
    }
    # check the hook and chains

    # init
    fw_init

    # ready to exit rules from uci config
    echo "exiting qos"
    fw_exit_qos

    syslog $LOG_INF_SERVICE_STOP
    
}

fw_restart() {
    fw_stop
    fw_start
}

fw_reload() {
    # make sure firewall is loaded
    fw_is_loaded || {
        echo "firewall is not loaded" >&2
        exit 1
    }
    # reload
    fw_init

    fw_config_once fw_rule_reload global
}

