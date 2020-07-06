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

-- FIXME:  Have to set cfgvalue on each Flag to prevent default from being
--         returned (which prevents it from being optional).
--         Also Flag.rmempty == "remove default" not "remove empty", which
--         is not what we want, so set false on all

local uci = require "luci.model.uci".cursor()

function build_conn_url(connname, sectname)
	if sectname then
		return luci.dispatcher.build_url(
			"admin", "services", "ipsec", "connections", connname, sectname)
	else
		return luci.dispatcher.build_url(
			"admin", "services", "ipsec", "connections", connname)
	end
end

-- Build a simple form to select the connection to configure
function build_form_select()
	local form = SimpleForm("ipsec-connection",
		translate("IPsec Connection Configuration"),
		translate([[To create a new connection, use the <a
		href="%s">status page</a>.]]):format(
			luci.dispatcher.build_url("admin", "services", "ipsec")))
	local section = form:section(SimpleSection,
		translate("Select a Connection"))
	local connfield = form:field(ListValue, "connection",
		translate("Connection to configure"))

	uci:load("ipsec")
	uci:foreach("ipsec", "ipsec_conn", function(cbisect)
			connfield:value(cbisect['.name'])
		end)

	function form.handle(self, state, data)
		if state == FORM_VALID and data.connection then
			luci.http.redirect(build_conn_url(data.connection))
			return false
		end

		return true
	end

	return form
end

-- Build the configuration form
function build_form_config(conname, sectname)
	local map = Map("ipsec", translate("IPsec Connection Configuration"),
		translate([[Configuration parameters for establishing inbound
		and/or outbound connections secured using IPsec.  Most
		parameters are direct mappings to their equivalents in
		ipsec.conf(5), which are displayed in parentheses for
		reference.]]))

	local switcher = Node()
	switcher.build_conn_url = build_conn_url
	switcher.template = "ipsec/pageswitch"
	switcher.conname = conname
	switcher.categories = {
		{ "left", translate("Left") },
		{ "shared", translate("Shared") },
		{ "right", translate("Right") }
	}
	switcher.category = sectname
	map:append(switcher)

	if sectname == "shared" then
		add_section_shared(map, conname)
	elseif sectname == "left" then
		add_section_end(map, conname, "left")
	elseif sectname == "right" then
		add_section_end(map, conname, "right")
	elseif not sectname then
		add_section_basic(map, conname)
	else
		luci.http.status(404, "Not Found")
		add_section_basic(map, conname)
	end

	return map
end

function add_section_shared(map, conname)
	local sect = map:section(NamedSection,
		conname, "ipsec_conn",
		translate("Shared Options for %s"):format(conname),
		translate([[Configuration options which apply to both ends of a
		connection.]]))

	add_options_shared_basic(sect)
	add_options_shared_advanced(sect)

	return sect
end


function add_section_end(map, conname, endname)
	local enddname
	if endname == "left" then
		enddname = translate("Left Endpoint Options for %s")
	else
		enddname = translate("Right Endpoint Options for %s")
	end

	local sect = map:section(NamedSection,
		conname, "ipsec_conn",
		enddname:format(conname),
		translate([[Configuration options which apply to the to the
		"%s" side of the connection.  The definition of sides is up to
		you (the user).  The IPsec daemons will determine which side of
		the connection each system is on heuristically at runtime based
		on the addresses assigned to each side.]]))

	add_options_end_basic(sect, endname)
	add_options_end_advanced(sect, endname)

	return sect
end

function add_section_basic(map, conname)
	local sect = map:section(NamedSection,
		conname, "ipsec_conn",
		translate("Basic Connection Configuration for %s"):format(conname),
		translate([[Configuration options relevant to common connection
		configuration scenarios.]]))

	add_options_shared_basic(sect)
	add_options_end_basic(sect, "left")
	add_options_end_basic(sect, "right")

	return sect
end

function add_options_shared_basic(sect)
	local opt

	opt = sect:option(ListValue, "auto",
		translate("auto - Operation, if any, to be done at IPsec startup"),
		translate([[Currently-accepted values are add (signifying an ipsec
		auto --add), route (signifying that plus an ipsec auto --route), start
		(signifying that plus an ipsec auto --up), manual (signifying an ipsec
		manual --up), and ignore (also the default) (signifying no automatic
		startup operation). See the config setup discussion below. Relevant
		only locally, other end need not agree on it (but in general, for an
		intended-to-be-permanent connection, both ends should use auto=start
		to ensure that any reboot causes immediate renegotiation).]]))
	opt.default = "ignore"
	opt.optional = true
	opt:value("add")
	opt:value("ignore")
	opt:value("manual")
	opt:value("route")
	opt:value("start")

	opt = sect:option(ListValue, "authby",
		translate("authby - How the two security gateways should authenticate each other."),
		translate([[Acceptable values are secret for shared secrets,
		rsasig for RSA digital signatures (the default), and never if
		negotiation is never to be attempted or accepted (useful for
		shunt-only conns). Digital signatures are superior in every way
		to shared secrets.]]))
	opt.default = "rsasig"
	opt.optional = true
	opt:value("secret")
	opt:value("rsasig")
	opt:value("never")

	opt = sect:option(Flag, "compress",
		translate("compress - Propose IPComp compression"),
		translate([[Whether IPComp compression of content is proposed on the
		connection (link-level compression does not work on encrypted data, so
		to be effective, compression must be done before encryption);
		The two ends need not
		agree. A value of yes causes IPsec to propose both compressed and
		uncompressed, and prefer compressed. A value of no prevents IPsec from
		proposing compression; a proposal to compress will still be
		accepted.]]))
	opt.cfgvalue = AbstractValue.cfgvalue
	opt.rmempty = false
	opt.default = "no"
	opt.optional = true
	opt.disabled = "no"
	opt.enabled = "yes"

	opt = sect:option(ListValue, "connaddrfamily",
		translate("connaddrfamily - Address family"))
	opt.optional = true
	opt:value("ipv4")
	opt:value("ipv6")

	opt = sect:option(ListValue, "type",
		translate("type - Type of connection"),
		translate([[Currently the accepted values are tunnel signifying
		a host-to-host, host-to-subnet, or subnet-to-subnet tunnel;
		transport, signifying host-to-host transport mode; passthrough,
		signifying that no IPsec processing should be done at all;
		drop, signifying that packets should be discarded; and reject,
		signifying that packets should be discarded and a diagnostic
		ICMP returned.]]))
	opt.default = "tunnel"
	opt.optional = true
	opt:value("drop")
	opt:value("passthrough")
	opt:value("reject")
	opt:value("transport")
	opt:value("tunnel")
end


function add_options_shared_advanced(sect)
	local opt

	opt = sect:option(Flag, "aggrmode",
		translate("aggrmode - Use Aggressive Mode instead of Main Mode"),
		translate([[Aggressive Mode is less secure, and vulnerable to Denial
		Of Service attacks. It is also vulnerable to brute force attacks with
		software such as ikecrack.  It should not be used, and it should
		especially not be used with XAUTH and group secrets (PSK). If the
		remote system administrator insists on staying irresponsible, enable
		this option.]]))
	opt.cfgvalue = AbstractValue.cfgvalue
	opt.rmempty = false
	opt.default = "no"
	opt.optional = true
	opt.disabled = "no"
	opt.enabled = "yes"

	opt = sect:option(Value, "ah",
		translate("ah - AH authentication algorithm to be used for the connection."),
		translate([[The options must be suitable as a value of
		ipsec_spi(8)'s --ah option. The default is not to use AH. If
		for some (invalid) reason you still think you need AH, please
			use esp with the null encryption cipher instead. Note
			also that not all ciphers available to the kernel (eg
			through CryptoAPI) are necessarilly supported here.]]))
	opt.optional = true

	opt = sect:option(Flag, "disablearrivalcheck",
		translate("disablearrivalcheck - Disable KLIPS's normal tunnel-exit check"),
		translate([[Whether KLIPS's check that a packet emerging from a tunnel
		has plausible addresses in its header should be disabled.  Tunnel-exit
		checks improve
		security and do not break any normal configuration. Relevant only
		locally, other end need not agree on it.]]))
	opt.cfgvalue = AbstractValue.cfgvalue
	opt.rmempty = false
	opt.default = "no"
	opt.optional = true
	opt.disabled = "no"
	opt.enabled = "yes"

	opt = sect:option(ListValue, "dpdaction",
		translate("dpdaction - Action when DPD peer is declared dead"),
		translate([[hold (default) means the eroute will be put into
		%hold status, while clear means the eroute and SA with both be
		cleared. restart means the the SA will immediately be
		renegotiated, and restart_by_peer means that ALL SA's to the
		dead peer will renegotiated.]]))
	opt.default = "hold"
	opt.optional = true
	opt:value("clear")
	opt:value("hold")
	opt:value("restart")
	opt:value("restart_by_peer")

	opt = sect:option(Value, "dpddelay",
		translate("dpddelay - Delay (in seconds) between Dead Peer Dectection keepalives"),
		translate([[Sets the delay (in seconds) between Dead Peer Dectection
		(RFC 3706) keepalives (R_U_THERE, R_U_THERE_ACK) that are sent for
		this connection. If dpddelay is set, dpdtimeout
		also needs to be set.]]))
	opt.optional = true
	opt.default = "30"
	opt.datatype = "uinteger"

	opt = sect:option(Value, "dpdtimeout",
		translate("dpdtimeout - Set the length of time (in seconds) we will idle without hearing either an R_U_THERE poll from our peer, or an R_U_THERE_ACK reply."),
		translate([[Set the length of time (in seconds) we will idle
		without hearing either an R_U_THERE poll from our peer, or an
		R_U_THERE_ACK reply. After this period has elapsed with no
		response and no traffic, we will declare the peer dead, and
		remove the SA (default 120 seconds). If dpdtimeout is set,
		dpdaction also needs to be set.]]))
	opt.optional = true
	opt.datatype = "uinteger"

	opt = sect:option(ListValue, "failureshunt",
		translate("failureshunt - Failed packet action"),
		translate([[what to do with packets when negotiation fails. The
		default is none: no shunt; passthrough, drop, and reject have the
		obvious meanings.]]))
	opt.default = "none"
	opt.optional = true
	opt:value("drop")
	opt:value("none")
	opt:value("passthrough")
	opt:value("reject")

	opt = sect:option(Flag, "forceencaps",
		translate("forceencaps - Force RFC-3948 encapsulation (for NAT)"),
		translate([[In some cases, for example when ESP packets are
		filtered or when a broken IPsec peer does not properly
		recognise NAT, it can be useful to force RFC-3948
		encapsulation. forceencaps=yes forces the NAT detection code to
		lie and tell the remote peer that RFC-3948 encapsulation (ESP
		in UDP port 4500 packets) is required. For this option to have
		any effect, the setup section option nat_traversal=yes needs to
		be set.]]))
	opt.cfgvalue = AbstractValue.cfgvalue
	opt.rmempty = false
	-- Not in same section
	-- opt:depends("nat_traversal", "yes")
	opt.default = "no"
	opt.optional = true
	opt.disabled = "no"
	opt.enabled = "yes"

	opt = sect:option(DynamicList, "ike",
		translate("ike - IKE encryption/authentication algorithm to be used for the connection (phase 1 aka ISAKMP SA)."),
		translate([[The format is
		"cipher-hash;modpgroup" Any left out
		option will be filled in with all allowed default options.
		If an ike option is specified, no
		other received proposals will be accepted. Formerly there was a
		distinction (by using a "!" symbol) between "strict mode" or not. That
		mode has been obsoleted. If an ike option is specified, the mode is
		always strict, meaning no other received proposals will be accepted.
		Some examples are ike=3des-sha1,aes-sha1, ike=aes,
		ike=aes128-md5;modp2048, ike=aes128-sha1;dh22,
		ike=3des-md5;modp1024,aes-sha1;modp1536 or ike=modp1536. The options
		must be suitable as a value of ipsec_spi(8)'s --ike option. The default
		is to use IKE, and to allow all combinations of: cipher: 3des or aes
		hash: sha1 or md5 pfsgroup (DHgroup): modp1024 or modp1536]]))
	opt.optional = true

	opt = sect:option(Value, "ikelifetime",
		translate("ikelifetime - How long the keying channel of a connection (buzzphrase: “ISAKMP SA”) should last before being renegotiated."),
		translate([[Acceptable values as for keylife (default set by
		ipsec_pluto(8), currently 1h, maximum 24h). The
		two-ends-disagree case is similar to that of keylife.]]))
	opt.optional = true

	opt = sect:option(ListValue, "ikev2",
		translate("ikev2 - IKEv2 (RFC4309) settings to be used."),
		translate([[Currently the
		accepted values are permit, signifying no IKEv2 should
		be transmitted, but will be accepted if the other ends initiates to us
		with IKEv2; never or no signifying no IKEv2 negotiation should be
		transmitted or accepted; propose or yes signifying that we permit
		IKEv2, and also use it as the default to initiate; insist, signifying
		we only accept and receive IKEv2 - IKEv1 negotiations will be
		rejected.]]))
	opt.optional = true
	opt:value("insist")
	opt:value("never")
	opt:value("permit")
	opt:value("propose")

	opt = sect:option(Value, "keyingtries",
		translate("keyingtries - Number of attempts to negotiate a connection"),
		translate([[How many attempts (a whole number or %forever)
		should be made to negotiate a connection, or a replacement for
		one, before giving up (default %forever). The value %forever
		means “never give up” (obsolete: this can be written 0).
		Relevant only locally, other end need not agree on it.]]))
	opt.optional = true
	opt:value("%forever")

	opt = sect:option(Value, "metric",
		translate("metric - Route metric for IPsec interface"),
		translate([[Set the metric for the routes to the ipsecX or mastX
		interface. This makes it possible to do host failover from another
		interface to ipsec using route management. This value is passed to the
		_updown scripts as PLUTO_METRIC. This option is only available with
		KLIPS or MAST on Linux. Acceptable values are positive numbers, with
		the default being 1.]]))
	opt.optional = true
	opt.datatype = "uinteger"
	opt.default = "1"

	opt = sect:option(DynamicList, "modecfgdns",
		translate("modecfgdns - IP address for DNS servers for the client to use."))
	opt.optional = true
	opt.datatype = "ipaddr"

	opt = sect:option(DynamicList, "modecfgwins",
		translate("modecfgwins - IP address for WINS servers for the client to use."))
	opt.optional = true
	opt.datatype = "ipaddr"

	opt = sect:option(Flag, "modecfgpull",
		translate("modecfgpull - Pull the Mode Config network information from the server."))
	opt.cfgvalue = AbstractValue.cfgvalue
	opt.rmempty = false
	opt.default = "no"
	opt.optional = true
	opt.disabled = "no"
	opt.enabled = "yes"

	opt = sect:option(Value, "mtu",
		translate("mtu - Connection MTU"),
		translate([[Set the MTU for the route(s) to the remote endpoint
		and/or subnets. This is sometimes required when the overhead of
		the IPsec encapsultion would cause the packet the become too
		big for a router on the path. Since IPsec cannot trust any
		unauthenticated ICMP messages, PATH MTU discovery does not
		work. This can also be needed when using "6to4" IPV6
		deployments, which adds another header on the packet size.
		Acceptable values are positive numbers. There is no
		default.]]))
	-- Depends in general section
	-- opt:depends("protostack", "klips")
	-- opt:depends("protostack", "mast")
	opt.optional = true
	opt.datatype = "uinteger"

	opt = sect:option(Flag, "overlapip",
		translate("overlapip - Allow subnets to overlap"),
		translate([[Determines if, when *subnet=vhost: is
		used, the virtual IP claimed by this connection can overlap with
		states created from other connections.]]))
	opt.cfgvalue = AbstractValue.cfgvalue
	opt.rmempty = false
	opt.optional = true
	opt.default = "no"
	opt.disabled = "no"
	opt.enabled = "yes"

	opt = sect:option(Flag, "pfs",
		translate("pfs - Enable Perfect Forward Secrecy"),
		translate([[Enables Perfect Forward Secrecy of keys on the
		connection's keying channel (with PFS, penetration of the
		key-exchange protocol does not compromise keys negotiated
		earlier); Since there is no reason to ever refuse PFS, Openswan
		will allow a connection defined with pfs=no to use PFS
		anyway.]]))
	opt.cfgvalue = AbstractValue.cfgvalue
	opt.rmempty = false
	opt.default = "yes"
	opt.optional = true
	opt.disabled = "no"
	opt.enabled = "yes"

	opt = sect:option(ListValue, "phase2",
		translate("phase2 - Sets the type of SA that will be produced."),
		translate([[Valid options are: esp for encryption, and ah for
		authentication only, ah+esp for both.]]))
	opt.optional = true
	opt.default = "esp"
	opt:value("ah+esp")
	opt:value("esp")
	opt:value("ah")

	opt = sect:option(Value, "phase2alg",
		translate("phase2alg - Specifies the algorithms that will be offered/accepted for a phase2 negotiation."),
		translate([[If not specified, a secure set of defaults will be
		used. Sets are separated using comma's.]]))
	opt.optional = true

	opt = sect:option(ListValue, "rekey",
		translate("rekey - Renegotiate connection when it is about to expire"),
		translate([[The two ends need not agree, but while a value of no
		prevents Pluto from requesting renegotiation, it does not prevent
		responding to renegotiation requested from the other end, so no will
		be largely ineffective unless both ends agree on it.]]))
	opt.default = "yes"
	opt.optional = true
	opt.disabled = "no"
	opt.enabled = "yes"

	opt = sect:option(Value, "rekeyfuzz",
		translate("rekeyfuzz - Maximum percentage by which rekeymargin should be randomly increased"),
		translate([[Maximum percentage by which rekeymargin should be
		randomly increased to randomize rekeying intervals (important
		for hosts with many connections); acceptable values are an
			integer, which may exceed 100, followed by a `%'
			(default set by ipsec_pluto8, currently 100%).  The
			value of rekeymargin, after this random increase, must
			not exceed salifetime. The value 0% will suppress time
			randomization. Relevant only locally, other end need
			not agree on it.]]))
	opt.optional = true

	opt = sect:option(Value, "rekeymargin",
		translate("rekeymargin - Time before expiry when attempts to negotiate a replacement begin."),
		translate([[How long before connection expiry or keying-channel expiry
		should attempts to negotiate a replacement begin; acceptable values as
		for salifetime (default 9m). Relevant only locally, other end need not
		agree on it.]]))
	opt.optional = true

	opt = sect:option(ListValue, "remote_peer_type",
		translate("remote_peer_type - Set the remote peer type."),
		translate([[This can enable additional processing during the
		IKE negotiation. Acceptable values are cisco or ietf (the
		default). When set to cisco, support for Cisco IPsec gateway
		redirection and Cisco obtained DNS and domainname are enabled.
		This includes automatically updating (and restoring)
		/etc/resolv.conf.  These options require that XAUTH is also
		enabled on this connection.]]))
	opt.default = "ietf"
	opt.optional = true
	opt:value("cisco")
	opt:value("ietf")

	opt = sect:option(Value, "salifetime",
		translate("salifetime - Security Association Lifetime"),
		translate([[How long a particular instance of a connection (a set of
		encryption/authentication keys for user packets) should last, from
		successful negotiation to expiry; acceptable values are an integer
		optionally followed by s (a time in seconds) or a decimal number
		followed by m, h, or d (a time in minutes, hours, or days
		respectively) (default 8h, maximum 24h). Normally, the connection is
		renegotiated (via the keying channel) before it expires. The two ends
		need not exactly agree on salifetime, although if they do not, there
		will be some clutter of superseded connections on the end which thinks
		the lifetime is longer.]]))
	opt.optional = true

	opt = sect:option(ListValue, "sareftrack",
		translate("sareftrack - Set the method of tracking reply packets with SArefs when using an SAref compatible stack."),
		translate([[Currently only the mast stack supports this.
		Acceptable values are yes (the default), no or conntrack. This
		option is ignored when SArefs are not supported. This option is
		passed as PLUTO_SAREF_TRACKING to the updown script which makes
		the actual decisions whether to perform any
		iptables/ip_conntrack manipulation. A value of yes means that
		an IPSEC mangle table will be created. This table will be used
		to match reply packets. A value of conntrack means that
		additionally, subsequent packets using this connection will be
		marked as well, reducing the lookups needed to find the proper
		SAref by using the ip_conntrack state. A value of no means no
		IPSEC mangle table is created, and SAref tracking is left to a
		third-party (kernel) module. In case of a third party module,
		the SArefs can be relayed using the HAVE_STATSD deamon.]]))
	opt:depends("protostack", "mast")
	opt.default = "yes"
	opt.optional = true
	opt:value("yes")
	opt:value("no")
	opt:value("conntrack")
end


function add_options_end_basic(sect, endname)
	local opt

	opt = sect:option(Value, endname.."",
		string.format(translate("%s - IP address of the participant's public network interface"), endname),
		translate([[In any form accepted
		by ipsec_ttoaddr(3). Currently, IPv4 and IPv6 IP addresses are
		supported. There are several magic values. If it is
		%defaultroute, and the config setup section's, interfaces
		specification contains %defaultroute, left will be filled in
		automatically with the local address of the default-route
		interface (as determined at IPsec startup time); this also
		overrides any value supplied for leftnexthop. (Either left or
		right may be %defaultroute, but not both.) The value %any
		signifies an address to be filled in (by automatic keying)
		during negotiation. The value %opportunistic signifies that
		both left and leftnexthop are to be filled in (by automatic
		keying) from DNS data for left's client. The value can also
		contain the interface name, which will then later be used to
		obtain the IP address from to fill in. For example %ppp0 The
		values %group and %opportunisticgroup makes this a policy
		group conn: one that will be instantiated into a regular or
		opportunistic conn for each CIDR block listed in the policy
		group file with the same name as the conn.]]))
	-- Note:  %defaultroute doesn't fit host datatype
	-- opt.datatype = "host"
	opt.optional = false

	opt = sect:option(FileBrowser, endname.."cert",
		string.format(translate("%scert - Certificate for authentication"), endname),
		translate([[If you are using leftrsasigkey=%cert this defines
		the certificate you would like to use. It should point to a
		X.509 encoded certificate file. If you do not specify a full
		pathname, by default it will look in /etc/ipsec.d/certs. If
		openswan has been compiled with USE_LIBNSS=true, then openswan
		will also check the NSS database for RSA keys. These can be
		software or hardware.]]))
	opt.optional = true
	opt.default_path = "/etc/ipsec.d/certs"

	opt = sect:option(Value, endname.."id",
		string.format(translate("%sid - Authentication Identification"), endname),
		translate([[how the left participant should be identified for
		authentication; defaults to left. Can be an IP address (in any
		ipsec_ttoaddr(3) syntax) or a fully-qualified domain name
		preceded by @ (which is used as a literal string and not
		resolved). The magic value %fromcert causes the ID to be set
		to a DN taken from a certificate that is loaded. Prior to
		2.5.16, this was the default if a certificate was specified.
		The magic value %none sets the ID to no ID. This is included
		for completeness, as the ID may have been set in the default
		conn, and one wishes for it to default instead of
		being explicitly set. The magic value %myid stands for
		the current setting of myid. This is set in config
		setup or by ipsec_whack(8), or, if not set, it is the
		IP address in %defaultroute (if that is supported by a
		TXT record in its reverse domain), or otherwise it is
		the system's hostname (if that is supported by a TXT
		record in its forward domain), or otherwise it is
		undefined.]]))
	opt.optional = true
	opt:value("%fromcert")
	opt:value("%myid")
	opt:value("%none")

	opt = sect:option(Value, endname.."protoport",
		string.format(translate("%sprotoport - Port Selectors"), endname),
		translate([[Allowed protocols and ports over connection The
		argument is in the form protocol, which can be a number or a
		name that will be looked up in /etc/protocols, such as
		protoport=icmp, or in the form of protocol/port, such as
		tcp/smtp. Ports can be defined as a number (eg. 25) or as a
		name (eg smtp) which will be looked up in /etc/services. A
		special keyword %any can be used to allow all ports of a
		certain protocol. The most common use of this option is for
		L2TP connections to only allow l2tp packets (UDP port 1701),
		eg: protoport=17/1701. Some clients, notably older Windows XP
		and some Mac OSX clients, use a random high port as source
		port. In those cases rightprotoport=17/%any can be used to
		allow all UDP traffic on the connection. Note that this option
		is part of the proposal, so it cannot be arbitrarily left out
		if one end does not care about the traffic selection over this
		connection - both peers have to agree. The Port Selectors show
		up in the output of ipsec eroute and ipsec auto --status
		eg:"l2tp":
		193.110.157.131[@aivd.xelernace.com]:7/1701...%any:17/1701 This
		option only filters outbound traffic. Inbound traffic selection
		must still be based on firewall rules activated by an updown
		script. The variablees $PLUTO_MY_PROTOCOL,
		$PLUTO_PEER_PROTOCOL, $PLUTO_MY_PORT, and $PLUTO_PEER_PORT are
		available for use in updown scripts. Older workarounds for bugs
		involved a setting of 17/0 to denote any single UDP port (not
		UDP port 0). Some clients, most notably OSX, uses a random high
		port, instead of port 1701 for L2TP.]]))
	opt.optional = true

	opt = sect:option(Value, endname.."sourceip",
		string.format(translate("%ssourceip - Connection source IP"), endname),
		translate([[IP address for this host to use when
		transmitting a packet to the other side of this link. Relevant
		only locally, the other end need not agree. This option is
		used to make the gateway itself use its internal IP, which is
		part of the leftsubnet, to communicate to the rightsubnet or
		right. Otherwise, it will use its nearest IP address, which is
		its public IP address. This option is mostly used when
		defining subnet-subnet connections, so that the gateways can
		talk to each other and the subnet at the other end, without
		the need to build additional host-subnet, subnet-host and
		host-host tunnels. Both IPv4 and IPv6 addresses are
		supported.]]))
	opt.datatype = "host"
	opt.optional = true

	opt = sect:option(DynamicList, endname.."subnets",
		string.format(translate("%ssubnets - Private subnets behind the participant"), endname),
		translate([[Subnets expressed as network/netmask (actually, any
		form acceptable to ipsec_ttosubnet(3)); Currentlly, IPv4 and
		IPv6 ranges are supported. if omitted, essentially assumed to
		be left/32, signifying that the left end of the connection goes
		to the left participant only.  If subnets is specified for both
		left and right, all combinations of subnet tunnels will be
		instantiated.]]))
	opt.optional = true
end


function add_options_end_advanced(sect, endname)
	local opt

	opt = sect:option(Value, endname.."ca",
		string.format(translate("%sca - Certificate Authority"), endname),
		translate([[specifies the authorized Certificate Authority
		(CA) that signed the certificate of the peer. If undefined, it
		defaults to the CA that signed the certificate specified in
		leftcert. The special rightca=%same is implied when not
		specifying a rightca and means that only peers with
		certificates signed by the same CA as the leftca will be
		allowed. This option is only useful in complex multi CA
		certificate situations. When using a single CA, it can be
		safely omitted for both left and right.]]))
	opt.optional = true
	opt:value("%same")

	opt = sect:option(Flag, endname.."modecfgclient",
		string.format(translate("%smodecfgclient - Is Mode Config client"), endname),
		translate([[A Mode Config client can receive network
		configuration from the server.]]))
	opt.cfgvalue = AbstractValue.cfgvalue
	opt.rmempty = false
	opt.default = "no"
	opt.optional = true
	opt.disabled = "no"
	opt.enabled = "yes"

	opt = sect:option(Flag, endname.."modecfgserver",
		string.format(translate("%smodecfgserver - Is Mode Config server."), endname),
		translate([[A Mode Config server can push network
		configuration to the client.]]))
	opt.cfgvalue = AbstractValue.cfgvalue
	opt.rmempty = false
	opt.default = "no"
	opt.optional = true
	opt.disabled = "no"
	opt.enabled = "yes"

	opt = sect:option(Value, endname.."nexthop",
		string.format(translate("%snexthop - Next-hop gateway IP address to the public network."), endname),
		translate([[Defaults to %direct (meaning right). If the value
		is to be overridden by the left=%defaultroute method (see
		above), an explicit value must not be given. If that method is
		not being used, but leftnexthop is %defaultroute, and
		interfaces=%defaultroute is used in the config setup section,
		the next-hop gateway address of the default-route interface
		will be used. The magic value %direct signifies a value to be
		filled in (by automatic keying) with the peer's address.
		Relevant only locally, other end need not agree on it.]]))
	opt.optional = true

	opt = sect:option(Value, endname.."rsasigkey",
		string.format(translate("%srsasigkey - Public key for authentication"), endname),
		translate([[The left participant's public key for RSA
		signature authentication, in RFC 2537 format using
		ipsec_ttodata(3) encoding. The magic value %none means the same
		as not specifying a value (useful to override a default). The
		value %dnsondemand (the default) means the key is to be
		fetched from DNS at the time it is needed. The value
		%dnsonload means the key is to be fetched from DNS at the time
		the connection description is read from ipsec.conf; currently
		this will be treated as %none if right=%any or
		right=%opportunistic. The value %dns is currently treated as
		%dnsonload but will change to %dnsondemand in the future. The
		identity used for the left participant must be a specific
		host, not %any or another magic value. The value %cert will
		load the information required from a certificate defined in
		%leftcert and automatically define leftid for you. Caution: if
		two connection descriptions specify different public keys for
		the same leftid, confusion and madness will ensue.]]))
	opt.optional = true
	opt.default = "%dnsondemand"
	opt:value("%cert")
	opt:value("%dnsondemand")
	opt:value("%dnsonload")
	opt:value("%none")

	opt = sect:option(Value, endname.."rsasigkey2",
		string.format(translate("%srsasigkey2 - Second public key for authentication."), endname),
		translate([[If present, a second public key. Either key can
		authenticate the signature, allowing for key rollover.]]))
	opt.optional = true
	opt:value("%cert")
	opt:value("%dnsondemand")
	opt:value("%dnsonload")
	opt:value("%none")

	opt = sect:option(ListValue, endname.."sendcert",
		string.format(translate("%ssendcert - Send X.509 certificate"), endname),
		translate([[This option configures when Openswan will send
		X.509 certificates to the remote host. Acceptable values are
		yes|always (signifying that we should always send a
		certificate), sendifasked (signifying that we should send a
		certificate if the remote end asks for it), and no|never
		(signifying that we will never send a X.509 certificate). The
		default for this option is sendifasked which may break
		compatibility with other vendor's IPSec implementations, such
		as Cisco and SafeNet. If you find that you are getting errors
		about no ID/Key found, you likely need to set this to always.
		This per-conn option replaces the obsolete global nocrsend
		option.]]))
	opt.default = "sendifasked"
	opt.optional = true
	opt:value("always")
	opt:value("sendifasked")
	opt:value("forcedtype")
	opt:value("never")

	opt = sect:option(Value, endname.."updown",
		string.format(translate("%supdown - Up/Down Script"), endname),
		translate([[What "updown" script to run to adjust routing
		and/or firewalling when the status of the connection changes
		(default ipsec _updown). May include positional parameters
		separated by white space (although this requires enclosing the
		whole string in quotes); including shell metacharacters is
		unwise. An example to enable routing when using the NETKEY
		stack, one can use: "ipsec _updown --route yes".  See
		ipsec_pluto(8) for details. Relevant only locally, other end
		need not agree on it.]]))
	opt.optional = true
	opt.default = "ipsec _updown"

	opt = sect:option(Flag, endname.."xauthclient",
		string.format(translate("%sxauthclient - Is XAUTH client"), endname),
		translate([[The xauth connection will
		have to be started interactively and cannot be configured
		using auto=start. Instead, it has to be started from the
		commandline using ipsec auto --up connname. You will then be
		prompted for the username and password. To setup an XAUTH
		connection non-interactively, which defeats the whole purpose
		of XAUTH, but is regularly requested by users, it is possible
		to use a whack command - ipsec whack --name baduser
		--ipsecgroup-xauth --xauthname badusername --xauthpass
		password --initiate The other side of the connection should be
		configured as rightxauthserver.]]))
	opt.cfgvalue = AbstractValue.cfgvalue
	opt.rmempty = false
	opt.default = "no"
	opt.optional = true
	opt.disabled = "no"
	opt.enabled = "yes"

	opt = sect:option(Flag, endname.."xauthserver",
		string.format(translate("%sxauthserver - Is XAUTH server"), endname),
		translate([[This can use PAM for authentication or md5
		passwords in /etc/ipsec.d/passwd. These are additional
		credentials to verify the user identity, and should not be
		confused with the XAUTH group secret, which is just a regular
		PSK defined in ipsec.secrets. The other side of the connection
		should be configured as rightxauthclient. XAUTH connections
		cannot rekey, so rekey=no should be specified in this conn. For
		further details on how to compile and use XAUTH, see
		README.XAUTH.]]))
	opt.cfgvalue = AbstractValue.cfgvalue
	opt.rmempty = false
	opt.default = "no"
	opt.optional = true
	opt.disabled = "no"
	opt.enabled = "yes"

	opt = sect:option(Value, endname.."xauthusername",
		string.format(translate("%sxauthusername - XAUTH Username"), endname),
		translate([[The XAUTH username associated with this XAUTH
		connection. The XAUTH password can be configured in the
		ipsec.secrets file.]]))
	opt.optional = true
end

-- If a connection has not been selected, present the choices
if #arg == 0 then
	return build_form_select()
else
	return build_form_config(arg[1], arg[2])
end
