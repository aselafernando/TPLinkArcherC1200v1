<!--
Copyright (C) 2015, Broadcom Corporation. All Rights Reserved.

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.


<<Broadcom-WL-IPTag/Open:>>

$Id: security.asp,v 1.54 2011-01-11 18:43:43 $
-->

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html lang="en">
<head>
<title>Broadcom Home Gateway Reference Design: Security</title>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script language="JavaScript" type="text/javascript" src="overlib.js"></script>
<script language="JavaScript" type="text/javascript">
<!--
function wl_key_update()
{
	var mode = document.forms[0].wl_auth_mode[document.forms[0].wl_auth_mode.selectedIndex].value;
	var wep = document.forms[0].wl_wep[document.forms[0].wl_wep.selectedIndex].value;
	var wpa= document.forms[0].wl_akm_wpa[document.forms[0].wl_akm_wpa.selectedIndex].value;
	var psk = document.forms[0].wl_akm_psk[document.forms[0].wl_akm_psk.selectedIndex].value;
	var wl_ibss = <% wl_ibss_mode(); %>;
	var wpa2 = document.forms[0].wl_akm_wpa2[document.forms[0].wl_akm_wpa2.selectedIndex].value;
	var psk2 = document.forms[0].wl_akm_psk2[document.forms[0].wl_akm_psk2.selectedIndex].value;
	var brcm_psk = document.forms[0].wl_akm_brcm_psk[document.forms[0].wl_akm_brcm_psk.selectedIndex].value;
	var i, cur, algos;

	/* enable network key 1 to 4 */
	if (wep == "enabled") {
		if (document.forms[0].wl_akm_wpa.disabled == 0 && wpa == "enabled" ||
			document.forms[0].wl_akm_psk.disabled == 0 && psk == "enabled"
			|| document.forms[0].wl_akm_wpa2.disabled == 0 && wpa2 == "enabled"
			|| document.forms[0].wl_akm_psk2.disabled == 0 && psk2 == "enabled"
			|| document.forms[0].wl_akm_brcm_psk.disabled == 0 && brcm_psk == "enabled"
			|| mode == "radius") {
			document.forms[0].wl_key1.disabled = 1;
			document.forms[0].wl_key4.disabled = 1;
		}
		else {
			document.forms[0].wl_key1.disabled = 0;
			document.forms[0].wl_key4.disabled = 0;
		}
		document.forms[0].wl_key2.disabled = 0;
		document.forms[0].wl_key3.disabled = 0;
	}
	else {
		document.forms[0].wl_key1.disabled = 1;
		document.forms[0].wl_key2.disabled = 1;
		document.forms[0].wl_key3.disabled = 1;
		document.forms[0].wl_key4.disabled = 1;
	}

	/* Save current network key index */
	for (i = 0; i < document.forms[0].wl_key.length; i++) {
		if (document.forms[0].wl_key[i].selected) {
			cur = document.forms[0].wl_key[i].value;
			break;
		}
	}

	/* Define new network key indices */
	if (mode == "radius" ||
		document.forms[0].wl_akm_wpa.disabled == 0 && wpa == "enabled" ||
		document.forms[0].wl_akm_psk.disabled == 0 && psk == "enabled"
		|| document.forms[0].wl_akm_wpa2.disabled == 0 && wpa2 == "enabled"
		|| document.forms[0].wl_akm_psk2.disabled == 0 && psk2 == "enabled"
		|| document.forms[0].wl_akm_brcm_psk.disabled == 0 && brcm_psk == "enabled"
		)
		algos = new Array("2", "3");
	else
		algos = new Array("1", "2", "3", "4");

	/* Reconstruct network key indices array from new network key indices */
	document.forms[0].wl_key.length = algos.length;
	for (var i in algos) {
		document.forms[0].wl_key[i] = new Option(algos[i], algos[i]);
		document.forms[0].wl_key[i].value = algos[i];
		if (document.forms[0].wl_key[i].value == cur)
			document.forms[0].wl_key[i].selected = true;
	}

	/* enable key index */
	if (wep == "enabled")
		document.forms[0].wl_key.disabled = 0;
	else
		document.forms[0].wl_key.disabled = 1;

	/* enable gtk rotation interval */
	if ((wep == "enabled") || (wl_ibss == "1"))
		document.forms[0].wl_wpa_gtk_rekey.disabled = 1;
	else {
		if (document.forms[0].wl_akm_wpa.disabled == 0 && wpa == "enabled" ||
			document.forms[0].wl_akm_psk.disabled == 0 && psk == "enabled"
			|| document.forms[0].wl_akm_wpa2.disabled == 0 && wpa2 == "enabled"
			|| document.forms[0].wl_akm_psk2.disabled == 0 && psk2 == "enabled"
			|| document.forms[0].wl_akm_brcm_psk.disabled == 0 && brcm_psk == "enabled"
			)
			document.forms[0].wl_wpa_gtk_rekey.disabled = 0;
		else
			document.forms[0].wl_wpa_gtk_rekey.disabled = 1;
	}
}

function wl_auth_change()
{
	var auth = document.forms[0].wl_auth[document.forms[0].wl_auth.selectedIndex].value;
	var wl_ure = <% wl_ure_enabled(); %>;
	var wl_ibss = <% wl_ibss_mode(); %>;

	if (auth == "1") {
		document.forms[0].wl_akm_wpa.disabled = 1;
		document.forms[0].wl_akm_psk.disabled = 1;
		document.forms[0].wl_akm_wpa2.disabled = 1;
		document.forms[0].wl_akm_psk2.disabled = 1;
		document.forms[0].wl_akm_brcm_psk.disabled = 1;
		document.forms[0].wl_preauth.disabled = 1;
/*
*/
		document.forms[0].wl_wpa_psk.disabled = 1;
		document.forms[0].wl_crypto.disabled = 1;
	}
	else {
		if ((wl_ure == "1") || (wl_ibss == "1")) {
			document.forms[0].wl_akm_wpa.disabled = 1;
		}
		else if (document.forms[0].wl_akm_wpa2.disabled == 1) {
			document.forms[0].wl_akm_wpa.disabled = 1;
		}
		else {
			if (document.forms[0].wl_akm_wpa2[document.forms[0].wl_akm_wpa2.selectedIndex].value == "enabled")
				document.forms[0].wl_akm_wpa.disabled = 0;
			else
				document.forms[0].wl_akm_wpa.disabled = 1;
		}
		if (wl_ibss == "1") {
			document.forms[0].wl_akm_psk.disabled = 1;
		}
		else if (document.forms[0].wl_akm_psk2.disabled == 1) {
			document.forms[0].wl_akm_psk.disabled = 1;
		}
		else {
			if (document.forms[0].wl_akm_psk2[document.forms[0].wl_akm_psk2.selectedIndex].value == "enabled")
				document.forms[0].wl_akm_psk.disabled = 0;
			else
				document.forms[0].wl_akm_psk.disabled = 1;
		}
		if (wl_ure == "1") {
			document.forms[0].wl_akm_wpa2.disabled = 1;
			document.forms[0].wl_preauth.disabled = 1;
			document.forms[0].wl_akm_brcm_psk.disabled = 1;
		} else if (wl_ibss == "1") {
			document.forms[0].wl_akm_wpa2.disabled = 1;
			document.forms[0].wl_preauth.disabled = 1;
			document.forms[0].wl_akm_psk2.disabled = 1;
			document.forms[0].wl_akm_brcm_psk.disabled = 0;

		} else {
			document.forms[0].wl_akm_wpa2.disabled = 0;
			document.forms[0].wl_akm_psk2.disabled = 0;
			document.forms[0].wl_preauth.disabled = 0;
			document.forms[0].wl_akm_brcm_psk.disabled = 1;
		}
/*
*/
		document.forms[0].wl_wpa_psk.disabled = 0;
		document.forms[0].wl_crypto.disabled = 0;
	}

	wl_key_update();
}

function wl_auth_mode_change()
{
	var mode = document.forms[0].wl_auth_mode[document.forms[0].wl_auth_mode.selectedIndex].value;
	var wpa = document.forms[0].wl_akm_wpa[document.forms[0].wl_akm_wpa.selectedIndex].value;
	var psk = document.forms[0].wl_akm_psk[document.forms[0].wl_akm_psk.selectedIndex].value;
	var wl_nmode = <% wl_nmode_enabled(); %>;
	var wpa2 = document.forms[0].wl_akm_wpa2[document.forms[0].wl_akm_wpa2.selectedIndex].value;
	var psk2 = document.forms[0].wl_akm_psk2[document.forms[0].wl_akm_psk2.selectedIndex].value;

	/* enable radius IP, port, password */
	if (mode == "radius" ||
		document.forms[0].wl_akm_wpa.disabled == 0 && wpa == "enabled"
		|| document.forms[0].wl_akm_wpa2.disabled == 0 && wpa2 == "enabled"
		) {
		document.forms[0].wl_radius_ipaddr.disabled = 0;
		document.forms[0].wl_radius_port.disabled = 0;
		document.forms[0].wl_radius_key.disabled = 0;
	} else {
		document.forms[0].wl_radius_ipaddr.disabled = 1;
		document.forms[0].wl_radius_port.disabled = 1;
		document.forms[0].wl_radius_key.disabled = 1;
	}

	/* enable network re-auth interval */
	if (mode == "radius" ||
		document.forms[0].wl_akm_wpa.disabled == 0 && wpa == "enabled"
		|| document.forms[0].wl_akm_wpa2.disabled == 0 && wpa2 == "enabled"
		)
		document.forms[0].wl_net_reauth.disabled = 0;
	else
		document.forms[0].wl_net_reauth.disabled = 1;

	wl_key_update();
}

function wl_akm_change()
{
	var mode = document.forms[0].wl_auth_mode[document.forms[0].wl_auth_mode.selectedIndex].value;
	var wpa = document.forms[0].wl_akm_wpa[document.forms[0].wl_akm_wpa.selectedIndex].value;
	var psk = document.forms[0].wl_akm_psk[document.forms[0].wl_akm_psk.selectedIndex].value;
	var wpa2 = document.forms[0].wl_akm_wpa2[document.forms[0].wl_akm_wpa2.selectedIndex].value;
	var psk2 = document.forms[0].wl_akm_psk2[document.forms[0].wl_akm_psk2.selectedIndex].value;
	var brcm_psk = document.forms[0].wl_akm_brcm_psk[document.forms[0].wl_akm_brcm_psk.selectedIndex].value;
/*
*/
	var wl_nmode = <% wl_nmode_enabled(); %>;
	var i;

	/* enable Pre-shared Key */
	if (psk == "enabled"
		|| psk2 == "enabled" || brcm_psk == "enabled"
/*
*/
		)
		document.forms[0].wl_wpa_psk.disabled = 0;
	else
		document.forms[0].wl_wpa_psk.disabled = 1;

	/* enable radius options */
	if (mode == "radius" || wpa == "enabled"
		|| wpa2 == "enabled"
		) {
		document.forms[0].wl_radius_ipaddr.disabled = 0;
		document.forms[0].wl_radius_port.disabled = 0;
		document.forms[0].wl_radius_key.disabled = 0;
	}
	else {
		document.forms[0].wl_radius_ipaddr.disabled = 1;
		document.forms[0].wl_radius_port.disabled = 1;
		document.forms[0].wl_radius_key.disabled = 1;
	}

	/* enable crypto */
	if (wpa == "enabled" || psk == "enabled"
		|| wpa2 == "enabled" || psk2 == "enabled" || brcm_psk == "enabled"
/*
*/
		)
		document.forms[0].wl_crypto.disabled = 0;
	else
		document.forms[0].wl_crypto.disabled = 1;

	/* enable re-auth interval */
	if (mode == "radius" || wpa == "enabled"
		|| wpa2 == "enabled"
		)
		document.forms[0].wl_net_reauth.disabled = 0;
	else
		document.forms[0].wl_net_reauth.disabled = 1;

	if (wpa2 == "enabled")
		document.forms[0].wl_preauth.disabled = 0;
	else
		document.forms[0].wl_preauth.disabled = 1;

	if ((wpa2 == "enabled") || (psk2 == "enabled") || (brcm_psk == "enabled") ||
		(wpa == "enabled") || (psk == "enabled") || (wl_nmode == "1")) {
		document.forms[0].wl_wep.selectedIndex = 1;
		document.forms[0].wl_wep.disabled = 1;
	} else {
		document.forms[0].wl_wep.disabled = 0;
	}
/*
#ifdef MFP
*/
	if (wpa2 == "enabled" || psk2 == "enabled") {
		document.forms[0].wl_mfp.disabled = 0;
	} else {
		document.forms[0].wl_mfp.disabled = 1;
	}
/*
#endif
*/
/*
*/
	wl_key_update();
	wl_crypto_update();
}

function wl_osen_change()
{
/*
#ifdef __CONFIG_HSPOT__
*/
	var osen = document.forms[0].wl_osenen[document.forms[0].wl_osenen.selectedIndex].value;

	/* If OSEN Enabled then Enable WPA2 */
	if(osen == "1") {
		document.forms[0].wl_akm_wpa2.disabled = 0;
		document.forms[0].wl_akm_wpa2.value = "enabled";
		wl_akm_wpa2_change()
	}
/*
#endif
*/
}

function wl_akm_wpa2_change()
{
	var wpa2 = document.forms[0].wl_akm_wpa2[document.forms[0].wl_akm_wpa2.selectedIndex].value;
	var psk2 = document.forms[0].wl_akm_psk2[document.forms[0].wl_akm_psk2.selectedIndex].value;

	if (wpa2 == "enabled") {
		document.forms[0].wl_akm_wpa.disabled = 0;
	} else {
		document.forms[0].wl_akm_wpa[document.forms[0].wl_akm_wpa.selectedIndex].value = "disabled";
		document.forms[0].wl_akm_wpa.disabled = 1;
/*
#ifdef __CONFIG_HSPOT__
*/
		/* If WPA2 Disabled then disable OSEN */
		document.forms[0].wl_osenen.selectedIndex = 0;
/*
#endif
*/
	}
	if (psk2 == "enabled") {
		document.forms[0].wl_akm_psk.disabled = 0;
	} else {
		document.forms[0].wl_akm_psk[document.forms[0].wl_akm_psk.selectedIndex].value = "disabled";
		document.forms[0].wl_akm_psk.disabled = 1;
	}

	wl_akm_change()
/*
#ifdef MFP
*/
	/* MFP must be set to "capable" by default when WPA2 is enabled, as per WFA MFP testplan */
	var wpa2 = document.forms[0].wl_akm_wpa2[document.forms[0].wl_akm_wpa2.selectedIndex].value;
	var psk2 = document.forms[0].wl_akm_psk2[document.forms[0].wl_akm_psk2.selectedIndex].value;

	if (wpa2 == "enabled" || psk2 == "enabled")
		document.forms[0].wl_mfp.selectedIndex = <% nvram_get("wl_mfp"); %>
/*
#endif
*/
}

function wl_wep_change()
{
/*
*/
<% wps_wep_change_display(); %>
/*
*/

	wl_key_update();
}

function wl_crypto_update()
{
	var i, cur, algos;
	var wl_ure = <% wl_ure_enabled(); %>;
	var wl_ibss = <% wl_ibss_mode(); %>;
	var wl_nmode = <% wl_nmode_enabled(); %>;

	/* Save current crypto algorithm */
	for (i = 0; i < document.forms[0].wl_crypto.length; i++) {
		if (document.forms[0].wl_crypto[i].selected) {
			cur = document.forms[0].wl_crypto[i].value;
			break;
		}
	}

	/* Define new crypto algorithms */
	if (<% wl_corerev(); %> >= 3) {
		if (wl_ibss == "1") {
			algos = new Array("AES");
		} else {
			if ((document.forms[0].wl_akm_wpa.disabled == 0 &&
				document.forms[0].wl_akm_wpa[document.forms[0].wl_akm_wpa.selectedIndex].value == "enabled") ||
				(document.forms[0].wl_akm_psk.disabled == 0 &&
				document.forms[0].wl_akm_psk[document.forms[0].wl_akm_psk.selectedIndex].value == "enabled"))
				algos = new Array("AES", "TKIP+AES");
			else
				algos = new Array("AES");

/*
*/
		}
	} else {
		if (wl_ibss == "0")
			algos = new Array("TKIP");
		else
			algos = new Array("");
	}

	/* Reconstruct algorithm array from new crypto algorithms */
	document.forms[0].wl_crypto.length = algos.length;
	for (var i in algos) {
		document.forms[0].wl_crypto[i] = new Option(algos[i], algos[i].toLowerCase());
		document.forms[0].wl_crypto[i].value = algos[i].toLowerCase();
		if (document.forms[0].wl_crypto[i].value == cur)
			document.forms[0].wl_crypto[i].selected = true;
	}
}

function wl_security_update()
{

	var wl_ure = <% wl_ure_enabled(); %>;
	var wl_ibss = <% wl_ibss_mode(); %>;
	var wl_nmode = <% wl_nmode_enabled(); %>;

/*
#ifdef __CONFIG_HSPOT__
*/
	var wl_hs2en = <% nvram_get_bitflag("wl_hsflag", 0); %>;

	/* If Passpoint Enabled then greyout OSEN */
	if (wl_hs2en == 1) {
		document.forms[0].wl_osenen.disabled = 1;
	}
	else {
		document.forms[0].wl_osenen.disabled = 0;
		wl_osen_change();
	}
/*
#endif
*/

	/* If nmode then disable WEP */
 	if (<% wl_corerev(); %> >= 3 && wl_nmode == "1") {
		document.forms[0].wl_wep.selectedIndex = 1;
		document.forms[0].wl_wep.disabled = 1;
  	}
	wl_crypto_update();

	wl_auth_change();
	wl_auth_mode_change();
	wl_akm_change();
	wl_wep_change();

	if ((wl_ure == "1") || (wl_ibss == "1")) {
		document.forms[0].wl_auth_mode.disabled = 1;
	}
	else {
		document.forms[0].wl_auth_mode.disabled = 0;
	}

/*
*/
}

function wpapsk_window()
{
	var psk_window = window.open("", "", "toolbar=no,scrollbars=yes,width=400,height=100");
	psk_window.document.write("The WPA passphrase is <% nvram_get("wl_wpa_psk"); %>");
	psk_window.document.close();
}

function pre_submit()
{
/*
*/
<% wps_security_pre_submit_display(); %>
/*
*/
	return true;
}

//-->

</script>
</head>

<body onLoad="wl_security_update();">
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>

<table border="0" cellpadding="0" cellspacing="0" width="100%" bgcolor="#cc0000">
  <% asp_list(); %>
</table>

<table border="0" cellpadding="0" cellspacing="0" width="100%">
  <tr>
    <td colspan="2" class="edge"><img border="0" src="blur_new.jpg" alt=""></td>
  </tr>
  <tr>
    <td><img border="0" src="logo_new.gif" alt=""></td>
    <td width="100%" valign="top">
	<br>
	<span class="title">SECURITY</span><br>
	<span class="subtitle">This page allows you to configure
	security for the wireless LAN interfaces.</span>
    </td>
  </tr>
</table>

<form method="post" action="security.asp">
<input type="hidden" name="page" value="security.asp">
<!--
-->
<input type="hidden" name="wl_wps_mode" value="<% nvram_get("wl_wps_mode"); %>">
<!--
-->
<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
	onMouseOver="return overlib('Selects which wireless interface to configure.', LEFT);"
	onMouseOut="return nd();">
	Wireless Interface:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="wl_unit" onChange="submit();">
	  <% wl_list("INCLUDE_SSID" , "INCLUDE_VIFS"); %>
	</select>
    </td>
    <td>
	<button type="submit" name="action" value="Select">Select</button>
    </td>
  </tr>
</table>

<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
	onMouseOver="return overlib('Selects 802.11 authentication method. Open or Shared.', LEFT);"
	onMouseOut="return nd();">
	802.11 Authentication:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="wl_auth" onChange="wl_auth_change();">
	  <% wl_auth_display(); %>
	</select>
    </td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Selects Network authentication type.', LEFT);"
	onMouseOut="return nd();">
	802.1X Authentication:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="wl_auth_mode" onChange="wl_auth_mode_change();">
	  <option value="radius" <% nvram_match("wl_auth_mode", "radius", "selected"); %>>Enabled</option>
	  <option value="none" <% nvram_invmatch("wl_auth_mode", "radius", "selected"); %>>Disabled</option>
	</select>
    </td>
  </tr>
<!--
#ifdef __CONFIG_HSPOT__
-->
	<tr>
		<th width="310"
		onMouseOver="return overlib('Enables or disables OSU OSEN (Anonymous EAP Auth) Security Status', LEFT);"
		onMouseOut="return nd();">
		OSEN:&nbsp;&nbsp;
		</th>
		<td>&nbsp;&nbsp;</td>
		<td>
		<select name="wl_osenen" onChange="wl_osen_change();">
			<option value="0" <% nvram_match_bitflag("wl_hsflag", 1, "0", "selected"); %>>Disabled</option>
			<option value="1" <% nvram_match_bitflag("wl_hsflag", 1, "1", "selected"); %>>Enabled</option>
		</select>
		</td>
	</tr>
<!--
#endif
-->
  <tr>
    <th width="310"
	onMouseOver="return overlib('Enables/Disables WPA Authenticated Key Management suite.', LEFT);"
	onMouseOut="return nd();">
	<input type="hidden" name="wl_akm" value="">
	WPA:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="wl_akm_wpa" onChange="wl_akm_change();">
	  <option value="enabled" <% nvram_inlist("wl_akm", "wpa", "selected"); %>>Enabled</option>
	  <option value="disabled" <% nvram_invinlist("wl_akm", "wpa", "selected"); %>>Disabled</option>
	</select>
    </td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Enables/Disables WPA-PSK Authenticated Key Management suite.', LEFT);"
	onMouseOut="return nd();">
	WPA-PSK:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="wl_akm_psk" onChange="wl_akm_change();">
	  <option value="enabled" <% nvram_inlist("wl_akm", "psk", "selected"); %>>Enabled</option>
	  <option value="disabled" <% nvram_invinlist("wl_akm", "psk", "selected"); %>>Disabled</option>
	</select>
    </td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Enables/Disables WPA2 Authenticated Key Management suite.', LEFT);"
	onMouseOut="return nd();">
	WPA2:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="wl_akm_wpa2" onChange="wl_akm_wpa2_change();">
	  <option value="enabled" <% nvram_inlist("wl_akm", "wpa2", "selected"); %>>Enabled</option>
	  <option value="disabled" <% nvram_invinlist("wl_akm", "wpa2", "selected"); %>>Disabled</option>
	</select>
    </td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Enables/Disables WPA2-PSK Authenticated Key Management suite.', LEFT);"
	onMouseOut="return nd();">
	WPA2-PSK:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="wl_akm_psk2" onChange="wl_akm_wpa2_change();">
	  <option value="enabled" <% nvram_inlist("wl_akm", "psk2", "selected"); %>>Enabled</option>
	  <option value="disabled" <% nvram_invinlist("wl_akm", "psk2", "selected"); %>>Disabled</option>
	</select>
    </td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Enables/Disables BRCM-PSK Authenticated Key Management suite.', LEFT);"
	onMouseOut="return nd();">
	BRCM-PSK:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="wl_akm_brcm_psk" onChange="wl_akm_change();">
	  <option value="enabled" <% nvram_inlist("wl_akm", "brcm_psk", "selected"); %>>Enabled</option>
	  <option value="disabled" <% nvram_invinlist("wl_akm", "brcm_psk", "selected"); %>>Disabled</option>
	</select>
    </td>
  </tr>
  <tr>
    <th width="310">WPA2 Preauthentication:&nbsp;&nbsp;</th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="wl_preauth">
	  <option value="disabled" <% nvram_match("wl_preauth", "0", "selected"); %>>Disabled</option>
	  <option value="enabled" <% nvram_invmatch("wl_preauth", "0", "selected"); %>>Enabled</option>
	</select>
    </td>
  </tr>
<!--
-->
</table>

<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
	onMouseOver="return overlib('Enables or disables WEP data encryption. Selecting <b>Enabled</b> enables WEP data encryption and requires that a valid network key be set and selected unless <b>802.1X</b> is enabled.', LEFT);"
	onMouseOut="return nd();">
	WEP Encryption:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="wl_wep" onChange="wl_wep_change();">
	  <option value="enabled" <% nvram_match("wl_wep", "enabled", "selected"); %>>Enabled</option>
	  <option value="disabled" <% nvram_invmatch("wl_wep", "enabled", "selected"); %>>Disabled</option>
	</select>
    </td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Selects the WPA data encryption algorithm.', LEFT);"
	onMouseOut="return nd();">
<!--
-->
	<div id="wl_wpa_encrypt_div">
	WPA Encryption:&nbsp;&nbsp;
	</div>
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="wl_crypto">
	  <option value="tkip" <% nvram_match("wl_crypto", "tkip", "selected"); %>>TKIP</option>
	  <option value="aes" <% nvram_match("wl_crypto", "aes", "selected"); %>>AES</option>
	  <option value="tkip+aes" <% nvram_match("wl_crypto", "tkip+aes", "selected"); %>>TKIP+AES</option>
<!--
-->
	</select>
    </td>
  </tr>
</table>

<!--
-->

<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
	onMouseOver="return overlib('Sets the IP address of the RADIUS server to use for authentication and dynamic key derivation.', LEFT);"
	onMouseOut="return nd();">
	RADIUS Server:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td><input name="wl_radius_ipaddr" value="<% nvram_get("wl_radius_ipaddr"); %>" size="15" maxlength="15"></td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Sets the UDP port number of the RADIUS server. The port number is usually 1812 or 1645 and depends upon the server.', LEFT);"
	onMouseOut="return nd();">
	RADIUS Port:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td><input name="wl_radius_port" value="<% nvram_get("wl_radius_port"); %>" size="5" maxlength="5"></td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Sets the shared secret for the RADIUS connection.', LEFT);"
	onMouseOut="return nd();">
	RADIUS Key:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td><input name="wl_radius_key" value="<% nvram_get("wl_radius_key"); %>" type="password"></td>
  </tr>
</table>

<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
	onMouseOver="return overlib('Sets the WPA passphrase.', LEFT);"
	onMouseOut="return nd();">
<!--
-->
	<div id="wl_wpa_psk_div">
	WPA passphrase:&nbsp;&nbsp;
	</div>
    </th>
    <td>&nbsp;&nbsp;</td>
    <td><input name="wl_wpa_psk" value="<% nvram_get("wl_wpa_psk"); %>" type="password"></td>
    <td>&nbsp;&nbsp;</td>
    <td> <A HREF="javascript:wpapsk_window()">Click here to display</A></td>
  </tr>
</table>

<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
	onMouseOver="return overlib('Enter 5 ASCII characters or 10 hexadecimal digits for a 64-bit key. Enter 13 ASCII characters or 26 hexadecimal digits for a 128-bit key.', LEFT);"
	onMouseOut="return nd();">
	Network Key 1:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td><input name="wl_key1" value="<% nvram_get("wl_key1"); %>" size="26" maxlength="26" type="password"></td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Enter 5 ASCII characters or 10 hexadecimal digits for a 64-bit key. Enter 13 ASCII characters or 26 hexadecimal digits for a 128-bit key.', LEFT);"
	onMouseOut="return nd();">
	Network Key 2:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td><input name="wl_key2" value="<% nvram_get("wl_key2"); %>" size="26" maxlength="26" type="password"></td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Enter 5 ASCII characters or 10 hexadecimal digits for a 64-bit key. Enter 13 ASCII characters or 26 hexadecimal digits for a 128-bit key.', LEFT);"
	onMouseOut="return nd();">
	Network Key 3:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td><input name="wl_key3" value="<% nvram_get("wl_key3"); %>" size="26" maxlength="26" type="password"></td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Enter 5 ASCII characters or 10 hexadecimal digits for a 64-bit key. Enter 13 ASCII characters or 26 hexadecimal digits for a 128-bit key.', LEFT);"
	onMouseOut="return nd();">
	Network Key 4:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td><input name="wl_key4" value="<% nvram_get("wl_key4"); %>" size="26" maxlength="26" type="password"></td>
  </tr>
  <tr>
    <th width="310"
	onMouseOver="return overlib('Selects which network key is used for encrypting outbound data and/or authenticating clients.', LEFT);"
	onMouseOut="return nd();">
	Current Network Key:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="wl_key">
	  <option value="1" <% nvram_match("wl_key", "1", "selected"); %>>1</option>
	  <option value="2" <% nvram_match("wl_key", "2", "selected"); %>>2</option>
	  <option value="3" <% nvram_match("wl_key", "3", "selected"); %>>3</option>
	  <option value="4" <% nvram_match("wl_key", "4", "selected"); %>>4</option>
	</select>
    </td>
  </tr>
</table>

<!--
#ifdef MFP
-->
<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310">Protected Management Frames:&nbsp;&nbsp;</th>
    <td>&nbsp;&nbsp;</td>
    <td>
	<select name="wl_mfp">
	  <option value="0" <% nvram_match("wl_mfp", "0", "selected"); %>>Off</option>
	  <option value="1" <% nvram_match("wl_mfp", "1", "selected"); %>>Capable</option>
	  <option value="2" <% nvram_match("wl_mfp", "2", "selected"); %>>Required</option>
	</select>
    </td>
  </tr>
</table>
<!--
#endif
-->
<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
	onMouseOver="return overlib('Sets the Network Key Rotation interval in seconds. Leave blank or set to zero to disable the rotation.', LEFT);"
	onMouseOut="return nd();">
	Network Key Rotation Interval:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td><input name="wl_wpa_gtk_rekey" value="<% nvram_get("wl_wpa_gtk_rekey"); %>" size="10" maxlength="10"></td>
  </tr>
<!--
-->
</table>

<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
	onMouseOver="return overlib('Sets the Network Re-authentication interval in seconds. Leave blank or set to zero to disable periodic network re-authentication.', LEFT);"
	onMouseOut="return nd();">
	Network Re-auth Interval:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td><input name="wl_net_reauth" value="<% nvram_get("wl_net_reauth"); %>" size="10" maxlength="10"></td>
  </tr>
</table>

<!--
#ifdef BCMDBG
-->
<p>
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
    <th width="310"
	onMouseOver="return overlib('Enables/Disables NAS debugging. 0:Disable | 1:Enable.', LEFT);"
	onMouseOut="return nd();">
	NAS debugging:&nbsp;&nbsp;
    </th>
    <td>&nbsp;&nbsp;</td>
    <td><input name="wl_nas_dbg" value="<% nvram_get("wl_nas_dbg"); %>" size="10" maxlength="10"></td>
  </tr>
</table>
<!--
#endif
-->

<p>
<table border="0" cellpadding="0" cellspacing="0">
    <tr>
      <td width="310"></td>
      <td>&nbsp;&nbsp;</td>
      <td>
	  <input type="submit" name="action" value="Apply" onClick="return pre_submit();">
	  <input type="reset" name="action" value="Cancel">
      </td>
    </tr>
</table>

</form>

<!--
-->

<p class="label">&#169;2001-2012 Broadcom Corporation. All rights reserved. 54g is a trademark of Broadcom Corporation.</p>

</body>
</html>
