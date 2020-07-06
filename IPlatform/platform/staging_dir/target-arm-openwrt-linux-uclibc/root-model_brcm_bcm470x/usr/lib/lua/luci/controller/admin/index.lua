--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  index.lua
Details :  Index for root and admin
Author  :  Ye Qianchuan <yeqianchuan@tp-link.net>
Version :  1.0.0
Date    :  30 May, 2014
]]--

module("luci.controller.admin.index", package.seeall)

local function error403()
    luci.http.status(403, "Forbidden")
    luci.http.prepare_content("text/plain")
    luci.http.write("Forbidden")
end

function index()
	local root = node()
        root.target = error403

	local page   = node("admin")
	page.target  = error403
	page.sysauth = true
	page.ucidata = true
end
