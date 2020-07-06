<!DOCTYPE html>
<!--
 HTML part for Channels sub Tab

 Copyright (C) 2015, Broadcom Corporation
 All Rights Reserved.
 
 This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 the contents of this file may not be disclosed to third parties, copied
 or duplicated in any form, in whole or in part, without the prior
 written permission of Broadcom Corporation.


 <<Broadcom-WL-IPTag/Proprietary:>>

 $Id: channels.asp 560528 2015-06-01 12:21:43Z $
-->

<html>
<head>
<meta charset="ISO-8859-1">
<link rel="stylesheet" href="visstyle.css">
<script src="jquery-2.0.3.min.js"></script>
<script src="jquery.flot.min.js"></script>
<script src="jquery.flot.orderBars.js"></script>
<script src="jquery.flot.axislabels.js"></script>
<script src="jquery.cookie.js"></script>
<script src="channels.js"></script>
<title>Visualization</title>
</head>
<body>
<!--
	<div id="headerplaceholder">
			<div id="topheader">
				<image id="logoplaceholder" src="../Images/broadcom.png">
				</image>
			</div>
	</div>
-->

	<div id="main_div">
		<div id="topnav" class="blackbrdrd">
			<div id="containerdiv">
				<ul class="custommenu">
				<li><a href="index.asp">Home</a></li>
					<li class="sitesurveytab">
						<a href="visindex.asp" class="selected">Site Survey</a>
						<ul>
							<li>
								<a href="visindex.asp">Networks</a>
							</li>
							<li>
								<a href="channels.asp">Channels</a>
							</li>
						</ul>
					</li>
					
					<li class="chnlcapacitytab"><a href="channelcapacity.asp">Channel Capacity</a></li>
					
					<li class="metricstab"><a href="metrics.asp">Metrics</a></li>
					
					<li><a href="configure.asp">Configure</a></li>
				</ul>
			</div>
		</div>
		
		<div id="contentarea" class="blkbrdr btmbrdr">
			<div class="tabheadings">
			<image src="tabheading.jpg" width="100%"/>
			<!--<p>Site Survey > Channels</p>-->
			</div>
			
			<div id="frequencybandselector">
				<form id="frequencybandform">
				<fieldset class="fontfamly cmnbrdr border ftalign">
				<input type="radio" name="frequencyband" id="24ghzband" checked="checked">2.4 GHZ</input>
				<input type="radio" name="frequencyband" id="5ghzband">5 GHZ</input>
				</fieldset>
				</form>
			</div>
			
			<div id="CongestiongraphPlaceHolder">
			</div>
			
		</div>
	</div>
</body>
</html>
