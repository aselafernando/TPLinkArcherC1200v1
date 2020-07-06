--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  network-log.lua
Details :  Log constants for network module.
Author  :  Ye Qianchuan <yeqianchuan@tp-link.net>
Version :  1.0.0
Date    :  12 Aug, 2014
]]--

module("luci.model.network_log", package.seeall)

ID = 290

UP_WAN            = 1
DOWN_WAN          = 2
CONNECT_WAN       = 3
DISCONNECT_WAN    = 4
RELOAD_WAN        = 5
DDNS_IFACE_CHANGE = 10
RM_SESSION        = 21
SET_IGMP          = 22
UPDATE_TFSTATS    = 23
SET_WAN_PROTO     = 100
SET_XXTP_BPA      = 101
SET_PPPOE         = 102
SET_STATIC        = 103
SET_STATICV6      = 104
SET_PPPOEV6       = 105
SET_MAC           = 106
SET_IPV6          = 130
SET_LAN           = 140
SET_MAC_CLONE     = 150
RESTART           = 250

NO_CHANGED        = 509
INVALID_ARGS      = 702
CONFIG_SAVE_FAIL  = 755
UBUS_CONN_FAILED  = 758
