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

require "luci.util"
require "nixio"
require "nixio.fs"
local uci = require "luci.model.uci".cursor()

--- Check if a file is listed in the ipsec.secrets configuration
local function file_in_secrets(filename)
	local relfile
	local privatedir = "/etc/ipsec.d/private"
	if filename:sub(1, privatedir:len()) == privatedir then
		relfile = filename:sub(privatedir:len()+1)
	elseif filename:sub(1, 1) ~= "/" then
		relfile = filename
		filename = privatedir .. "/" .. relfile
	end

	local found = false
	uci:foreach("ipsec", "ipsec_secret_cs", function(ucisect)
		if ucisect["file"] == filename or ucisect["file"] == relfile then
			found = true
			return false
		end
		return true
	end)
	return found
end

-- Certificate types, directory => display name
local certtypes = {
	aacerts = translate("Authorization Authority"),
	cacerts = translate("Certificate Authority"),
	certs = translate("Host (Public Key)"),
	crls = translate("Revocation List"),
	private = translate("Host (Private Key)"),
}

local form = SimpleForm("ipsec-certificates",
	translate("IPsec Certificates"),
	translate([[<p>Configure available X.509 certificates for IPsec</p>
<p>X.509 certificate files must be encoded in DER (binary) or PEM (ASCII - RFC 1421).
If the file is encrypted (only supported for private key files) it must use
DES-EDE3-CBC as the encryption algorithm.  In this case, for automated
operation, a password must be provided in the <a href="secrets">Secrets</a>
configuration.</p>
<p>Error checking is currently only performed by the pluto daemon.
If a certificate is invalid, an error message will be logged when pluto
attempts to use the certificate.</p>]]))

local section = form:section(SimpleSection)
section.title = translate("Upload Certificate")

local certrole = form:field(ListValue, "certrole", translate("Certificate Role"))
for certdir, certname in luci.util.vspairs(certtypes) do
	certrole:value(certdir, certname)
end
certrole.default = "certs"

local certfile = form:field(FileUpload, "certfile",
	translate("Certificate File"))
certfile.optional = false

-- FIXME:  Ugly monkey patch hack.  Give me a better option...
-- CBI ignores the filename (field.file) in its filehandler.
-- mimedecode_message_body in the protocol library saves the filename into
-- message.params, but this is overwritten by the target name which is
-- saved by CBI in its file handler
-- So, hook in before it does and save it
-- Note:  Must set file handler after prepare, before http parsing (in parse)
local uploadfilenames = {}
local oldparse = form.parse
function form.parse(self, ...)
	local oldfilehandler = luci.http.context.request.filehandler
	luci.http.setfilehandler(function(field, ...)
		if field and field.name and field.file then
			uploadfilenames[field.name] = field.file
		end
		return oldfilehandler(field, ...)
	end)
	return oldparse(self, ...)
end

function form.handle(self, state, data)
	if state ~= FORM_VALID then
		return
	end

	-- Security/Sanity check
	if not data.certfile then
		self.errmessage = translate("Certificate File must be specified.")
		return
	elseif not certtypes[data.certrole] then
		self.errmessage = translate("Unrecognized certificate role.")
		return
	end

	local uplpath = data.certfile
	local origname = uploadfilenames[certfile:cbid(1)]
	local filename
	if origname then
		filename = nixio.fs.basename(origname)
	else
		filename = nixio.fs.basename(uplpath)
	end
	local destpath = "/etc/ipsec.d/" .. data.certrole .. "/" .. filename
	if not nixio.fs.rename(uplpath, destpath) then
		self.errmessage = string.format(
			translate("Unable to save certificate file to %s: %s"),
			destpath, nixio.strerror(nixio.errno()))
		return
	end

	self.message = string.format(
		translate("Uploaded certificate saved as %s."),
		destpath)

	-- File must be in ipsec.secrets to be usable by pluto
	-- (even if it is not encrypted).  So, add it automatically.
	if data.certrole == "private" and not file_in_secrets(destpath) then
		local ucisectname = uci:add("ipsec", "ipsec_secret_cs")
		print("Added section " .. ucisectname)
		uci:set("ipsec", ucisectname, "file", destpath)
		uci:save("ipsec")
	end
end

return form
