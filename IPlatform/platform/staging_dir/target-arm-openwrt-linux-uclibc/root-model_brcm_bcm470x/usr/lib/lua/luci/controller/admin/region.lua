--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  region.lua
Details :  Controller for obtaining regions
Author  :  Ye Qianchuan <yeqianchuan@tp-link.net>
Version :  1.0.0
Date    :  24 Jul, 2014
]]--

module("luci.controller.admin.region", package.seeall)

local uci = require "luci.model.uci"
local dbg = require "luci.tools.debug"
local ctl = require "luci.model.controller"

local uci_r = uci.cursor()

function regions()
    return function(region_list, i)
        i = i + 1
        local region = region_list[i]
        if region then
            local country_code, country_name, no_autodetect
                = region:match("^([^:]*):([^:]*):([^:]*)$")
            no_autodetect = no_autodetect == "y" and true or nil
            return i, country_code, country_name, no_autodetect
        end
    end, uci_r:get_profile("region", "country"), 0
end

function read_regions()
    local data = {}

    for _, country_code, country_name, no_autodetect in regions() do
        data[#data + 1] = {
            value = country_code,
            name = country_name,
            no_autodetect = no_autodetect
        }
    end

    return data
end

local dispatch_tbl = {
    region = {
        [".super"] = {cb = read_regions}
    }
}

function dispatch(http_form)
    return ctl.dispatch(dispatch_tbl, http_form)
end

function _index()
    return ctl._index(dispatch)
end

function index()
    entry({"admin", "region"}, call("_index")).leaf = true
end
