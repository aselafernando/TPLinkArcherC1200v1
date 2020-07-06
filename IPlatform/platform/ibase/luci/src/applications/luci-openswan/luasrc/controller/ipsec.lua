--[[
LuCI - Lua Configuration Interface
IPsec Support

Copyright 2008 Steven Barth <steven@midlink.org>
Copyright 2008 Jo-Philipp Wich <xm@leipzig.freifunk.net>
Copyright 2011 Kevin Locke <klocke@digitalenginesoftware.com>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

$Id$
]]--
module("luci.controller.ipsec", package.seeall)

function index()
	require("luci.i18n")
	luci.i18n.loadc("ipsec")

	local e = entry( {"admin", "services", "ipsec"},
		cbi("ipsec-status"),
		luci.i18n.translate("IPsec") )
	e.i18n = "ipsec"
	e.subindex = true

	e = entry( {"admin", "services", "ipsec", "certificates"},
		form("ipsec-certificates"),
		luci.i18n.translate("Certificates") )
	e.i18n = "ipsec"
	e.leaf = true
	e.order = 10

	e = entry( {"admin", "services", "ipsec", "connections"},
		cbi("ipsec-connection"),
		luci.i18n.translate("Connections") )
	e.i18n = "ipsec"
	e.leaf = true
	e.order = 20

	e = entry( {"admin", "services", "ipsec", "secrets"},
		cbi("ipsec-secrets"),
		luci.i18n.translate("Secrets") )
	e.i18n = "ipsec"
	e.leaf = true
	e.order = 30

	e = entry( {"admin", "services", "ipsec", "settings"},
		cbi("ipsec-settings"),
		luci.i18n.translate("Settings") )
	e.i18n = "ipsec"
	e.leaf = true
	e.order = 40
end
