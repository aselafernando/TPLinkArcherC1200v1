# Copyright (C) 2009-2010 OpenWrt.org
MOD_ID=249

fw_config_get_dos_entry() {
	fw_config_get_section "$1" dos_entry { \
		string mac "" \
		string ipaddr "" \
	} || return
}

fw_exit_all() {
	fw flush 4 f dos_protection
}

fw_load_dos_entry() {
	fw_config_get_dos_entry $1
	local mac=$(echo $dos_entry_mac | tr [a-z] [A-Z])
	local rule="-m mac --mac-source ${mac//-/:}"
	fw s_add 4 f dos_protection DROP { "$rule" }
	syslog DOS_PROTECTION_LOG_DBG_BLOCK_LIST_ADD "$mac"

    conntrack -D -s "$dos_entry_ipaddr"
    syslog DOS_PROTECTION_LOG_INF_FLUSH_CT "$dos_entry_ipaddr"
}

fw_load_all() {
	local global_enable=$(uci get dos_protection.settings.enable)
	case $global_enable in
		on )
		config_foreach fw_load_dos_entry dos_entry
		syslog $LOG_INF_FUNCTION_ENABLE
			;;

		off )
		syslog $LOG_INF_FUNCTION_DISABLE
			;;
	esac
}
