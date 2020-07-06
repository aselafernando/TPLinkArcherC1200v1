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

require "luci.sys"
require "luci.util"
local uci = require "luci.model.uci".cursor()

--- Retrieve the current connection status for all connections registered with
-- pluto.
-- @return table of connections indexed by connection name where each
-- connection has a table of statuses indexed by the instance serial number,
-- or an empty table if there are no active connections
local function get_connection_statuses()
	local statuses = {}
	local pfile = io.popen("ipsec auto --status")
	for line in pfile:lines() do
		-- Match connection description
		-- (to create an empty table for inactive connections)
		local conname = line:match("^%d+%s\"([^\"]+)\"")
		if conname then
			if not statuses[conname] then
				statuses[conname] = {}
			end
		end

		-- Match individual connection instances
		local coninstnum, conname, coninststatename, coninststatestory =
			line:match("^%d+%s+#(%d+):%s+\"([^\"]+)\".-:%d+%s+(%S+)%s+%(([^)]+)%)")
		if coninstnum then
			coninstnum = tonumber(coninstnum)

			if not statuses[conname] then
				statuses[conname] = {}
			end

			statuses[conname][coninstnum] = coninststatestory
		end
	end
	pfile:close()

	return statuses
end

local map = Map("ipsec", translate("IPsec Connection Status"),
	translate("To troubleshoot connection problems, check the <a href=\"%s\">System Log</a>."):format(
		luci.dispatcher.build_url("admin", "status", "syslog")))
local sect = map:section(TypedSection, "ipsec_conn",
	translate("IPsec Connections"),
	translate("This section contains a list of the configured IPsec connections and their current states."))
sect.template = "cbi/tblsection"
sect.template_addremove = "ipsec/cbi-select-input-add"
sect.addremove = true
sect.add_select_options = { }
sect.extedit = luci.dispatcher.build_url(
	"admin", "services", "ipsec", "connections", "%s"
)

uci:load("ipsec_recipes")
uci:foreach("ipsec_recipes", "ipsec_recipe",
	function(cbisect)
		sect.add_select_options[cbisect['.name']] =
			cbisect['_shortdesc'] or cbisect['.name']
	end
)

function sect.parse(self, section)
	local recipe = luci.http.formvalue(
		luci.cbi.CREATE_PREFIX .. self.config .. "." ..
		self.sectiontype .. ".select"
	)

	if recipe and not sect.add_select_options[recipe] then
		self.invalid_cts = true
	else
		TypedSection.parse(self, section)
	end
end

function sect.create(self, name)
	local connname = luci.http.formvalue(
		luci.cbi.CREATE_PREFIX .. self.config .. "." ..
		self.sectiontype
	)
	local recipe = luci.http.formvalue(
		luci.cbi.CREATE_PREFIX .. self.config .. "." ..
		self.sectiontype .. ".select"
	)

	if connname and connname:match("^[a-zA-Z][a-zA-Z0-9._%-]*$") then
		uci:section(
			"ipsec", "ipsec_conn", connname,
			uci:get_all("ipsec_recipes", recipe)
		)

		-- Options which begin with "_" describe the template
		-- Remove them from the instantiated copy
		for optname,optval in pairs(uci:get_all("ipsec", connname)) do
			if optname:sub(1,1) == "_" then
				uci:delete("ipsec", connname, optname)
			end
		end
		uci:save("ipsec")

		luci.http.redirect(self.extedit:format(connname))
	else
		self.invalid_cts = true
	end
end


constatus = get_connection_statuses()
local statusopt = sect:option(DummyValue, "_status", translate("Status"))
statusopt.rawhtml = true
function statusopt.cfgvalue(self, section)
	if not constatus[section] then
		return translate("Not Loaded")
	else
		local val = translate("Loaded")
		for num, state in luci.util.kspairs(constatus[section]) do
			val = val .. "<br />" .. num .. ") " .. state
		end
		return val
	end
end

-- The ipsec_auto(8) terminology is too confusing in this context due to
-- ambiguities between deleting the connection and deleting its configuration
-- Instead add/delete => Enable/Disable, up/down => Connect/Disconnect
local adddelopt = sect:option(Button, "_adddel", translate("Enable/Disable"))
function adddelopt.cbid(self, section)
	if not constatus[section] then
		self.option = "add"
	else
		self.option = "delete"
	end
	return AbstractValue.cbid(self, section)
end
function adddelopt.cfgvalue(self, section)
	if self.option == "add" then
		self.title = translate("Enable")
		self.inputstyle = "add"
	else
		self.title = translate("Disable")
		self.inputstyle = "remove"
	end
end
function adddelopt.write(self, section, value)
	if self.option == "add" then
		luci.sys.call("ipsec auto --add " .. section .. " >/dev/null")
		constatus[section] = {}
	elseif self.option == "delete" then
		luci.sys.call("ipsec auto --delete " .. section .. " >/dev/null")
		constatus[section] = nil
	end
end

local updownopt = sect:option(Button, "_updown", translate("Connect/Disconnect"))
function updownopt.cbid(self, section)
	if not constatus[section] then
		self.option = "addup"
	elseif #constatus[section] == 0 then
		self.option = "up"
	else
		self.option = "down"
	end
	return AbstractValue.cbid(self, section)
end
function updownopt.cfgvalue(self, section)
	if self.option == "addup" then
		-- Only display when connection is added/enabled
		return false
	elseif self.option == "up" then
		self.title = translate("Connect")
		self.inputstyle = "apply"
	else
		self.title = translate("Disconnect")
		self.inputstyle = "reset"
	end
end
function updownopt.write(self, section, value)
	if self.option == "up" then
		luci.sys.call("ipsec auto --asynchronous --up " .. section .. " >/dev/null")
	elseif self.option == "down" then
		luci.sys.call("ipsec auto --down " .. section .. " >/dev/null")
		constatus[section] = {}
	end
end


local leftopt = sect:option(DummyValue, "left", translate("Left"))
local rightopt = sect:option(DummyValue, "right", translate("Right"))

return map
