# Copyright (C) 2009-2010 OpenWrt.org

AC_LIBDIR=${AC_LIBDIR:-/lib/basic_security}

include /lib/network
# check firewall
fw_is_loaded() {
	local bool=$(uci_get_state firewall.core.loaded)
	return $((! ${bool:-0}))
}


fw_init() {
	[ -z "$AC_INITIALIZED" ] || return 0

	. $AC_LIBDIR/config.sh

	# export the basic_security config
	fw_config_append basic_security

	for file in $AC_LIBDIR/core_*.sh; do
		. $file
	done
	
	AC_INITIALIZED=1
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
	echo "loading basic_security"
	fw_load_all basic_security

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
	echo "exiting basic_security"
	fw_exit_all basic_security

	syslog $LOG_INF_SERVICE_STOP
	
}

fw_restart() {
	fw_stop
	fw_start
}

fw_reload() {
	fw_restart
}

fw_hotplug() {

	# make sure firewall is loaded
	fw_is_loaded || {
		echo "firewall is not loaded" >&2
		exit 1
	}
	# check the hook and chains

	# init
	fw_init

	# ready to exit rules from uci config
	echo "basic_security: br-lan changed"
	fw_update_br_lan

}