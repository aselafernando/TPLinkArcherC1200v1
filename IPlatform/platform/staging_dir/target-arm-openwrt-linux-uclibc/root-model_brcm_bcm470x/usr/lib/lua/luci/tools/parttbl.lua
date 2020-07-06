--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  parttbl.lua
Details :  
Author  :  huluyao <huluyao@tp-link.net>
Version :  1.0.0
Date    :  5Mar, 2014
]]--

local nixio   = require "nixio"
local debug   = require "luci.tools.debug"

-- parttbl_check( flash_path, file_path ) :
-- @flash_path: old parttion table in flash
-- @file_path: new parttion table in fs
-- @return: table
-- 			same: 			same parttion table return 1, else 0
-- 			magic: 			same magic number return 1, else 0
-- 			flash_size: 	same flash size return 1, else 0
-- 			version: 		same version return 1, else 0
-- 			rootfs: 		rootfs patition exist return 1, else 0
-- 			kernel:         kernel patition exist return 1, else 0
-- 			rootfs_same:    same rootfs patition return 1, else 0
-- 			diff: 			a table of different partitions
-- 			rootfs_theif: 	a table of patitions which take use of flash zone that old rootfs have.(if rootfs changed)
module("luci.tools.parttbl", package.seeall)


-- WARNING: make sure endian
function str_to_num( str )
	local sum = 0
	local strlen = string.len(str)
	for i=1,strlen do
		local s = string.byte(str,i)
		s = s * (256^(i-1))
		sum = sum + s
	end
	return sum
end

function strcmp( str1, str2 )
	return string.find(str1, str2)
end

function print_lua_table(lua_table, indent)
	indent = indent or 0
	for k, v in pairs(lua_table) do
		if type(k) == "string" then
			k = string.format("%q", k)
		end
		local szSuffix = ""
		if type(v) == "table" then
			szSuffix = "{"
		end
		local szPrefix = string.rep("    ", indent)
		formatting = szPrefix.."["..k.."]".." = "..szSuffix
		if type(v) == "table" then
			print(formatting)
			print_lua_table(v, indent + 1)
			print(szPrefix.."},")
		else
			local szValue = ""
			if type(v) == "string" then
				szValue = string.format("%q", v)
			else
				szValue = tostring(v)
			end
			print(formatting..szValue..",")
		end
	end
end

function parttbl_check( flash_path, file_path )

	local flash_fp = io.open(flash_path, 'r')
	local file_fp  = io.open(file_path, 'r')

	local old = flash_fp:read("*all")
	local new = file_fp:read("*all")

	local old_magic1  		= string.sub(old,1,4)
	local old_flash_size  	= string.sub(old,5,8)
	local old_version 		= string.sub(old,9,12)
	local old_sum  	  		= string.sub(old,13,13)

	local new_magic1  		= string.sub(new,1,4)
	local new_flash_size  	= string.sub(new,5,8)
	local new_version 		= string.sub(new,9,12)
	local new_sum  	  		= string.sub(new,13,13)

	local kernel_ok = 0
	local rootfs_ok = 0
	local rootfs_same = 1
	local parttbl_offset = 0

	local old_rootfs_postion
	local new_rootfs_postion

	local ret  = {}
	local diff = {}
	local hit  = {}
	local part = {}

	old_sum = string.byte(old_sum) -1
	new_sum = string.byte(new_sum) -1
	tmp_new_sum = new_sum

	-- same partition
	if strcmp(old, new)  ~= nil then
		ret.same = 1
	else
		ret.same = 0
	end

	-- check magic number
	if strcmp(old_magic1, new_magic1)  ~= nil then
		ret.magic = 1
	else
		ret.magic = 0
	end

	-- check flash_size
	if strcmp(old_flash_size, new_flash_size)  ~= nil then
		ret.flash_size = 1
	else
		ret.flash_size = 0
	end

	-- check version
	if strcmp(old_version, new_version)  ~= nil then
		ret.version = 1
	else
		ret.version = 0
	end

	while old_sum >= 0 do
		local flash_part_name    = string.sub(old,17+old_sum*32, 32+old_sum*32)

		-- debug.printf(flash_part_name)

		local flash_part_offset  = string.sub(old,33+old_sum*32, 40+old_sum*32)
		local flash_part_size    = string.sub(old,41+old_sum*32, 48+old_sum*32)

		if string.find(flash_part_name,"0:boardinfo\0")  ~= nil then
			boardinfo_end = str_to_num(flash_part_offset) + str_to_num(flash_part_size)
		end

		-- find partition in new table
		tmp_new_sum = new_sum
		while tmp_new_sum >= 0 do
			local new_part_name    = string.sub(new,17+tmp_new_sum*32, 32+tmp_new_sum*32)

			-- try to find kernel and rootfs
			if kernel_ok == 0 then
				if string.find(new_part_name,"0:HLOS\0")  ~= nil then
					kernel_ok = 1
					-- print("kernel parttion found")
				end
		    end

		    if rootfs_ok == 0 then
				if string.find(new_part_name,"0:rootfs\0") ~= nil then
					rootfs_ok = 1
					-- print("rootfs parttion found")
				end
		    end

			if strcmp(flash_part_name, new_part_name) ~= nil then

				hit[tmp_new_sum] = 1
				part_diff = {}
				-- debug.printf(flash_part_name)

				local new_part_offset  = string.sub(new,33+tmp_new_sum*32, 40+tmp_new_sum*32)
				local new_part_size    = string.sub(new,41+tmp_new_sum*32, 48+tmp_new_sum*32)

				-- partition is same, do nothing
				if strcmp(flash_part_offset, new_part_offset) ~= nil then
					if strcmp(flash_part_size, new_part_size) ~= nil then

						break
					end
				end

				part_diff.name			= new_part_name
				part_diff.old_offset 	= str_to_num(flash_part_offset)
				part_diff.old_size 		= str_to_num(flash_part_size)
				part_diff.new_offset	= str_to_num(new_part_offset)
				part_diff.new_size		= str_to_num(new_part_size)

				diff[#diff+1] = part_diff

				if string.find(new_part_name,"0:rootfs\0") ~= nil then
					rootfs_same = 0
					-- print("rootfs parttion moved")
					local min = str_to_num(flash_part_offset)
					local max = str_to_num(flash_part_size)
					max = min + max
					local tmp_new_sum2 = new_sum
					local rootfs_map = {}
					while tmp_new_sum2 >= 0 do
						local new_part_name2    = string.sub(new,17+tmp_new_sum2*32, 32+tmp_new_sum2*32)
						local new_part_offset2  = string.sub(new,33+tmp_new_sum2*32, 40+tmp_new_sum2*32)
						local new_part_size2    = string.sub(new,41+tmp_new_sum2*32, 48+tmp_new_sum2*32)
						local tmp_min = str_to_num(new_part_offset2)
						local tmp_max = str_to_num(new_part_size2)
						tmp_max = tmp_max + tmp_min

						if not (tmp_min >= max or tmp_max <= min) then
							local rootfs_diff = {}
							rootfs_diff.name = new_part_name2
							rootfs_diff.offset = str_to_num(new_part_offset2)
							rootfs_diff.size = str_to_num(new_part_size2)
							if not (string.find(new_part_name2,"0:rootfs\0") ~= nil) then
								rootfs_map[#rootfs_map+1] = rootfs_diff
							end
						end
						tmp_new_sum2 = tmp_new_sum2 -1
					end
					ret.rootfs_theif = rootfs_map
				end			
				break
			end

			tmp_new_sum = tmp_new_sum -1
		end

		if tmp_new_sum == 0 then
			part_diff = {}
			part_diff.name			= flash_part_name
			part_diff.old_offset 	= str_to_num(flash_part_offset)
			part_diff.old_size 		= str_to_num(flash_part_size)
			part_diff.new_offset	= nil
			part_diff.new_size		= nil

			diff[#diff+1] = part_diff
		end


		if strcmp(flash_part_name,"0:HLOS\0")  ~= nil then
			-- print("kernel parttion found")
		end

		if strcmp(flash_part_name,"0:rootfs\0") ~= nil then
			old_rootfs_postion = flash_part_offset
			-- print("rootfs parttion found")
		end

		old_sum = old_sum -1
	end

	-- try to find new added parttion in new partition table
	while new_sum >= 0 do

		local flash_part_name    = string.sub(new,17+new_sum*32, 32+new_sum*32)
		local flash_part_offset  = string.sub(new,33+new_sum*32, 40+new_sum*32)
		local flash_part_size    = string.sub(new,41+new_sum*32, 48+new_sum*32)
		local part_new = {}
		part_new.name			= flash_part_name
		part_new.offset			= str_to_num(flash_part_offset)
		part_new.size			= str_to_num(flash_part_size)
		part[#part+1] = part_new

		if hit[new_sum] ~= 1 then
			

			-- try to find kernel and rootfs
			if kernel_ok == 0 then
				if string.find(new_part_name,"0:HLOS\0")  ~= nil then
					kernel_ok = 1
					-- print("kernel parttion found")
				end
		    end

		    if rootfs_ok == 0 then
				if string.find(new_part_name,"0:rootfs\0") ~= nil then
					rootfs_ok = 1
					-- print("rootfs parttion found")
				end
		    end

		    part_diff = {}
			part_diff.name			= flash_part_name
			part_diff.old_offset 	= nil
			part_diff.old_size 		= nil
			part_diff.new_offset	= str_to_num(flash_part_offset)
			part_diff.new_size		= str_to_num(flash_part_size)

			diff[#diff+1] = part_diff
		end

		new_sum = new_sum -1

	end
	
	ret.diff = diff
	ret.part = part
	ret.rootfs = rootfs_ok 
	ret.kernel = kernel_ok
	ret.rootfs_same = rootfs_same
	ret.boardinfo_end = boardinfo_end

		
	flash_fp:close()
	file_fp:close()

	return ret

end
