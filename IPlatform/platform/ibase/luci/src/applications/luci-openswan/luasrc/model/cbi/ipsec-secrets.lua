--[[
LuCI - Lua Configuration Interface
IPsec Support

Copyright 2008 Steven Barth <steven@midlink.org>
Copyright 2008 Jo-Philipp Wich <xm@leipzig.freifunk.net>
Copyright 2011 Kevin Locke <klocke@digitalenginesoftware.com>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

$Id$
]]--

local map = Map("ipsec", translate("IPsec Secrets"),
	translate([[Stored secrets for IKE/IPsec authentication.
Secrets can include preshared secrets, passphrases for X.509 certificates, and
XAUTH static passwords.]]))
local opt


local csect = map:section(TypedSection, "ipsec_secret_cs",
	translate("IPsec Certificate Passwords"),
	translate([[Private key files available for use with IPsec.
	Passwords are used to decrypt files encrypted using 3DES and should be
	left blank for files which are not encrypted.]]))
csect.template = "cbi/tblsection"
csect.addremove = true
csect.anonymous = true

opt = csect:option(FileBrowser, "file", translate("Certificate File"))
opt.default_path = "/etc/ipsec.d/private"

opt = csect:option(Value, "secret", translate("Password"))
opt.password = true


local psect = map:section(TypedSection, "ipsec_secret_ss",
	translate("IPsec Preshared Secrets"),
	translate([[Shared secrets are used to authenticate connection peers
to each other.  Each endpoint must use the same shared secret for
authentication to be successful.  Each shared secret may be shared by many
hosts.]]))
psect.template = "cbi/tblsection"
psect.addremove = true
psect.anonymous = true

opt = psect:option(DynamicList, "indices", translate("Applicable Hosts"),
	translate("Hosts can be specified as IP addresses or Fully Qualified Domain Names prefixed with \"@\"."))
opt:value("%any")
opt:value("%any6")

opt = psect:option(Value, "secret", translate("Secret"))
opt.password = true


local xsect = map:section(TypedSection, "ipsec_secret_xs",
	translate("IPsec XAUTH Passwords"),
	translate("Usernames and passwords sent to peers for user authentication via XAUTH."))
xsect.template = "cbi/tblsection"
xsect.addremove = true
xsect.anonymous = true

opt = xsect:option(Value, "username", translate("Username"))

opt = xsect:option(Value, "secret", translate("Password"))
opt.password = true

return map
