--[[
LuCI - config Module

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

]]--

local luci = {}
luci.sys   = require "luci.sys"
luci.util   = require "luci.util"
local fs = require "luci.fs"
local uci = require "luci.model.uci"
local accountmgnt = require "luci.model.accountmgnt"

--local debug = require "luci.tools.debug"
local debug = {}
function debug.printf(s)
	local file = io.open("/dev/console", "w")
	file:write(tostring(s))
	file:write("\n")
	file:close()
end

local type, os, string, io, tostring, require, pairs, ipairs, table = type, os, string, io, tostring, require, pairs, ipairs, table

module("luci.sys.config")

local function getfilenames( path, files )
	files = files or {}
	local filetable = fs.dir(path)
	--debug.printf(filetable)
	for _, entry in ipairs(filetable) do
		if entry ~= '.' and entry ~= '..' then
			local cpath = path .. '\\' .. entry
		        --debug.printf(entry .. ":" .. tostring(value))
			if fs.isdirectory(cpath) then
				getfilenames(cpath,files)
			else
				table.insert(files, entry)
			end
		end
	end
	return files
end

local function get_mtd( part_name )
    local string  = require "string"

    local file    = io.input("/proc/mtd")
    while true do
        local lines = file:read("*line")
        if not lines then return nil end
        if string.match(lines, '\"' .. part_name .. '\"') then
            return string.match(lines, "mtd%d+")
        end
    end
end

local src = {"&", "<", ">"}
local dst = {"amp", "lt", "gt"}

function toEscaped(line)
        if line == nil then
                return ""
        end

        local ret = line
        for index , key in ipairs(src) do
                ret = string.gsub(ret, "(".. key .. ")", "&" .. dst[index])
        end

        return ret
end

function toOrig(line)

        local sorg = line
        local sdst = ""
        local pos
        pos = string.find(sorg,"&")
        --print("pos:" .. tostring(pos))
        while pos and pos > 0 do
                -- head
                sdst = sdst .. string.sub(sorg, 1, pos - 1)
                --print("dst:" .. sdst)
                sorg = string.sub(sorg, pos + 1 )
                --print("org:" .. sorg)
                -- escaped fit
                for index , key in ipairs(dst) do
                        xx,yy = string.find(sorg, "^" .. key)
                        if xx then
                                sorg = string.sub(sorg, yy + 1)
                                sdst = sdst .. src[index]
                                --print("org::".. sorg)
                                --print("dst::".. sdst)
                                break
                        end
                end

                pos = string.find(sorg, "&")
                --print("pos:" .. tostring(pos))
        end
        sdst = sdst .. sorg
        return sdst
end

function fileToXml(xmlpath)
	local files = getfilenames("/etc/config")
	local uci_r = uci.cursor()

	if xmlpath == nil then
		debug.printf("error xmlpath is needed")
		return
	end

	local fp    = io.open(xmlpath, "w+")

	if fp == nil then
		debug.printf("error open file failed:".. xmlpath)
		return
	end

	fp:write('<?xml version="1.0" encoding="utf-8"?>\n')
	fp:write('<config>\n')
	for _, file in pairs(files) do
		--debug.printf(file)
		local cfgs_org = uci_r:get_all(file)
		if cfgs_org then
			fp:write("<" .. toEscaped(file) .. ">\n")
			
			local cfgs = {}
			-- sort by index
			for _ , scfg in pairs(cfgs_org) do
				cfgs[scfg['.index'] + 1] = scfg
			end

			for _ , option in ipairs(cfgs) do
				--debug.printf("--" .. cfg)
				if option['.anonymous'] then
					fp:write("<" .. toEscaped(option['.type']) .. ">\n")
				else
					fp:write('<' .. toEscaped(option['.type']) .. ' name="' .. toEscaped(option['.name']) .. '">\n')
				end

				for key, value in pairs(option) do
					--debug.printf("----".. tostring(key) .. " " .. tostring(value))
					if type(value) == 'table' then
						fp:write("<list>\n")
						--get a list
						for _, rule in ipairs(value) do
							--debug.printf("------" ..rule)
							fp:write('<' .. toEscaped(key) ..'>'.. toEscaped(rule) .. '</' .. toEscaped(key) .. '>\n')
						end
						fp:write("</list>\n")
					elseif tostring(key):byte() ~= 46 then
						fp:write("<" .. toEscaped(key) .. ">" .. toEscaped(value) .. "</" .. toEscaped(key) .. ">\n")
					end
				end
				fp:write("</" .. toEscaped(option['.type']) .. ">\n")
			end
			fp:write("</" .. toEscaped(file) .. ">\n")
		else
			debug.printf("incorrect format cfg : "..tostring(file))
            fp:write("<".. toEscaped(file) ..">\n</" .. toEscaped(file) ..">\n")
		end
	end
	fp:write('</config>')
	fp:close()
end

local function getxmlkey(line)
	local keys = {'empty', 'single', 'end', 'new'}
	local exps = { '<(.+)/>', '<(.-)>(.-)</(.+)>', '</(.+)>', '<(.+)>'}
	
	line =string.gsub(line, "(')","'\\''")
	for key, exp in ipairs(exps) do
		local data = string.match(line, exps[key])
		if data then
			if keys[key] == 'single' then
				--check single entry'name
				local single1 = string.match(line, '<(.-)>.-</.+>')
				local single2 = string.match(line, '<.->.-</(.+)>')
				if single1 == single2 then
					return {['key'] = toOrig(keys[key]), ['value'] = toOrig(string.gsub(line, '<(.-)>(.-)</(.-)>', "%1 '%2'"))}
				else
					debug.printf('error line.' .. line)
					return nil
				end
			else
				return {['key'] = toOrig(keys[key]), ['value']  = toOrig(data)}
			end
		end
	end
	return nil
end

function xmlToFile(xmlpath, filepath)
	local fp = io.open(xmlpath, 'r')
	local step = 'dir' 
	--	dir, file, config, option
	local data,filefp,dir,file,config,option

	if not (xmlpath and filepath and fp) then
		--debug.printf("xmlpath,filepath is must exist")
		return
	end

	local stepaddentry = {
		['dir']	= function(data, mov)
				os.execute('mkdir '.. filepath .. '/'.. data)
				dir = data
				if mov then
					step = 'file'
				end
			  end,
		['file']= function(data, mov)
				if filefp then
					debug.printf("error last file is still open. " .. data)
					return 1
				else
					if mov then
						step = 'config'
						--file = data
					end
					file = data
					--debug.printf("open file" .. '/tmp/' .. dir .. '/' .. file)
					filefp = io.open(filepath .. '/' .. dir .. '/' .. file, 'w')
					if filefp then
						--debug.printf("filefp is ok.")
					end
					if mov == false and filefp then
						debug.printf("fail to write to : "..file)
						filefp:close()
						filefp = nil
						file = nil
					end
				end
			  end,
		['config'] = function(data, mov)
				if filefp then
					local tdata = data
					filefp:write('\n')
					local name = string.match(data, 'name="(.+)"')
					if name then
						filefp:write('config '.. string.gsub(data, '(.+) name="(.+)"', "%1 '%2'") .. '\n')
						tdata = string.match(data, '(.+) name=".+"')
					else
						filefp:write('config '.. data .. '\n')
					end
					if mov then
						step = 'option'
						if config then
							debug.printf("error ".. config .. "config not closed. new: " .. data)
							return 1
						else
							--debug.printf('set config: ' .. tdata)
							config = tdata
						end
					end
				else
					debug.printf("error:no file is open, config: " .. data)
					return 1
				end
			   end,
		['option'] = function(data, mov)
				if filefp then
					if mov then
						step = 'list'
						option = data
					else
						filefp:write('    option ' .. data .. '\n')
					end
				else
					debug.printf("error:no file is open, option: " .. data)
					return 1
				end
			   end,
		['list']   = function(data)
				if filefp then
					filefp:write('    list '.. data .. '\n')
				else
					debug.printf("error:no file is open, list: " .. data)
					return 1
				end
			     end	
	}

	local stependentry = {
		-- end is step forward
		['dir']	  = function(data)
				debug.printf("error where to go ?")
			    end,
		['file']  = function(data)
				if data ~= dir then
					debug.printf("error file end." .. data)
					return 1
				end
			    end,
		['config']= function(data)
				if data == file and filefp then
					filefp:close()
					filefp = nil
					file = nil
					step = 'file'
				else
					debug.printf("error file end." .. data)
					return 1
				end
			    end,
		['option']= function(data)
				if data == config and filefp then
					step = 'config'
					config = nil
				else
					debug.printf('error config end. '.. data .. ' != ' .. config)
					return 1
				end
			    end,
		['list']= function(data)
				if data == option and filefp then
					step = 'option'
					option = nil
				else
					debug.printf('error option end.'.. data)
					return 1
				end
			    end
	}

	local func = {
		['empty'] = function(data)
				return stepaddentry[step](data, false)
			  end,
		['end']	= function(data)
				return stependentry[step](data)
			  end,
		['single']= function(data)
				return stepaddentry[step](data, false)
			  end,
		['new']	= function(data)
				return stepaddentry[step](data, true)
			  end
	}
	-- drop xml head
	local tmp = 0
	local errorline = nil
	
	-- do work
	for line in fp:lines() do
		--errorline in middle
		if errorline then
			debug.printf("error unkown line: " .. line)
			break
		end
		if tmp == 1 then
			--debug.printf(line)
			data = getxmlkey(line)
			if data then
				--debug.printf("key:" .. data.key .. " value:" .. data.value)
				if func[data.key](data.value) then
					debug.printf("error operation failed: ".. line)
					break
				end
			else
				errorline = line
			end
		end
		tmp = 1
	end
	
	--integrity check
	if filefp or file or config or option then
		debug.printf("error:config is not finish(filefp,file,config,option): " .. tostring(filefp) .. tostring(file) .. tostring(config) .. tostring(option))	
	end

	fp:close()
end

function checkxml(xmlpath)
	local tmp = 0
	--1->config,2->cfgfile,3->section,4->list,option is single line and do not occupy the stack
	local num = 0
	--cfgtb[1]='config',cfgtb[2]=file.name,cfgtb[3]=section.name,cfgtb[3]='list'
	local cfgtb = {}
	
	local fp = io.open(xmlpath, 'r')
	for line in fp:lines() do
		if tmp == 1 then
			data = getxmlkey(line)
			if data then
					data.value = string.match(data.value, '(.+) name=".+"') or data.value					
					if data.key == "new" then
						num = num + 1
						cfgtb[num] = data.value
					elseif data.key == "end" then
						if cfgtb[num] ~= data.value then
							debug.printf("error unkown line: " .. line)
							fp:close()
							return false
						end
						num = num -1
					elseif data.key == "empty" then
						debug.printf("error unkown line: " .. line)
						fp:close()
						return false
					end
			else
				debug.printf("error unkown line: " .. line)
				fp:close()
				return false
			end
		end
		tmp = 1
		if (num > 4) or (num < 0) then
			debug.printf("error unkown line: " .. line)
			fp:close()
			return false
		end
	end
	fp:close()
	if num ~= 0 then
		debug.printf("xml is not integrity")
		return false
	end
	return true
end

function saveconfig()
	--debug.printf("saveconfig() begin:")
	os.execute("lock /tmp/commit_flash_lock")	
	fileToXml("/tmp/save-userconf.xml")
	if checkxml("/tmp/save-userconf.xml") then
        os.execute("nvrammanager -e  -p  user-config  >/dev/null 2>&1")
        os.execute("nvrammanager -w /tmp/save-userconf.xml -p  user-config   >/dev/null 2>&1")
	end
	os.execute("rm -f /tmp/save-userconf.xml >/dev/null 2>&1")	
	os.execute("lock -u /tmp/commit_flash_lock")
	--debug.printf("saveconfig() end:")
end

function resetconfig()
	--os.execute("mtd erase userconf >/dev/null 2>&1; cat /dev/" .. get_mtd("defconf") .. " > /dev/".. get_mtd("userconf"))
	debug.printf("resetconfig() begin:")
	os.execute("nvrammanager -e  -p  user-config  >/dev/null 2>&1")
	os.execute("nvrammanager -r /tmp/reset-defconfig.xml -p  default-config  >/dev/null 2>&1")
	os.execute("nvrammanager -w /tmp/reset-defconfig.xml -p  user-config   >/dev/null 2>&1")
	os.execute("rm -f /tmp/reset-defconfig.xml >/dev/null 2>&1")
	debug.printf("resetconfig() end:")
end

function isdftconfig()
	local username,password
	local defaultcfg
	local head,tail,value
	local usernames,i
	
	--get default username
	os.execute("nvrammanager -r /tmp/default-config.xml -p  default-config  >/dev/null 2>&1")
	local fp = io.open("/tmp/default-config.xml",'r')
	defaultcfg = fp:read("*a")
	fp:close()
	os.execute("rm -f /tmp/default-config.xml >/dev/null 2>&1")
	
	head="<accountmgnt>"
	tail="</accountmgnt>"
	_,_,value=string.find(defaultcfg, head.."(.-)"..tail)
	
	head="<username>"
	tail="</username>"
	_,_,value=string.find(value, head.."(.-)"..tail)


	usernames = accountmgnt.get_name()

	if table.getn(usernames) > 0 then
		for i=1,table.getn(usernames) do 
			if usernames[i] == value then
				username = usernames[i]
			end
		end  
	else
		return false
	end

	if username == value then
		password = accountmgnt.get_password(username)
		if password == "admin" then
			return true
		else
			return false
		end
	else
		return false
	end

end

function isupgrade()
	local upgrade
	local fp = io.open("/tmp/isupgrade",'r')
	if fp == nil then
		return false
	else
		upgrade = fp:read("*a")
		fp:close()
		if upgrade:match("[.]*true[.]*") then
			return true
		else
			return false
		end
	end
	return false
end
function clear_upgrade_flag()
	os.execute("rm -f /tmp/isupgrade")
end

function reloadconfig()

	debug.printf("reloadconfig() begin:")
	os.execute("nvrammanager  -r /tmp/reload-userconf.xml -p  user-config  >/dev/null 2>&1")
	
	local file,err = io.open("/tmp/reload-userconf.xml",'r')
			
    if file == nil then
        debug.printf(err)

	elseif file:read(6) == '<?xml 'then
		file:close()
		os.execute("rm -rf /etc/config >/dev/null 2>&1")
		xmlToFile("/tmp/reload-userconf.xml", "/etc")
	else
		file:close()
		--load from defconfig
		debug.printf("no userconfig")
		resetconfig()
		--try to load again
		os.execute("rm -f /tmp/reload-userconf.xml >/dev/null 2>&1")
		os.execute("nvrammanager -r /tmp/reload-userconf.xml -p  user-config   >/dev/null 2>&1")
		file,err = io.open("/tmp/reload-userconf.xml",'r')
		if file:read(6) == '<?xml 'then
                	file:close()
                	os.execute("rm -rf /etc/config >/dev/null 2>&1")
                	xmlToFile("/tmp/reload-userconf.xml", "/etc")
		else
			debug.printf("error no config")
		end
	end
	
	os.execute("rm -f /tmp/reload-userconf.xml >/dev/null 2>&1")
	
	debug.printf("reloadconfig() end:")
	
--[[
	local file,err = io.open("/dev/" .. get_mtd("userconf"),'r')
	
    if file == nil then
        debug.printf(err)

	elseif file:read(6) == '<?xml 'then
		file:close()
		os.execute("rm -rf /etc/config >/dev/null 2>&1")
		xmlToFile("/dev/" .. get_mtd("userconf"), "/etc")
	else
		file:close()
		--load from defconfig
		debug.printf("no userconfig")
		resetconfig()
		--try to load again
		file,err = io.open("/dev/" .. get_mtd("userconf"),'r')
		if file:read(6) == '<?xml 'then
                	file:close()
                	os.execute("rm -rf /etc/config >/dev/null 2>&1")
                	xmlToFile("/dev/" .. get_mtd("userconf"), "/etc")
		else
			debug.printf("error no config")
		end
	end
	
]]--

end

function getsysinfo(option)

	local fp,err
        local  softVerOption = "soft_ver"
	
	if option == "SOFTVERSION" then	
		--debug.printf("zlw debug :  SOFTVERSION")
		os.execute("nvrammanager  -r /tmp/softversion -p  soft-version  >/dev/null 2>&1")
		fp,err = io.open("/tmp/softversion",'r')
		
		if fp == nil then
			debug.printf(err)
		else
			local value
			for line in fp:lines() do
				value = string.match(line, string.format("%s:(.+)", softVerOption))
				--debug.printf(line)
				--debug.printf(value)
				if value then
					fp:close()
					return value
				end
			end
			fp:close()
		end
	end
	
	if option == "HARDVERSION" then
		--debug.printf("zlw debug :  HARDVERSION")
		local  aliasForVerOption = "product_ver"
		local  productNameOption = "product_name"
		local  product_name
		local  product_ver
		--debug.printf("zlw debug :  " .. aliasForOption)
		os.execute("nvrammanager  -r /tmp/productinfo -p  product-info  >/dev/null 2>&1")
		fp,err = io.open("/tmp/productinfo",'r')
		
		if fp == nil then
			debug.printf(err)
		else
			local value1, value2
			for line in fp:lines() do
				value1 = string.match(line, string.format("%s:(.+)", productNameOption))
				value2 = string.match(line, string.format("%s:(.+)", aliasForVerOption))
				--debug.printf(value1)
				--debug.printf(value2)
				if value1 then
					product_name = value1
					--debug.printf(product_name)
				end
				if value2  then
					if string.len(value2) == 5 then 
						product_ver = string.sub(value2, 1, -3)
					else 
						product_ver = value2
					end
					--debug.printf(product_ver)
				end
				if product_name and product_ver  then
					--debug.printf(product_name .. " v" .. product_ver)
					fp:close()
					return (product_name .. " v" .. product_ver)
				end
			end
			fp:close()
		end
	end
	
	if option == "PIN" then
		--debug.printf("zlw debug :  PIN")
		os.execute("nvrammanager  -r /tmp/pin -p  pin  >/dev/null 2>&1")	
		fp,err = io.open("/tmp/pin",'r')
		
		if fp == nil then
			debug.printf(err)
		else
			local value
			for line in fp:lines() do
				--value = string.match(line, string.format("%s:(.+)", option))
				value = line
				--debug.printf(line)
				--debug.printf(value)
				if value then
					fp:close()
					return value
				end
			end
			fp:close()
		end
	end
	
	if option then
		--debug.printf("zlw debug :  option")
		os.execute("nvrammanager  -r /tmp/productinfo -p  product-info  >/dev/null 2>&1")
		fp,err = io.open("/tmp/productinfo",'r')
		
		if fp == nil then
			debug.printf(err)
		else
			local value
			for line in fp:lines() do
				value = string.match(line, string.format("%s:(.+)", option))
				if value then
					fp:close()
					return value
				end
			end
			fp:close()
		end

	end
	
	return nil

--[[
	if option then
		local fp = io.open("/dev/" .. get_mtd("softinfo"), "r")
		local value
		for line in fp:lines() do
			value = string.match(line, string.format("%s:(.+)", option))
			if value then
				return value
			end
		end
		fp:close()
		fp = io.open("/dev/" .. get_mtd("boardinfo"), "r")
                for line in fp:lines() do
                        value = string.match(line, string.format("%s:(.+)", option))
                        if value then
                                return value
                        end
                end
		fp:close()
	end
	return nil
]]--

end

function reload_profile()
	
	debug.printf("reload_profile() begin:")
	os.execute("nvrammanager -r /tmp/reload-profile.xml -p  profile   >/dev/null 2>&1")
		
	local file,err = io.open("/tmp/reload-profile.xml",'r')

    if file == nil then
        debug.printf(err)

	elseif file:read(6) == '<?xml 'then
		file:close()
		os.execute("rm -rf /etc/profile.d >/dev/null 2>&1")
		xmlToFile("/tmp/reload-profile.xml", "/etc")
		os.execute("chmod -R 444 /etc/profile.d >/dev/null 2>&1")

	else
		file:close()
		debug.printf("error no profile")
	end
	
	os.execute("rm -f /tmp/reload-profile.xml >/dev/null 2>&1")
	debug.printf("reload_profile() end:")
end

