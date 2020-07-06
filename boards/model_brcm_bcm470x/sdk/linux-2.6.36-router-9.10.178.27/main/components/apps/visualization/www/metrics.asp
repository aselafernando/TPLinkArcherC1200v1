<!DOCTYPE html>
<!--
 HTML part for Advanced Troubleshooting Tab

 Copyright (C) 2015, Broadcom Corporation
 All Rights Reserved.
 
 This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 the contents of this file may not be disclosed to third parties, copied
 or duplicated in any form, in whole or in part, without the prior
 written permission of Broadcom Corporation.


 <<Broadcom-WL-IPTag/Proprietary:>>

 $Id: metrics.asp 560528 2015-06-01 12:21:43Z $
-->

<html>
<head>
<meta charset="ISO-8859-1">

<link rel="stylesheet" href="visstyle.css">
<link rel="stylesheet" href="matrics.css">
<script src="jquery-2.0.3.min.js"></script>
<script src="common.js"></script>
<script src="jquery.flot.min.js"></script>
<script src="jquery.flot.axislabels.js"></script>
<script src="jquery.flot.orderBars.js"></script>
<script src="jquery.flot.time.min.js"></script>
<script src="jquery.flot.navigate.min.js"></script>
<script src="jquery.cookie.js"></script>
<script src="matrics.js"></script>
<script src="jquery.flot.pie.js"></script>
<script src="jquery.flot.stack.js"></script>
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
					<span id="TextHeading">Advanced Troubleshooting</span><br>
					<span id="TextHeadingDesc">In this page you will see most of the counters like AMPDU(if available), Glitch, Chanim and Packet Queue Statistics</span>
				</td>
			</tr>
		</table>
		<div id="contentarea" class="blkbrdr btmbrdr">
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
			</div>
			<table>
				<tr>
					<td>
						<br/><br/>
						<table class = "ampdutable"></table>
					</td>
				</tr>
			</table>
			<br/><br/>
			<div id="countermatrics">
				<div id="counterstatsplace">
				</div>
				<div id="apstainfoplaceholder">
					<table class="ampdustatstable fontfamly cmngraphbox">
						<thead class="cmngraphboxbkgrnd-h1">
							<tr><th>Associated Station's</th></tr>
						</thead>
						<tbody>
							<tr>
								<td width="90%" align="center">
									<table class="apstamappingtbl fontfamly">
										<caption><b>Click on station's to see the Packet Queue Statistics</b><br><br></caption>
										<tbody>
											<tr>
												<td width=30% align="right">
													<table class="apstamapleftcol fontfamily">
														<tbody></tbody>
													</table>
												</td>
												<td class="sprator" align="left" width=5px>
													<div class="staseparator"></div>
												</td>
												<td width=40%>
													<table class="cmntable">
														<tr>
															<td align="center">
																<div class="boxdiv">
																	<font class="fontonwhite">
																	<table class="centerap">
																		<tbody>
																			<tr>
																				<td align="right">SSID</td>
																				<td> : </td>
																			</tr>
																			<tr>
																				<td align="right">BSSID</td>
																				<td> : </td>
																			</tr>
																			<tr>
																				<td align="right">Channel</td>
																				<td> : </td>
																			</tr>
																		</tbody>
																	</table>
																	</font>
																	<img src="ap-image.jpg" width="100px"/>
																</div>
																<div class="verticalline"></div>
																<div class="horizontalline"></div>
															</td>
														</tr>
													</table>
												</td>
												<td class="sprator" align="left" width=5px>
													<div class="staseparator"></div>
												</td>
												<td width=30% align="left">
													<table class="apstamaprightcol fontfamily">
														<tbody></tbody>
													</table>
												</td>
											</tr>
										</tbody>
									</table>
									<table id ="apstalisttable" class="apstainfolisttable fontfamly cmnbrdr">
										<thead class="cmngraphboxbkgrnd-h1">
											<tr>
											<th>SR.</th>
											<th>MAC</th>
											<th>RSSI [dBm]</th>
											<th>PHY Rate [Mbps]</th>
											</tr>
										</thead>
										<tbody></tbody>
									</table>
								</td>
							</tr>
						</tbody>
					</table>
				</div>

				<table id="packetqueuestatsgrphtbl" class="packetqueuetable fontfamly cmngraphbox">
					<thead class="cmngraphboxbkgrnd-h1">
						<tr>
							<th>Packet Queue Statistics</th>
						</tr>
					</thead>
					<tbody></tbody>
				</table>
			</div>

		</div>
	</div>
</body>
</html>
