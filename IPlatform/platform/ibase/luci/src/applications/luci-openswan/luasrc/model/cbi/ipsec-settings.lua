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

-- FIXME:  Have to set cfgvalue on each Flag to prevent default from being
--         returned (which prevents it from being optional).
--         Also Flag.rmempty == "remove default" not "remove empty", which
--         is not what we want, so set false on all

function build_map(advanced)
	local map = Map("ipsec", translate("IPsec Global Settings"),
		translate([[Configuration parameters for establishing inbound
		and/or outbound connections secured using IPsec.  Most
		parameters are direct mappings to their equivalents in
		ipsec.conf(5).]]))

	local gensect = map:section(NamedSection, "setup", "ipsec_config",
		translate("Global Configuration"),
		translate("Options which affect all IPsec connections and/or the behavior of the IPsec daemons"))

	add_basic_options(gensect)
	if advanced then
		add_advanced_options(gensect)
	end

	return map
end

function add_basic_options(sect)
	local opt

	opt = sect:option(Value, "crlcheckinterval",
		translate("crlcheckinterval - Interval (in seconds) after which pluto will verify loaded X.509 CRL's for expiration"),
		translate([[If any of the CRL's is expired, or if they
		previously failed to get updated, a new attempt at updating the
		CRL is made. The first attempt to update a CRL is started at
		two times the crlcheckinterval. If set to 0, which is also the
		default value if this option is not specified, CRL updating is
		disabled.]]))
	opt.optional = true
	opt.datatype = "uinteger"

	opt = sect:option(Flag, "force_keepalive",
		translate("force_keepalive - Force sending NAT-T keep-alives"),
		translate([[NAT-T keep-alives are send to prevent the NAT
		router from closing its port when there is not enough traffic
		on the IPsec connection.  This option forces keep-alive
		messages to be sent on a regular interval.]]))
	opt.cfgvalue = AbstractValue.cfgvalue
	opt.rmempty = false
	opt.optional = true
	opt.default = "no"
	opt.disabled = "no"
	opt.enabled = "yes"

	opt = sect:option(Value, "interfaces",
		translate("interfaces - Interfaces for IPsec to use"),
		translate([[Virtual and physical interfaces for IPsec to use: a
		single virtual=physical pair, a list of pairs separated by
		white space, or %none.  One of the pairs may be written as
		%defaultroute, which means: find the interface d that the
		default route points to, and then act as if the value was
		``ipsec0=d''. %defaultroute is the default; %none must be used
		to denote no interfaces, or when using the NETKEY stack. If
		%defaultroute is used (implicitly or explicitly) information
		about the default route and its interface is noted for use by
		ipsec_manual(8) and ipsec_auto(8).)]]))
	opt.optional = true

	opt = sect:option(MultiValue, "klipsdebug",
		translate("klipsdebug - How much KLIPS debugging output should be logged."),
		translate([[An empty value, or the magic value none, means no
		debugging output. The magic value all means full output.
		Otherwise only the specified types of output (a quoted list,
		names separated by white space) are enabled; for details on
		available debugging types, see ipsec_klipsdebug(8). This KLIPS
		option has no effect on NETKEY, Windows or BSD stacks.]]))
	-- Contrary to the documentation, this seems to apply to NETKEY too
	--opt:depends("protostack", "klips")
	opt.optional = true
	opt:value("none")
	opt:value("ah")
	opt:value("comp")
	opt:value("eroute")
	opt:value("esp")
	opt:value("nattraversal")
	opt:value("netlink")
	opt:value("pfkey")
	opt:value("radij")
	opt:value("rcv")
	opt:value("spi")
	opt:value("tunnel")
	opt:value("tunnel-xmit")
	opt:value("verbose")
	opt:value("xform")
	opt:value("all")

	opt = sect:option(Value, "listen",
		translate("listen - IP address on which to listen"),
		translate([[The IP address on which incoming connections can be
		made.  (default depends on interfaces setting).]]))
	opt.optional = true

	opt = sect:option(Flag, "nat_traversal",
		translate("nat_traversal - Accept/Offer to support NAT (NAPT, also known as \"IP Masqurade\")"))
	opt.cfgvalue = AbstractValue.cfgvalue
	opt.rmempty = false
	opt.optional = true
	opt.default = "no"
	opt.disabled = "no"
	opt.enabled = "yes"

	opt = sect:option(Flag, "oe",
		translate("oe - Enable Opportunistic Encryption"),
		translate([[Opportunistic Encryption is the term to describe
		using IPsec tunnels without prearrangement. It uses IPSECKEY or
		TXT records to announce public RSA keys for certain IP's or
		identities.]]))
	opt.cfgvalue = AbstractValue.cfgvalue
	opt.rmempty = false
	opt.optional = true
	opt.default = "no"
	opt.disabled = "no"
	opt.enabled = "yes"

	opt = sect:option(MultiValue, "plutodebug",
		translate("plutodebug - How much Pluto debugging output should be logged."),
		translate([[An empty value, or the magic value none, means no
		debugging output. The magic value all means full output.
		Otherwise only the specified types of output (a quoted list,
		names without the
		--debug- prefix, separated by white space) are enabled; for
		--details on available debugging types, see ipsec_pluto(8).]]))
	opt.optional = true
	opt:value("none")
	opt:value("control")
	opt:value("controlmore")
	opt:value("crypt")
	opt:value("dns")
	opt:value("dpd")
	opt:value("emitting")
	opt:value("klips")
	opt:value("lifecycle")
	opt:value("nattraversal")
	opt:value("oppo")
	opt:value("oppoinfo")
	opt:value("parsing")
	opt:value("pfkey")
	opt:value("private")
	opt:value("raw")
	opt:value("x509")
	opt:value("all")

	opt = sect:option(Flag, "strictcrlpolicy",
		translate("strictcrlpolicy - Require current X.509 Certificate Revocation Lists"),
		translate([[If not set, pluto is tolerant about missing or
		expired X.509 Certificate Revocation Lists (CRL's), and will
		allow peer certificates as long as they do not appear on an
		expired CRL. When this option is enabled, all connections with
		an expired or missing CRL will be denied. Active connections
		will be terminated at rekey time.  This setup is more secure,
		but also dangerous. If the CRL is fetched through an IPsec
		tunnel with a CRL that expired, the entire VPN server will be
		dead in the water until a new CRL is manually transferred to
		the machine (if it allows non-IPsec connections).]]))
	opt.cfgvalue = AbstractValue.cfgvalue
	opt.rmempty = false
	opt.optional = true
	opt.default = "no"
	opt.disabled = "no"
	opt.enabled = "yes"

	opt = sect:option(DynamicList, "virtual_private",
		translate("virtual_private - Permitted vhost:/vnet: private networks"),
		translate([[Networks that are allowed as subnet= for the remote
		clients when using the vhost: or vnet: keywords in the subnet=
		parameters.  In other words, the address ranges that may live
		behind a NAT router through which a client connects. This value
		is usually set to all the RFC-1918 address space, excluding the
		space used in the local subnet behind the NAT (An IP address
		cannot live at two places at once). IPv4 address ranges are
		denoted as %v4:a.b.c.d/mm and IPv6 is denoted as
		%v6:aaaa::bbbb:cccc:dddd:eeee/mm. One can exclude subnets by
		using the !. For example, if the VPN server is giving access to
		192.168.1.0/24, this option should be set to:
		virtual_private=%v4:10.0.0.0/8,%v4:192.168.0.0/16,%v4:172.16.0.0/12,%v4:!192.168.1.0/24.
		This parameter is only needed on the server side and not on the
		client side that resides behind the NAT router, as the client
		will just use its IP address for the inner IP setting. This
		parameter may eventually become per-connection. See also
		leftsubnet=]]))
	opt.optional = true
end

function add_advanced_options(sect)
	local opt

	opt = sect:option(Flag, "disable_port_floating",
		translate("disable_port_floating - Enable the newer NAT-T standards for port floating"))
	opt.cfgvalue = AbstractValue.cfgvalue
	opt.rmempty = false
	opt.optional = true
	opt.default = "no"
	opt.disabled = "no"
	opt.enabled = "yes"

	opt = sect:option(Value, "dumpdir",
		translate("dumpdir - Core dump directory"),
		translate([[In what directory should things started by setup
		(notably the Pluto daemon) be allowed to dump core? The empty
		value (the default) means they are not allowed to.]]))
	opt.optional = true
	opt.datatype = "directory"

	opt = sect:option(Flag, "fragicmp",
		translate("fragicmp - Report fragmentation using ICMP"),
		translate([[Whether a tunnel's need to fragment a packet should
		be reported back with an ICMP message, in an attempt to make
		the sender lower his PMTU estimate. This KLIPS option has no
		effect on NETKEY, Windows or BSD stacks.]]))
	opt.cfgvalue = AbstractValue.cfgvalue
	opt.rmempty = false
	opt:depends("protostack", "klips")
	opt.optional = true
	opt.default = "yes"
	opt.disabled = "no"
	opt.enabled = "yes"

	opt = sect:option(Flag, "hidetos",
		translate("hidetos - Don't copy TOS field to containing packet"),
		translate([[Whether a tunnel packet's TOS field should be set
		to 0 rather than copied from the user packet inside This KLIPS
		option has no effect on NETKEY, Windows or BSD stacks.]]))
	opt.cfgvalue = AbstractValue.cfgvalue
	opt.rmempty = false
	opt:depends("protostack", "klips")
	opt.optional = true
	opt.default = "yes"
	opt.disabled = "no"
	opt.enabled = "yes"

	opt = sect:option(Value, "keep_alive",
		translate("keep_alive - Delay (in seconds) for NAT-T keep-alive packets"))
	opt:depends("force_keepalive", "yes")
	opt.optional = true

	opt = sect:option(Value, "myid",
		translate("myid - The identity to be used for %myid."),
		translate([[%myid is used in the implicit policy group conns
		and can be used as an identity in explicit conns. If
		unspecified, %myid is set to the IP address in %defaultroute
		(if that is supported by a TXT record in its reverse domain),
		or otherwise the system's hostname (if that is supported by a
		TXT record in its forward domain), or otherwise it is
		undefined. An explicit value generally starts with ``@''.]]))
	opt.optional = true

	opt = sect:option(Value, "nhelpers",
		translate("nhelpers - Number of pluto helper processes to start"),
		translate([[Pluto will start (n-1) of them, where n is the
		number of CPU's you have (including hypherthreaded CPU's). A
		value of 0 forces pluto to do all operations in the main
		process. A value of -1 tells pluto to perform the above
		calculation. Any other value forces the number to that
		amount.]]))
	opt.optional = true
	opt.datatype = "uinteger"

	opt = sect:option(Value, "overridemtu",
		translate("overridemtu - Force MTU"),
		translate([[Value that the MTU of the ipsecn interface(s)
		should be set to, overriding IPsec's (large) default. This
		parameter is needed only in special situations. This KLIPS
		option has no effect on NETKEY, Windows or BSD stacks.]]))
	opt:depends("protostack", "klips")
	opt.optional = true
	opt.datatype = "uinteger"

	opt = sect:option(Value, "plutoopts",
		translate("plutoopts - Additional options to pass to pluto upon startup"),
		translate([[See ipsec_pluto(8) for available options.]]))
	opt.optional = true

	opt = sect:option(Flag, "plutorestartoncrash",
		translate("plutorestartoncrash - Restart pluto after crash"),
		translate([[When disabled, prevents pluto from restarting after
		it crashes.  This option should only be used when debugging a
		crasher. It will prevent overwriting a core file on a new
		start, or a cascade of core files.  This option is also
		required if used with plutostderrlog= to avoid clearing the
		logs of the crasher.]]))
	opt.cfgvalue = AbstractValue.cfgvalue
	opt.rmempty = false
	opt.optional = true
	opt.default = "yes"
	opt.disabled = "no"
	opt.enabled = "yes"

	opt = sect:option(FileBrowser, "plutostderrlog",
		translate("plutostderrlog - Log to file instead of syslog"))
	opt.optional = true

	opt = sect:option(Flag, "pluto",
		translate("pluto - Start pluto automatically"),
		translate([[Disabling automatic start is useful only in special
		circumstances.]]))
	opt.cfgvalue = AbstractValue.cfgvalue
	opt.rmempty = false
	opt.optional = true
	opt.default = "yes"
	opt.disabled = "no"
	opt.enabled = "yes"

	opt = sect:option(Flag, "plutowait",
		translate("plutowait - Serialize startup"),
		translate([[Wait for each negotiation attempt that is part of
		startup to finish before proceeding with the next.]]))
	opt.cfgvalue = AbstractValue.cfgvalue
	opt.rmempty = false
	opt.optional = true
	opt.default = "no"
	opt.disabled = "no"
	opt.enabled = "yes"

	opt = sect:option(Value, "postpluto",
		translate("postpluto - Shell command to run after starting Pluto"),
		translate([[For example, a command to remove a decrypted copy
		of the ipsec.secrets file.  The command is run in a very simple
		way; complexities like I/O redirection are best hidden within a
		script. Any output is redirected for logging, so running
		interactive commands is difficult unless they use /dev/tty or
		equivalent for their interaction.]]))
	opt.optional = true

	opt = sect:option(Value, "prepluto",
		translate("prepluto - Shell command to run before starting Pluto"),
		translate([[For example, a command to decrypt an encrypted copy
		of the ipsec.secrets file. The command is run in a very simple
		way; complexities like I/O redirection are best hidden within a
		script. Any output is redirected for logging, so running
		interactive commands is difficult unless they use /dev/tty or
		equivalent for their interaction.]]))
	opt.optional = true

	opt = sect:option(ListValue, "protostack",
		translate("protostack - Protocol stack to use"),
		translate([[Valid values are "auto", "klips", "netkey" and
		"mast". The "mast" stack is a variation for the klips
		stack.]]))
	opt.optional = true
	opt:value("auto")
	opt:value("klips")
	opt:value("mast")
	opt:value("netkey")

	opt = sect:option(Value, "syslog",
		translate("syslog - The syslog2 “facility” name and priority to use for startup/shutdown log messages"))
	opt.optional = true
	opt.default = "daemon.error"

	opt = sect:option(Flag, "uniqueids",
		translate("uniqueids - Keep IDs unique for each participant"),
		translate([[Whether a particular participant ID should be kept
		unique, with any new (automatically keyed) connection using an
		ID from a different IP address deemed to replace all old ones
		using that ID.  Participant IDs normally are unique, so a new
		(automatically-keyed) connection using the same ID is almost
		invariably intended to replace an old one.]]))
	opt.cfgvalue = AbstractValue.cfgvalue
	opt.rmempty = false
	opt.default = "yes"
	opt.optional = true
	opt.disabled = "no"
	opt.enabled = "yes"
end

-- If advanced options are overwhelming, can split based on arg.
-- For now, always display advanced options
return build_map(true)
