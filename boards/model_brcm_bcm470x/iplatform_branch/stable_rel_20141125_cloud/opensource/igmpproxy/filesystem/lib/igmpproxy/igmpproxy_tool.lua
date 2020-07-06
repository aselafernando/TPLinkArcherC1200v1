#!/usr/bin/env lua
--[[
Copyright(c) 2013 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  igmpproxy_tool.lua
Details :  Tools for igmp proxy
Author  :  Guo Dongxian<guodongxian@tp-link.com.cn>
Version :  1.0.0
Date    :  28May, 2014
]]--

local uci = require "luci.model.uci"
local valid = require "luci.tools.datatypes"

local uci_r = uci.cursor()
local save  = false

local function igmp_conf_commit()
    if save then uci_r:commit("igmpproxy") end
end

local function igmp_downstream_check(interface)
    local found

    uci_r:foreach("igmpproxy", "phyint",
    function(sec)
        local dir = uci_r:get("igmpproxy", sec[".name"], "direction")
        local iface = uci_r:get("igmpproxy", sec[".name"], "network")
        if dir == "downstream" and iface == interface then
            found = true     
        end
    end
    )

    return found and true or false
end

-- When interface be changed, it needs to update igmpproxy config
local function igmp_upstream_iface_chg()
    local wan_type = uci_r:get("network", "wan", "wan_type")
    
    local iface
    if wan_type == "static" or wan_type == "dhcp" or wan_type == "none" then
        iface = "wan"
    else
        iface = "internet"
    end

    local section
    uci_r:foreach("igmpproxy", "phyint",
    function(sec)
        local dir = uci_r:get("igmpproxy", sec[".name"], "direction")
        local net = uci_r:get("igmpproxy", sec[".name"], "network")
        if dir == "upstream" and net == "internet" then
            section = sec[".name"]     
        end
    end
    )
    
    if iface == "wan" then
        if section then
            uci_r:delete("igmpproxy", section)
            save = true
        end
    else
        if not section then
            uci_r:section_first("igmpproxy", "phyint", nil, {["network"] = iface, ["direction"] = "upstream"})
            save = true
        end
    end
    
    return true
end

-- Update upstream interface altnet when network interface changed
local function igmp_upstream_altnet(interface, ipaddr, mask)
    local altnet = {}

    if not valid.ip4addr(ipaddr) or not valid.ip4prefix(mask) then
        return false
    end

    local found
    local net = ipaddr .. "/" .. mask
    uci_r:foreach("igmpproxy", "phyint",
	    function(section)
		    local iface = uci_r:get("igmpproxy", section[".name"], "network")

		    if iface == interface then
		     	altnet = uci_r:get_list("igmpproxy", section[".name"], "altnet")
                if altnet then
			        for _, addr in ipairs(altnet) do
				        if addr == net then
					        found = true
					        break
				        end
			        end
                end
                
                if not found then
                    -- This maybe terriable operation
                    uci_r:delete("igmpproxy", section[".name"], "altnet")
                    uci_r:set_list("igmpproxy", section[".name"], "altnet", net)	
                    save = true				
                end
		    end
	    end)

    return true
end

if arg[1] and arg[2] and arg[3] then
    local check
    check = igmp_downstream_check(arg[1])

    if not check then
        -- Update upstream interface information
        igmp_upstream_iface_chg()
        local ret
        -- Update upstream altnet address
        ret = igmp_upstream_altnet(arg[1], arg[2], arg[3]) 
        -- Save config
        igmp_conf_commit()

        if ret then
            print(true)
        else
            print(false)
        end
    end   
end

