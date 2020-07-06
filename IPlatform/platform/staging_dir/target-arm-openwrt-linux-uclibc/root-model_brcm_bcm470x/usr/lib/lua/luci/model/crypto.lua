--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  crypto.lua
Details :  Crypto library. Depends on openssl-util.
Author  :  Ye Qianchuan <yeqianchuan@tp-link.net>
Version :  1.0.0
Date    :  24Apr, 2014
]]--

local sys = require "luci.sys"
local nixio = require "nixio"
local util = require "luci.util"

module("luci.model.crypto", package.seeall)

local cipher = "aes-256-cbc"

local enc_cmd = "openssl zlib -e %s | openssl " .. cipher .. " -e %s"
local dec_cmd = "openssl " .. cipher .. " -d %s %s | openssl zlib -d"
local enc_cmd_nozlib = "openssl " .. cipher .. " -e %s %s"
local dec_cmd_nozlib = "openssl " .. cipher .. " -d %s %s"
local file_opt = "-in %q"
local pwd_opt = "-k %q"
local pwdfile_opt = "-kfile /etc/secretkey"

local function build_cmd(file, pwd, nozlib, enc)
    local cmd
    if enc then
        cmd = nozlib and enc_cmd_nozlib or enc_cmd
    else
        cmd = nozlib and dec_cmd_nozlib or dec_cmd
    end
    return cmd % {file and (file_opt % file) or "",
                  pwd and (pwd_opt % pwd) or pwdfile_opt}
end

--- The iterator function returned by crypto function.
-- This function is exactly nixio.File:read without self argument.
-- See @nixio.File.read. Note that this is a case of pipes.
-- This function is as a valid LTN12 source, so, for example, call
-- luci.ltn12.pump.all(@this_function, luci.http.write) will dump all
-- the result to http.
-- @class function
-- @name crypto_result_iterator
-- @param length  (optional) amount of data to read (in Bytes). Default to 2048.
-- @return string with the characters read, or nil if it cannot read anymore.

--- Encrypt file
-- @param file    file name
-- @param pwd     (optional) password. If none, use password from /etc/secretkey.
-- @param nozlib  (optional) Don't compress if true.
-- @return        nil on error, or an iterator function. See @crypto_result_iterator above.
function enc_file(file, pwd, nozlib)
    if type(file) ~= "string" or #file == 0 then
        return nil
    end
    local cmd = build_cmd(file, pwd, nozlib, true)
    return sys.ltn12_popen(cmd)
end

--- Decrypt file
-- @param file    file name
-- @param pwd     (optional) password. If none, use password from /etc/secretkey.
-- @param nozlib  (optional) Don't compress if true.
-- @return        nil on error, or an iterator function. See @crypto_result_iterator above.
function dec_file(file, pwd, nozlib)
    if type(file) ~= "string" or #file == 0 then
        return nil
    end
    local cmd = build_cmd(file, pwd, nozlib, false)
    return sys.ltn12_popen(cmd)
end

--- Encrypt string
-- @param buf     string to be encrypted.
-- @param pwd     (optional) password. If none, use password from /etc/secretkey.
-- @param nozlib  (optional) Don't compress if true.
-- @return        nil on error, or an iterator function. See @crypto_result_iterator above.
function enc(buf, pwd, nozlib)
    if type(buf) ~= "string" then
        return nil
    end
    local cmd = build_cmd(nil, pwd, nozlib, true)
    return sys.ltn12_popen(cmd, buf)
end

--- Decrypt string
-- @param buf     string to be decrypted.
-- @param pwd     (optional) password. If none, use password from /etc/secretkey.
-- @param nozlib  (optional) Don't compress if true.
-- @return        nil on error, or an iterator function. See @crypto_result_iterator above.
function dec(buf, pwd, nozlib)
    if type(buf) ~= "string" then
        return nil
    end
    local cmd = build_cmd(nil, pwd, nozlib, false)
    return sys.ltn12_popen(cmd, buf)
end

--- Dump LTN12 source to file
-- @param src     LTN12 source
-- @param fname   file name
function dump_to_file(src, fname)
    local file = io.open(fname, "w")
    if not file then return end
    local buf = src()
    while buf do
        file:write(buf)
        buf = src()
    end
    file:close()
end
