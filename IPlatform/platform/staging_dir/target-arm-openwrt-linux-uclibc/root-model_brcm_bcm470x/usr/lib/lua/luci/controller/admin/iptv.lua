--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  iptv.lua
Details :  Controller for IPTV webpage
Author  :  Guo Dongxian <guodongxian@tp-link.net>
Version :  1.0.0
Date    :  12 Aug, 2014
]]--

module("luci.controller.admin.iptv", package.seeall)

local uci    = require "luci.model.uci"
local dtypes = require "luci.tools.datatypes"
local dbg    = require "luci.tools.debug"
local sys    = require "luci.sys"
local ctl    = require "luci.model.controller"  

local RELOAD_IPTV = "/etc/init.d/iptv restart"
local cfg_changed = false

local function uci_get()
    local uci_r = uci.cursor()

    local function _get_iptv(u, opt)
        return u:get("iptv", "iptv", opt)    
    end

    local data = {
        enable = _get_iptv(uci_r, "enable"),
        mode   = _get_iptv(uci_r, "mode"),
        internet_tag   = _get_iptv(uci_r, "internet_tag"),
        internet_vid   = _get_iptv(uci_r, "internet_vid"),
        internet_vprio = _get_iptv(uci_r, "internet_vprio"),
        ipphone_vid    = _get_iptv(uci_r, "ipphone_vid"),
        ipphone_vprio  = _get_iptv(uci_r, "ipphone_vprio"),
        iptv_vid   = _get_iptv(uci_r, "iptv_vid"),
        iptv_vprio = _get_iptv(uci_r, "iptv_vprio"),
        mciptv_enable = _get_iptv(uci_r, "mciptv_enable"),
        mciptv_vid    = _get_iptv(uci_r, "mciptv_vid"),
        mciptv_vprio  = _get_iptv(uci_r, "mciptv_vprio"),
        lan1 = _get_iptv(uci_r, "lan1"),
        lan2 = _get_iptv(uci_r, "lan2"),
        lan3 = _get_iptv(uci_r, "lan3"),
        lan4 = _get_iptv(uci_r, "lan4"),
        igmp_enable = _get_iptv(uci_r, "igmp_enable"),
        igmp_version = _get_iptv(uci_r, "igmp_version"),
        wait_time = _get_iptv(uci_r, "handle_time"),
        cfg_changed = cfg_changed
    }

    return data
end

function set_network_attr()
    local iptv = uci_get()
    local uci_r = uci.cursor()

    local wtype = uci_r:get("network", "wan", "wan_type")
    if iptv.enable == "on" then
        if iptv.mode == "Bridge" then
            if iptv.lan1 == "IPTV" or iptv.lan2 == "IPTV"
                or iptv.lan3 == "IPTV" or iptv.lan4 == "IPTV"
            then
                uci_r:set("network", "wan", "type", "bridge")
                uci_r:set("network", "wan", "igmp_snooping", "1")
                
                if wtype == "pppoe" then
                    uci_r:set("network", "internet", "ifname", "br-wan")                
                end
                uci_r:commit("network")
            end
        else
            local l3_ifname = uci_r:get("iptv", "iptv", "wan")
	    local i = string.find(l3_ifname, '%.')
            local l3_dev = nil
	    if i ~= nil then
		   l3_dev = string.sub(l3_ifname, 0, i - 1)
	    else
		    l3_dev = l3_ifname
	    end
            local ifname = l3_dev .. "." .. iptv.internet_vid
            uci_r:set("network", "wan", "ifname", ifname)
            if wtype == "pppoe" then
                uci_r:set("network", "internet", "ifname", ifname)                
            end
            uci_r:commit("network")
        end        
    end
end

local function check(data)
    if not data or type(data) ~= "table" then
        return false
    end
    if data.mode ~= "Bridge" and data.mode ~= "Custom"
        and data.mode ~= "Russia" and data.mode ~= "Maxis"       
            and data.mode ~= "ExStream" and data.mode ~= "Unifi"
    then
        return false
    end

    if data.igmp_enable ~= "on" and data.igmp_enable ~= "off"
    then
	    return false
    end

    if tonumber(data.igmp_version) < 2 or tonumber(data.igmp_version) > 3 then
        return false
    end

    local function _vid(vid)
        local uvid = tonumber(vid)
        if dtypes.integer(vid) and (uvid >= 0 and uvid <= 4094)
        then
            return true        
        end   
    end

    local function _prio(prio)
        local uprio = tonumber(prio)
        if dtypes.integer(prio) and (uprio >= 0 and uprio <= 7)
        then            
            return true        
        end    
    end

    local function _port_type(port, flag)
        local ptype = {}
        if flag then
            ptype = {"Internet", "IPTV", "IP-Phone"}
        else
            ptype = {"Internet", "IPTV"}
        end
        
        for _, k in ipairs(ptype) do
            if port == k then
                return true            
            end
        end
        return false
    end
    
    if not _vid(data.internet_vid) or not _prio(data.internet_vprio)
        or not _vid(data.iptv_vid) or not _prio(data.iptv_vprio)
            or not _vid(data.ipphone_vid) or not _prio(data.ipphone_vprio)
                or not _vid(data.mciptv_vid) or not _prio(data.mciptv_vprio)
    then
        return false    
    end

    local flag 
    if data.mode == "Custom" or data.mode == "Russia" or data.mode == "Maxis"
    then
        flag = true
    else
        flag = false
    end

    if not _port_type(data.lan1, flag) or not _port_type(data.lan2, flag)
        or not _port_type(data.lan3, flag) or not _port_type(data.lan4, flag)
    then
        return false
    end

    return true
end

local function parse_url(formvalue)
    local iptv_old = uci_get()
    local iptv_new = {
	-- igmp proxy
	igmp_enable = formvalue["igmp_enable"] or iptv_old.igmp_enable,
	igmp_version = formvalue["igmp_version"] or iptv_old.igmp_version,
        enable = formvalue["enable"] or iptv_old.enable,
        mode   = formvalue["mode"] or iptv_old.mode,
        -- internet
        internet_tag   = formvalue["internet_tag"] or iptv_old.internet_tag,
        internet_vid   = formvalue["internet_vid"] or iptv_old.internet_vid,
        internet_vprio = formvalue["internet_vprio"] or iptv_old.internet_vprio,
        -- ip-phone
        ipphone_vid    = formvalue["ipphone_vid"] or iptv_old.ipphone_vid,
        ipphone_vprio  = formvalue["ipphone_vprio"] or iptv_old.ipphone_vprio,
        -- iptv
        iptv_vid   = formvalue["iptv_vid"] or iptv_old.iptv_vid,
        iptv_vprio = formvalue["iptv_vprio"] or iptv_old.iptv_vprio,
        -- multicast iptv
        mciptv_enable = formvalue["mciptv_enable"] or iptv_old.mciptv_enable,
        mciptv_vid    = formvalue["mciptv_vid"] or iptv_old.mciptv_vid,
        mciptv_vprio  = formvalue["mciptv_vprio"] or iptv_old.mciptv_vprio,
        -- port type information
        lan1 = formvalue["lan1"] or iptv_old.lan1,
        lan2 = formvalue["lan2"] or iptv_old.lan2,
        lan3 = formvalue["lan3"] or iptv_old.lan3,
        lan4 = formvalue["lan4"] or iptv_old.lan4,

        -- igmp = formvalue["igmp"] or iptv_old.igmp
    }
    if iptv_new.igmp_enable == iptv_old.igmp_enable
        and iptv_new.igmp_version == iptv_old.igmp_version
    then
        if iptv_new.enable == iptv_old.enable 
            and iptv_new.mode == iptv_old.mode
                -- and iptv_new.igmp == iptv_old.igmp
        then
            if iptv_new.lan1 == iptv_old.lan1
                and iptv_new.lan2 == iptv_old.lan2
                    and iptv_new.lan3 == iptv_old.lan3
                        and iptv_new.lan4 == iptv_old.lan4
            then
                if iptv_new.mode == "Bridge" then
                    return false, true            
                elseif iptv_new.internet_tag == iptv_old.internet_tag
                        and iptv_new.internet_vid == iptv_old.internet_vid
                            and iptv_new.internet_vprio == iptv_old.internet_vprio
                                and iptv_new.ipphone_vid == iptv_old.ipphone_vid
                                    and iptv_new.ipphone_vprio == iptv_old.ipphone_vprio
                                        and iptv_new.mciptv_enable == iptv_old.mciptv_enable
                                            and iptv_new.mciptv_vid == iptv_old.mciptv_vid
                                                and iptv_new.mciptv_vprio == iptv_old.mciptv_vprio
                                                    and iptv_new.iptv_vid == iptv_old.iptv_vid
                                                        and iptv_new.iptv_vprio == iptv_old.iptv_vprio
                then
                    return false, true
                end
            end
        end
    end
    
    if iptv_new.mode == "ExStream" then
        iptv_new.internet_tag   = formvalue["internet_tag"] or "on"
        iptv_new.internet_vid   = formvalue["internet_vid"] or "10"
        iptv_new.internet_vprio = formvalue["internet_vprio"] or "0"

        iptv_new.iptv_vid   = formvalue["iptv_vid"] or "20"
        iptv_new.iptv_vprio = formvalue["iptv_vprio"] or "4"
        
        iptv_new.lan1 = formvalue["lan1"] or "Internet"
        iptv_new.lan2 = formvalue["lan2"] or "Internet"
        iptv_new.lan3 = formvalue["lan3"] or "Internet"
        iptv_new.lan4 = formvalue["lan4"] or "IPTV" 
        iptv_new.mciptv_enable = formvalue["mciptv_enable"] or "off"
    elseif iptv_new.mode == "Unifi" then
        iptv_new.internet_tag   = formvalue["internet_tag"] or "on"
        iptv_new.internet_vid   = formvalue["internet_vid"] or "500"
        iptv_new.internet_vprio = formvalue["internet_vprio"] or "0"

        iptv_new.iptv_vid   = formvalue["iptv_vid"] or "600"
        iptv_new.iptv_vprio = formvalue["iptv_vprio"] or "0"

        iptv_new.lan1 = formvalue["lan1"] or "IPTV"
        iptv_new.lan2 = formvalue["lan2"] or "Internet"
        iptv_new.lan3 = formvalue["lan3"] or "Internet"
        iptv_new.lan4 = formvalue["lan4"] or "Internet"
        iptv_new.mciptv_enable = formvalue["mciptv_enable"] or "off"
    elseif iptv_new.mode == "Maxis" then
        iptv_new.internet_tag   = formvalue["internet_tag"] or "on"
        iptv_new.internet_vid   = formvalue["internet_vid"] or "621"
        iptv_new.internet_vprio = formvalue["internet_vprio"] or "0"

        iptv_new.iptv_vid   = formvalue["iptv_vid"] or "823"
        iptv_new.iptv_vprio = formvalue["iptv_vprio"] or "0"

        iptv_new.ipphone_vid = formvalue["ipphone_vid"] or "821"
        iptv_new.ipphone_vprio = formvalue["ipphone_vprio"] or "0"
        -- Multicast IPTV Disabled
        iptv_new.mciptv_enable = formvalue["mciptv_enable"] or "off"

        iptv_new.lan1 = formvalue["lan1"] or "IPTV"
        iptv_new.lan2 = formvalue["lan2"] or "Internet"
        iptv_new.lan3 = formvalue["lan3"] or "Internet"
        iptv_new.lan4 = formvalue["lan4"] or "IP-Phone" 
    elseif iptv_new.mode == "Russia" then
        iptv_new.internet_tag   = formvalue["internet_tag"] or "on"
        iptv_new.internet_vid   = formvalue["internet_vid"] or "1257"
        iptv_new.internet_vprio = formvalue["internet_vprio"] or "0"

        iptv_new.iptv_vid   = formvalue["iptv_vid"] or "4000"
        iptv_new.iptv_vprio = formvalue["iptv_vprio"] or "4"

        iptv_new.ipphone_vid = formvalue["ipphone_vid"] or "263"
        iptv_new.ipphone_vprio = formvalue["ipphone_vprio"] or "0"
        iptv_new.mciptv_enable = formvalue["mciptv_enable"] or "off"

        iptv_new.lan1 = formvalue["lan1"] or "IP-Phone"
        iptv_new.lan2 = formvalue["lan2"] or "Internet"
        iptv_new.lan3 = formvalue["lan3"] or "Internet"
        iptv_new.lan4 = formvalue["lan4"] or "IPTV" 
    end


    if not check(iptv_new) then
        return false, false
    end

    return iptv_new
end

local function get()
    return uci_get()
end

local function set(formvalue)
    local save, err = parse_url(formvalue)
    
    if not save and not err then
        return false, "Invalid URL"
    end

    if save then
        local uci_r = uci.cursor()
        local log = require("luci.model.log").Log(216)
        if save.enable == "on" then
            log(502)
        else
            log(503)
        end
        cfg_changed = true

        -- Set igmp proxy
        uci_r:set("iptv", "iptv", "igmp_enable", save.igmp_enable)
        uci_r:set("iptv", "iptv", "igmp_version", save.igmp_version)
        uci_r:set("iptv", "iptv", "enable", save.enable)
        uci_r:set("iptv", "iptv", "mode",   save.mode)
        uci_r:set("iptv", "iptv", "internet_tag",   save.internet_tag)
        uci_r:set("iptv", "iptv", "internet_vid",   save.internet_vid)
        uci_r:set("iptv", "iptv", "internet_vprio", save.internet_vprio)
        uci_r:set("iptv", "iptv", "ipphone_vid",    save.ipphone_vid)
        uci_r:set("iptv", "iptv", "ipphone_vprio",  save.ipphone_vprio)
        uci_r:set("iptv", "iptv", "iptv_vid",   save.iptv_vid)
        uci_r:set("iptv", "iptv", "iptv_vprio", save.iptv_vprio)
        uci_r:set("iptv", "iptv", "mciptv_enable", save.mciptv_enable)
        uci_r:set("iptv", "iptv", "mciptv_vid",    save.mciptv_vid)
        uci_r:set("iptv", "iptv", "mciptv_vprio",  save.mciptv_vprio)
        uci_r:set("iptv", "iptv", "lan1", save.lan1)
        uci_r:set("iptv", "iptv", "lan2", save.lan2)
        uci_r:set("iptv", "iptv", "lan3", save.lan3)
        uci_r:set("iptv", "iptv", "lan4", save.lan4)

        uci_r:commit("iptv")
        
        sys.fork_exec(RELOAD_IPTV)
    end
    
    return uci_get()
end

local dispatch_tbl = {
    ["setting"] = {
        ["read"]  = { cb = get },
        ["write"] = { cb = set }
    }
}

function dispatch(http_form)
    return ctl.dispatch(dispatch_tbl, http_form)
end

function _index()
    return ctl._index(dispatch)
end

function index()
    entry({"admin", "iptv"}, call("_index")).leaf = true
end

