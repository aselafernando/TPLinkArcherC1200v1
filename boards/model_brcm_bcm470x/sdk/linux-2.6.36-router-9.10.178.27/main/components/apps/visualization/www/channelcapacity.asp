<!DOCTYPE html>
<!--
 HTML part for Channel Statistics Tab

 Copyright (C) 2015, Broadcom Corporation
 All Rights Reserved.
 
 This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 the contents of this file may not be disclosed to third parties, copied
 or duplicated in any form, in whole or in part, without the prior
 written permission of Broadcom Corporation.


 <<Broadcom-WL-IPTag/Proprietary:>>

 $Id: channelcapacity.asp 560528 2015-06-01 12:21:43Z $
-->

<html>
<head>
<meta charset="ISO-8859-1">
<link rel="stylesheet" href="visstyle.css">
<link rel="stylesheet" href="channelCapacity.css">
<script src="jquery-2.0.3.min.js"></script>
<script src="common.js"></script>
<script src="jquery.flot.min.js"></script>
<script src="jquery.flot.orderBars.js"></script>
<script src="jquery.flot.axislabels.js"></script>
<script src="jquery.flot.stack.js"></script>
<script src="jquery.cookie.js"></script>
<script src="channelcapacity.js"></script>
<title>WiFi Insight</title>
</head>
<body>
	<div id="main_div">
		<script>commonTopNavBar()</script> <!-- Defined custom menu in common.js -->

		<table class="logotable">
			<tr>
				<td colspan="2" class="edge"><img border="0" src="blur_new.jpg" alt=""></td>
			</tr>
			<tr>
				<td><img border="0" src="logo_new.gif" alt=""></td>
				<td width="100%" valign="top">
					<br>
					<span id="TextHeading">Channel Statistics</span><br>
					<span id="TextHeadingDesc">In this page you will see the Wi-Fi and Non Wi-Fi Interference also Available Capacity</span>
				</td>
			</tr>
		</table>
		<div id="contentarea" class="btmbrdr">
			<br>
			<div id="showerrmessage">
			</div>
			<div id="frequencybandselector">
				<form id="frequencybandform">
					<fieldset class="fontfamly cmnfilterclass">
						<select id="bandselectcntrl" class="selectbandctrl cmnctrlborder">
							<option>No Interfaces</option>
						</select>
					</fieldset>
				</form>
				<br/>
				<table id="channelcapacityhdrid" class="channelcapacityhdr fontfamly">
					<tbody><tbody>
				</table>
			</div>

			<div id="channelcapacitygraphcontent">
			</div>

		</div>
	</div>
</body>
</html>
