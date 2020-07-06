/*
 * Jquery implementation for Device Summary Tab
 *
 * $ Copyright Broadcom Corporation $
 *
 *
 * <<Broadcom-WL-IPTag/Proprietary:>>
 *
 * $Id: iQoS.js,v 1.0 2015-02-12 15:25:43 vbemblek $
 */

/*****
* Function which will get called when DOM for the page gets loaded.START
*****/

var gDeviceListTimers = [];
var gSelectedCatgry = [];
var gRefSelectedCatgryLst = []; /* array for maintaining selected categories for all sessions. */
var firsttime = 1;
var gDeviceList = [];
var gDeviceSummary = [];
var gLinkXVal = [];
var gLinkYVal = [];
var gComboval = '';
var gComboindex = -1;
var isHTTPDServer = 1;
var urldevicelist;
var urldevicesummary;
var MbpstoBytes = (1024 * 1024 * 8); //Conversion of Mbps to bytes
var KbpstoBytes = (1024 * 8);
var refreshTime = 5000;
var maxToBeSelected = 6;
var urlGetConfig;
var urlNetworkSummary;
var isHTTPDServer = 1;

function initPage()
{
	if (isHTTPDServer == 1) {
		urldevicelist = 'http://'+ window.location.host +'/iQoS.cgi?Req=DeviceList';
		urldevicesummary = 'http://'+ window.location.host +'/iQoS.cgi?Req=DeviceSummary';
		urlGetConfig  = 'http://'+ window.location.host +'/iQoS.cgi?Req=GetConfig';
		urlNetworkSummary = 'http://'+ window.location.host +'/iQoSNetworkSummary.asp';
	} else {
		urldevicelist = "http://localhost/devicelist.php";
		urldevicesummary = "http://localhost/DeviceSummary.php";
	}
}

/*****
* Function To Check iQoS State.START
*****/
function CheckiQoSState(array)
{
	if (array.EnableiQoS == 0) {
		alert('Please Enable iQoS In Quick Settings' + '\n' + 'To Access Device Summary');
		$(location).attr('href',urlNetworkSummary);
		return;
	}
}

/*****
* Function For getting the configuration.START
*****/
function getConfiguration()
{
	var array = [];

	$.ajax({
		type:"GET",
		url:urlGetConfig,
		success:function(result){
			if (isHTTPDServer == 1)
				array = result;
			else
				array = JSON.parse(result);

			CheckiQoSState(array);
		}
	});
}

/*****
* Function For updating the combo list details.
* if name is empty we are updating the IPAddress of the device in the Combo list. START
*****/
function populateDeviceList(array)
{
	var bDeviceFound = 0;
	var ipAddr,devName,displayName;
	$('#devlist').empty();

	for (i = 0; i < array.Device.length; i++)
	{
		if(array.Device[i].Name == "") {
			ipAddr = array.Device[i].IPAddr;
			devName = array.Device[i].DevName;
			displayName = ipAddr;
			if (devName != undefined)
				displayName += "-" + devName;

			$("#devlist").append($("<option/>").attr("value", displayName).text(displayName));
		}
		else {
			Name = array.Device[i].Name;
			devName = array.Device[i].DevName;
			displayName = Name;
			if (devName != undefined)
				displayName += "-" + devName;

			$("#devlist").append($("<option/>").attr("value", displayName).text(displayName));
		}

		if ( gComboval == displayName) {
			gComboindex = i;
			bDeviceFound = 1;
		}
	}

	if(bDeviceFound == 0 && array.Device.length > 0) {
		gComboindex = 0;
		devName = array.Device[0].DevName;
		if (array.Device[0].Name == "") {
			if (devName != undefined)
				gComboval = array.Device[0].IPAddr + '-' + devName;
			else
				gComboval = array.Device[0].IPAddr;
		}
		else
		{
			if (devName != undefined)
				gComboval = array.Device[0].Name + '-' + devName;
			else
				gComboval = array.Device[0].Name;
		}
		PopulateDeviceListDetails(array,gComboindex);
	}
}
/*****
* Function For updating the combo list details. END
*****/

/*****
* Function For updating the device list details.START
*****/
function PopulateDeviceListDetails(array, index)
{
	var $IPAddress;
	var $OSData;
	var $LastUsed;

	$('.systemdetailstable tr').remove();
	$IPAddress = '<tr><td>IP Address</td><td>: '+array.Device[index].IPAddr+'</td></tr>';
	$OSData += '<tr><td>OS</td><td>: '+array.Device[index].OS+'</td></tr>';
	$LastUsed += '<tr><td>Last Internet Access</td><td>: ' + ConvertEpochTimeToString(array.Device[index].LastUsed) + '</td></tr>';

	$('.systemdetailstable').append($IPAddress);
	$('.systemdetailstable').append($OSData);
	$('.systemdetailstable').append($LastUsed);
}
/*****
* Function For updating the device list details.END
*****/
/*****
* Function For Converting data from bytes to Mbps and Kbps.START
*****/
function DataConversionFromBytes(tobeconverted, DataTag)
{
	var ConvertedData = tobeconverted;
	if(DataTag == true)
	{
		if(ConvertedData >=MbpstoBytes)
			return( (ConvertedData/MbpstoBytes).toFixed(2) + " Mbps");
		else if(ConvertedData >=KbpstoBytes)
			return((ConvertedData/KbpstoBytes).toFixed(2) + " Kbps");
		else
			return(ConvertedData + " bytes");
	}
	else
	{
		if(ConvertedData >=MbpstoBytes)
			return( (ConvertedData/MbpstoBytes).toFixed(2) + " Mb");
		else if(ConvertedData >=KbpstoBytes)
			return((ConvertedData/KbpstoBytes).toFixed(2) + " Kb");
		else
			return(ConvertedData + " bytes");
	}
}
/*****
* Function For Converting data from bytes to Mbps and Kbps.START
*****/
/*****
* Function For updating the Device summary.START
*****/
function populateDeviceSummary(array)
{
	var UpStreamBandwidth;
	var DownStreamBandwidth;
	var UpLinkBandwidth;
	var TotaldataDownloaded;
	var TotaldataUploaded;

	var downStream = DataConversionFromBytes(array.UpBW, true);
	var upStream = DataConversionFromBytes(array.DownBW, true);
	var upLinkBand = DataConversionFromBytes(array.BW, true);
	var totalDownloadedData = DataConversionFromBytes(array.DownBytes, false);
	var totalUploadedData = DataConversionFromBytes(array.UpBytes, false);

	$('.datatransferdetails tr').remove();

	DownStreamBandwidth = '<tr><td>Current Downstream Bandwidth Used</td><td>: '+downStream+' </td></tr>';
	UpStreamBandwidth += '<tr><td>Current Upstream Bandwidth Used</td><td>: '+upStream+' </td></tr>';
	UpLinkBandwidth += '<tr><td>Uplink Bandwidth Used in Network</td><td>: '+upLinkBand+' </td></tr>';
	TotaldataDownloaded += '<tr><td>Total data Downloaded in last Session</td><td>: '+totalDownloadedData+'</td></tr>';
	TotaldataUploaded += '<tr><td>Total data Uploaded in last Session</td><td>: '+totalUploadedData+'</td></tr>';

	$('.datatransferdetails').append(UpStreamBandwidth);
	$('.datatransferdetails').append(DownStreamBandwidth);
	$('.datatransferdetails').append(UpLinkBandwidth);
	$('.datatransferdetails').append(TotaldataDownloaded);
	$('.datatransferdetails').append(TotaldataUploaded);

	populateApplicationList(array);
	drawLinkRate(array);
	populateSelectCategoryBox(array);
	drawBandwidthUsagebyCategory(array);
}
/*****
* Function For updating the Device summary.END
*****/
/*****
* Function For Filling the progress bar and the Application Packet Details.START
*****/
function populateApplicationList(array)
{
	var $element ='';
	var i, maxband = 0, incr= 0, nappcount = 0;
	var left=0;
	var vlineelem = '';
	var start = 0;

	$('.applicationlist tr').remove();

	var apps = array.Apps;

	for (i = 0; i < apps.length; i++) {
		if (apps[i].Name == undefined)
			continue;
		//bandwidth to Mbps....
		apps[i].BW = apps[i].BW/MbpstoBytes;

		if (maxband < apps[i].BW)
			maxband = apps[i].BW;
	}
	//Rounding off the bandwidth to fill the progress bar
	if (maxband > 50)
		maxband += (100-(maxband%100));
	else if (maxband > 10)
		maxband = 50;
	else if(maxband > 1)
		maxband = 10;
	else
		maxband = 1;

	incr = maxband/10;

	for (i = 0; i < 21; i++) {
		if (i %2 == 0) {
			vlineelem += '<div class="vlines" style="left:'+left+'%;">'+start+'</div>';
			start += incr;
			start = Math.round(start*100)/100;
		}
		else
			vlineelem += '<div class="vlines1" style="left:'+left+'%;"></div>';
		/* Gap between two lines in the bandwidth axis */
		left += 4.37;
	}
	$('.bandwidthyaxis').html(vlineelem);

	for (i = 0; i < apps.length; i++) {
		if (apps[i].Name == undefined)
			continue;
		var tofill = (apps[i].BW * 100)/maxband;

		$element += '<tr><td class="Line1"></td>';
		$element += '<td class="RectTextbox1">'+apps[i].Name+'<br>';
		$element += '<font COLOR=#008bb0><b>'+apps[i].Category+'</b></font></td>';
		$element += '<td class="RectTextbox2">Last Access:'+ConvertEpochTimeToString(apps[i].LastUsed)+'<br/>Downloaded Packets:'+apps[i].DownPkts+'<br/>Uploaded Packets: '+apps[i].UpPkts+'</td>';
		$element += '<td class="bandwidthusage"><div class="bandwidthusagediv" style="width:'+tofill+'%;"></div></td>'
		$element += '</tr>';
		nappcount++;
	}

	$('.applicationlist').append($element);

	$("#DeviceApplicationTable").height(nappcount*56+140);
	$(".vertical-line").height(nappcount*56);
}
/*****
* Function For Filling the progress bar and the Application Packet Details.END
*****/
function formatter(val, axis) {
    var d = new Date(val);
	var ret = ""+d.getMinutes()+":"+d.getSeconds()+"";

	return ret;
}
/*****
* Function For drawing the LinkRate Graph.START
*****/
function drawLinkRate(array)
{
	var dataval1 = [];
	var xaxisticks = [];
	var i;

	var tempData = [];

	if (gLinkYVal.length >= 10) {
		gLinkXVal.splice(0,1);
		gLinkYVal.splice(0,1);
	}
	var dt = new Date();
	var time = dt.getMinutes() + ":" + dt.getSeconds();
	gLinkXVal.push(time); // get current time and add
	gLinkYVal.push(array.BW); // change to MBPS

	for(i = 0; i < gLinkYVal.length; i++){
		tempData.push([i,(gLinkYVal[i]/MbpstoBytes).toFixed(3)]);
	}
	dataval1.push(tempData);

	for(i=0;i<gLinkXVal.length;i++){
		xaxisticks.push([i,gLinkXVal[i]]);
	}

	var options = {
		series:{
			lines:{
				show:true,
				align: 'center',
			},
			points:{
				show:true,
			}
		},
		xaxis:{
			show:true,
			axisLabel:'Time(Seconds)',
			tickLength:1,
			ticks:xaxisticks,
			autoscaleMargin:.05,
			axisLabelUseCanvas: true,
			axisLabelFontSizePixels: 16,
			axisLabelFontFamily: 'Arial, Verdana',
			axisLabelPadding: 10,
			//tickFormatter:formatter,
		},
		yaxis:{
			show:true,
			axisLabel: 'Mbps',
			axisLabelUseCanvas: true,
			axisLabelFontSizePixels: 16,
			axisLabelFontFamily: 'Arial, Verdana',
			axisLabelPadding: 3,
			min:0,
		},

		 legend: {
            labelBoxBorderColor: "none",
            position: "right"
        },
	};

	var dataset = [];
	dataset.push({hoverable:true,data:dataval1[0]});

	$.plot($("#LinkRateGraphContent"), dataset, options);
}
/*****
* Function For drawing the LinkRate Graph.END
*****/
/*****
* Function For drawing the Bandwidth usage category Graph.START
*****/
function drawBandwidthUsagebyCategory(array)
{
	var dataval1 = [];
	var xaxisticks = [];
	var tempData = [];
	var dataset = [];
	var i, j;
	var separators = [' ','-'];
	var cat = array.Categories;

	for(i = 0, j = 0; i < cat.length; i++){
		if ($.inArray(cat[i].Name, gSelectedCatgry) >= 0) {
			cat[i].BW = (cat[i].BW/MbpstoBytes).toFixed(3);
			tempData.push([++j,cat[i].BW]);
		}
	}
	dataval1.push(tempData);

	for(i=0, j = 0;i<cat.length;i++){
		if ($.inArray(cat[i].Name, gSelectedCatgry) >= 0) {
			var xval = cat[i].Name;
			$(separators).each(function (index, element) {
				xval = xval.replace(element, element + '<br/>');
			});
			xaxisticks.push([++j,xval]);
		}
	}

	var options = {
			series: {
				bars: {
					show: true
				}
			},
			bars: {
				align: "right",
				barWidth: 0.5
			},
			xaxis: {
				show:true,
				axisLabel:'Categories',
				axisLabelUseCanvas: true,
				axisLabelFontSizePixels: 16,
				axisLabelFontFamily: 'Arial, Verdana',
				axisLabelPadding: 10,
				ticks: xaxisticks,
				tickLength:1,
				min:0,
				max:maxToBeSelected,
			},
			yaxis: {
				show:true,
				axisLabel: 'Mbps',
				axisLabelUseCanvas: true,
				axisLabelFontSizePixels: 16,
				axisLabelFontFamily: 'Arial, Verdana',
				axisLabelPadding: 10,
				min:0,
			}
		};

	dataset.push({hoverable:true,data:dataval1[0]});

	$.plot($("#BandwidthBarGraph"), dataset, options);
}
/*****
* Function For drawing the Bandwidth usage category Graph.END
*****/
/*****
* Function For populating the category box.START
* gRefSelectedCatgryLst array holds the categories checked by user
* between different sessions of 5000ms. while gSelectedCatgry holds the
* category values to be displayed in the graph for current session.
*****/
function populateSelectCategoryBox(array)
{
	var i, j;
	var element;
	var selectedLen = 0;
	element = '';
	var cat = array.Categories;
	var catnames = [];

	if (firsttime == 1) {
		var len = maxToBeSelected;
		firsttime = 0;
		len = cat.length > maxToBeSelected ? maxToBeSelected : cat.length;
		for(i=0;i<len;i++){
			gSelectedCatgry.push(cat[i].Name);
			gRefSelectedCatgryLst.push(cat[i].name);
		}
	}
	for (i=0;i<cat.length;i++) {
		catnames.push(cat[i].Name);
	}

	/* If the names is not there in the previous global array remove the item */
	for (i = 0; i < gSelectedCatgry.length; i++) {
		if ($.inArray(gSelectedCatgry[i], catnames) < 0)
			gSelectedCatgry.splice( i, 1 );
	}
	element += '<tr><td>Select Category List:Max('+maxToBeSelected+')</td></tr>';
	for(i=0;i<cat.length;i++) {
		/* If the name is there in selected array or the selected length is less than max to be selected */
		if (selectedLen <= (maxToBeSelected-1)) {
			if( ($.inArray(cat[i].Name, gRefSelectedCatgryLst) >= 0) && ($.inArray(cat[i].Name, gSelectedCatgry) < 0) ) {
				gSelectedCatgry.push(cat[i].Name);
			}
			if( ($.inArray(cat[i].Name, gSelectedCatgry) >= 0) ) {
				element += '<tr><td><input id="selectcategorychk" type="checkbox" value="'+cat[i].Name+'" checked="checked">'+cat[i].Name+'</input></td></tr>';
				$('#selectcategorychk').not(':checked').prop("disabled",true);
				selectedLen++;
			} else {
				element += '<tr><td><input id="selectcategorychk" type="checkbox" value="'+cat[i].Name+'">'+cat[i].Name+'</input></td></tr>';
				$('#selectcategorychk').not(':checked').prop("disabled",false);
			}
		} else { /* Else add in the list and disable the item */
			element += '<tr><td><input id="selectcategorychk" type="checkbox" value="'+cat[i].Name+'">'+cat[i].Name+'</input></td></tr>';
			$('#selectcategorychk').not(':checked').prop("disabled",false);
		}
	}

	catnames = [];
	$('.selectcategorytbl tr').remove();
	$('.selectcategorytbl').append(element);
}
/*****
* Function For populating the category box.END
*****/
function populateApplication(array)
{
	updateApplicationSummary();
	if (array.Req == 'DeviceSummary')
		populateDeviceSummary(array.Device[0]);
}
/*****
* Function For updating Image based on the Device type.START
*****/
function populateDeviceImage(devname)
{
	var deviceImage, currentname, currentdevname;
	var index = -1;
	$('.toplogo tr td:nth-child(3)').empty();
	$('.DeviceImages').empty();

	index = devname.lastIndexOf("-");
	if(index != -1) {
		currentname = devname.slice(0,index);
		currentdevname = devname.slice(index + 1, devname.length);
	}

	for (i = 0; i < gDeviceList.Device.length; i++)
	{
		if ((gDeviceList.Device[i].Name == currentname) || (gDeviceList.Device[i].IPAddr == currentname) || (gDeviceList.Device[i].DevName == currentdevname)) {
			deviceImage = GetIcon("devtype", gDeviceList.Device[i].DevTypeInt);
			$('.toplogo tr td:nth-child(3)').append('<image src='+deviceImage+' width="100px"  style="height=100px;"/>');
			$('.DeviceImages').append('<image src='+deviceImage+' width="7%"  style="position: relative; margin-top: 0px;"/>');
			break;
		}
	}
}
/*****
* Function For updating Image based on the Device type.END
*****/
/*****
* Function For updating the summary and Application Name.START
*****/
function updateApplicationSummary()
{
	var deviceSelectedElem;
	var appSelectedElem;

	var devname = $("#devlist option:selected").text();
	$('.deviceselectedtbl tr').remove();
	$('.appselectedtbl tr').remove();

	$deviceSelectedElem = '<tr><td>Summary : '+devname+'</td></tr>';
	$appSelectedElem = '<tr><td>Application : '+devname+'</td></tr>';

	$('.deviceselectedtbl').append($deviceSelectedElem);
	$('.appselectedtbl').append($appSelectedElem);

	populateDeviceImage(devname);
}
/*****
* Function For updating the summary and Application Name.END
*****/

function getDeviceList(devname)
{
	var array = [];

	for(i=0;i<gDeviceListTimers.length;i++){
		clearTimeout(gDeviceListTimers[i]);
	}
	gDeviceListTimers = [];

	$.ajax({
		type:"GET",
		url:urldevicelist,
		async:false,
		timeout:refreshTime,
		success:function(result){
			if (isHTTPDServer == 1)
				array = result;
			else
				array = JSON.parse(result);

			gDeviceList = array;
			populateDeviceList(array);
			if (gComboval.length <= 0) {
				$("#devlist option:first-child").attr("selected","selected");
				gComboindex = 0;
				PopulateDeviceListDetails(array,gComboindex);
			}
			else
				$('#devlist').val(gComboval);
			gDeviceListTimers.push(setTimeout(function(){getDeviceList(devname);},refreshTime));
		}
	});

	if (gComboindex >= 0 && jQuery.isEmptyObject(array) == false && array.Device.length > gComboindex)
		getApplicationList(gComboval, array.Device[gComboindex].MacAddr);
	else
		gDeviceListTimers.push(setTimeout(function(){getDeviceList(devname);},refreshTime));
}

function getApplicationList(devname, mac)
{
	var arrayApps = [];

	$.ajax({
		type:"GET",
		url:urldevicesummary+'&MacAddr='+mac,
		async:false,
		timeout:refreshTime,
		success:function(result){
			if (isHTTPDServer == 1)
				arrayApps = result;
			else
				arrayApps = JSON.parse(result);
			gDeviceSummary = arrayApps;
			populateApplication(arrayApps);
		}
	});
}

function onSelectDevice(devname)
{
	for(i=0;i<gDeviceListTimers.length;i++){
		clearTimeout(gDeviceListTimers[i]);
	}
	gDeviceListTimers = [];

	gLinkXVal = [];
	gLinkYVal = [];

	getDeviceList(devname);

}

function showcategory()
{
	var top = $("#selectcategorybtn").position().top;
	var left = $("#selectcategorybtn").position().left;
	$('.selectcategorytbl').css('top', top+33);
	$('.selectcategorytbl').css('left', left);
	$('.selectcategorytbl').toggle();
	$('.selectcategorytbl').css("z-index", 1000);
}
/*****
* Function which will get called when DOM for the page gets loaded.START
*****/
$(document).ready(function()
{
	initPage();

	getConfiguration();

	for(i=0;i<gDeviceListTimers.length;i++){
		clearTimeout(gDeviceListTimers[i]);
	}
	gDeviceListTimers = [];

	$("#devlist").change(function() {
		gComboval = $(this).val();
		gComboindex = $("#devlist option:selected").index();
		$('#devlist').val(gComboval);
		onSelectDevice($(this).val());
		PopulateDeviceListDetails(gDeviceList, gComboindex);
		updateApplicationSummary();

	});
	$('body').click(function(evt) {
		if($(evt.target).parents('.selectcategory').length==0) {
			$('.selectcategorytbl').css('display','none');
		}
	});
	$('#selectcategorybtn').click(function(e) {
		showcategory();
	});

	/****
	 * In below click event handler gSelectedCatgry array maintains the selected
	 * categories for the current session while gRefSelectedCatgryLst array
	 * maintains these items for all sessions.
	****/
	$('.selectcategorytbl').on('click', '#selectcategorychk', function(){
		if($(this).is(":checked")) {
			var countchecked = $(".selectcategorytbl input[type=checkbox]:checked").length;
			if (countchecked > maxToBeSelected) {
				$(this).removeAttr('checked');
				$('.selectcategorytbl input[type=checkbox]').not(':checked').attr("disabled",true);
			} else {
				$('.selectcategorytbl input[type=checkbox]').not(':checked').attr("disabled",false);
				gSelectedCatgry.push($(this).val());
				gRefSelectedCatgryLst.push($(this).val());
			}
		} else {
			gSelectedCatgry.splice( $.inArray($(this).val(), gSelectedCatgry), 1 );
			gRefSelectedCatgryLst.splice( $.inArray($(this).val(), gRefSelectedCatgryLst), 1 );
			$('.selectcategorytbl input[type=checkbox]').not(':checked').attr("disabled", false);
		}
	});

	$('.selectcategorytbl').css('display','none');
	var combotext = $("#devlist option:selected").text();
	onSelectDevice(combotext);
});
