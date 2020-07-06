--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  usbshare.lua
Details :  
Author  :  Zhu Xianfeng<zhuxianfeng@tp-link.net>
Version :  1.0.0
Date    :  08May, 2014
]]--

local sys      = require "luci.sys"
local muci     = require "luci.model.uci"
local mdebug   = require "luci.tools.debug"
local usbshare = require "luci.model.usbshare"
local ctl      = require "luci.model.controller"
local printsvr = require "luci.controller.admin.printsvr"
local UCICFG   = "usbshare"

module("luci.controller.admin.usbshare", package.seeall)

-- Get disk status
-- @param N/A
-- @return table
function disk_status()
    local data = {
        storage_vendor = "",
        storage_capacity = 0, storage_capacity_unit = "GB", 
        storage_available = 0, storage_available_unit = "GB",
    }
    local parser = usbshare.CfgParser()
    local disks = parser:get_devices()
    local volumns = parser:get_allvolumns()
    local size = 0

    for _, disk in pairs(disks) do 
        size = size + disk.capacity
        data.storage_vendor = data.storage_vendor .. disk.vendor .. " "
    end
    size = usbshare.format_size2(size)
    if size then
        data.storage_capacity = size[1]
        data.storage_capacity_unit = size[2]
    end

    size = 0
    for _, volumn in pairs(volumns) do 
        size = size + (volumn.capacity - volumn.used)
    end
    size = usbshare.format_size2(size)
    if size then
        data.storage_available = size[1]
        data.storage_available_unit = size[2]
    end

	local storages = {} 
    local parser    = usbshare.CfgParser()
    local devices   = parser:get_devices()
    local storage   = nil
    local capacity  = nil
    local available = nil

    -- TODO
    -- USB storage unknown
    -- data.storage_vendor = "unknown"

    for _, device in pairs(devices) do
        capacity = device.capacity
        capacity = usbshare.format_size2(capacity)

        available = 0
        for _, volumn in pairs(device.volumns) do
            available = available + (volumn.capacity - volumn.used)
        end
        available = usbshare.format_size2(available)

        storage = {
            vendor         = device.vendor,
            capacity       = capacity[1],
            capacity_unit  = capacity[2],
            available      = available[1],
            available_unit = available[2],
        }

        table.insert(storages, storage)
    end
	data.usb_storages = storages
    return data
end

-- Get printer status
-- @param N/A
-- @return table
function printer_status()
    local uci = muci.cursor()
    data = {}

    -- TODO
    -- data["printer_enable"] = uci:get(UCICFG, "global", "printer")
    data["printer_name"] = printsvr.printinfo()
    data["printer_count"] = printsvr.printcnt()

    return data
end

-- General controller routines

local usbshare_forms = {
    disk_status = {
        [".super"] = {cb = disk_status}
    },
    printer_status = {
        [".super"] = {cb = printer_status}
    }
}

function index()
    entry({"admin", "usbshare"}, call("usbshare_index")).leaf = true
end

function usbshare_index()
    ctl._index(usbshare_dispatch)
end

function usbshare_dispatch(http_form)
    return ctl.dispatch(usbshare_forms, http_form)
end