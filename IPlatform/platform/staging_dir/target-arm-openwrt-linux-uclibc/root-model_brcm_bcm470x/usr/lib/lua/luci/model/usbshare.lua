--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  usbshare.lua
Details :  
Author  :  Zhu Xianfeng<zhuxianfeng@tp-link.net>
Version :  1.0.0
Date    :  05Nov, 2014
]]--

module("luci.model.usbshare", package.seeall)

local sys     = require "luci.sys"
local util    = require "luci.util"
local uci     = require "luci.model.uci"
local nixio   = require "nixio"
local dbg     = require "luci.tools.debug"
local logm    = require "luci.model.log"

local CFG_PATH = "/etc/config/sharecfg"
local CFG_UCI  = "usbshare"

local SHARE_ID = 291
local LOGX_ID_SAMBA_START    = 250
local LOGX_ID_SAMBA_STOP     = 251
local LOGX_ID_PROFTPD_START  = 252
local LOGX_ID_PROFTPD_STOP   = 253
local LOGX_ID_MINIDLNA_START = 254
local LOGX_ID_MINIDLNA_STOP  = 255

-- Debug
-- @param N/A
-- @return N/A
local function debug(str)
    dbg.print(str)
end

-- Create class of usb device sharing config
-- devices = { {serial = ?, vendor = ?, model = ?, size = ?, volumns = {}},
--             {serial = ?, vendor = ?, model = ?, size = ?, volumns = {}} }
-- volumns = { {uuid = ?, label = ?, devname = ?, mntdir = ?, enable = ?, capacity = ?, used = ?, path_prefix = ?, folders = {}},
--             {uuid = ?, label = ?, devname = ?, mntdir = ?, enable = ?, capacity = ?, used = ?, path_prefix = ?, folders = {}} }
-- folders = { {index = ?, path = ?, sharename = ?, gnetwork = ?, read_users = ?, write_users = ?, sharetype = ?, enable = ?},
--             {index = ?, path = ?, sharename = ?, gnetwork = ?, read_users = ?, write_users = ?, sharetype = ?, enable = ?} }
CfgParser  = util.class()

local PATH_PREFIX = {"G:", "H:", "I:", "J:", "K:", "L:", "M:", "N:", "O:",
"P:", "Q:", "R:", "S:", "T:", "U:", "V:", "W:", "X:", "Y:", "Z:"}

-- Initialization for CfgParser class.
-- @param  N/A
-- @return N/A
function CfgParser:__init__(path)
    path = path or CFG_PATH
    -- Generate usbshare.cfg
    sys.fork_call("usbshare cfg -o " ..  path)

    local uci_r = uci:cursor()
    local volcnt = 1
    
    self.devices = {}
    self.volumns = {}
    self.folders = {}

    uci_r:foreach("sharecfg", "device", 
    function(section)
        local device = {
            serial   = section.serial,
            model    = section.model,
            vendor   = section.vendor,
            capacity = section.capacity,
        }

        self.devices[section.serial] = device
    end)

    uci_r:foreach("sharecfg", "volumn", 
    function(section)
        local device = self.devices[section.serial]
        local volumn = {
            serial   = section.serial,
            uuid     = section.uuid,
            label    = section.label,
            devname  = section.devname,
            mntdir   = section.mntdir,
            capacity = section.capacity,
            used     = section.used,
            enable   = section.enable,
        }
        
        volumn.device = device
        volumn.path_prefix = PATH_PREFIX[volcnt]
        volumn.label = volumn.label or ("volumn" .. volcnt)
        volcnt = volcnt + 1

        self.volumns[section.uuid] = volumn
        device.volumns = device.volumns or {}
        device.volumns[section.uuid] = volumn
    end)

    uci_r:foreach("sharecfg", "folder",
    function(section)
        local volumn = self.volumns[section.uuid]
        local folder = {
            index       = section.index,
            uuid        = section.uuid,
            sharename   = section.sharename,
            path        = section.path,
            read_users  = section.read_users,
            write_users = section.write_users, 
            sharetype   = section.sharetype,
            gnetwork    = section.gnetwork,
            enable      = section.enable,
        }

        folder.volumn = volumn
        self.folders[section.sharename] = folder
        volumn.folders = volumn.folders or {}
        volumn.folders[section.sharename] = folder
    end)
end

-- Return all the devices.
-- @param  N/A
-- @return table
function CfgParser:get_devices()
    return self.devices
end

-- Find volumn by uuid.
-- @param  N/A
-- @return table
function CfgParser:get_volumn(uuid)
    return self.volumns[uuid]
end

-- Return all the data.
-- @param  N/A
-- @return table
function CfgParser:get_volumns(serial)
    return self.devices[serial].volumns or {}
end

-- Return all the volumns
-- @param  N/A
-- @return table
function CfgParser:get_allvolumns()
    return self.volumns
end

-- Return all the sharing folders
-- @param  N/A
-- @return table
function CfgParser:get_allfolders()
    return self.folders
end

-- Return folder by index or sharename
-- @param  N/A
-- @return table
function CfgParser:get_folder(args)
    if args.index then
        for _, folder in pairs(self.folders) do
            if folder.index == args.index then
                return folder
            end
        end
    elseif args.sharename then
        return self.folders[args.sharename] or {}
    end

    return {}
end

-- Convert size in human readable format (e.g., 1KB 234MB 2GB)
-- @param  size The number to be converted.
-- @return number
function format_size(size)    
    size = format_size2(size)

    if size then
        return size[1] .. " " .. size[2]
    end

    return nil
end

-- Convert size in human readable format
-- @param  size The number to be converted
-- @return number
function format_size2(size)    
    if size == nil or type(tonumber(size)) ~= "number" then
        return nil
    end

    local number = tonumber(size)

    if number > 1099511627776 then
        number = number / 1099511627776
        number = math.ceil(number * 100) / 100
        size = {number, "TB"}
    elseif number > 1073741824 then
        number = number / 1073741824
        number = math.ceil(number * 100) / 100
        size = {number, "GB"}
    elseif number > 1048576 then
        number = number / 1048576
        number = math.ceil(number * 100) / 100
        size = {number, "MB"}
    elseif number > 1024 then
        number = number / 1024
        number = math.ceil(number * 100) / 100
        size = {number, "KB"}
    elseif number >= 0 then
        size = {number, "B"}
    else
        size = nil
    end

    return size
end

-- Update Linux/Samba user
-- @param N/A
-- @return N/A
function account_update(oldname, username, password)
    local cmd = "usbuser " .. username .. " '" .. password .. "'"
    debug(cmd)
    sys.call(cmd)
end

-- Stop Samba/FTP/DLNA Service
-- @param N/A
-- @return N/A
function stop(wait)
    local exec_cb = sys.fork_exec
    local log     = logm.Log(SHARE_ID)

    if wait ~= nil then
        exec_cb = sys.fork_call
    end

    exec_cb("/etc/init.d/samba stop")
    log(LOGX_ID_SAMBA_STOP)
    exec_cb("/etc/init.d/proftpd stop")
    log(LOGX_ID_PROFTPD_STOP)
    exec_cb("/etc/init.d/minidlna stop")
    log(LOGX_ID_MINIDLNA_STOP)
end

-- Restart or stop Samba/FTP/DLNA Service
-- @param N/A
-- @return N/A
function apply()
    local uci_r = uci:cursor()
    local log   = logm.Log(SHARE_ID)

    stop(true)
    -- Wait 500ms
    nixio.nanosleep(0, 500000000)

    if uci_r:get(CFG_UCI, "global", "samba") == "on" then
        sys.fork_exec("/etc/init.d/samba start")
        log(LOGX_ID_SAMBA_START)
    end

    if uci_r:get(CFG_UCI, "global", "ftp") == "on" then
        sys.fork_exec("/etc/init.d/proftpd start")
        log(LOGX_ID_PROFTPD_START)
    end

    sys.fork_exec("/etc/init.d/minidlna start")
    log(LOGX_ID_MINIDLNA_START)
end
