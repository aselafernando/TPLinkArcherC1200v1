--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  menu.lua
Details :  Controller for obtaining menu list
Author  :  Zhang zhongwei <zhangzhongwei@tp-link.net>
Version :  1.0.0
Date    :  17 Nov, 2014
]]--

module("luci.controller.admin.menu", package.seeall)

local dbg   = require "luci.tools.debug"
local cfgt  = require "luci.sys.config"
local uci_r = require("luci.model.uci").cursor()
local ctl   = require "luci.model.controller"

local MODEL = cfgt.getsysinfo("product_name") or "AC2500"

function read_menu(formval, prefix)
    local menu = {}
    local data = {
        model = string.lower(MODEL),
        page  = prefix,
        menu  = menu
    }

    local main_menus = uci_r:get_profile(prefix .. "_menu", "menu")
    for _, menu_name in pairs(main_menus or {}) do
        menu[#menu + 1] = {name = menu_name}

        local sub_menus = uci_r:get_profile(prefix .. "_" .. menu_name, "menu")
        if sub_menus and type(sub_menus) == "table" then
            local children = {}
            for _, child_name in pairs(sub_menus) do
                children[#children + 1] = {name = child_name}
            end
            menu[#menu].children = children
        end
    end

    return data
end

local dispatch_tbl = {
    basic_menu = {
        [".super"] = {cb = read_menu, args = "basic"}
    },
    advanced_menu = {
        [".super"] = {cb = read_menu, args = "advanced"}
    },
    mobile_menu = {
        [".super"] = {cb = read_menu, args = "mobile"}
    }
}

function dispatch(http_form)
    return ctl.dispatch(dispatch_tbl, http_form)
end

function _index()
    return ctl._index(dispatch)
end

function index()
    entry({"admin", "menu"}, call("_index")).leaf = true
end
