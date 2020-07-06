/*
 * Jquery implementation for Common API for iQoS Pages
 *
 * $ Copyright Broadcom Corporation $
 *
 *
 * <<Broadcom-WL-IPTag/Proprietary:>>
 *
 * $Id: iqoscommon.js, v 1.0 2015-02-12 15:25:43 vbemblek $
 */

/* Function to check if a given variable has a numeric value or not */
function isNumber(n) {
	  return !isNaN(parseFloat(n)) && isFinite(n);
}

/* Function for getting device type / application category icon */
function GetIcon(which, id)
{
	if (isNumber(id) != true) {
		return "unknown.png";
	}
	if ((which != "devtype") && (which != "appcat")) {
		return "unknown.png";
	}
	if ((which == "devtype") && (id <= 0)) {
		return "unknown.png";
	}
	if ((which == "appcat") && (id < 0)) {
		return "unknown.png";
	}
	return which + id + ".png";
}

function ManageMissingIcon(image)
{
	image.onerror = "";
	image.src = "default.png";
	return true;
}

function ConvertEpochTimeToString(tm)
{
	months = ['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec']
	var d = new Date(0); // set the date to epoch
	var ampm = "am";
	d.setUTCSeconds(tm);
	var hr = d.getHours();
	if (hr >= 12) {
		ampm = "pm";
		hr -= 12;
	}
	var min = d.getMinutes();
	if (min < 10)
		min = "0" + min;

	var sec = d.getSeconds();
	if (sec < 10)
		sec = "0" + sec;

	var dstr = d.getDate() + " " + months[d.getMonth()] + ", " + hr + ":" + min + ":" + sec + " " + ampm;
	return dstr;
}

