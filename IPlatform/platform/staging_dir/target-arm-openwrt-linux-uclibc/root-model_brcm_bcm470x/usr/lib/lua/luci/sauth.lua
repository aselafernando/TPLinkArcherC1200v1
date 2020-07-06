--[[

Session authentication
(c) 2008 Steven Barth <steven@midlink.org>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

$Id: sauth.lua 8900 2012-08-08 09:48:47Z jow $

]]--

--- LuCI session library.
module("luci.sauth", package.seeall)
require("luci.util")
require("luci.sys")
require("luci.config")
local nixio = require "nixio", require "nixio.util"
local fs = require "nixio.fs"
local dbg = require "luci.tools.debug"


luci.config.sauth = luci.config.sauth or {}
sessionpath = luci.config.sauth.sessionpath
sessiontime = tonumber(luci.config.sauth.sessiontime) or 15 * 60
maxclients = tonumber(luci.config.sauth.maxclients) or 1
sessionlock = "/var/run/luci-session.lock"

--- Prepare session storage by creating the session directory.
function prepare()
    fs.mkdir(sessionpath, 700)
    if not sane() then
        error("Security Exception: Session path is not sane!")
    end
end

local _lock

local function lock(w)
    _lock = nixio.open(sessionlock, "w", 600)
    _lock:flock(w and "ex" or "sh")
end

local function unlock()
    _lock:close()
    _lock = nil
end

local function _read(id)
    lock()
    local blob = fs.readfile(sessionpath .. "/" .. id)
    unlock()
    return blob
end

local function _write(id, data)
    lock(true)
    local f = nixio.open(sessionpath .. "/" .. id, "w", 600)
    f:writeall(data)
    f:close()
    unlock()
end

local function _checkid(id)
    return not not (id and #id == 32 and id:match("^[a-fA-F0-9]+$"))
end

--- Write session data to a session file.
-- @param id    Session identifier
-- @param data    Session data table
function write(id, data)
    if not sane() then
        prepare()
    end

    assert(_checkid(id), "Security Exception: Session ID is invalid!")
    assert(type(data) == "table", "Security Exception: Session data invalid!")

    data.atime = luci.sys.uptime()

    _write(id, luci.util.get_bytecode(data))
end

--- Read a session and return its content.
-- @param id    Session identifier
-- @return        Session data table or nil if the given id is not found
function read(id)
    if not id or #id == 0 then
        return nil
    end

    assert(_checkid(id), "Security Exception: Session ID is invalid!")

    if not sane(sessionpath .. "/" .. id) then
        return nil
    end

    local blob = _read(id)
    local func = loadstring(blob)
    setfenv(func, {})

    local sess = func()
    assert(type(sess) == "table", "Session data invalid!")

    if sess.atime and sess.atime + sessiontime < luci.sys.uptime() then
        kill(id)
        return nil
    end

    return sess
end

--- Refresh atime in session
-- @param id	session identifier
-- @param sess	session data table
function touch(id, sess)
    write(id, sess)
end

--- Check whether Session environment is sane.
-- @return Boolean status
function sane(file)
    local mode = fs.stat(file or sessionpath, "modestr")
    return luci.sys.process.info("uid")
            == fs.stat(file or sessionpath, "uid")
        and (mode == (file and "rw-------" or "rwx------")
        or mode == (file and "rw-------" or "rwx--S---"))
end

--- Kills a session
-- @param id    Session identifier
function kill(id)
    assert(_checkid(id), "Security Exception: Session ID is invalid!")
    fs.unlink(sessionpath .. "/" .. id)
end

--- Remove all expired session data files
function reap()
    if sane() then
        local id
        for id in nixio.fs.dir(sessionpath) do
            if _checkid(id) then
                -- reading the session will kill it if it is expired
                read(id)
            end
        end
    end
end

--- Check login limitation.
function limit(addr, remote, login_name)
    if sane() then
        local sid
        local sdat, return_sdat
        local clients = 0
        for id in nixio.fs.dir(sessionpath) do
            while true do
                if _checkid(id) then
                    sid = id
                    sdat = read(id)
                    if sdat then
                        if login_name ~= nil then
                            if login_name == sdat.user then
                            else
                                -- continue
                                break
                            end
                        end
                        return_sdat = sdat

                        clients = clients + 1
                        if sdat.remote == remote then
                            if sdat.addr == addr then
                                return true, sid, sdat
                            end
                        end
                    end
                end

                break
            end
        end

        if clients < maxclients then
            return true
        else
            return false, sid, return_sdat
        end
    else
        return true
    end
end
