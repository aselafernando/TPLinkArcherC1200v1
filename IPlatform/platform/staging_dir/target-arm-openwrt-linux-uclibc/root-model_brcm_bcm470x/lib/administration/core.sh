# Copyright (C) 2009-2010 OpenWrt.org

syslog() {
    local log_id=$2
    local model_id=$1
    shift
    logx -p $$ $model_id $log_id "$@"
}

add_device() {
    local lenable
    local lmac
    config_get lenable $1 enable "off"
    config_get lmac $1 mac "0"

    # Process firewall rule for each device.
    if [ $lenable == 'on' ]; then
        if [ $lmac != '0' ]; then
            fw add_local_mgnt $lmac
            echo fw add_local_mgnt $lmac
        fi
    fi
}

del_device() {
    local lenable
    local lmac
    config_get lenable $1 enable "off"
    config_get lmac $1 mac "0"

    # Process firewall rule for each device.
    if [ $lenable == 'on' ]; then
        if [ $lmac != '0' ]; then
            fw del_local_mgnt $lmac
            echo fw del_local_mgnt $lmac
        fi
    fi
}

administration_start() {
    echo "loading administration management"

    config_load 'administration'

    # Local management, getting config.
    local lmode
    config_get lmode 'local' mode 'all'

    # Process the local management firewall rules.
    if [ "$lmode" == 'all' ]; then
        fw unload_local_mgnt
        echo fw unload_local_mgnt
    elif [ "$lmode" == 'partial' ]; then
        fw load_local_mgnt
        echo fw load_local_mgnt
        config_foreach add_device device
    fi

    # Remote management, getting config.
    local lenable
    local lport
    local lip
    config_get lenable 'remote' enable 'off'
    config_get lport 'remote' port '80'
    config_get lip 'remote' ipaddr '0.0.0.0'

    # Process the remote management firewall rules.
    if [ "$lenable" != "off" ]; then
        syslog 282 504
        if [ "$lenable" == "all" ]; then
            fw add_remote_mgnt port "$lport"
            echo fw add_remote_mgnt port "$lport" 
            nat add http { 255.255.255.255 "$lport" }
            echo nat add http { 255.255.255.255 "$lport" }
        elif [ "$lenable" == "partial" ]; then
            fw add_remote_mgnt port "$lport" ip "$lip"
            echo fw add_remote_mgnt port "$lport" ip "$lip"
            nat add http { "$lip" "$lport" }
            echo nat add http { "$lip" "$lport" }
        fi       
    fi
}

administration_stop() {
    echo "stopping administration management"

    config_load 'administration'

    # Unload the local management firewall rules.
    fw unload_local_mgnt
    echo fw unload_local_mgnt
    config_foreach del_device device

    local lenable
    local lport
    local lip
    config_get lenable 'remote' enable 'off'
    config_get lport 'remote' port '80'
    config_get lip 'remote' ipaddr '0.0.0.0'

    # Unload the remote management firewall rules.
    if [ "$lenable" == "all" ]; then
        fw del_remote_mgnt port "$lport" 
        echo fw del_remote_mgnt port "$lport"
    elif [ "$lenable" == "partial" ]; then
        fw del_remote_mgnt port "$lport" ip "$lip"
        echo fw del_remote_mgnt port "$lport" ip "$lip"
    fi
    syslog 282 505
    nat del http
    echo nat del http
}

administration_restart() {
    administration_stop 
    administration_start
}
