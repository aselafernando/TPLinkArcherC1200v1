--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  syslog.lua
Details :  Controller for system log webpage
Author  :  Ye Qianchuan <yeqianchuan@tp-link.net>
Version :  1.0.0
Date    :  22 May, 2014
]]--

module("luci.controller.admin.syslog", package.seeall)

local sys = require "luci.sys"
local uci = require "luci.model.uci"
local ctypes = require "luci.model.checktypes"
local ctl = require "luci.model.controller"

local uci_r = uci.cursor()

local AUTOMAIL_RELOAD = "/etc/init.d/logmail-auto restart"
local EC_ARGS = "invalid args"
local PRIO_TBL = {
    [0] = "EMERGENCY",
    [1] = "ALERT",
    [2] = "CRITICAL",
    [3] = "ERROR",
    [4] = "WARNING",
    [5] = "NOTICE",
    [6] = "INFO",
    [7] = "DEBUG"
}
local PRIO_MAP = {
    ["EMERGENCY"] = 0,
    ["ALERT"]     = 1,
    ["CRITICAL"]  = 2,
    ["ERROR"]     = 3,
    ["WARNING"]   = 4,
    ["NOTICE"]    = 5,
    ["INFO"]      = 6,
    ["DEBUG"]     = 7
}

function read_log(http_form)
    local logread = require "logread"
    local proj
    local prio
    local filter = uci_r:get_all("syslog", "filter")
    if filter.type ~= "ALL" then
        proj = filter.type
    end
    if filter.level ~= "ALL" then
        prio = PRIO_MAP[filter.level]
    end
    local lang = uci_r:get("locale", "sysinfo", "locale")

    local data = {}
    local log = logread.open()
    filter = {
        proj = proj,
        prio = prio,
        lang = lang
    }
    log:read(filter, function(rec)
                 data[#data + 1] = {
                     time = rec.date,
                     type = rec.name:upper():gsub("-", "_"),
                     level = PRIO_TBL[rec.prio],
                     content = string.format("[%u] %s",
                                             rec.pid,
                                             rec.msg)
                 }
    end)
    log:close()

    return data
end

function read_filter(http_form)
    local filter = uci_r:get_all("syslog", "filter")
    return {
        type = filter.type,
        level = filter.level
    }
end

function write_filter(http_form)
    local filter = {
        type = http_form.type,
        level = http_form.level
    }

    -- Check filter.
    local types = uci_r:get_profile("syslog", "module")
    if filter.type ~= "ALL" and not ctypes.check_in(filter.type, types) then
        return false, EC_ARGS
    end

    if filter.level ~= "ALL" and not PRIO_MAP[filter.level] then
        return false, EC_ARGS
    end

    uci_r:section("syslog", "filter", "filter", filter)
    uci_r:commit("syslog")

    return true
end

function remove_all(http_form)
    sys.fork_call("logreset")
    return true
end

function read_types(http_form)
    local types = uci_r:get_profile("syslog", "module")
    local data = {
        {name = "ALL", value = "ALL"}
    }

    for i, m in ipairs(types) do
        data[i + 1] = {
            value = m,
            name = m:upper():gsub("-", "_")
        }
    end
    return data
end

function save_log(http_form)
    local http = require "luci.http"
    local lang = uci_r:get("locale", "sysinfo", "locale")

    http.header('Content-Disposition',
                'attachment; filename="syslog-%s.log"' %
                    os.date("%Y-%m-%d"))
    http.prepare_content("text/plain")
    luci.ltn12.pump.all(sys.ltn12_popen("logread -w -l %s" % lang),
                        http.write)
    return true
end

function mail_log(http_form)
    local ret = sys.fork_call("logmail")
    return {
        send = ret == 0 and true or false
    }   
end

local mail_keys = {
    "from", "to", "smtp_server", "auth", "user", "password",
    "auto_mail", "auto_mail_type", "every_day_time", "every_hours"
}

function get_mail(http_form)
    local config = uci_r:get_all("syslog", "mail")
    local data = {}
    for _, key in ipairs(mail_keys) do
        data[key] = config[key]
    end
    return data
end

local function auth_empty(form)
    return form.auth ~= "on"
end

local function automail_type_empty(form, t)
    return form.auto_mail_type ~= t
end

local CHECK_MAIL_TBL = {
    {
        field = {"from", "to"}, canbe_absent = false,
        check = {
            ctypes.check_email,
            {ctypes.check_rangelen, 6, 128}
        }
    },
    {
        field = "smtp_server", canbe_absent = false,
        check = {
            ctypes.check_host
        }
    },
    {
        field = {"auth", "auto_mail"},
        check = {
            ctypes.check_onoff
        }
    },
    {
        field = "user",
        canbe_absent = auth_empty,
        canbe_empty = true,
        check = {
            ctypes.check_visible,
            {ctypes.check_rangelen, 0, 96}
        }
    },
    {
        field = "password",
        canbe_absent = auth_empty,
        canbe_empty = true,
        check = {
            ctypes.check_visible,
            {ctypes.check_rangelen, 0, 32}
        }
    },
    {
        field = "auto_mail_type",
        check = {
            {ctypes.check_in, {"day_time", "hours"}}
        }
    },
    {
        field = "every_day_time",
        canbe_absent = {automail_type_empty, "day_time"},
        check = {
            ctypes.check_day_time
        }
    },
    {
        field = "every_hours",
        canbe_absent = {automail_type_empty, "hours"},
        check = {
            {ctypes.check_range, 1, 24}
        }
    }
}

function set_mail(http_form)
    if not ctl.check(http_form, CHECK_MAIL_TBL) then
        return false, EC_ARGS
    end

    local config = {}
    for _, key in ipairs(mail_keys) do
        config[key] = http_form[key]
    end

    uci_r:section("syslog", "mail", "mail", config)
    uci_r:commit("syslog")

    sys.fork_exec(AUTOMAIL_RELOAD)

    return true
end

-- General controller routines

local dispatch_tbl = {
    types = {
        [".super"] = {cb = read_types}
    },
    filter = {
        ["read"] = {cb = read_filter},
        ["write"] = {cb = write_filter}
    },
    log = {
        ["load"] = {cb = read_log},
        ["delete"] = {cb = delete_log},
        ["mail"] = {cb = mail_log},
        ["remove"] = {cb = remove_all},
        ["save"] = {cb = save_log, own_response = true}
    },
    mail = {
        ["read"] = {cb = get_mail},
        ["write"] = {cb = set_mail}
    }
}

function dispatch(http_form)
    return ctl.dispatch(dispatch_tbl, http_form)
end

function _index()
    return ctl._index(dispatch)
end

function index()
    entry({"admin", "syslog"}, call("_index")).leaf = true
end
