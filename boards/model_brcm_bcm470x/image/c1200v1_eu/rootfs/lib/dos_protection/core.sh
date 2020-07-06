# Copyright (C) 2009-2010 OpenWrt.org
AC_LIBDIR=${AC_LIBDIR:-/lib/dos_protection}

fw_is_loaded() {
    UCI_CONFIG_DIR="/etc/config/"
    local bool=$(uci_get_state firewall.core.loaded)
    UCI_CONFIG_DIR="/tmp/state/"
    return $((! ${bool:-0}))
}

fw_init() {
    [ -z "$AC_INITIALIZED" ] || return 0
    . $AC_LIBDIR/config.sh
    fw_config_append dos_protection

    for file in $AC_LIBDIR/core_*.sh; do
        . $file
    done
    
    AC_INITIALIZED=1
    return 0
}

fw_start() {
    # init
    fw_init

    fw_is_loaded || {
        syslog $DOS_PROTECTION_LOG_WNG_FIREWALL_NOT_LOADED
        exit 1
    }

    echo "loading dos_protection"
    fw_load_all dos_protection
    syslog $LOG_INF_SERVICE_START
}

fw_stop() {
    # init
    fw_init

    fw_is_loaded || {
        syslog $DOS_PROTECTION_LOG_WNG_FIREWALL_NOT_LOADED
        exit 1
    }

    echo "exiting dos_protection"
    fw_exit_all dos_protection
    syslog $LOG_INF_SERVICE_STOP
}

fw_restart() {
    fw_stop
    fw_start
}

fw_reload() {
    fw_restart
}