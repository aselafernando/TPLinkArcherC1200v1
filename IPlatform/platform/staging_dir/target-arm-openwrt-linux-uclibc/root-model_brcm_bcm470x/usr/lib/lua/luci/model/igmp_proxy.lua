--[[
Copyright(c) 2013 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  IGMP_Proxy.lua
Details :  controller for diagnostic.html webpage
Author  :  Zhou Guofeng<Zhouguofeng@tp-link.net>
           Guo Dongxian<guodongxian@tp-link.com.cn>
Version :  1.0.0
Date    :  19Mar, 2014
]]--

module ("luci.model.igmp_proxy",package.seeall)

local uci  = require "luci.model.uci"
local util = require "luci.util"
local sys  = require "luci.sys"
local IGMP_SHELL = "/etc/init.d/igmpproxy"
local MCPROXY_CMD = "/etc/init.d/improxy restart"
local FIREWALL_SHELL = "/etc/init.d/firewall"
local debug = require "luci.tools.debug"

local uci_r = uci.cursor()

local function get_mask_num(netmask)
	local mask_tbl=util.split(netmask,".")
	local mask_num=0
	for i,k in pairs(mask_tbl) do	
		k=k-0	
		if k~=255 then
			local bit=0
			local map=128
			while true do
				if k==0 or  k<0 or bit == 8 then break end
				bit=bit+1
				k=k-map
				map=map/2
			end
			mask_num=(i-1)*8+bit
			break
		end
	end
	
	return mask_num
end

local function get_and_set_altnet(lan_addr,netmask)
	
	local mask = get_mask_num(netmask)
	local altnet ={}
	local ret = false
	local addr_mask=lan_addr.."/"..mask
	uci_r:foreach("igmpproxy", "phyint",
		function(section)
			local net =uci_r:get("igmpproxy",section[".name"],"network")
			if net == "wan" then
			 	altnet=uci_r:get_list("igmpproxy",section[".name"],"altnet")
				for i,k in pairs(altnet) do
					if k == addr_mask then
						ret=true
						break
					end
				end
			
				if not ret then
					--print ("here")
					uci_r:delete("igmpproxy",section[".name"],"altnet")
					uci_r:set_list("igmpproxy",section[".name"],"altnet",addr_mask)					
				end
			end
		end
	)
	uci_r:commit("igmpproxy")
	uci_r:apply("igmpproxy")
	return ret
end

local fw_tab = {
    ["zone_wan"] = {
        [1] = { 
            target = "ACCEPT",
            cont = "-p igmp" 
        },

        [2] = {
            target = "ACCEPT",
            cont = "-p udp -d 224.0.0.0/4"
        }                               
    },       

    ["zone_wan_forward"] = {
        [1] = {
            target = "zone_lan_ACCEPT",
            cont = "-p udp -d 224.0.0.0/4"                    
        }                           
    } 
}

local function set_igmp_fw_rule()
    local chain = {"zone_wan", "zone_wan_forward"}

    for _, c in ipairs(chain) do
        for _, r in ipairs(fw_tab[c]) do
            os.execute("fw add 4 f " .. c .. " " .. r.target .. " " .. "^" .. " { " .. r.cont .. " }")                    
        end
    end
    
    return true
end

local function del_igmp_fw_rule()
    local chain = {"zone_wan", "zone_wan_forward"}

    for _, c in ipairs(chain) do
        for _, r in ipairs(fw_tab[c]) do
            os.execute("fw del 4 f " .. c .. " " .. r.target .. " { " .. r.cont .. " }")       
        end
    end
end

local function get_and_set_firewall()
	local igmp_wan = false
	local multcast_wan =false 

	uci_r:foreach("firewall","rule",
			function(section)
				local name = uci_r:get("firewall",section[".name"],"name")
				if name == "ALLOW-IGMP-WAN" then
					igmp_wan=true
				end
				if name == "ALLOW-MULTICAST-WAN" then
					multcast_wan=true
				end
			end
			)

	if not igmp_wan then
		local rule_name =uci_r:add("firewall","rule")
		uci_r:set("firewall",rule_name,"name","ALLOW-IGMP-WAN" )
		uci_r:set("firewall",rule_name,"src","wan")
		uci_r:set("firewall",rule_name,"proto","igmp")
		uci_r:set("firewall",rule_name,"target","ACCEPT")
	end

	if not multcast_wan then
		local rule_name =uci_r:add("firewall","rule")
		uci_r:set("firewall",rule_name,"name","ALLOW-MULTICAST-WAN" )
		uci_r:set("firewall",rule_name,"src","wan")
		uci_r:set("firewall",rule_name,"proto","udp")
		uci_r:set("firewall",rule_name,"dest","lan")
		uci_r:set("firewall",rule_name,"dest_ip","224.0.0.0/4")
		uci_r:set("firewall",rule_name,"target","ACCEPT")
		uci_r:set("firewall",rule_name,"family","ipv4")
	end

	if igmp_wan and multcast_wan then
		return false
	end

	uci_r:commit("firewall")
	-- uci_r:apply("firewall")
	-- anyone is not found ,return true
	return true

end

function get_igmp_proxy()
	local igmp = uci_r:get("network", "lan", "igmp_snooping")
	--local ret  = "off"
 
	--if igmp == "1" then
		--ret ="on"
    --end
	
    return (igmp == "1") and "on" or "off"
end
		

function enable_igmp_proxy(igmp,lan_addr,netmask)
	local current_igmp = get_igmp_proxy()

	if current_igmp == "off" then
        local log = require("luci.model.log").Log(213)
        log(502)
		uci_r:set("network", "lan", "igmp_snooping", "1")
		uci_r:commit("network")

        set_igmp_fw_rule()
        sys.fork_exec(MCPROXY_CMD)
        --sys.fork_exec("%q restart &"%IGMP_SHELL)
	end

    return true
end


function disable_igmp_proxy()
    if get_igmp_proxy() == "on" then
        local log = require("luci.model.log").Log(213)
        log(503)
	    uci_r:set("network", "lan", "igmp_snooping", "0")
	    uci_r:commit("network")	

        del_igmp_fw_rule()
        sys.fork_exec(MCPROXY_CMD)
	    --sys.fork_exec("%q stop &"%IGMP_SHELL)
    end
    return true
end

