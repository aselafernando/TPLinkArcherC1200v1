--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  login.lua
Details :  Controller for login webpage. No authentication for running this script, so be cautious
Author  :  Ye Qianchuan <yeqianchuan@tp-link.net>
Version :  1.0.0
Date    :  28 Apr, 2014
]]--

module("luci.controller.login", package.seeall)

local ctl = require "luci.model.controller"
local nixio = require "nixio"
local fs = require "nixio.fs"
local sys = require "luci.sys"
local util = require "luci.util"
local pwdrec = require "luci.model.passwd_recovery"
local dbg = require "luci.tools.debug"
local _lock
local ATTEMPTS_LOCKFILE = "/var/run/luci-attempts.lock"
local ATTEMPTS_FILE = "/tmp/luci-attempts"
local ATTEMPTS_INTERVAL = 2 * 60 * 60
local ATTEMPTS_MAX = 10

local function lock(w)
    _lock = nixio.open(ATTEMPTS_LOCKFILE, "w", 600)
    _lock:flock(w and "ex" or "sh")
end

local function unlock()
    _lock:close()
    _lock = nil
end

local function read_attempts()
    if not fs.access(ATTEMPTS_FILE, "r") then
        return {}
    end

    lock()
    local blob = fs.readfile(ATTEMPTS_FILE)
    unlock()

    local func = loadstring(blob)
    setfenv(func, {})
    local attempts = func()
    assert(type(attempts) == "table")
    return attempts
end

local function write_attempts(attempts)
    lock(true)
    local f = nixio.open(ATTEMPTS_FILE, "w", 600)
    f:writeall(util.get_bytecode(attempts))
    f:close()
    unlock()
end

local function reap_attempts(attempts)
    for k, v in pairs(attempts) do
        if v.ltime + ATTEMPTS_INTERVAL < sys.uptime() then
            attempts[k] = nil
        end
    end
end

function login(http_form)
    local data = {}

    local sauth = require "luci.sauth"
    local sys = require "luci.sys"
    local ctypes = require "luci.model.checktypes"

    local user = http_form["username"]
    local pass = http_form["password"]
    local ipaddr = sys.getenv("REMOTE_ADDR")

    local macaddr
    local remote = not ctypes.check_ip_in_lan(ipaddr)

    local cm = require "luci.model.client_mgmt"
    if not remote then
        macaddr = cm.get_mac_by_ip(ipaddr)
        assert(macaddr, "lan mac is nil!")
    end
    local addr = remote and ipaddr or macaddr

    local allow, sid, sdat = sauth.limit(addr, remote)
    if not allow then
        data.logined_user = sdat.user
        data.logined_remote = sdat.remote
        if sdat.remote then
            data.logined_ip = sdat.addr
        else
            data.logined_mac = sdat.addr
            local client = cm.get_client_by("mac", data.logined_mac)
            if client then
                data.logined_ip = client.ip
                data.logined_host = client.hostname
            end
        end

        local uci_r = require("luci.model.uci").cursor()
        local preempt = uci_r:get("administration", "login", "preempt")
        if preempt == "off" then
            return false, "user conflict", data
        end
    end

    local attempts = read_attempts()
    reap_attempts(attempts)
    local att = attempts[addr] or {attempts = 0}
    if att.attempts >= ATTEMPTS_MAX then
		data = {}
		data.failureCount = att.attempts
		data.attemptsAllowed = ATTEMPTS_MAX - att.attempts
        return false, "exceeded max attempts", data
    end

    local acc_check = require("luci.model.accountmgnt").check
    if acc_check(user, pass) then
        attempts[addr] = nil
        write_attempts(attempts)
    else
        att.attempts = att.attempts + 1
        att.ltime = sys.uptime()
        attempts[addr] = att
        write_attempts(attempts)
		data = {}
		data.failureCount = att.attempts
		data.attemptsAllowed = ATTEMPTS_MAX - att.attempts
        return false, "login failed", data
    end

    local token = sys.uniqueid(16)
    if sid then
        sauth.kill(sid)
    end

    sid = sys.uniqueid(16)
    sauth.write(sid, {
                    addr = addr,
                    remote = remote,
                    user = user,
                    token = token,
                    secret = sys.uniqueid(16)
    })

    local dl = require "luci.controller.domain_login"
    dl.tips_cancel()

    local http = require "luci.http"
    http.header("Set-Cookie", "sysauth=" .. sid
                    .. ";path=" .. sys.getenv("SCRIPT_NAME") or "")
    data.stok = token
    return data
end

function read_keys(http_form)
    local asycrypto = require("luci.model.asycrypto").Crypto("rsa")
    local pubkey = asycrypto.read_pubkey()
    local keys   = { pubkey.n, pubkey.e } 
    return {
        username = "",
        password = keys
    }
end

function read_recovery(http_form)
    return pwdrec.recovery_read()
end

function read_vercode(http_form)
    return pwdrec.vercode_get()
end

function check_vercode(http_form)
    return pwdrec.vercode_check(http_form.vercode)
end

-- General controller routines

local dispatch_tbl = {
    login = {
        ["login"] = {cb = login},
        ["read"] = {cb = read_keys}
    },
    password = {
        ["read"] = {cb = read_recovery}
    },
    vercode = {
        ["read"] = {cb = read_vercode},
        ["write"] = {cb = check_vercode}
    }
}

function dispatch(http_form)
    return ctl.dispatch(dispatch_tbl, http_form)
end

function _index()
    return ctl._index(dispatch)
end

function index()
    entry({"login"}, call("_index")).leaf = true
end
