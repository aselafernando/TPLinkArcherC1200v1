--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  administration.lua
Details :  Change account, enable password recovery, local and remote management.
Author  :  Zhang Zhongwei <zhangzhongwei@tp-link.net>
Version :  1.0.0
Date    :  03 Apr, 2014
]]--
module("luci.controller.admin.administration", package.seeall)


local nixio  = require "nixio"
local uci    = require "luci.model.uci"
local dtypes = require "luci.tools.datatypes"
local ctypes = require "luci.model.checktypes"
local dbg    = require "luci.tools.debug"
local ctl    = require "luci.model.controller"
local json_m = require "luci.json"

local uci_r = uci.cursor()

local ADMINCFG = "administration"              -- Config file name.
local ACCMGNT  = "accountmgnt"                 -- Config file name.

local LOG_ACC = 280
local LOG_LCA = 281
local LOG_RMO = 282

-- For account management.
local ENABLE_AUTH       = 51
local DISABLE_AUTH      = 52
local SET_EMAIL         = 53
local UPDATE_SESSION    = 54
local RESTART_USB       = 55
local INVALID_VERCODE   = 56
local VERCODE_TIMEOUT   = 57
local VERCODE_NOT_GEN   = 58
local PWDREC_SEV_START  = 59
local PWDREC_SEV_STOP   = 60
local UPDATE_ACCOUNT    = 101
local RESET_ACCOUNT     = 102
local UPDATE_ACC_FAILED = 103
local SET_EMAIL_FAILED  = 104

-- For local management.
local INSERT_DEVICE    = 51
local UPDATE_DEVICE    = 52
local DELETE_DEVICE    = 53
local MODE_ALL         = 101
local MODE_PARTIAL     = 102
local INVALID_MAC_ADR  = 54
local NO_DEVICE_FOUND  = 152
local INSERT_FAILED    = 203
local UPDATE_FAILED    = 204
local DELETE_FAILED    = 205

-- For remote management.
local UPDATE_REMOTE    = 51
local UPDATE_REMOTE_P  = 52
local INVALID_PORT_ADR = 151

-- For all.
local SERVICE_START     = 504
local SERVICE_STOP      = 505
local REACH_MAX_RULES   = 701
local INVALID_HTTP_ARGS = 702


--- Run the command and print the command to the serial window.
-- @param cmd Command to be run.
-- @return #bool True if command was running successfully.
local function exec(cmd)
    dbg(cmd)
    return luci.sys.fork_call(cmd)
end

function compact_account_data(oacc, opwd, nacc, npwd, cpwd)
    return {
        old_acc = oacc,
        old_pwd = opwd,
        new_acc = nacc,
        new_pwd = npwd,
        cfm_pwd = cpwd
    }
end

--- Read the password recovery info but not the account and password.
-- @param formvalues Values from web request.
-- @return #table Data of password recovery info to be return.
function account_read(formvalues)
    local asycrypto = require("luci.model.asycrypto").Crypto("rsa")

    local pubkey = asycrypto.read_pubkey()
    local keys   = {pubkey.n, pubkey.e}

    return compact_account_data("", keys, "", keys, keys)
end

--- Change the session status.
-- @param olduser The old username.
-- @param username The new username.
-- @return N/A
function session_update(olduser, username)
    local log   = require("luci.model.log").Log(LOG_ACC)
    local sauth = require "luci.sauth"
    local sess  = require "luci.dispatcher".context.authsession
    local sdat  = sauth.read(sess)
    if sdat and sdat.user == olduser then
        log(UPDATE_SESSION)
        sauth.write(sess, {
            addr   = sdat.addr,
            remote = sdat.remote,
            user   = username,
            token  = sdat.token,
            secret = sdat.secret
        })
    end
end

--- Change the USB share status.
-- @param olduser The old username.
-- @param username The new username.
-- @param password The new password.
-- @return N/A
function usbshare_update(olduser, username, password)
    local log      = require("luci.model.log").Log(LOG_ACC)
    local usbshare = require "luci.model.usbshare"
    local enable   = uci_r:get("usbshare", "account", "use_login_user")

    if enable and enable == "on" then
        log(RESTART_USB)
        usbshare.account_update(olduser, username, password)
        uci_r:set("usbshare", "account", "username", username)
        uci_r:set("usbshare", "account", "password", password)
        uci_r:commit("usbshare")
        usbshare.apply()
    end
end

--- Update the account, password.
-- @param formvalues Values from web request.
-- @return #bool True if update success.
-- @return #bool, #string False if update failed and return error code.
function account_update(formvalues)
    local log       = require("luci.model.log").Log(LOG_ACC)
    local accmgnt   = require "luci.model.accountmgnt"
    local asycrypto = require("luci.model.asycrypto").Crypto("rsa")

    local acc_tbl = {
        old_acc = formvalues["old_acc"] or "",
        old_pwd = formvalues["old_pwd"] or "",
        new_acc = formvalues["new_acc"] or "",
        new_pwd = formvalues["new_pwd"] or "",
        cfm_pwd = formvalues["cfm_pwd"] or ""
    }

    local new_pwd = asycrypto.decrypt(acc_tbl.new_pwd) or ""
    local cfm_pwd = asycrypto.decrypt(acc_tbl.cfm_pwd) or ""

    if new_pwd == cfm_pwd and accmgnt.update(acc_tbl.new_acc, 
        acc_tbl.new_pwd, acc_tbl.old_acc, acc_tbl.old_pwd) 
    then
        log(UPDATE_ACCOUNT, acc_tbl.new_acc)  -- Update account (XXX) success

        -- Update the new user to the session, so that user don't need to login again.
        session_update(acc_tbl.old_acc, acc_tbl.new_acc)

        -- Do somthing for usbshare.
        usbshare_update(acc_tbl.old_acc, acc_tbl.new_acc, new_pwd)   
        return true
    end

    log(UPDATE_ACC_FAILED, acc_tbl.new_acc)   -- Update account (XXX) failed
    return false, "update account failed"
end

--- Set the account in first time using router.
-- @param formvalues Values from web request.
-- @return #bool True if update success.
-- @return #bool, #string False if update failed and return error code.
function account_set(formvalues)
    local log       = require("luci.model.log").Log(LOG_ACC)
    local accmgnt   = require "luci.model.accountmgnt"
    local asycrypto = require("luci.model.asycrypto").Crypto("rsa")

    local acc_tbl = {
        new_acc = formvalues["new_acc"] or "",
        new_pwd = formvalues["new_pwd"] or "",
        cfm_pwd = formvalues["cfm_pwd"] or ""
    }

    local new_pwd = asycrypto.decrypt(acc_tbl.new_pwd) or ""
    local cfm_pwd = asycrypto.decrypt(acc_tbl.cfm_pwd) or ""

    if new_pwd == cfm_pwd and accmgnt.set(acc_tbl.new_acc, acc_tbl.new_pwd) then
        -- Update the new user to the session, so that user don't need to login again.
        local sauth = require "luci.sauth"
        local sess  = require "luci.dispatcher".context.authsession
        local sdat  = sauth.read(sess)
        if sdat then
            sauth.write(sess, {
                addr   = sdat.addr,
                remote = sdat.remote,
                user   = acc_tbl.new_acc,
                token  = sdat.token,
                secret = sdat.secret
            })
        end

        local users   = accmgnt.get_name()
        if not users then
            old_acc = "admin"
        else
            old_acc = type(users) == "table" and users[1] or users
            old_acc = old_acc or "admin"
        end
        usbshare_update(old_acc, acc_tbl.new_acc, new_pwd)

        log(UPDATE_ACCOUNT, acc_tbl.new_acc)  -- Update account (XXX) success
        return true
    end

    log(UPDATE_ACC_FAILED)   -- Update account (XXX) failed
    return false, "Set account failed"
end

--- For TMP Server.
-- @param formvalues Values from web request.
-- @return #bool True if update success.
-- @return #bool, #string False if update failed and return error code.
function account_appset(formvalues)
    local log       = require("luci.model.log").Log(LOG_ACC)
    local accmgnt   = require "luci.model.accountmgnt"

    local new_acc = formvalues["new_acc"]
    local new_pwd = formvalues["new_pwd"]

    if accmgnt.set_no_encrypt(new_acc, new_pwd) then
        log(UPDATE_ACCOUNT, new_acc)  -- Update account (XXX) success
        return true
    end

    log(UPDATE_ACC_FAILED)   -- Update account (XXX) failed
    return false, "Set account failed"
end

--- For TMP Server.
-- @param formvalues Values from web request.
-- @return #bool True if update success.
-- @return #bool, #string False if update failed and return error code.
function account_appget(formvalues)
    local accmgnt   = require "luci.model.accountmgnt"

    local users  = accmgnt.get_name()
    local username = type(users) == "table" and users[1] or users
    username = username or "admin"

    local password = accmgnt.get_password(username) or "admin"

    return {
        username = username,
        password = password
    }
end

--- Read the password recovery info but not the account and password.
-- @param formvalues Values from web request.
-- @return #table Data of password recovery info to be return.
function recovery_read(formvalues)
    local asycrypto = require("luci.model.asycrypto").Crypto("rsa")

    local pubkey = asycrypto.read_pubkey()
    local keys   = {pubkey.n, pubkey.e}

    return {
        enable_rec  = uci_r:get(ADMINCFG, "account", "recovery"),
        enable_auth = uci_r:get(ADMINCFG, "account", "authentication"),
        from        = uci_r:get(ADMINCFG, "account", "from") or "",
        to          = uci_r:get(ADMINCFG, "account", "to") or "",
        smtp        = uci_r:get(ADMINCFG, "account", "smtp") or "",
        username    = uci_r:get(ADMINCFG, "account", "username") or "",
        password    = keys
    }
end

--- Check the validity of the recovery email info.
-- @param  email The info of recovery email to be check.
-- @return #bool Return true if the info is ok, or false.
function email_check(email)
    if email.enable_rec == "on" then
        if #email.from < 2 or #email.from > 32
            or #email.to < 2 or #email.to > 32
            or #email.smtp < 2 or #email.smtp > 32 
        then
            return false
        end

        if email.enable_auth == "on" then
            if #email.username < 2 or #email.username > 32
                or #email.password < 2 or #email.password > 32
                or email.username:find("[%s%c]") or email.password:find("[%s%c]")
            then
                return false
            end
        end
    end
    return true
end

--- Update the new recovery email info.
-- @param  email_info The info of the recovery email info.
-- @return #bool True if email info update successfully, or false.
function email_update(email_info)
    if email_info.enable_rec ~= "" then
        uci_r:set(ADMINCFG, "account", "recovery", email_info.enable_rec)
        uci_r:set(ADMINCFG, "account", "authentication", email_info.enable_auth)
        uci_r:set(ADMINCFG, "account", "from", email_info.from)
        uci_r:set(ADMINCFG, "account", "to", email_info.to)
        uci_r:set(ADMINCFG, "account", "smtp", email_info.smtp)
        uci_r:set(ADMINCFG, "account", "username", email_info.username)
        -- Due to the password may be not nil
        if email_info.enable_auth == "on" then
            uci_r:set(ADMINCFG, "account", "password", email_info.password)
        else
            uci_r:set(ADMINCFG, "account", "password", "")
        end
        uci_r:commit(ADMINCFG)
        return true
    end
    return false
end

--- Update the account, password and the password recovery info.
-- @param formvalues Values from web request.
-- @return #table Data of password recovery info to be return if update successfully.
-- @return #bool, #string False if update failed and return error code.
function recovery_update(formvalues)
    local log       = require("luci.model.log").Log(LOG_ACC)
    local asycrypto = require("luci.model.asycrypto").Crypto("rsa")

    local email_info = {
        enable_rec  = formvalues["enable_rec"] or "off",
        enable_auth = formvalues["enable_auth"] or "off",
        from        = formvalues["from"] or "",
        to          = formvalues["to"] or "",
        smtp        = formvalues["smtp"] or "",
        username    = formvalues["username"] or "",
        password    = formvalues["password"] or ""
    }

    email_info.password = asycrypto.decrypt(email_info.password) or ""

    if email_check(email_info) and email_update(email_info) then      
        if email_info.enable_rec == "on" then
            log(SET_EMAIL, email_info.from, email_info.to)
            log(PWDREC_SEV_START)  -- Password recovery service start

            if email_info.enable_auth == "on" then
                log(ENABLE_AUTH)  -- Email account authentication
            else
                log(DISABLE_AUTH)  -- No email account authentication
            end
        else
            log(PWDREC_SEV_STOP)  -- Password recovery service stop
        end
        
        return {
            enable_rec  = formvalues["enable_rec"] or "off",
            enable_auth = formvalues["enable_auth"] or "off",
            from        = formvalues["from"],
            to          = formvalues["to"],
            smtp        = formvalues["smtp"],
            username    = formvalues["username"]
        }
    end

    log(SET_EMAIL_FAILED)  -- Recovery email update failed
    return false, "email check failed"
end

--- Read the local management mode.
-- @param  formvalues Values from web request.
-- @return #table Data will be return.
function local_mgnt_read(formvalues)
    return {mode = uci_r:get(ADMINCFG, "local", "mode") or "all"}
end

--- Enable all the devices on LAN to access the web server
-- @param  uci_r
-- @return N/A
function local_enable_all()
    -- Disable white list rule on firewall.
    exec("fw unload_local_mgnt")

    -- Delete the entries from white list.
    uci_r:foreach(ADMINCFG, "device",
        function(section)
            if dtypes.macaddr(section.mac:gsub("%-", ":"))
                and section.enable == "on" 
            then
                exec("fw del_local_mgnt " .. section.mac)
            end
        end
    )
end

--- Enable some of the devices on LAN to access the web server.
-- @param N/A
-- @return N/A
function local_enable_partial()
    -- Enable the white list rule firewall.
    exec("fw load_local_mgnt")

    -- Add the entries into white list.
    uci_r:foreach(ADMINCFG, "device",
        function(section)
            if dtypes.macaddr(section.mac:gsub("%-", ":"))
                and section.enable == "on" 
            then
                exec("fw add_local_mgnt " .. section.mac)
            end
        end
    )
end

--- Get the MAC address of remote device which requesting for web server operation.
-- @param  N/A
-- @return #string Mac address.
-- @return #bool   False if get host mac address failed.
function get_host_mac()
    local arptable = luci.sys.net.arptable()
    local usr_ip   = luci.http.getenv("REMOTE_ADDR")
    for _, entry in pairs(arptable) do
        if entry["IP address"] == usr_ip then
            return string.upper(entry["HW address"]:gsub(":", "%-"))
        end
    end
    return false
end

--- Change the Local MAC Authentication mode.
-- If mode == "all", all the devices on the LAN can access the web server.
-- If mode == "partial", only the devices in the list can access the web server.
-- @param formvalues Values from web request.
-- @return #table Data to be return.
function local_mgnt_write(formvalues)
    local log   = require("luci.model.log").Log(LOG_LCA)
    local mode  = formvalues["mode"] or "all"
    local old_mode = uci_r:get(ADMINCFG, "local", "mode") or "all"

    -- If mode is not change, return the result directly.
    if mode ~= old_mode then
        if mode == "all" then
            uci_r:set(ADMINCFG, "local", "mode", mode)
            uci_r:commit(ADMINCFG)

            -- Enable all the devices on firewall.
            local_enable_all()
            
            log(MODE_ALL)  -- Mode change: enable all
        elseif mode == "partial" then
            local usr_mac    = get_host_mac()
            local new_device = {mac = usr_mac, description = "Your PC!", enable = "on"}

            local form_m = require("luci.tools.form").Form(uci_r)
            form_m:insert(ADMINCFG, "device", new_device, {"mac"})
            uci_r:set(ADMINCFG, "local", "mode", mode)
            uci_r:commit(ADMINCFG)

            -- TODO: Enable only some of the devices on firewall.
            local_enable_partial()
            
            log(MODE_PARTIAL)  -- Mode change: enable partial
        end
    end
    return {mode = mode}
end

--- Get the max limited number of the devices.
-- @param  N/A
-- @return #table Data of the max limited number of the devices.
function local_max_devs()
    return {max_rules = uci_r:get_profile("local_mgnt", "max_dev") or 32}
end

--- Read the info of all the devices that are allowed to access the web server.
-- @param formvalues Values from web request.
-- @return #table Data to be return.
function local_devs_read(formvalues)
    local usr_mac = get_host_mac()
    local data    = {}

    uci_r:foreach(ADMINCFG, "device",
        function(section)
            table.insert(data, {
                mac         = section.mac,
                description = section.description or "",
                enable      = section.enable,
                key         = section.mac:gsub("[^%w]", ""),
                host        = section.mac == usr_mac and true or false
            })
        end)

    return data
end

--- Insert a device info to the allow list.
-- @param formvalues Values from web request.
-- @return #table Data to be return.
-- @return #bool, #string False if update failed and return error code.
function local_devs_insert(formvalues)
    local log       = require("luci.model.log").Log(LOG_LCA)
    local max_rules = uci_r:get_profile("local_mgnt", "max_dev") or 32
    local new_str   = formvalues["new"]
    if not new_str then
        log(INVALID_HTTP_ARGS)  -- Invalid http value!
        return false, "Invalid http value!"
    end

    -- MAC address must be the form likes "11-22-33-44-55-66".
    local new_device = json_m.decode(new_str)
    if not new_device.mac
        or not dtypes.macaddr(new_device.mac:gsub("%-", ":"))
    then
        log(INVALID_MAC_ADR) -- Invalid MAC address
        return false, "Invalid MAC address!"
    end

    local form_m = require("luci.tools.form").Form(uci_r)
    local count  = form_m:count("administration", "device") or 0

    -- Device count
    if count < max_rules then

        -- MAC address must be capitalized.
        new_device.mac = string.upper(new_device.mac)

        -- If description is nil, it must be "" instead of nil.
        new_device.description = new_device.description or ""

        -- Length of description must be less than 32.
        new_device.description = new_device.description:sub(1, 32)

        -- Insert the entry to the config file.
        local ret = form_m:insert(ADMINCFG, "device", new_device, {"mac"})
        if not ret then
            log(INSERT_FAILED, new_device.mac)
            return false, "Insert failed!"
        end

        uci_r:commit(ADMINCFG)
        log(INSERT_DEVICE, new_device.mac)

        if new_device.enable == "on" then
            exec("fw add_local_mgnt " .. new_device.mac)
        end

        local data = new_device
        data.host  = false
        data.key   = new_device.mac:gsub("[^%w]", "")

        return data
    else
        log(REACH_MAX_RULES)  -- Reach max rules
        return false, "Device's number is out of range!"
    end
end

--- Update a device info from the allow list.
-- @param formvalues Values from web request.
-- @return #table Data to be return.
-- @return #bool, #string False if update failed and return error code.
function local_devs_update(formvalues)
    local log     = require("luci.model.log").Log(LOG_LCA)
    local new_str = formvalues["new"]
    local old_str = formvalues["old"]
    if not new_str or not old_str then
        log(INVALID_HTTP_ARGS)  -- Invalid http value!
        return false, "Invalid http value!"
    end

    -- MAC address must be the form likes "11-22-33-44-55-66".
    local new_device = json_m.decode(new_str)
    local old_device = json_m.decode(old_str)

    if not dtypes.macaddr(new_device.mac:gsub("%-", ":"))
        or not dtypes.macaddr(old_device.mac:gsub("%-", ":"))
        or (old_device.mac == get_host_mac()                -- The MAC address of the device which requesting
        and (new_device.mac ~= old_device.mac                 -- for this operation was not allowed to modify.
        or new_device.enable == "off"))
    then
        log(INVALID_MAC_ADR) -- Invalid MAC address
        return false, "Invalid MAC address!"
    end

    -- Format the data
    new_device.mac  = string.upper(new_device.mac)
    old_device.mac  = string.upper(old_device.mac)

    -- This value won't be saved.
    new_device.host = ""
    old_device.host = ""

    -- If description is nil, it must be "" instead of nil.
    new_device.description = new_device.description or ""

    -- Length of description must be less than 32.
    new_device.description = new_device.description:sub(1, 32)

    -- Update the entry from the config file.
    local form_m = require("luci.tools.form").Form(uci_r)
    local ret    = form_m:update(ADMINCFG, "device", old_device, new_device, {"mac"})
    if not ret then
        log(UPDATE_FAILED, new_device.mac)  -- Update failed
        return false, "Update failed!"
    end
    uci_r:commit(ADMINCFG)

    -- TODO: Update the firewall rule.
    exec("fw del_local_mgnt " .. old_device.mac)
    if new_device.enable == "on" then
        exec("fw add_local_mgnt " .. new_device.mac)
    end

    log(UPDATE_DEVICE, new_device.mac)  -- Update success

    local data = new_device
    data.host = false
    data.key  = new_device.mac:gsub("[^%w]", "")
    return data
end

--- Remove a device info from the allow list.
-- @param formvalues Values from web request.
-- @return #table Data to be return.
-- @return #bool, #string False if update failed and return error code.
function local_devs_remove(formvalues)
    local log       = require("luci.model.log").Log(LOG_LCA)
    local key_str   = formvalues["key"]
    local index_str = formvalues["index"]
    if not key_str or not index_str then
        log(INVALID_HTTP_ARGS)  -- Invalid http value!
        return false, "Invalid http value!"
    end

    local key_tab  = (type(key_str)=="table") and key_str or {key_str}
    local index_tab = (type(index_str)=="table") and index_str or {index_str}

    local usr_mac = get_host_mac()
    local usr_key = usr_mac:gsub("[^%w]", "")

    -- The MAC address of the device which requesting for this operation was not allowed to delete.
    local index, key
    for i, k in pairs(key_tab) do
        if k == usr_key then
            index = index_tab[i]
            key = key_tab[i]
            table.remove(index_tab, i)
            table.remove(key_tab, i)
            break
        end
    end

    -- Cache all the entries' MAC address.
    local macs  = {}
    uci_r:foreach(ADMINCFG, "device",
        function(section)
            macs[#macs + 1] = section["mac"]
        end
    )

    -- Delete the entries from the config file.
    local form_m = require("luci.tools.form").Form(uci_r)
    local ret    = form_m:delete(ADMINCFG, "device", key_tab, index_tab)
    if not ret then
        log(DELETE_FAILED)   -- Delete devices failed.
        return false, "Delete failed!"
    end
    uci_r:commit(ADMINCFG)

    -- Delete the firewall rule from white list.
    local success = false
    for _, entry in pairs(ret)  do
        if entry.success then
            exec("fw del_local_mgnt " .. macs[tonumber(entry.index) + 1])
            success = true
            log(DELETE_DEVICE, macs[tonumber(entry.index) + 1])   -- Delete devices success.
        end
    end

    if success then                   -- It means some of the entries had been deleted successfully.
        local data = ret
        if key and index then
            table.insert(data, {key = key, index = index, success = false})
        end
        return data
    else
        log(DELETE_FAILED)   -- Delete devices failed.
        return false, "Delete failed!!"
    end
end

--- Get the existing local device list from client management and wireless.
-- @param formvalues Values from web request.
-- @return #table Data to be return.
-- @return #bool, #string False if update failed and return error code.
function local_devs_view(formvalues)
    local log     = require("luci.model.log").Log(LOG_LCA)
    local climgmt = require "luci.model.client_mgmt"
    
    local devs = climgmt.get_client_list_dev()
    if not devs then
        log(NO_DEVICE_FOUND)  -- No device data
        return false, "No device data!"
    end

    local dev_list = {}
    for _, dev in pairs(devs) do
        dev_list[#dev_list + 1] = {
            mac  = dev.mac,
            ip   = dev.ip,
            name = dev.hostname or "UNKNOWN"
        }
    end

    return dev_list
end

--- Read the port and IP address that are opened to the remote devices to access the web server.
-- @param formvalues Values from web request.
-- @return #table DAta to be return.
function remote_mgnt_read(formvalues)
    local data = {
        enable = uci_r:get(ADMINCFG, "remote", "enable"),
        port   = uci_r:get(ADMINCFG, "remote", "port"),
        ipaddr = uci_r:get(ADMINCFG, "remote", "ipaddr")
    }
    return data
end

--- Delete a entry from firewall.
-- @param port
-- @param ip
-- @return #bool
function remote_del_entry(enable, port, ip)
    if port then
        if enable == "all" then
            exec("fw del_remote_mgnt port " .. port)
            exec("nat del http")
        elseif ip and enable == "partial" then
            exec("fw del_remote_mgnt port " .. port .. " ip " .. ip)
            exec("nat del http")
        end
        return true
    end
    return false
end

--- Add a entry from firewall.
-- @param port
-- @param ip
-- @return #bool
function remote_add_entry(enable, port, ip)
    if port then
        if enable == "all" then
            exec("fw add_remote_mgnt port " .. port)
            exec("nat add http { 255.255.255.255 " .. port .. " }")
        elseif ip and enable == "partial" then
            exec("fw add_remote_mgnt port " .. port .. " ip " .. ip)
            exec("nat add http { " .. ip .. " " .. port .. " }")
        end
        return true
    end
    return false
end

--- Change the port and IP address that are opened to the remote devices to access the web server.
-- @param formvalues Values from web request.
-- @return #table Data to return.
function remote_mgnt_write(formvalues)
    local log    = require("luci.model.log").Log(LOG_RMO)
    local enable = formvalues["enable"] or "off"
    local port   = formvalues["port"] or ""
    local ip     = formvalues["ipaddr"] or ""
    local old_enable = uci_r:get(ADMINCFG, "remote", "enable")
    local old_port   = uci_r:get(ADMINCFG, "remote", "port")
    local old_ip     = uci_r:get(ADMINCFG, "remote", "ipaddr")

    if enable ~= "off" then
        if enable == "partial" then
            if ip == "" or not dtypes.ipaddr(ip) or ctypes.check_ip_in_lan(ip)
                or not ctypes.check_unicast_ipv4(ip)
            then
                log(INVALID_PORT_ADR)   -- Invalid port or ip
                return false, "Invalid port!"
            end
        end

        port = tonumber(port) or ""
        if port == "" or (port < 1024 and port ~= 80) or port > 65535 then
            log(INVALID_PORT_ADR)   -- Invalid port or ip
            return false, "Invalid ip!"
        end
    end

    -- Save the new config to the file.
    uci_r:set(ADMINCFG, "remote", "enable", enable)
    if enable ~= "off" then
        uci_r:set(ADMINCFG, "remote", "port", port)
        if enable == "partial" then
            uci_r:set(ADMINCFG, "remote", "ipaddr", ip)
        end
    end
    uci_r:commit(ADMINCFG)
    
    -- Delete the old entry from firewall and nat.
    remote_del_entry(old_enable, old_port, old_ip)

    -- Add the new entry to firewall and nat. "255.255.255.255" for all and "0.0.0.0" for none.
    if enable ~= "off" then
        if enable == "all" then
            log(UPDATE_REMOTE_P, port)  -- Save success. port (XX)
        else
            log(UPDATE_REMOTE, ip, port)  -- Save success. IP (XX), port (XX)
        end
        log(SERVICE_START)            -- Service start
        remote_add_entry(enable, port, ip)        
    else
        log(SERVICE_STOP)            -- Service stop
    end

    return {
        enable = enable,
        port   = enable ~= "off" and port or old_port,
        ipaddr = enable == "partial" and ip or old_ip
    }
end

--- Get DUT IP address.
-- @param mode "LAN" or "WAN"
-- @return #string IP address.
function get_ip(mode)
    local network = require ("luci.model.network").init()
    local net     = network:get_network(mode)
    return net:ipaddr()
end

--- 检查IP是否与DUT的IP统一网段�?-- @param mode "LAN" or "WAN"
-- @param ip   IP address to be check.
-- @return #bool True or false.
function check_ip(mode, ip)
    local ip_check
    if mode == "lan" then
        ip_check = get_ip("lan")
    elseif mode == "wan" then
        ip_check = get_ip("wan")
    end

    local pos1 = 0
    local pos2 = 0
    for i = 1, 3 do
        pos1 = ip_check:find("%.", pos1 + 1)
        pos2 = ip:find("%.", pos2 + 1)
    end

    if ip:sub(1, pos2) == ip_check:sub(1, pos1) then 
        return true
    else 
        return false 
    end
end

--- Check if the email work successfully.
-- @param N/A
-- @return #bool True or false.
function recovery_testmail()
    local pwdrec = require "luci.model.passwd_recovery"

    local message = "Please do not reply to this email.\r\n\r\n"
        .. "This is a testing mail.\r\n\r\n"

    return pwdrec.vercode_send(message)
end

function login_read()
    local data = {
        preempt = uci_r:get(ADMINCFG, "login", "preempt")
    }
    return data
end

function login_write(http_form)
    local preempt = http_form["preempt"]
    if not ctypes.check_onoff(preempt) then
        return false, "invalid args"
    end

    if login_read().preempt ~= preempt then
        uci_r:set(ADMINCFG, "login", "preempt", preempt)
        uci_r:commit(ADMINCFG)
    end
    return {
        preempt = preempt
    }
end

--- Dispatch table
local dispatch_tbl = {
    ["account"] = {
        ["read"]   = {cb = account_read},
        ["write"]  = {cb = account_update},
        ["set"]    = {cb = account_set},
        ["appset"] = {cb = account_appset},
        ["appget"] = {cb = account_appget},
    },
    ["recovery"] = {
        ["read"]   = {cb = recovery_read},
        ["write"]  = {cb = recovery_update},
    },
    ["mode"] = {
        ["read"]   = {cb = local_mgnt_read},
        ["write"]  = {cb = local_mgnt_write},
    },
    ["local"] = {
        ["load"]   = {cb = local_devs_read,   others = local_max_devs},
        ["insert"] = {cb = local_devs_insert, others = local_max_devs},
        ["update"] = {cb = local_devs_update, others = local_max_devs},
        ["remove"] = {cb = local_devs_remove, others = local_max_devs},
    },
    ["view"] = {
        ["load"]   = {cb = local_devs_view},
    },
    ["remote"] = {
        ["read"]   = {cb = remote_mgnt_read},
        ["write"]  = {cb = remote_mgnt_write},
    },
    ["testmail"] = {
        [".super"] = {cb = recovery_testmail},
    },
    ["login"] = {
        ["read"] = {cb = login_read},
        ["write"] = {cb = login_write},
    },
}

function dispatch(http_form)
    return ctl.dispatch(dispatch_tbl, http_form)
end

function _index()
    return ctl._index(dispatch)
end

--- Module entrance
function index()
    entry({"admin", "administration"}, call("_index")).leaf = true
end
