#!/bin/sh

RAMFS_COPY_BIN="/usr/bin/nvrammanager"

platform_check_image() {
	echo "Platform image check OK ..."
	return 0
}

platform_do_upgrade() {
	if type 'platform_upgrading_hook' >/dev/null 2>/dev/null; then
		echo "Call platform upgrading hook ..."
		platform_upgrading_hook 
	fi
	echo "Perform nvrammanager upgrading ..."
	nvrammanager -u $@
}
