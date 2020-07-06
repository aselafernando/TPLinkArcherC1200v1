--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  passwd_recovery.lua
Details :  Reset the account and password by sending email.
Author  :  Zhang Zhongwei <zhangzhongwei@tp-link.net>
Version :  1.0.0
Date    :  03 Apr, 2014
]]--

module("luci.model.passwd_recovery", package.seeall)

local dbg      = require "luci.tools.debug"
local uci_r    = require("luci.model.uci").cursor()

local TMP_FILE = "/tmp/vercode"
local ADMINCFG = "administration"
local TIMEOUT  = 600  -- senconds

local MODEL    = uci_r:get("locale", "sysinfo", "model") or "ArcherC9"


--- Read the information about whether using email recovery.
-- @param N/A
-- @return      Table
function recovery_read()
    local enable_rec = uci_r:get(ADMINCFG, "account", "recovery") or "off"
    local email      = uci_r:get(ADMINCFG, "account", "to")

    if enable_rec == "on" then
        if not email or not email:find("@") then 
            return false 
        end

        local pos = email:find("@")
        local postfix = email:sub(pos)
        local prefix
       
        pos = pos > 0 and pos - 1 or 0
        if pos < 3 then
            prefix = email:sub(1, pos)
        else
            prefix = email:sub(1, 3)
        end
    
        return {
            enable_rec = true,
            email      = prefix .. "**" .. postfix
        }
    else
        return { enable_rec = false}
    end
end

--- Get the verification code by email, and save the code in tmp file.
-- @param N/A
-- @return      The result.
function vercode_get()
    local vercode = nil
    local file    = io.open(TMP_FILE, "r")

    if file then
        local nixio = require "nixio"
        local mtime = nixio.fs.stat(TMP_FILE, "mtime")
        local ctime = os.time()
        local ttime = ctime - mtime

        if ttime < TIMEOUT then
            vercode = file:read("*all")
        end
        file:close()
    end

    if not vercode then

        -- Generate and save the verification code.
        math.randomseed(os.time())
        vercode = math.random(100000, 999999)
        file    = io.open(TMP_FILE, "w")
        if not file then 
            return false, "Create verification code failed!"
        end

        file:write(vercode)
        file:close()
    end

    local message = "Please do not reply to this email.\r\n\r\n"
        .. "Your verification code is " .. vercode .. ".\r\n\r\n"

    return vercode_send(message)
end

--- Send email.
-- @param  vercode Verification code to be sent.
-- @return      The result.
function vercode_send(message)
    local email = require "luci.model.email"

    local enable_rec  = uci_r:get(ADMINCFG, "account", "recovery")
    local enable_auth = uci_r:get(ADMINCFG, "account", "authentication")
    if enable_rec ~= "on" then 
        return false, "Password recovery is disable!"
    end

    -- Get the email information and send the message.
    local username = uci_r:get(ADMINCFG, "account", "username")
    local password = uci_r:get(ADMINCFG, "account", "password")
    local server   = uci_r:get(ADMINCFG, "account", "smtp")
    local from     = uci_r:get(ADMINCFG, "account", "from")
    local to       = uci_r:get(ADMINCFG, "account", "to")

    local head = {
        from    = MODEL .. from,
        to      = to,
        subject = MODEL .. ": Password Recovery!"
    }

    local source = {
        headers = head,
        body = message
    }

    local ret, err = email.send({
        server   = server,
        user     = enable_auth == "on" and username or nil,
        password = enable_auth == "on" and password or nil,
        from     = from,
        rcpt     = to,
    }, source)

    return ret, err
end

--- Check the verification code. The verification code won't be changed in 10 minutes.
-- @param code      Verification code to be check.
-- @return          The result.
function vercode_check(code)
    if not code then
        return false, "Parameter is null!"
    end
    
    local log   = require("luci.model.log").Log(280)
    local nixio = require "nixio"
    local mtime = nixio.fs.stat(TMP_FILE, "mtime")
    
    if not mtime then 
        return false, "Verification code is time out!"
    end
    
    local ctime = os.time()
    local ttime = ctime - mtime

    if ttime > TIMEOUT then
        log(152)   -- Verification code is time out!
        return false, "Verification code is time out!"
    end

    local file = io.open(TMP_FILE, "r")
    if not file then
        log(153)   -- Verification code is not generated yet!
        return false 
    end

    local vercode = file:read("*all") or ""
    file:close()

    if code ~= "" and vercode ~= ""
        and tonumber(code) == tonumber(vercode) then

        -- Remove the tmp file.
        luci.sys.fork_call("rm " .. TMP_FILE)

        -- Reset the default username and password (admin/admin)
        local accmgnt = require "luci.model.accountmgnt"
        accmgnt.reset()
        log(103)  -- Account was reset to 'admin'/'admin'
        
        -- Reset the usb share account.
        usbshare_update("null", "admin", "admin")
        
        -- Remove the luci sessions.
        luci.sys.fork_call("rm -rf /tmp/luci*")
        return true
    end

    log(151)  -- Incorrect verification code
    return false, "Incorrect verification code"
end

--- Change the USB share status.
-- @param
-- @return
function usbshare_update(olduser, username, password)
    local usbshare = require "luci.model.usbshare"

    local enable = uci_r:get("usbshare", "account", "use_login_user")    
    if enable and enable == "on" then
        usbshare.account_update(olduser, username, password)
        uci_r:set("usbshare", "account", "username", username)
        uci_r:set("usbshare", "account", "password", password)
        uci_r:commit("usbshare")
        usbshare.apply()
    end
end

