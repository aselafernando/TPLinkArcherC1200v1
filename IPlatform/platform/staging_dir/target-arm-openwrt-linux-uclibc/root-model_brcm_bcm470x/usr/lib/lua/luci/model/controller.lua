--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  controller.lua
Details :  General controller subroutines.
Author  :  Ye Qianchuan <yeqianchuan@tp-link.net>
Version :  1.0.0
Date    :  08 Mar, 2014
]]--

module("luci.model.controller", package.seeall)

local util = require "luci.util"
local http = require "luci.http"

-- An example of controller forms
--[[
local dispatch_tbl = {
    formX = {
        [.args] = arg
        ["opX"] = {cb = cbX, others = cbY},
        ["opX"] = {cb = cbX, args = argX},
        ...
    },
    formX = {
        [".super"] = {cb = cbS},
        ...
    },
    ...
}
--]]

local function write_json(json)
    http.prepare_content("application/json")
    http.write_json(json)
end

function _index(dsp, hook)
    if hook then
        local ret, errorcode = hook(http)
        if not ret then
            write_json({
                    success = false,
                    errorcode = errorcode
            })
        end
    end

    local form = http.formvalue()
    local ret = dsp(form)
    if ret then
        write_json(ret)
    end
end

function dispatch(dispatch_tbl, http_form, hook)
    local forms = http_form and http_form["form"]
    local op = http_form and http_form["operation"] or ""

    local success = true
    local own_response
    local errorcode_tbl = {}
    local data, others, errorcode

    local function _action_cb(cb, args)
        local ret, errorcode, error_tbl = cb(unpack(args))
        local data
        if ret then
            data = ret
        else
            success = false
            if errorcode then
                errorcode_tbl[#errorcode_tbl + 1] = errorcode
            end
            if error_tbl then
                data = error_tbl
            end
        end
        return ret, type(data) == "table" and data or nil
    end

    local function _dispatch(form)
        local op_tbl = dispatch_tbl[form]
        local action
        if op_tbl then
            action = op_tbl[op] or op_tbl[".super"]
        end

        if action and action.cb then
            local args = op_tbl[".args"]
                and {http_form, op_tbl[".args"], action.args}
                or {http_form, action.args}
            local ret, data = _action_cb(action.cb, args)

            local others
            if action.others then
                _, others = _action_cb(action.others, args)
            end

            if hook and hook.post_hook then
                _action_cb(hook.post_hook, {ret, action})
            end

            own_response = own_response or action.own_response

            return data, others, action
        else
            success = false
            errorcode_tbl[#errorcode_tbl + 1] = "no such callback"
        end
    end

    local function _merge(tbl, updates, action, form)
        if #updates == 0 then
            -- @updates is a key-value table, or empty array
            local prefix = action and action.merge_prefix
            if prefix then
                util.update_prefix(tbl, updates, prefix)
            else
                util.update(tbl, updates)
            end
        else
            -- @updates is a non-empty array
            util.update(tbl, {[form] = updates})
        end
    end

    if type(forms) ~= "table" then
        -- Single form
        local form = forms
        data, others = _dispatch(form)
    else
        -- Multiple forms
        for _, form in ipairs(forms) do
            local data_ret, others_ret, action = _dispatch(form)
            if data_ret then
                data = data or {}
                _merge(data, data_ret, action, form)
            end

            if others_ret then
                others = others or {}
                _merge(others, others_ret, action, form)
            end
        end
    end

    if #errorcode_tbl > 0 then
        errorcode = table.concat(errorcode_tbl, "&")
    end

    if own_response and success then
        return nil
    else
        return {
            success = success,
            data = data,
            others = others,
            errorcode = errorcode
        }
    end
end

-- An example of check table
--[[
local check_tbl = {
    {
        field = "field", canbe_empty = true, canbe_absent = false,
        check = {
            check_cb1,
            {check_cb2, arg1, arg2, ...},
            ...
        }
    },
    {
        field = "field2", canbe_empty = cb, canbe_absent = cb,
        check = {
            ...
        }
    },
    {
        field = {"field1", "field2", ...}, canbe_empty = {cb, arg1, ...},
        check = {
            ...
        }
    },
    {
        check = {
            check_cb_extra
        }
    }
}
--]]

local function _check_one(obj, cb)
    if type(cb) == "function" then
        cb = {cb}
    end

    local args = cb
    cb = cb[1]
    if cb(obj, unpack(args, 2)) then
        return true
    else
        return false
    end
end

local function _check(obj, check_tbl)
    for _, cb in ipairs(check_tbl) do
        if not _check_one(obj, cb) then
            return false
        end
    end

    return true
end

function check_canbe_empty(obj, canbe_empty)
    if type(canbe_empty) == "function"
        or type(canbe_empty) == "table"
    then
        return _check_one(obj, canbe_empty)
    else
        return canbe_empty
    end
end

function check_canbe_absent(obj, canbe_absent)
    if type(canbe_absent) == "function"
        or type(canbe_absent) == "table"
    then
        return _check_one(obj, canbe_absent)
    elseif canbe_absent == nil then
        return true
    else
        return canbe_absent
    end
end

function check(obj, tbl)
    if not tbl then
        return true
    end

    for _, action in ipairs(tbl) do
        if action.field then
            local fields = type(action.field) == "table" and action.field or {action.field}
            for _, field in ipairs(fields) do
                if obj[field] ~= nil then
                    if type(obj[field]) ~= "string"
                        or #obj[field] > 0
                    then
                        if not _check(obj[field], action.check) then
                            return false
                        end
                    elseif not check_canbe_empty(obj, action.canbe_empty)
                    then
                        return false
                    end
                elseif not check_canbe_absent(obj, action.canbe_absent)
                then
                    return false
                end
            end
        else
            if not _check(obj, action.check) then
                return false
            end
        end
    end

    return true
end
