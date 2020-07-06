#!/usr/bin/env lua

local uci = require "luci.model.uci"

local function dlogin_config_set(cfg, sec, opt, val)
    local uci_c = uci.cursor()
    
    uci_c:set(cfg, sec, opt, val)
    uci_c:commit(cfg)
end

if arg[1] and arg[2] and arg[3] then
    dlogin_config_set(arg[1], arg[2], arg[3], arg[4])
end

