# Copyright (C) 2009-2010 OpenWrt.org
AC_LIBDIR=${AC_LIBDIR:-/lib/access_control}

fw_is_loaded() {
    local bool=$(uci_get_state firewall.core.loaded)
    return $((! ${bool:-0}))
}


fw_init() {
    [ -z "$AC_INITIALIZED" ] || return 0
    . $AC_LIBDIR/config.sh
    fw_config_append access_control

    for file in $AC_LIBDIR/core_*.sh; do
        . $file
    done
    
    AC_INITIALIZED=1
    return 0
}

fw_start() {
    fw_init

    fw_is_loaded || {
        syslog $ACCESS_CONTROL_LOG_WNG_FIREWALL_NOT_LOADED
        exit 1
    }

    echo "loading access_control"
    fw_load_all access_control
    syslog $LOG_INF_SERVICE_START
}

fw_stop() {
    fw_init
    
    fw_is_loaded || {
        syslog $ACCESS_CONTROL_LOG_WNG_FIREWALL_NOT_LOADED
        exit 1
    }

    echo "exiting access_control"
    fw_exit_all access_control
    syslog $LOG_INF_SERVICE_STOP
}

fw_restart() {
    fw_stop
    fw_start
}

fw_reload() {
    fw_restart
}

