--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  debug.lua
Details :  Debuging utilities
Author  :  Ye Qianchuan <yeqianchuan@tp-link.net>
Author  :  Wen Kun <wenkun@tp-link.net>
Version :  1.0.0
Date    :  15 Jan, 2014
]]--

module("luci.tools.debug", package.seeall)

--- Debug file output method
-- @param   file    file to record debug info (optional)
-- @param   info    string to write into file 
-- @return  N/A 
function fprint(file, info)
	local file = file or "/tmp/tp_lua.log"
	local fp = io.open(file, "a+")
	
	local function print_table(info)
		if info and (type(info) == "table") then
		  for k, v in pairs(info) do
		  	if v and (type(v) == "table") then
		  		fp:write(k .. ":\n")
		  		print_table(v)
		  	elseif v then
		       		fp:write(k .. ":" .. tostring(v) .. "\n")
		     end
		  end
		 elseif info then
		 	fp:write(tostring(info) .. ":\n")
		end
	end
	
	print_table(info)	
	
	fp:flush()
	fp:close()

end

--- Debug console output method
-- @param   string to display on console
-- @return  N/A 
function print(...)
    local file = io.open("/dev/console", "w")
    for i = 1, select("#", ...) do
        local v = select(i, ...)

        file:write(tostring(v))
        file:write("\t")
    end
    file:write("\n")
    file:close()
end

-- For backward compatible.
printf = print

local function _print(self, ...)
    self.print(...)
end

getmetatable(_M).__call = _print

--- Recursively dumps a table to console, modified from luci.util.dumptable.
-- @param t	Table value to dump
-- @param maxdepth	Maximum depth
-- @return	Always nil
function dumptable(t, maxdepth, i, seen)
    i = i or 0
    seen = seen or setmetatable({}, {__mode="k"})

    for k,v in pairs(t) do
        print(string.rep("\t", i) .. tostring(k) .. "\t" .. tostring(v))
        if type(v) == "table" and (not maxdepth or i < maxdepth) then
            if not seen[v] then
                seen[v] = true
                dumptable(v, maxdepth, i+1, seen)
            else
                print(string.rep("\t", i) .. "*** RECURSION ***")
            end
        end
    end
end
