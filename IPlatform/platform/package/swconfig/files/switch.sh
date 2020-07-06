#!/bin/sh
# Copyright (C) 2009 OpenWrt.org

setup_switch_dev() {
	config_get name "$1" name
	name="${name:-$1}"
	[ -d "/sys/class/net/$name" ] && ifconfig "$name" up
	swconfig dev "$name" load switch
}

setup_switch() {
	config_load switch
	config_foreach setup_switch_dev switch
}
