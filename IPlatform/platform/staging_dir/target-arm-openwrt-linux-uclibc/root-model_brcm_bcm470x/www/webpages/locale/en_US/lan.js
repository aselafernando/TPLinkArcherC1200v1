(function($){

	$.su = $.su || {};
	$.su.CHAR = {
		LOGIN: {
			USERNAME: 					"Username",
			PASSWORD: 					"Password",
			FORGET_PASSWORD: 			"Forgot password?",
			LOGIN: 						"Login",
			IMPORTANT_UPDATE_INFO: 		"To avoid IP conflict with the front-end device, your router's IP address has been changed to",
			CONTINUE: 					"Continue",

			IMPORTANT_NOTICE: 			"Important Notice",
			OR: 						", are you sure you want to continue to visit",
			END: 						".",
			END2: 						"?",

			FORGET_PASSWORD_INFO_0: 	"Press and hold the Reset button for 10 seconds to restore the router to its factory defaults.",
			FORGET_PASSWORD_INFO_1: 	"Enabling the Password Recovery function allows the system to send a verfication code to your alternate email address to reset the default username and password.",
			FORGET_PASSWORD_SEND_FAILED:"Failed to send the code. Please check your Internet connection and verify the password recovery parameters in \"System Tools -> Administration\".",

			VERIFICATION_CODE: 			"Verification code",

			RECEIVE_CODE: 				"Send Code",

			CONFIRM: 					"Confirm",

			SEC: 						"s",

			USER_CONFLICT: 				"Login Conflict.",
			FIRST_TIME: 				"Welcome to use Archer C9 designed by TP-LINK in China. To begin, create a device password to manage it.",
			
			USER_CONFLICT_INFO: 		"User %USER% with host %HOST% (%IP%/%MAC%) is currently logged into the router. You cannot login at the same time. Please try again later.",
			USER_CONFLICT_INFO_2: 		"User %USER% (%IP%) is currently logged into the router. You cannot login at the same time. Please try again later.",
			
			LOGIN_FAILED: 				"Login Failure.",
			LOGIN_FAILED_COUNT: 		"Login failed for %num1 times and you still have %num2 attempts left.",
			NO_COOKIE: 					"Cookies must be enabled to log in. Please enable Cookies or turn off Private/Incognito browsing mode.", 

			FORGET_PASSWORD_NOTE: 		"You can also press and hold the Reset button for 10 seconds to restore the router to its default username and password (admin/admin)."
		},
		INIT: {
			PASSWORD: 					"Password",
			BEGIN: 						"Begin",
			IMPORTANT_UPDATE_INFO: 		"To avoid the conflict with the front-end device, your router's IP address has been updated to",
			CONTINUE: 					"Next",

			IMPORTANT_NOTICE: 			"Important Notice",
			OR: 						", are you sure you want to continue to visit",
			END: 						".",
			END2: 						"?",

			CONFIRM: 					"Confirm",

			SEC: 						"s",

			USER_CONFLICT: 				"Login Conflict!",
			
			USER_CONFLICT_INFO: 		"User %USER% with host %HOST% (%IP%/%MAC%) is currently logged into the Router. You cannot login at the same time. Please try again later.",
			USER_CONFLICT_INFO_2: 		"User %USER% (%IP%) is currently logged into the Router. You cannot login at the same time. Please try again later.",
			
			LOGIN_FAILED: 				"Login Failure!",
			LOGIN_FAILED_COUNT: 		"Login failed for %num1 times and you still have %num2 attempts left.",
			NO_COOKIE: 					"Cookies must be enabled to login. Please enable Cookies or turn off  Private/Incognito browsing mode.", 

			INIT_NOTE_TITLE: 			"Dear Customer,",
			INIT_NOTE_CONTENT: 			"Please set up your Archer C9 to connect to the Internet first. To begin, create a device password to manage your Archer C9."

		},

		UPGRADE: {
			FIRMWARE_VERSION: 				"Firmware Version: ",
			FEATURE: 						"Modifications and Bug Fixes: ",
			TITLE: 							"Available firmware upgrade is detected.",
			UPGRADE_NOW: 					"Upgrade now",
			REMIND: 						"Remind me later",
			NOTICE:    						"Hi, a new firmware is available for the Archer C1200 router.",
			NEVER: 							"Ignore this version"
			
		},

		WAN_ERROR: {
			TITLE: 							"WAN connection error!",
			STATUS: 						"Status",
			INFO: 							"Information",
			SETUP: 							"Set up an Internet connection",
			NEVER: 							"Don't remind me again"
		},

		INDEX: {
			FIRMWARE_VERSION: 				"Firmware Version:",
			HARDWARE_VERSION: 				"Hardware Version:",
			HELP_SUPPORT: 					"Support",
			FAQ: 							"FAQ",
			CONFIRM_REBOOT: 				"Are you sure you want to reboot the router?",
			CONFIRM_LOGOUT: 				"Are you sure you want to log out?",
			UPGRADE_ALERT_1: 				"The current firmware does not support TP-LINK cloud service. We strongly recommend that you download the latest firmware at www.tp-link.com and update it.",
			UPGRADE_ALERT_2: 				"The current firmware does not support TP-LINK cloud service. We strongly recommend that you update the firmware by clicking the Update icon in the upper right corner.",

			REBOOTING: 						"Rebooting... <br/>Please do NOT operate during the rebooting process.",
		},

		NAV: {
			QUICK_SETUP: 				"Quick Setup",
			BASIC: 						"Basic",
			ADVANCED: 					"Advanced"
		},

		CONTROL: {
			LOGIN: 						"Login",
			LOGOUT: 					"Logout",
			UPDATE: 					"Update",
			REBOOT: 					"Reboot",
			LED: 						"LED",
			LED_ON: 					"LED On",
			LED_OFF: 					"LED Off",
			LED_DISABLED: 				"The LED status cannot be changed during the night mode period"
		},

		LANGUAGE: {
			EN_US: 						"English",
			ZH_CN: 						"简体中文"
		},

		REGION: {
			ALBANIA: 					"Albania",
			ALGERIA: 					"Algeria",
			AMERICAN_SAMOA: 			"American Samoa",
			ARGENTINA: 					"Argentina",
			ARMENIA: 					"Armenia",
			ARUBA: 						"Aruba",
			AUSTRALIA: 					"Australia",
			AUSTRIA: 					"Austria",
			AZERBAIJAN: 				"Azerbaijan",
			BAHAMAS: 					"Bahamas",
			BAHRAIN: 					"Bahrain",
			BANGLADESH: 				"Bangladesh",
			BARBADOS: 					"Barbados",
			BELARUS: 					"Belarus",
			BELGIUM: 					"Belgium",
			BELIZE: 					"Belize",
			BERUMUDA: 					"Bermuda",
			BOLIVIA: 					"Bolivia",
			BOSNIA_HERZEGOWINA: 		"Bosnia and Herzegovina",
			BRAZIL: 					"Brazil",
			BRUNEI_DARUSSALAM: 			"Brunei Darussalam",
			BULGARIA: 					"Bulgaria",
			CAMBODIA: 					"Cambodia",
			CANADA: 					"Canada",
			CAYMAN_ISLANDS: 			"Cayman Islands",
			CHILE: 						"Chile",
			CHINA: 						"China",
			COLOMBIA: 					"Colombia",
			COSTA_RICA: 				"Costa Rica",
			CROATIA: 					"Croatia",
			CYPRUS: 					"Cyprus",
			CZECH_REPUBLIC: 			"Czech Republic",
			DENMARK: 					"Denmark",
			DOMINICAN_REPUBLIC: 		"Dominican Republic",
			ECUADOR: 					"Ecuador",
			EGYPT: 						"Egypt",
			EL_SALVADOR: 				"El Salvador",
			ESTONIA: 					"Estonia",
			ETHIOPIA: 					"Ethiopia",
			FAEROE_ISLANDS: 			"Faroe Islands",
			FINLAND: 					"Finland",
			FRANCE: 					"France",
			FRENCH_GUIANA: 				"French Guiana",
			FRENCH_POLYNESIA: 			"French Polynesia",
			GEORGIA: 					"Georgia",
			GERMANY: 					"Germany",
			GREECE: 					"Greece",
			GREENLAND: 					"Greenland",
			GRENADA: 					"Grenada",
			GUADELOUPE: 				"Guadeloupe",
			GUAM: 						"Guam",
			GUATEMALA: 					"Guatemala",
			HAITI: 						"Haiti",
			HONDURAS: 					"Honduras",
			HONG_KONG: 					"Hong Kong",
			HUNGARY: 					"Hungary",
			ICELAND: 					"Iceland",
			INDIA: 						"India",
			INDONESIA: 					"Indonesia",
			IRAN: 						"Iran",
			IRAQ: 						"Iraq",
			IRELAND: 					"Ireland",
			ISRAEL: 					"Israel",
			ITALY: 						"Italy",
			JAMAICA: 					"Jamaica",

			JAPAN: 						"Japan",
			JAPAN_1: 					"Japan 1",
			JAPAN_2: 					"Japan 2",
			JAPAN_3: 					"Japan 3",
			JAPAN_4: 					"Japan 4",
			JAPAN_5: 					"Japan 5",
			JAPAN_6: 					"Japan 6",

			JORDAN: 					"Jordan",
			KAZAKHSTAN: 				"Kazakhstan",
			KENYA: 						"Kenya",

			NORTH_KOREA: 				"North Korea",
			KOREA_REPUBLIC: 			"Korea Republic",
			KOREA_REPUBLIC_3: 			"Korea Republic 3",

			KUWAIT: 					"Kuwait",
			LATVIA: 					"Latvia",
			LEBANON: 					"Lebanon",
			LIBYA: 						"Libya",
			LIECHTENSTEIN: 				"Liechtenstein",
			LITHUANIA: 					"Lithuania",
			LUXEMBOURG: 				"Luxembourg",
			MACAU: 						"Macau",
			MACEDONIA: 					"Macedonia",
			MALAWI: 					"Malawi",
			MALAYSIA: 					"Malaysia",
			MALDIVES: 					"Maldives",
			MALTA: 						"Malta",
			MARTHINIQUE: 				"Martinique",
			MAURITIUS: 					"Mauritius",
			MAYOTTE: 					"Mayotte",
			MEXICO: 					"Mexico",
			MONACO: 					"Monaco",
			MONGOLIA: 					"Mongolia",
			MOROCCO: 					"Morocco",
			NEPAL: 						"Nepal",
			NETHERLANDS: 				"Netherlands",
			NETHERLANDS_ANTILLES: 		"Netherlands Antilles",

			NEW_ZEALAND: 				"New Zealand",
			NICARAGUA: 					"Nicaragua",
			NIGERIA: 					"Nigeria",
			NORWAY: 					"Norway",
			NORTHERN_MARIANA_ISLANDS: 	"Northern Mariana Islands",
			OMAN: 						"Oman",
			PAKISTAN: 					"Pakistan",
			PANAMA: 					"Panama",
			PAPUA_NEW_GUINEA: 			"Papua New Guinea",
			PARAGUAY: 					"Paraguay",
			PERU: 						"Peru",
			PHILIPPINES: 				"Philippines",
			POLAND: 					"Poland",
			PORTUGAL: 					"Portugal",
			PUERTO_RICO: 				"Puerto Rico",
			QATAR: 						"Qatar",
			REUNION: 					"Reunion",
			ROMANIA: 					"Romania",
			RUSSIA: 					"Russia",
			RWANDA: 					"Rwanda",
			SAMOA: 						"Samoa",
			SAUDI_ARABIA: 				"Saudi Arabia",
			SINGAPORE: 					"Singapore",
			SLOVAK_REPUBLIC: 			"Slovak Republic",
			SLOVENIA: 					"Slovenia",
			SOUTH_AFRICA: 				"South Africa",
			SPAIN: 						"Spain",
			SRI_LANKA: 					"Sri Lanka",
			SURINAME: 					"Suriname",
			SWEDEN: 					"Sweden",
			SWITZERLAND: 				"Switzerland",
			SYRIA: 						"Syria",
			TAIWAN: 					"Taiwan",
			TANZANIA: 					"Tanzania",
			THAILAND: 					"Thailand",
			TRINIDAD_TOBAGO: 			"Trinidad & Tobago",
			TUNISIA: 					"Tunisia",
			TURKEY: 					"Turkey",
			UGANDA: 					"Uganda",
			UKRAINE: 					"Ukraine",
			UNITED_ARAB_EMIRATES: 		"United Arab Emirates",
			UNITED_KINGDOM: 			"United Kingdom",
			UNITED_STATES: 				"United States",
			URUGUAY: 					"Uruguay",
			UZBEKISTAN: 				"Uzbekistan",
			VENEZUELA: 					"Venezuela",
			VIETNAM: 					"Viet Nam",
			VIRGIN_ISLANDS: 			"Virgin Islands (U.S.)",
			YEMEN: 						"Yemen",
			ZIMBABWE: 					"Zimbabwe"
		},

		TIME_ZONE: {
			ENIWETOK: 					"(GMT-12:00) Eniwetok, Kwajalein", 
			MIDWAY_ISLAND_SAMOA: 		"(GMT-11:00) Midway Island, Samoa",
			HAWAII: 					"(GMT-10:00) Hawaii",
			ALASKA: 					"(GMT-09:00) Alaska",
			PACIFIC_TIME: 				"(GMT-08:00) Pacific Time",
			MOUNTAIN_TIME: 				"(GMT-07:00) Mountain Time (US Canada)",
			CENTRAL_TIME: 				"(GMT-06:00) Central Time (US Canada)",
			EASTERN_TIME: 				"(GMT-05:00) Eastern Time (US Canada)",
			CARACAS:					"(GMT-04:30) Caracas",
			ATLANTIC_TIME: 				"(GMT-04:00) Atlantic Time (Canada)",
			NEWFOUNDLAND: 				"(GMT-03:30) Newfoundland",

			BRASILIA_BUENOS_AIRES: 		"(GMT-03:00) Brasilia, Buenos Aires",
			MID_ATLANTIC: 				"(GMT-02:00) Mid-Atlantic",
			AZORES_CAPE_VERDE_IS: 		"(GMT-01:00) Azores, Cape Verde Is",
			GREENWICH_MEAN_TIME: 		"(GMT) Greenwich Mean Time, Dublin, London",
			BERLIN_STOCKHOLM: 			"(GMT+01:00) Berlin, Stockholm, Rome, Bern, Brussels",
			ATHENS_HELSINKI: 			"(GMT+02:00) Athens, Helsinki, Eastern Europe, Israel",
			BAGHDAD_KUWAIT: 			"(GMT+03:00) Baghdad, Kuwait, Nairobi, Riyadh, Moscow",

			TEHERAN: 					"(GMT+03:30) Teheran",

			ABU_DHABI: 					"(GMT+04:00) Abu Dhabi, Muscat, Kazan, Volgograd",

			KABUL: 						"(GMT+04:30) Kabul",

			ISLAMABAD_KARACHI: 			"(GMT+05:00) Islamabad, Karachi, Ekaterinburg",

			MADRAS_CALCUTTA: 			"(GMT+05:30) Madras, Calcutta, Bombay, New Delhi",
			KATMANDU: 					"(GMT+05:45) Katmandu",

			ALMA_ATA_DHAKA: 			"(GMT+06:00) Alma-Ata, Dhaka",
			RANGOON: 					"(GMT+06:30) Rangoon",

			BANGKOK_JAKARTA_HANOI: 		"(GMT+07:00) Bangkok, Jakarta, Hanoi",
			BEIJING_HONGKONG: 			"(GMT+08:00) Beijing, Hong Kong, Perth, Singapore",
			TOKYO_OSAKA_SAPPORO: 		"(GMT+09:00) Tokyo, Osaka, Sapporo, Seoul, Yakutsk",

			ADELAIDE: 					"(GMT+09:30) Adelaide",

			BRISBANE_CANBERRA: 			"(GMT+10:00) Brisbane, Canberra, Melbourne, Sydney",
			MAGADAN_SOLOMAN_IS: 		"(GMT+11:00) Magadan, Solomon Is., New Caledonia",
			FIJI_KAMCHATKA: 			"(GMT+12:00) Fiji, Kamchatka, Auckland",
			NUKU: 						"(GMT+13:00) Nuku'alofa"
		},

		APPLIST:{
			APP:						"Application",
			GAME:						"Game",
			QQ:							"QQ",
			MSN:						"MSN",
			LINE:						"LINE",
			Skype:                      "Skype",
			PPStream: 					"PPStream",
			SIP:  						"SIP",
			PPTC: 						"PPTC", 
			H323: 						"H323",
			HTTPFD: 					"HTTPFD",
			PPTP: 						"PPTP",
			L2TP:  						"L2TP",
			IPSec:                      "IPSec",
			IMAP: 						"IMAP",
			xl_others:  				"xl_others",
			Vonage:  					"Vonage",
			netTalk:  					"netTalk",
			iTalkBB: 					"iTalkBB",
			HTTP: 						"HTTP",
			MMS:  						"MMS",
			RTSP:                       "RTSP",
			WOW:						"WOW",
			LOL:						"LOL",
			SSH:						"SSH",
			TELNET:						"Telnet",
			VPN:						"VPN",
			FTP:						"FTP",
			WWW:						"WWW",
			DNS:						"DNS",
			ICMP:						"ICMP",
			SMTP:						"SMTP",
			NNTP:						"NNTP",
			POP3:						"POP3",
			HTTPS:  					"HTTPS",
			BT: 						"BT",
			Emule: 						"Emule",
			Facebook: 					"Facebook",
			GTalk: 						"GTalk",
			Whatsapp: 					"WhatsApp",


			END:						""
		},

		DATE: {
			DAY: 						"Day",

			MONDAY: 					"Monday",
			TUESDAY: 					"Tuesday",
			WEDNESDAY: 					"Wednesday",
			THURSDAY: 					"Thursday",
			FRIDAY: 					"Friday",
			SATURDAY: 					"Saturday",
			SUNDAY: 					"Sunday",
			
			MON: 						"Mon",
			TUES: 						"Tue",
			WED: 						"Wed",
			THUR: 						"Thu",
			FRI: 						"Fri",
			SAT: 						"Sat",
			SUN: 						"Sun",

			JAN: 						"Jan",
			FEB: 						"Feb",
			MAR: 						"Mar",
			APR: 						"Apr",
			MAY: 						"May",
			JUN: 						"Jun",
			JUL: 						"Jul",
			AUG: 						"Aug",
			SEP: 						"Sep",
			OCT: 						"Oct",
			NOV: 						"Nov",
			DEC: 						"Dec"

		},

		HOUR: {
			AM_1: 						"1 AM",
			AM_2: 						"2 AM",
			AM_3: 						"3 AM",
			AM_4: 						"4 AM",
			AM_5: 						"5 AM",
			AM_6: 						"6 AM",
			AM_7: 						"7 AM",
			AM_8: 						"8 AM",
			AM_9: 						"9 AM",
			AM_10: 						"10 AM",
			AM_11: 						"11 AM",
			AM_12: 						"12 AM",
			PM_1: 						"1 PM",
			PM_2: 						"2 PM",
			PM_3: 						"3 PM",
			PM_4: 						"4 PM",
			PM_5: 						"5 PM",
			PM_6: 						"6 PM",
			PM_7: 						"7 PM",
			PM_8: 						"8 PM",
			PM_9: 						"9 PM",
			PM_10: 						"10 PM",
			PM_11: 						"11 PM",
			PM_12: 						"12 PM"
		},

		ORDER: {
			"1ST": 						"First",
			"2ND": 						"2nd",
			"3RD": 						"3rd",
			"4TH": 						"4th",
			"5TH": 						"Last",
			"1ST_": 					"1st",

			TH: 						"th"
		},

		GRID: {
			CLIENT_NUMBER: 				"Total Clients",

			ID: 						"ID",
			MODIFY: 					"Modify",
			STATUS: 					"Status",
			ENABLE: 					"Enable",

			OPERATION: 					"Operation",
			CHOOSE: 					"Choose",
			DESCRIPTION: 				"Description",
			

			AUTO_REFRESH: 				"Auto Refresh",
			REFRESH: 					"Refresh",
			NUMBER: 					"Number",
			ENABLED: 					"Enabled",
			DISABLED: 					"Disabled",
			ACTIVE: 					"Active",
			SELECTED: 					""
		},

		OPERATION: {
			ADD: 						"Add",
			CHOOSE: 					"Choose",
			EDIT: 						"Edit",
			DELETE: 					"Delete",
			DELETE_ALL: 				"Delete All",
			REMOVE: 					"Remove",
			RESET: 						"Reset",
			RESET_ALL: 					"Reset All",
			DETECT: 					"Detect",
			ENABLE: 					"Enable",
			DISABLE: 					"Disable",

			REFRESH: 					"Refresh",
			SEARCH: 					"Search...",
			BROWSE: 					"Browse",

			SAVE: 						"Save",
			BACK: 						"Back",

			PREV: 						"Prev",
			NEXT: 						"Next",
			FINISH: 					"Finish",
			
			ON: 						"On",
			OFF: 						"Off",
			LOW: 						"Low",
			MIDDLE: 					"Middle",
			HIGH: 						"High",
			
			OK: 						"OK",
			CANCEL: 					"Cancel",

			YES: 						"Yes",
			NO: 						"No",
			
			CONNECTED: 					"Connected",
			CONNECTING: 				"Connecting",
			DISCONNECTING: 				"Disconnecting",
			DISCONNECTED: 				"Not Connected",

			PASSWORD_HINT: 				"Password",
			FILEBUTTONTEXT: 			"Browse",
			FILEBLANKTEXT: 				"Please select a file.",
			NOSELECTEDTEXT: 			"-Please Select-",

			ADD_A_NEW_KEYWORD: 			"Add a New Keyword",

			SUCCESSED: 					"Success!",
			FORM_SAVED: 				"Saved",
			FORM_FAILED: 				"Failed",
			GRID_ID_COLUMN: 			"ID",
			GRID_SAVED: 				"Saved",
			GRID_FAILED: 				"Failed",
			GRID_NONE_SELECT: 			"Please select at least one entry.",
			GRID_DELETE_COMFIRM: 		"Are you sure you want to delete the selected entries?",
			GRID_DELETE_ALL_COMFIRM: 	"Are you sure you want to delete all the entries?",
			GRID_MAX_RULES: 			"Maximum entries exceeded.",
			KEYWORD_MAX_OVERFLOW: 		"The number of keywords has reached the limit.",

			NOTE: 						"Note:"
		},
		
		VTYPETEXT: {
			INVALIDTEXT: 				"Invalid format.",
			BLANKTEXT: 					"This field is required.",

			EMAIL: 						"Invalid email format.",
			NUMBER: 					"Invalid format.",

			NUMBER_MIN: 				"This number should be greater than %min.",
			NUMBER_MAX: 				"This number should be less than %max.",

			NUMBER_MIN_MAX: 			"This number should be between %min and %max.",
			HEX: 						"This field should be a hexadecimal number.",

			IP: 						"Invalid format.",

			IP_NO_ALL_ZERO:				"The address cannot be 0.0.0.0.",
			IP_NO_LOOP:					"The address cannot be loopback address.",
			IP_NO_D_TYPE:				"The IP address cannot be a Class D address.",
			IP_NO_E_TYPE:				"The IP address cannot be a Class E address.",
			IP_NO_ALL_ONE:				"The address cannot be 255.255.255.255.",
			IP_NO_FIRST_ALL_ONE:		"The address cannot start with 255.",
			IP_NO_FIRST_ZERO:			"The address cannot start with 0.",
			MASK_NO_ALL_ONE:			"Subnet mask cannot be 255.255.255.255.",

			IPV6: 						"Invalid format.",
			IPV6_NOT_GLOBAL:			"Invalid format.",
			IPV6_NOT_PREFIX:			"Invalid format.",
			IP_DOMAIN: 					"Invalid format.",
			IPV6_DOMAIN: 				"Invalid format.",
			MAC: 						"Invalid format.",
			MULTI_MAC:					"Invalid format.",
			DATE: 						"Invalid format.",
			DATE_INVALID: 				"Please enter a valid date between 01/01/1970 and 12/31/2030.",
			MASK: 						"Invalid format.",
			DOMAIN: 					"Invalid format.",
			STRING_DOMAIN:              "Invalid format.",
			USER: 						"Invalid format.",
			NOTE: 						"Invalid format.",
			PWD: 						"Invalid format.",
			SSID: 						"Invalid format.",
			NAME:						"Invalid format.",
			ASCII_VISIBLE:				"Invalid format.",
			STRING_VISIBLE:				"Invalid format.",
			STRING_VISIBLE_NO_COMMA:    "Invalid format.",
			STRING_VISIBLE_ALLOW_BLANK: "Invalid format.",
			VPN_NAME_PWD: 				"Please enter 1-15 alpha characters, numbers, - and _."	
			
		},


		ERROR: {			
			"00000001":					"Invalid file type.",
			"00000002":					"Checksum error.",
			"00000003":					"The file is too large.",
			"00000004":					"Upload error.",
			"00000005":					"Reboot error.",
			"00000006":					"Unknown error.",
			"00000007":					"The item already exists. Please enter another one.",

			"00000009":					"Invalid port.",
			"00000010":					"The port should be a number.",

			"00000011":					"Username should be the same as the From field value.",
			"00000012": 				"The username must start with an alphabet character.",

			"00000021":					"Invalid format.",

			"00000032": 				"Value must be less than Low.",
			"00000033": 				"Value must be less than Middle and Low.",
			"00000034": 				"Invalid value. Please enter a number between 5 and 7200.",

			"00000039": 				"Please use the default value 0 or enter a value between 30 and 86400.",
			"00000040": 				"SSID and MAC address are required.",

			"00000042": 				"Please use the default value 80 or enter a value between 1024 and 65535.",

			"00000045": 				"Default Gateway and LAN IP address should be in a same subnet. Please enter again.",

			"00000047": 				"IP address and LAN IP address should be in the same subnet. Please enter again.",

			
			"00000049":					"Invalid destination network.",

			"00000050": 				"Invalid DNS server IP address. Please enter another one.",
			"00000051": 				"This MAC address already exists. Please enter another one.",
			"00000052": 				"This IP address already exists. Please enter another one.",

			"00000053": 				"The starting address cannot be greater than the ending address. <br/>Please enter again.",

			"00000054": 				"The IP address pool and LAN IP address should be in the same subnet. Please enter another one.",

			"00000055": 				"The IP address cannot be the same as the LAN address.",

			"00000056": 				"The remote IP address and the current LAN IP address cannot be in the same subnet. Please enter another one.",

			"00000057": 				"Invalid PSK Password. Please enter again.",
			"00000058": 				"Invalid WEP Password. Please enter again.",

			"00000059": 				"Invalid Subnet Mask and LAN IP address, please enter a valid one.",

			"00000060": 				"WAN IP address and LAN IP address cannot be in the same subnet. <br/>Please enter another one.",

			"00000061": 				"The starting time should be earlier than the ending time.",

			"00000062": 				"This field is required.",
			"00000063": 				"This field is required.",

			"00000064": 				"Cannot block the host MAC address.",
			"00000065": 				"This item conflicts with existed items. Please try again.",
			
			"00000066": 				"The password should be 8 to 63 characters or 64 hexadecimal digits.",
			"00000067": 				"The password should be 10 hexadecimal digits.",
			"00000068": 				"The password should be 5 ASCII characters.",
			"00000069": 				"The password should be 26 hexadecimal digits.",
			"00000070": 				"The password should be 13 ASCII characters.",
			"00000071": 				"The password should be 32 hexadecimal digits.",
			"00000072": 				"The password should be 16 ASCII characters.",
			"00000073": 				"The password should be less than 64 characters.",

			"00000074": 				"Invalid file type.",

			"00000075": 				"The PIN length should be 8 digits.",

			"00000076": 				"The entry conflicts with existed items. Please check trigger port and trigger protocol.",
			"00000077": 				"The IP address cannot be the same with the LAN IP address. Please enter again.",
			"00000078": 				"The host IP address cannot be the same with the LAN IP address. Please enter again.",

			"00000080": 				"Passwords mismatch. Please try again.",

			"00000088": 				"This operation is not allowed for remote management.",
			"00000089": 				"Maximum login attempts %num exceeded. Please try again in two hours.",

			"00000090": 				"The destination cannot be the LAN IP address.",
			"00000091": 				"The destination cannot be the WAN IP address.",

			"00000092": 				"The IP address and the LAN IP address cannot be in the same subnet. <br/>Please enter again.",
			"00000093": 				"The IP address and the WAN IP address cannot be in the same subnet. <br/>Please enter again.",

			"00000094": 				"The VLAN IDs cannot be the same.",
			"00000095": 				"At least one Internet port is required.",

			"00000096": 				"The keyword already exists.",

			"00000097": 				"Configurations made to the 2.4GHz frequency bands will not take effect until the Wi-Fi button is ON.",
			"00000098": 				"Configurations made to the 5GHz frequency bands will not take effect until the Wi-Fi button is ON.",
			"00000099": 				"Configurations made to the 2.4GHz and 5GHz frequency bands will not take effect until the Wi-Fi button is ON.",

			"00000100": 				"The 5GHz network is unavailable due to restrictions in your region/country.",

			"00000101": 				"Your wireless function is off. If you want to use this function, please switch ON the Wi-Fi button.",
			"00000102": 				"Your wireless function is off. If you want to use this function, please switch ON the Wi-Fi button.",

			"00000103": 				"Your wireless function is off. If you want to use this function, please switch ON the Wi-Fi button.",
			"00000104": 				"Your wireless function is disabled.",

			"00000105": 				"QoS and IPTV cannot be enabled at the same time.",

			"00000106": 				"IP address cannot be the same with the LAN IP address.",
			
			"00000107": 				"The destination already exists.",

			"00000110": 				"The IP address and LAN IP address should be in the same subnet.",
			
			"00000111": 				"QoS and <a onclick=\"$.su.menu.advanced.goTo('system-parameters');\" src=\"void(0)\">NAT Boost</a> cannot be enabled at the same time.",
			"00000112": 				"The WDS function can work either at 2.4GHz or 5GHz band. Also, the Guest Network is not available on the WDS band.",
			"00000113": 				"WDS and Guest Network cannot be enabled at the same time.",
			"00000114": 				"Traffic Statistics and <a onclick=\"$.su.menu.advanced.goTo('system-parameters');\" src=\"void(0)\">NAT Boost</a> cannot be enabled at the same time.",
			"00000115": 				"IPTV and <a onclick=\"$.su.menu.advanced.goTo('system-parameters');\" src=\"void(0)\">NAT Boost</a> cannot be enabled at the same time.",

			"00000117": 				"The domain name already exists.",
			"00000118": 				"The number of domain names has reached the limit.",
			"00000119":				"NAT Boost will be disabled when either <a onclick=\"$.su.menu.advanced.goTo('qos_settings');\" src=\"void(0)\">QoS</a> or <a onclick=\"$.su.menu.advanced.goTo('sysstatistics');\" src=\"void(0)\">Traffic Statistics</a> is enabled.",

			"00000120": 				"The password should be 5 or 13 ASCII characters.",
			"00000121": 				"The password should be 10 or 26 hexadecimal digits.",
			"00000122": 				"The username and password are empty, are you sure you want to continue?",
			"00000123": 				"Saving... <br/>Please do NOT operate during the process.",
			"00000124": 				"The router's PIN is locked due to repeatedly connections using wrong PIN. Please generate a new one.",

			"00000125": 				"The number of QoS rules has exceeded the limit.",
			"00000126": 				"The file size has exceeded the limit.",
			"00000127": 				"The content of the file is incorrect.",
			"00000128": 				"Please select at least one application.",
			"00000129": 				"Please select at least one physical port.",
			"00000130":					"Your WPS function is disabled.",
			"00000131": 				"NTP Server cannot be loopback address.",
			"00000132": 				"Mode switch failed. Please try again later or reboot your router.",
			"00000133": 				"Invalid DMZ Host IP Address. Please enter a valid one.",
			"00000134":  				"Invalid Internal IP. Please enter a valid one.",
			"00000135": 				"System error.",
			"00000136": 				"Backup file error.",
			"00000137": 				"The username is empty, are you sure you want to continue?",
			"00000138": 				"The password is empty, are you sure you want to continue?",
			"00000139": 				"Incorrect password recovery parameters.",
			"00000140": 				"Incorrect code.",
			"00000141": 				"Password recovery is disabled.",
			"00000142": 				"Failed to send the code. Please check your Internet connection.",
			"00000143": 				"Invalid email addresses.",
			"00000144": 				"Invalid email message.",
			"00000145": 				"Could not find the host.",
			"00000146": 				"Authentication failed.",
			"00000147": 				"The port should be between 1 and 65535.",
			"00000148": 				"For a port range, the start port number should be less than the end port number. Please enter again.",
			"00000149": 				"Port number overlaps. Please enter again.",
			"00000150": 				"Invalid Subnet Mask and WAN IP address, please enter a valid one.",
			"00000151": 				"Please select at least one day.",
			"00000152": 				"Please set the Internet access time.",
			"10000139": 				"No Internet connection",
			"10000140": 				"Request timed out. Please check your Internet connection and  try again later.",
			"10000158": 				"The WAN port is unplugged.",
			"10000159": 				"Unable to connect to the Internet. Please contact your service provider or try again later. ",
			"10000160": 				"Unable to obtain IP address from the DHCP Server. Please check the WAN connection type or try again later.",
			"10000161": 				"PPPoE authentication failed. Please check your username and password.",
			"10000162": 				"Unable to connect to the PPPoE server.",

			"10000164": 				"PPTP authentication failed. Please check your username and password.",
			"10000165": 				"Unable to connect to the PPTP server.",

			"10000167": 				"L2TP authentication failed. Please check your username and password.",
			"10000168": 				"Unable to connect to the L2TP server.",
			"10000169": 				"Unknown error. Please try again later.",
			"10000172": 				"Connection failed.",

			"10000185": 				"System error.",	
			"10000186": 				"Firmware file error.",	
			"10000187": 				"Firmware download error.",	
			"10000188": 				"Firmware upgrade error.",	
			"10000191": 				"Unable to download the firmware file.",
			"10000192": 				"Unable to upgrade.",
			"10000193": 				"Unable to connect to the server.",	
			
			"99999999":					""
		},

		MENU: {
			STATUS: 					"Status",
			NETWORK: 					"Network",
			NETWORK_MAP: 				"Network Map",
			INTERNET: 					"Internet",

			LAN: 						"LAN",
			IPTV:                       "IPTV",
			DHCP_SERVER: 				"DHCP Server",
			DYNAMIC_DNS: 				"Dynamic DNS",
			ADVANCED_ROUTING: 			"Advanced Routing",

			WIRELESS: 					"Wireless",
			WIRELESS_SETTINGS: 			"Wireless Settings",
			WDSBRIDGING: 				"WDS Bridging",
			WPS: 						"WPS",
			MACFILTERING: 				"MAC Filtering",
			WIRE_STATISTICS: 			"Statistics",
			
			
			GUEST_NETWORK: 				"Guest Network",
			WIRELESS_SETTINGS: 			"Wireless Settings",
			STORAGE_SHARING: 			"Storage Sharing",
			NAT_FORWARDING: 			"NAT Forwarding",
			NAT: 						"ALG",
			VIRTUAL_SERVERS: 			"Virtual Servers",
			PORT_TRIGGERING: 			"Port Triggering",
			DMZ: 						"DMZ",
			UPNP: 						"UPnP",
			ALG: 						"ALG",
			
			USB_SETTINGS: 				"USB Settings",
			BASIC_SET: 					"Basic Settings",
			DISK_SET: 					"Device Settings",
			FOLDER_SHARING: 			"Sharing Access",
			STORAGE_SHARING: 			"Storage Sharing",
			FTP_SERVER: 				"FTP Server",
			MEDIA_SERVER: 				"Media Server",
			PRINT_SERVER: 				"Print Server",
			G3_G4: 						"3G/4G",
			
			
			PARENTAL_CONTROL: 			"Parental Controls",
			
			QOS:  						"QoS",
			DATABASE:  					"Database",

			STREAMBOOST: 				"Stream Boost",
			MAP: 						"Map",
			SB_MAP: 					"Map",
			SB_BANDWIDTH:  				"Bandwidth",
			SB_PRIORITY: 				"Priority",
			SB_STATISTICS: 				"Statistics",

			
			SECURITY: 					"Security",
			SETTINGS: 					"Settings",
			ACCESS_CONTROL: 			"Access Control",
			IP_MAC_BINDING: 			"IP & MAC Binding",

			IPV6: 						"IPv6",
			
			
			SYSTEM_TOOLS: 				"System Tools",
			TIME_SETTINGS: 				"Time Settings",
			DIAGNOSTIC: 				"Diagnostics",
			FIRMWARE_UPGRADE: 			"Firmware Upgrade",
			BACKUP_RESTORE: 			"Backup & Restore",
			ADMINISTRATION: 			"Administration",
			SYSTEM_LOG: 				"System Log",
			STATISTICS: 				"Traffic Statistics",
			SYSTEM_PARAMETERS: 			"System Parameters",
			VPN: 						"VPN Server",
			OPEN_VPN: 					"OpenVPN",
			PPTP_VPN: 					"PPTP VPN",
			VPN_CONNECTIONS: 			"VPN Connections"
		},

		QUICK_SETUP: {
			REGION_TIME_ZONE: 			"Time Zone",
			INTERNET_CONNECTION_TYPE: 	"Internet Connection Type",
			WIRELESS_SETTINGS: 			"Wireless Settings",
			SUMMARY: 					"Summary",
			SETUP_COMPLETE: 			"Test Internet Connection",

			EXIT: 						"Exit",
			NEXT: 						"Next",
			SAVE: 						"Save",
			FINISH: 					"Finish",
			OK: 						"OK",
			NONE: 						"Detection failed.",

			REGION: 					"Region",
			TIME_ZONE: 					"Time Zone",

			AUTO_DETECT: 				"Auto Detect",
			DYNAMIC_IP: 				"Dynamic IP",
			STATIC_IP: 					"Static IP",
			PPPOE: 						"PPPoE",
			L2TP: 						"L2TP",
			PPTP: 						"PPTP",
			INTERNET_CONNECTION_TYPE_NOTE: "If you are not sure about which Internet Connection Type you have, use Auto Detect or contact your Internet Service Provider (ISP) for assistance.",

			DYNAMIC_IP_INFO: 			"If your ISP only allows Internet access to a specific MAC address, you need to clone the MAC address of the primary computer. If you are not sure, select Do NOT clone MAC Address.",
			MAC_CLONE_NO: 				"Do NOT Clone MAC Address",
			MAC_CLONE_YES: 				"Clone Current Computer MAC Address",
			MAC_CLONE_NOTE: 			"If you select Clone MAC Address, you need to clone the original computer's MAC address that is registered with your ISP.",

			PPPOE_INFO: 				"Please enter your PPPoE username and password.",
			
			STATIC_IP_INFO: 			"Please enter your IP information.",

			L2TP_INFO: 					"Please enter your L2TP username and password",
			PPTP_INFO: 					"Please enter your PPTP username and password.",
			
			USERNAME: 					"Username",
			PASSWORD: 					"Password",
			SERVER_IP_ADDRESS_NAME: 	"VPN Server IP/Domain Name",
			IP_ADDRESS: 				"IP Address",
			SUBNET_MASK: 				"Subnet Mask",
			DEFAULT_GATEWAY: 			"Default Gateway",
			PRIMARY_DNS: 				"Primary DNS",
			SECOND_DNS: 				"Secondary DNS",
			OPTIONAL: 					"(Optional)",
			
			ON: 						"On",
			OFF: 						"Off",
			WIRELESS_24GHZ: 			"2.4GHz Wireless",
			WIRELESS_5GHZ: 				"5GHz Wireless",
			ENABLE_WIRELESS_RADIO: 		"Enable Wireless Radio",
			NAME_SSID: 					"Network Name (SSID)",

			SUMMARY_INFO1: 				"You need to reconnect your wireless devices to the new wireless network before clicking the <strong>Next</strong> button.",
			SUMMARY_INFO2: 				"Your wireless name and password has been modified as bellow:",
			SUMMARY_INFO3: 				"Make sure you have connected to the new wireless network.",

			WIRELESS_24GHZ_SSID: 		"Wireless 2.4GHz SSID",
			WIRELESS_24GHZ_PASSWORD: 	"Wireless 2.4GHz Password",
			WIRELESS_5GHZ_SSID: 		"Wireless 5GHz SSID",
			WIRELESS_5GHZ_PASSWORD: 	"Wireless 5GHz Password",

			SORRY: 						"Failed.",
			SUCCESS: 					"Success!",
			TEST_INTERNET_SUCCESS_INFO: "Click Finish to end the Quick Setup process.",

			TEST_INTERNET_FAILED_INFO_0:"Please verify that all Quick Setup parameters are correct and try again. If all Quick Setup parameters are correct, please reboot your modem, wait 2 minutes, and click Test Internet Connection one more time. If you are not using a modem, you may need to contact your Internet Service Provider (ISP) for assistance.",
			SUMMARY_INFO4: 				"Sorry. We detect that you have not reconnected your wireless device to the new wireless network. Please do it and then click <strong>OK</strong>.",
                                         
			CONGRATULARIONS: 			"Congratulations!",
			COMPLETE_INFO_0: 			"You have completed the Quick Setup process.",
			COMPLETE_INFO_1:			"Click Test Internet Connection below, then click Finish.",
			TEST_INTERNET: 				"Test Internet Connection",

			
			RESET_USER_TITLE: 			"Set up a new username and password",
			NEW_USERNAME: 				"New Username",
			NEW_PASSWORD: 				"New Password",
			CONFIRM_PASSWORD: 			"Confirm New Password",
			CONFIRM: 					"Confirm"
		},
		
		BASIC_NETWORK:{
			INTERNET: 					"Internet",
			INTERNET_STATUS:			"Internet Status",

			GHZ24: 						"2.4GHz",
			GHZ5: 						"5GHz",
			
			CONNECTION_TYPE: 			"Connection Type",
			SECONDARY_CONN: 			"Secondary Connection",

			POOR_CONNECTED: 			"Poor network.",
			UNPLUGGED: 					"WAN port is unplugged.",
			
			CONNECTED: 					"Connected",
			DISCONNECTED: 				"Disconnected",

			INTERNET_IP_ADDR: 			"IP Address",
			
			IP_ADDR: 					"IP Address",
			MAC_ADDR: 					"MAC Address",
			GATEWAY: 					"Gateway",

			AUTO: 						"Auto",
			
			ROUTER: 					"Router",
			WIRELESS_CLIENTS: 			"Wireless Clients",
			HOST_CLIENTS: 				"Host Clients",
			GUEST_CLIENTS: 				"Guest Clients",
			WIRE_CLIENTS: 				"Wired Clients",
			PRINTER: 					"Printer",
			USB_DISK: 					"USB Disk",
			WIRELESS: 					"Wireless",
			NAME: 						"Name",
			
			

			SSID: 						"SSID",
			CHANNEL: 					"Channel",
			MAC: 						"MAC",
			
			WIRELESS_24GHZ: 			"2.4GHz Wireless",
			WIRELESS_5GHZ: 				"5GHz Wireless",
			
			GUEST_24GHZ: 				"2.4GHz Guest Network",
			GUEST_5GHZ: 				"5GHz Guest Network",
			
			STATUS: 					"Status",
			TOTAL: 						"Total",
			AVAILABLE: 					"Available",
			GB: 						"GB",
			BRAND: 						"Brand",

			DYNAMIC_IP: 				"Dynamic IP",
			STATIC_IP: 					"Static IP",
			SUBNET_MASK: 				"Subnet Mask",
			PPPOE: 						"PPPoE",
			BIGPOND_CABLE: 				"BigPond Cable",
			L2TP: 						"L2TP",
			PPTP: 						"PPTP",
			TUNNEL_6TO4: 				"6to4 Tunnel",
			NONE: 						"None"
		},
		
		BASIC_INTERNET: {
			TITLE: 						"Internet",
			AUTO_DETECT: 				"Auto Detect",
			INTERNET_CONN_TYPE: 		"Internet Connection Type",
			DYNAMIC_IP: 				"Dynamic IP",
			STATIC_IP: 					"Static IP",
			PPPOE: 						"PPPoE",
			BIGPOND: 					"BigPond Cable",
			L2TP: 						"L2TP",
			PPTP: 						"PPTP",
			DHCP: 						"Dhcp",
			UNPLUGGED: 					"WAN port is unplugged.",
			NONE: 						"None",
			DETECT_FAIL: 				"Auto Detect failed.",
			SECONDARY_CONN: 			"Secondary Connection",

			DYNAMIC_YES: 				"Do NOT Clone MAC Address",
			DYNAMIC_NO: 				"Clone Current Computer MAC Address",
			
			IP_ADDR: 					"IP Address",
			SUBNET_MASK: 				"Subnet Mask",
			DEFAULT_GATEWAY: 			"Default Gateway",
			PRIMARY_DNS: 				"Primary DNS",
			SECOND_DNS: 				"Secondary DNS",
			OPTIONAL: 					"(Optional)",
			USER_NAME: 					"Username",
			PASSWORD: 					"Password",
			SERVER_IP_ADDR_NAME: 		"VPN Server IP/Domain Name",
			
			DNS_ADDR_MODE: 				"",

			NOTE: 						"If you are not sure about which Internet Connection Type you have, use Auto Detect or contact your Internet Service Provider (ISP) for assistance."
		},
		
		BASIC_WIRELESS:{
			TITLE: 						"Wireless Settings",
			MODE_2G: 					"2.4GHz Wireless",
			MODE_5G: 					"5GHz Wireless",
			WIRELESS_NETWORK_NAME: 		"Wireless Network Name (SSID)",
			WIRELESS_PASSWORD: 			"Password",
			ENABLE_WIRELESS: 			"Enable Wireless Radio",
			SAVE: 						"Save",
			ENCRYPTION_2G_NOTICE:		"2.4GHz encryption is %s.",
			ENCRYPTION_5G_NOTICE:		"5GHz encryption is %s.",
			ENCRYPTION_2G_NO: 			"2.4GHz wireless network is not encrypted.",
			ENCRYPTION_5G_NO: 			"5GHz wireless network is not encrypted.",
			ENCRYPTION_SURE: 			"Are you sure you want to continue?",

			HIDE_SSID: 					"Hide SSID"
		},
		
		
		
		BASIC_USB: {
			TITLE: 						"Basic Settings",
			TITIL_NEW:					"Disk & Account",
			DISK_SET:					"Device Settings",

			SELFLY_REMOVE:				"Safely Remove",
			SCANING:					"Scanning...",
			SCAN_RESULT:				"Found %n disks",
			
			DISKS:						"Disks",
			USERS: 						"Users Account",
			DEVICENAME: 				"Device Name",
			VOLUMN: 					"Volume",

			USBSPACE: 					"Used Space",
			FREESPACE: 					"Free Space",
			STATUS: 					"Status",
			INACT: 						"Inactivate",
			USAGE: 						"Usage",
			CAPACITY: 					"Capacity",
			OPERATION: 					"Operation",
			
			ACC: 						"Account Management",
			USERNAME: 					"Username",
			PASSWORD: 					"Password",
			USE_LOGIN: 					"Use Login user",
			SCAN: 						"Scan",
			ENJECT_ALL: 				"Eject All",
			ENJECT: 					"Eject",
			ADD_USER: 					"Add User",
			AUTH: 						"Authority",


			LOCATION: 					"Location",
			MOBILE_ISP: 				"Mobile ISP",
			DIAL_NUMBER: 				"Dial Number",
			APN: 						"APN",
			USERNAME: 					"Username",
			PASSWORD: 					"Password",
			MTU_SIZE: 					"MTU Size(in bytes)",
			OPTIONAL: 					"(Optional)"
		},
		
		BASIC_PARENTAL_CTR:{
			TITLE: 						"Parental Controls",

			DEVICE_CTR: 				"Devices Under Parental Controls",
			ID: 						"ID",
			DEVICE: 					"Device Name",
			MAC_ADDRESS: 				"MAC Address",
			TIME: 						"Internet Access Time",
			DESCRIPTION: 				"Description",
			ENABLE: 					"Enable This Entry",
			ENABLE_THIS_ENTRY: 			"Enable This Entry",
			OPTIONAL: 					"(Optional)",
			BTN_VIEW: 					"View Existing Devices",
			ACCESS_DEVICES_LIST: 		"Device List",
			OPT: 						"Operation",
			STATUS: 					"Status"
		},
		
		BASIC_GUEST:{
			TITLE: 						"Guest Network",
			MODE_2G: 					"2.4GHz Wireless",
			MODE_5G: 					"5GHz Wireless",
			WIRELESS_NETWORK_NAME: 		"Wireless Network Name (SSID)",
			WIRELESS_PASSWORD: 			"Password",
			ENABLE_WIRELESS: 			"Enable Guest Network",
			SAVE:						"Save",
			HIDE_SSID: 					"Hide SSID"
		},

		STATUS: {
			TITLE: 						"Status",
			INTERNET:					"Internet",
			WIRELESS:					"Wireless",
			LAN:						"LAN",
			USB_TITLE:					"USB Devices",
			PERFORMANCE: 				"Performance",
			GUEST_NETWORK: 				"Guest Network",
			ACCESS_DEVICES: 			"Access Devices",
			
			IPV4: 						"IPv4",
			IPV6: 						"IPv6",
			HZ24G: 						"2.4GHz",
			HZ5G: 						"5GHz",

			CONNECTION_TYPE: 			"Connection Type",

			MAC_ADDRESS: 				"MAC Address",
			IP_ADDRESS: 				"IP Address",
			RELEASE: 					"Release",
			RENEW: 						"Renew",
			
			DYNAMIC_IP: 				"Dynamic IP",
			STATIC_IP: 					"Static IP",
			SUBNET_MASK: 				"Subnet Mask",
			PPPOE: 						"PPPoE",
			BIGPOND_CABLE: 				"BigPond Cable",
			L2TP: 						"L2TP",
			PPTP: 						"PPTP",
			TUNNEL_6TO4: 				"6to4 Tunnel",
			RD6:  						"6RD",
			DSLITE: 					"DS-Lite",
			PASSTHROUGH: 				"Pass-Through (Bridge)",
			RDNSS: 						"RDNSS",
			SLAAC: 						"SLAAC",
			NONE: 						"None",
			
			DEFAULT_GATEWAY: 			"Default Gateway",
			DNS: 						"DNS Server",
			MAC: 						"MAC Address",
			WDS_STATUS: 				"WDS Status",
			
			IPV6_ADDRESS: 				"IP Address",
			PRIMARY_DNS: 				"Primary DNS",
			SECOND_DNS: 				"Secondary DNS",

			RADIO: 						"Wireless Radio",

			NAME_SSID: 					"Name (SSID)",
			NETWORK_NAME_SSID:			"Network Name (SSID)",
			HIDE_SSID: 					"Hide SSID",
			MODE: 						"Mode",
			CHANNEL: 					"Channel",
			CHANNEL_WIDTH: 				"Channel Width",
			AUTO: 						"Auto",
			CURRENT_CHANNEL: 			"Current Channel",

			WDS: 						"WDS Status",
			WIRED_CLIENTS: 				"Wired Clients",
			WIRELESS_CLIENTS: 			"Wireless Clients",

			ENABLE_DHCP: 				"DHCP",
			LINKADDR:					"Link-local Address",
			ASSIGN_TYPE: 				"Assigned Type",

			ALLOW_TO_SEE_EACH: 			"Allow guests to see each other",

			TOTAL: 						"Total:",
			AVAILABLE: 					"Available:",

			USB: 						"USB Disk",
			PRINTER: 					"Printer",

			CPU_LOAD: 					"CPU Load",
			MEMORY_USAGE: 				"Memory Usage",

			IP_ADDR_P: 					"IP Address:",
			MAC_ADDR_P: 				"MAC Address:",
			CONN_TYPE_P: 				"Connection Type:",

			DISABLED: 					"Disabled",
			INIT: 						"Init",
			SCAN: 						"Scan",
			AUTH: 						"Auth",
			ASSOC: 						"Assoc",
			RUN: 						"Run",
			HOST: 						"Host",
			GUEST: 						"Guest",

			ON: 						"On",
			OFF: 						"Off"
		},

		INTERNET: {
			TITLE: 						"Internet",
			IPV4: 						"IPv4",
			IPV6: 						"IPv6",
			CONNECTION_TYPE: 			"Internet Connection Type",
			INTERNET_MAC_ADDRESS: 		"MAC Address",
			
			CONNECT: 					"Connect",
			DISCONNECT: 				"Disconnect",
			
			IP_ADDR: 					"IP Address",
			
			USE_DEFAULT_MAC: 			"Use Default MAC Address",
			USE_COMPUTER_MAC: 			"Use Current Computer MAC Address",
			USE_CUSTOM_MAC: 			"Use Custom MAC Address",
			MAC_CLONE: 					"MAC Clone",
			
			CONFIG: 					"Config",
			
			IP_ADDRESS: 				"IP Address",
			SUBNET_MASK: 				"Subnet Mask",
			DEFAULT_GATEWAY: 			"Default Gateway",
			
			MANUAL_DNS: 				"Manual DNS",
			PRIMARY_DNS: 				"Primary DNS",
			SECOND_DNS: 				"Secondary DNS",
			
			RENEW: 						"Renew",
			RELEASE: 					"Release",
			VIEW_MODE: 					"View Mode",
			
			GET_DYNAMICALLY_FROM_ISP: 	"Get dynamically from ISP",
			USE_FOLLOW_IP_ADDR: 		"Use the following IP Address",
			USE_FOLLOW_DNS_ADDR: 		"Use the following DNS Addresses",
			USE_FOLLOW_DNS_SERVER: 		"Use the following DNS Server",
			
			BASIC: 						"Basic",
			ADVANCED: 					"Advanced",
			
			DNS_ADDR_MODE: 				"DNS Address",
			MTU_SIZE: 					"MTU Size",
			MTU_1500: 					"bytes. (The default is 1500, do not change unless necessary.)",
			MTU_1480: 					"bytes. (The default is 1480, do not change unless necessary.)",
			MTU_1460: 					"bytes. (The default is 1460, do not change unless necessary.)",
			MTU_1420: 					"bytes. (The default is 1420, do not change unless necessary.)",
			
			HOST_NAME: 					"Host Name",

			HOST_NAME_CONFIRM: 			"The host name contains illegal characters that may cause unexpected system behaviour. Are you sure you want to continue?",

			GET_IP_WITH_UNICAST_DHCP: 	"Get IP using Unicast DHCP (It is usually not required.)",
			OPTIONAL: 					"(Optional)",
			
			STATIC_IP: 					"Static IP",
			PPPOE: 						"PPPoE",
			DHCPV6:                     "DHCPv6",
			SLAAC:                      "SLAAC",
			
			USER_NAME: 					"Username",
			PASSWORD: 					"Password",
			
			INTERNET_IP_ADDR: 			"IP Address",
			INTERNET_DNS: 				"Internet DNS",
			SECONDARY_CONN: 			"Secondary Connection",
			NONE: 						"None",
			INTERNET_PRIMARY_DNS:		"Primary DNS",
			INTERNET_SECONDARY_DNS: 	"Secondary DNS",
			
			DYNAMIC_IP: 				"Dynamic IP",
			DYNAMIC_IP_v6: 				"Dynamic IP (SLAAC/DHCPv6)",
			STATIC_IP: 					"Static IP",
			SERVICE_NAME: 				"Service Name",
			ACCESS_CONCENTRATOR_NAME:  	"Access Concentrator Name",
			DETECT_ONLINE_INTERVAL: 	"Detect Online Interval",
			INTERVAL_TIPS: 				"seconds. (0-120. The default is 10.)",
			IP_ADDR_MODE:  				"IP Address",
			CONN_MODE: 					"Connection Mode",
			DHCP_LINK_UNPLUGGED: 		"WAN port is unplugged.",
			
			AUTO: 						"Auto",
			ON_DEMAND: 					"On Demand",
			TIME_BASED: 				"Time-Based",
			MANUALLY: 					"Manually",
			MAX_IDLE_TIME: 				"Max Idle Time",
			MAX_IDLE_TIME_TIPS: 		"minutes. (0 means always active.)",
			PERIOD_OF_TIME: 			"Period of Time",
			TIME_TIPS: 					"(HH:MM)",
			BIGPOND_CABLE: 				"BigPond Cable",
			AUTH_SERVER: 				"Auth. Server",
			AUTH_DOMAIN: 				"Auth. Domain",
			L2TP: 						"L2TP",
			GATEWAY: 					"Gateway",
			SERVER_IP_ADDR_NAME: 		"VPN Server IP/Domain Name",
			PPTP: 						"PPTP",
			TO: 						"to",
			
			TUNNEL_6TO4: 				"6to4 Tunnel",
			ENABLE_IPV6: 				"IPv6",
			GET_NONE_TEMPORARY_ADDR: 	"Get non-temporary IPv6 Address",
			GET_PREFIX_DELEGATION: 		"Get IPv6 prefix delegation",
			IPV6_ADDR: 					"IPv6 Address",

			GET_IPV6_WAY: 				"Get IPv6 Address",
			AUTOMATICALLY: 				"Get Automatically",
			SPECIFIED_BY_ISP: 			"Specified by ISP",

			IPV6_ADDR_PREFIX: 			"IPv6 Address Prefix",
			NONE_TEMPORARY: 			"Non-temporary",

			PREFIX_DELEGATION: 			"Prefix Delegation",
			ENABLE: 					"Enable",
			DISABLE: 					"Disable",

			RD6:  						"6RD", 
			IPV4_MASK_LEN: 				"IPv4 Mask Length",
			CONFIG_TYPE: 				"Configuration Type",
			RD6_PREFIX: 				"6RD Prefix",
			RD6_PREFIX_LENGTH: 			"6RD Prefix Length",
			REPLY_IPV4_ADDR: 			"Border Reply IPv4 Address",
			MANUAL: 					"Manual",
			DSLITE:  					"DS-Lite",
			DS_LITE:  					"DS-Lite",
			PASS_THROUGH:  				"Pass-Through (Bridge)",
			LOCAL_IPV6: 				"Local IpV6 Address",
			PEER_IPV6: 					"Peer IpV6 Address",
			TUNNEL_ADDR: 				"Tunnel Address",
			IPV4_NETMASK: 				"IPv4 Netmask",
			CUSTOM: 					"Custom",
		    AFTR_NAME: 					"AFTR Name",


			
			
			IPV4_ADDR: 					"IPv4 Address",
			IPV4_MASK: 					"IPv4 Subnet Mask",
			IPV4_GATEWAY: 				"IPv4 Default Gateway",
			TUNNEL_ADDR: 				"Tunnel Address",

			DUPLEX: 					"Duplex",
			AUTO_NEGOTIATION: 			"Auto Negotiation",
			FULL_DUPLEX_1000: 			"1000Mbps full duplex",
			HALF_DUPLEX_1000:			"1000Mbps half duplex",
			FULL_DUPLEX_100: 			"100Mbps full duplex",
			HALF_DUPLEX_100: 			"100Mbps half duplex",
			FULL_DUPLEX_10: 			"10Mbps full duplex",
			HALF_DUPLEX_10: 			"10Mbps half duplex"

		},

		LAN: {
			TITLE: 						"LAN",
			LAN: 						"LAN",
			IPV4: 						"IPv4",
			IPV6: 						"IPv6",
			
			LAN_IPv4: 					"LAN IPv4",
			LAN_IPv6: 					"LAN IPv6",

			MAC_ADDRESS: 				"MAC Address",
			IP_ADDRESS: 				"IP Address",
			SUBNET_MASK: 				"Subnet Mask",
			CUSTOM: 					"Custom",

			IGMP: 						"Enable IGMP Proxy",
			

			ASSIGNED_TYPE: 				"Assigned Type",
			DHCP_SERVER: 				"DHCPv6",
			SLAAC: 						"SLAAC+Stateless DHCP",
			RDNSS: 						"SLAAC+RDNSS",

			PREFIX: 					"Address Prefix",
			PREFIX_UNIT: 				"/64",
			ADDRESS: 					"Address",
			DELEFATED: 					"Delegated",
			STATIC: 					"Static",
			SITE_PREFIX: 				"Site Prefix",
			SITE_PREFIX_LEN: 			"Site Prefix Length",

			PREFIX_TYPE:  				"Site Prefix configuration Type",
			LAN_IPV6_ADDR:  			"LAN IPV6 Address",

			RELEASE_TIME: 				"Release Time",
			RELEASE_TIME_TIP: 			"seconds. (The default is 86400, do not change unless necessary.)",
			ADDRESS:					"Address",
			SAVE: 						"Save",

			REBOOT_TIP: 				"The router is skipping to the new login page. <br/>Please wait..."

		},

		IPTV:{
			TITLE: 						"Settings",
			IPTV:                       "IPTV", 
			ENABLE_IPTV:                "Enable IPTV", 
			MODE:  						"Mode",
			IGMP_PROXY: 				"IGMP Proxy",
			IGMP_VERSION: 				"IGMP Version",
			V2:  						"V2",
			V3:  						"V3",
			BRIDGE: 					"Bridge",
			BASIC: 						"Custom",
			EXSTREAM: 					"Singapore-ExStream",
			RUSSIA:  					"Russia",
			UNIFY:  					"Malaysia-Unifi",
			MAXIS:  					"Malaysia-Maxis",
			LAN1: 						"LAN1",
			LAN2: 						"LAN2",
			LAN3: 						"LAN3",
			LAN4: 						"LAN4",
			INTERNET: 					"Internet",
			IP_PHONE: 					"IP-Phone", 

			Q_TAG: 						"802.1Q Tag",
			ENABLE: 					"Enable",
			
			INTERNET_VLAN_ID: 			"Internet VLAN ID",
			INTERNET_VLAN_PRIORITY: 	"Internet VLAN Priority",
			IP_PHONE_VLAN_ID: 			"IP-Phone VLAN ID",
			IP_PHONE_VLAN_PRIORITY: 	"IP-Phone VLAN Priority",
			IPTV_VLAN_ID: 				"IPTV VLAN ID",
			IPTV_VLAN_PRIORITY: 		"IPTV VLAN Priority",
			IPTV_MULTI_VLAN_ID: 		"IPTV Multicast VLAN ID",
			IPTV_MULTI_VLAN_PRIORITY: 	"IPTV Multicast VLAN Priority"
		},

		DHCP_SERVER: {
			TITLE: 						"DHCP Server",
			
			SETTINGS: 					"Settings",

			DHCP_SERVER: 				"DHCP Server",
			ENABLE_DHCP_SERVER: 		"Enable DHCP Server",

			IP_ADDR_POOL: 				"IP Address Pool",
			LEASETIME: 					"Address Lease Time",
			LEASENOTE: 					"minutes. (1-2880. The default value is 120.)",
			
			GATEWAY: 					"Default Gateway",
			DOMAIN: 					"Default Domain",
			PRIMARYDNS: 				"Primary DNS",
			SECONDARYDNS: 				"Secondary DNS",

			OPTIONAL: 					"(Optional)",

			CLIENTSLIST: 				"DHCP Client List",
			CLIENT_NUMBER: 				"Total Clients",
			CLIENT_NAME: 				"Client Name",
			MAC_ADDR: 					"MAC Address",
			ASSIGNED_IP: 				"Assigned IP Address",
			LEASE_TIME: 				"Lease Time",

			RESERVATION: 				"Address Reservation",

			RESERVED_IP: 				"Reserved IP Address",
			IP_ADDRESS: 				"IP Address",
			DESCRIPTION: 				"Description",

			CLIENTSLIST: 				"DHCP Client List",
			CLIENT_NUMBER: 				"Total Clients",

			ENABLE: 					"Enable This Entry",
			ENABLE_THIS_ENTRY: 			"Enable This Entry"
			
		},

		DDNS: {
			DDNS: 						"Dynamic DNS",
			SERVICEPROVIDER: 			"Service Provider",
			COMEXE: 					"Comexe", 
			DYNDNS: 					"DynDNS",
			NOIP: 						"NO-IP",
			
			GO_TO_REGISTER: 			"Go to register...",
			USERNAME: 					"Username",
			PASSWORD: 					"Password",
			DOMAIN_NAME_LIST: 			"Domain Name List",
			DOMAIN_NAME: 				"Domain Name",
			LOGIN: 						"Login",
			LOGIN_SAVE: 				"Login and Save", 
			LOGOUT: 					"Logout",
			STATU_SUCCESS:				"Success!",
			UPDATE_INTERVAL:			"Update Interval",
			ONE_HOUR:					"1 hour",
			SIX_HOUR:					"6 hours",
			TWETEEN_HOUR:				"12 hours",
			ONE_DAY:					"1 day",
			TWO_DAY:					"2 days",
			THREE_DAY:					"3 days",
			NEVER:						"Never",
			UPDATE:						"Update",
			STATU_INCORRENT:			"Incorrect username or password",
			STATU_ERR_DOMAIN:			"Domain name error",
			WAN_IP_BIND: 				"WAN IP binding",
			DISABLE: 					"Disable",
			ENABLE: 					"Enable",	
			STATU_NO_LAUNCH:			"Failed to update DDNS.",
			STATU_CONN:					"Connecting"
		},

		ADVANCED_ROUTING: {
			TITLE: 						"Advanced Routing",
			STATIC_ROUTING: 			"Static Routing",

			DESTINATION_NETWORK:		"Network Destination",
			SUBNET_MASK: 				"Subnet Mask",
			DEFAULT_GATEWAY: 			"Default Gateway",
			DESCRIPTION: 				"Description",
			
			SYSTEM_ROUTING_TABLE: 		"System Routing Table",
			CLIENT_NUMBER: 				"Active Routes Number",

			GATEWAY: 					"Gateway",
			INTERFACE: 					"Interface",
			LAN: 						"LAN",
			WAN: 						"WAN",
			ENABLE: 					"Enable This Entry",
			ENABLE_THIS_ENTRY: 			"Enable This Entry"
		},

		WIRELESS: {

			GLOBAL_SETTINGS: 			"Settings",
			NOT_SUPPORT_5G: 			"The region only supports 2.4GHz. Please ensure you select the correct region.",

			REGION: 					"Region",
			NOTICE:  					"To use wireless function, you should keep wireless hardware switch on.",
			
			MODE_2G:					"2.4GHz",
			MODE_5G:					"5GHz",

			WIRELESS: 					"Wireless Settings",
			WIRELESS_RADIO: 			"",
			ENABLE_WIRELESS: 			"Enable Wireless Radio",

			WIRELESS_NETWORK_NAME: 		"Wireless Network Name (SSID)",
			WIRELESS_PASSWORD: 			"Password",
			HIDE_SSID: 					"Hide SSID",

			SECURITY: 					"Security",
			NO_SECURITY: 				"No Security",
			WPA_WPA2_PERSONAL: 			"WPA/WPA2-Personal (Recommended)",
			WPA_WPA2_ENTERPRISE: 		"WPA/WPA2-Enterprise",
			WEP: 						"WEP",

			VERSION: 					"Version",

			AUTO: 						"Auto",
			WPA_PSK: 					"WPA-PSK",
			WPA2_PSK: 					"WPA2-PSK",
			WPA: 						"WPA",
			WPA2: 						"WPA2",

			ENCRYPTION: 				"Encryption",
			TKIP: 						"TKIP",
			AES: 						"AES",

			MODE:  						"Mode",
			MODE_B:  					"802.11b only",
			MODE_G:  					"802.11g only",
			MODE_N:  					"802.11n only",
			MODE_BG:  					"802.11b/g mixed",
			MODE_GN: 					"802.11g/n mixed",
			MODE_BGN:  					"802.11b/g/n mixed",

			MODE_A_5: 					"802.11a only",
			MODE_AN_5: 					"802.11a/n mixed",
			MODE_N_5: 					"802.11n only",
			MODE_AC_5:					"802.11ac only",
			MODE_NAC_5:					"802.11n/ac mixed",
			MODE_ANAC_5:				"802.11a/n/ac mixed",


			CHANNEL_WIDTH: 				"Channel Width",
			CHANNEL: 					"Channel",

			TRANSMIT_POWER: 			"Transmit Power",

			RADIUS_SERVER_IP: 			"RADIUS Server IP",
			RADIUS_PORT: 				"RADIUS Port",
			RADIUS_PASSWORD: 			"RADIUS Password",

			TYPE: 						"Type",
			OPEN_SYSTEM: 				"Open System",
			SHARED_KEY: 				"Shared Key",

			KEY_SELECTED: 				"Key Selected",
			KEY1: 						"Key1",
			KEY2: 						"Key2",
			KEY3: 						"Key3",
			KEY4: 						"Key4",

			WEP_KEY_FORMAT: 			"WEP Key Format",
			ASCII: 						"ASCII",
			HEXADECIMAL: 				"Hexadecimal",

			KEY_TYPE: 					"Key Type",
			BIT64: 						"64-bit",
			BIT128: 					"128-bit",
			BIT152: 					"152-bit",

			KEY_VALUE: 					"Key Value",
			
			MHZ: 						"MHz",
			MHZ20: 						"20 MHz",
			MHZ40: 						"40 MHz",
			MHZ80: 						"80 MHz",
			
			LOW: 						"Low",
			MIDDLE: 					"Middle",
			HIGH: 						"High"
		},

		WPS: {

			TITLE2: 					"Router's PIN",
			ROUTERS_PIN_INFO: 			"Wireless clients can connect to the router using WPS PIN method.",
			ENABLE_ROUTE_PIN: 			"Router's PIN",
			ROUTE_PIN: 					"PIN",
			GENERAL: 					"Generate",
			DFT: 						"Default",

			TITLE: 						"WPS Wizard",
			SELECT_SETUP: 				"Select a WPS connection method",
			PUSH_BTN: 					"Push Button (Recommended)",
			PUSH_DES: 					"Press the physical WPS button on the router or click the software Connect below.",
			CONNECT: 					"Connect",
			CANCEL: 					"Cancel",

			RESULT_HEAD: 				"The wireless client",
			RESULT_END: 				"has been added to the network successfully.",
			NOT_FOUND: 					"No client is found around. Click the button to try again.",

			PIN_NUMBER: 				"PIN",
			ENTER_CLIENT_PIN: 			"Client's PIN",
			
			PIN_BTN: 					"PIN",
			NOT_FOUND: 					"Not Found",
			ENTER_CLIENT_PIN: 			"Enter the client's PIN",
			SWITCH_NOTE:				"To connect using WPS, please turn on the wireless function.",
			SWITCH_NOTE2:				"To use WPS Wizard, you should configure the right wireless parameters first.",
			STATUS_PIN_ERROR: 			"Invalid WPS PIN. Please check and try again.",
			STATUS_ERROR: 				"Error.",
			STATUS_CONN_ERROR: 			"Connection failed.",
			STATUS_CONNING: 			"Connecting...",
			STATUS_CANCEL: 				"Connection cancelled.",
			
			NOTE: 						"Note:",
			
			STATUS_CONN_OVERLAP: 		"Connection failed (Overlapped).",
			STATUS_CONN_TIMEOUT: 		"Connection failed (Timed out).",
			STATUS_CONN_INACT: 			"Connection inactive."

		},

		STATISTICS_WIRELESS:{
			TITLE: 						"Wireless Stations Online",
			CLIENT_NUMBER: 				"Total Clients",
			MAC_ADDRESS: 				"MAC Address",
			CONN_TYPE: 					"Connection Type",
			SECURITY: 					"Security",
			RECEIVED_PACKETS: 			"Received Packets",
			SEND_PACKETS: 				"Sent Packets"
		},

		GUEST_SETTINGS:{
			GLOBAL_SETTINGS: 			"Settings",
			
			MODE_2G: 					"2.4GHz",
			MODE_5G:					"5GHz",
			
			ALLOW_EACH: 				"Allow guests to see each other",
			
			ALLOW_LOCAL: 				"Allow guests to access my local network",
			
			WIRELESS: 					"Wireless",
			WIRELESS_24G_RADIO: 		"2.4GHz Wireless",
			WIRELESS_5G_RADIO: 			"5GHz Wireless",
			ENABLE_GUEST: 				"Enable Guest Network",

			NAME_SSID: 					"Wireless Network Name (SSID)",
			HIDE_SSID: 					"Hide SSID",

			SECURITY: 					"Security",
			NO_SECURITY: 				"No Security",	
			WPA_WPA2_PERSONAL: 			"WPA/WPA2-Personal",

			VERSION: 					"Version",
			AUTO: 						"Auto",
			WPA_PSK: 					"WPA-PSK",
			WPA2_PSK: 					"WPA2-PSK",

			ENCRYPTION: 				"Encryption",
			TKIP: 						"TKIP",
			AES: 						"AES",

			WIRELESS_PASSWORD: 			"Password"
		},

		NAT:{
			SETTINGS: 					"Hardware NAT",
			STATUS: 					"Hardware NAT",
			
			ALG_TITLE: 					"Application Layer Gateway (ALG)",

			FTP_ALG: 					"FTP ALG",
			TFTP_ALG: 					"TFTP ALG",
			H323_ALG: 					"H323 ALG",
			RTSP_ALG: 					"RTSP ALG",
			PPTP_ALG: 					"PPTP Passthrough",
			L2TP_ALG: 					"L2TP Passthrough",
			IPSEC_ALG: 					"IPSec Passthrough",

			ENABLE_FTP_ALG: 			"Enable FTP ALG",
			ENABLE_TFTP_ALG: 			"Enable TFTP ALG",
			ENABLE_H323_ALG: 			"Enable H323 ALG",
			ENABLE_RTSP_ALG: 			"Enable RTSP ALG",
			ENABLE_PPTP_ALG: 			"Enable PPTP Passthrough",
			ENABLE_L2TP_ALG: 			"Enable L2TP Passthrough",
			ENABLE_IPSEC_ALG: 			"Enable IPSec Passthrough",
			NAT_ENABLE_NOTICE: 			"Note: Your configurations will not take effect until NAT function is enabled."
		},

		VIRTUAL_SERVERS:{
			TITLE: 						"Virtual Servers",

			SERVICE_NAME: 				"Service Type",
			EXTERNAL_PORT: 				"External Port",
			INTERNAL_IP: 				"Internal IP",
			INTERNAL_PORT: 				"Internal Port",
			PROTOCAL: 					"Protocol",

			BTN_VIEW: 					"View Existing Services",

			EXSITTING_SERVICE: 			"Existing Services",
			
			PROTOCAL_ALL: 				"ALL",
			PROTOCAL_TCP: 				"TCP",
			PROTOCAL_UDP: 				"UDP",
			
			INTERNAL_UNIT: 				"(XX)",
			EXTERNAL_UNIT: 				"(XX-XX or XX)",
			EXT_PORT_TIPS: 				"(XX or XX-XX ,1-65535)",
			INT_PORT_TIPS: 				"(XX or Blank ,1-65535)",

			NOTICE:						"In conflict with the remote management port. Are you sure you want to continue?",

			ENABLE_THIS_ENTRY: 			"Enable This Entry",
			OPERATION: 					"Operation",
			CHOOSE: 					"Choose",
			NAT_ENABLE_NOTICE: 			"Note: Your configurations will not take effect until NAT function is enabled."
		},

		PORT_TRIGGERING:{
			TITLE: 						"Port Triggering",
			APPLICATION: 				"Application",
			TRIGGER_PORT: 				"Triggering Port",
			TRIGGER_PROTOCOL: 			"Triggering Protocol",

			EXTERNAL_PORTS: 			"External Port",
			EXTERNAL_PROTOCOL: 			"External Protocol",

			BTN_VIEW: 					"View Existing Applications",

			EXSITTING_APPLICATION: 		"Existing Applications",
			APPLICATION_NAME: 			"Application Name",
			TRIGGER_TIPS: 				"(XX,1-65535)",
			EXTERNAL_TIPS: 				"(XX or XX-XX,1-65535,at most 5 pairs)",
			
			ENABLE_THIS_ENTRY: 			"Enable This Entry",
			OPERATION: 					"Operation",
			
			PROTOCAL_ALL: 				"ALL",
			PROTOCAL_TCP: 				"TCP",
			PROTOCAL_UDP: 				"UDP",
			NAT_ENABLE_NOTICE: 			"Note: Your configurations will not take effect until NAT function is enabled."
		},

		DMZ:{
			TITLE: 						"DMZ",
			ENABLE_DMZ: 				"Enable DMZ",
			HARDWARESTATUS: 			"DMZ Host IP Address",
			NAT_ENABLE_NOTICE: 			"Note: Your configurations will not take effect until NAT function is enabled."
		},

		UPNP:{
			TITLE: 						"UPnP",
			CURRENT_UPNP_STATUS: 		"UPnP",

			UPNP_LIST: 					"UPnP Service List",
			CLIENT_NUMBER: 				"Total Clients",
			SERVICE: 					"Service Description",
			EXTERNAL_PORT: 				"External Port",
			PROTOCAL: 					"Protocol",
			IP_ADDR: 					"Internal IP Address",
			INTERNAL_PORT: 				"Internal Port",
			NAT_ENABLE_NOTICE: 			"Note: Your configurations will not take effect until NAT function is enabled."
		},

		G3_G4:{
			TITLE: 						"3G/4G",
			USB_MODEM: 					"USB Modem",
			LOCATION: 					"Location",
			MOBILE_ISP: 				"Mobile ISP",

			USA: 						"USA",
			AT_T: 						"AT&T",

			CONNECTION_MODE: 			"Connection Mode",
			CONNECT_ON_DEMAND: 			"Connect on Demand",
			CONNECT_AUTOMATICALLY: 		"Connect Automatically",
			CONNECT_MANUALLY: 			"Connect Manually",
			MAX_IDLE_TIME: 				"Max Idle Time",
			CONNECTION_TIP: 			"The current Internet Access is WAN Preferred.",
			IDLE_TIME_TIP: 				"The Connection Mode and Max Idle Time could not be set manually.",
			MINUTES: 					"minutes. (0 means remain active at all times.)",

			AUTHENTICATION_TYPE: 		"Authentication Type",
			AUTO: 						"Auto",
			PAP: 						"PAP",
			CHAP: 						"CHAP",
			AUTH_TYPE_TIP: 				"The default is Auto, do not change unless necessary.",

			CONNECT: 					"Connect",
			DISCONNECT: 				"Disconnect",

			SET_TIP: 					"Set the Dial Number, APN, Username and Password manually.",
			DIAL_NUMBER: 				"Dial Number",
			APN: 						"APN",
			USERNAME: 					"Username",
			PASSWORD: 					"Password",
			OPTIONAL: 					"(Optional)",
			MTU_SIZE: 					"MTU Size(in bytes)",
			MTU_SIZE_TIP: 				"The default is 1480, do not change unless necessary",

			USE_FOLLOW_DNS_SERVER: 		"Use The following DNS Servers",
			PRIMARY_DNS: 				"Primary DNS",
			SECOND_DNS: 				"Secondary DNS",

			UNPLUGGED: 					"WAN port is unplugged.",
			IDENTIFYING: 				"Identifying...",
			IDENTIFY_SUCCESS: 			"Identify successfully"
		},

		DISK_SETTING: {
			DISK_SET: 					"Device Settings",
			SCAN: 						"Scan",
			SELFLY_REMOVE: 				"Safely Remove",
			SCAN_RESULT: 				"Found %n disks",
			NOT_FOUND: 					"Not Found",
			SELFLY_REMOVE: 				"Safely Remove",
			
			VOLUMN: 					"Volume",
			CAPACITY: 					"Capacity",
			FREESPACE: 					"Free Space",
			USBSPACE: 					"Used Space",
			
			STATUS: 					"Status",
			INACT: 						"Inactivate",
			ACTIVE: 					"Active",
			
			USAGE: 						"Usage",
			CAPACITY: 					"Capacity",
			OPERATION: 					"Operation",	
			
			ACC: 						"Account Management", 	 	
			USERNAME: 					"Username",
			PASSWORD: 					"Password",
			USE_LOGIN: 					"Use Login user",
			SCAN: 						"Scan",
			ENJECT_ALL: 				"Eject All",
			ENJECT: 					"Eject",
			ADD_USER: 					"Add User",
			AUTH: 						"Authority"
		},

		FOLDER: {
			TITLE: 						"Sharing Settings",
			ACCOUNT_TITLE: 				"Sharing Account",
			ACCOUNT:					"Account",
			AC_NOTE: 					"You can use the default login account or create a new account to access the shared contents.",
			
			AC_LOGIN: 					"Use Default Account",
			AC_FOLLOW: 					"Use New Account",

			USERNAME: 					"Username",
			PASSWORD: 					"Password",
			CONFIRM: 					"Confirm Password",

			SHARING_SETTING: 			"Sharing Settings",
			SERVER_NAME: 				"Network/Media Server Name",

			METHOD: 					"Access Method",
			LINK: 						"Link",
			PORT: 						"Port",

			NETWORK_NEIGHBORHOOD: 		"Network Neighborhood",
			FTP: 						"FTP",
			FTPEX: 						"FTP (Via Internet)",

			SHARE_FOLDER: 				"Folder Sharing",
			SHAREING_ALL: 				"Share All",
			NOTE:  						"Toggle On to share all files and folders or keep it Off to only share the specified folders.", 
			ENABLE_AUTHENTICATION: 		"Enable Authentication",
			SHAREING_FOLDER: 			"Sharing Folders",
			
			SHARE_NAME: 				"Share Name",
			FOLDER_PATH: 				"Folder Path",
			VOLUMN_NAME: 				"Volume Name",

			SHARE_NAME: 				"Folder Name",
			FOLDER_PATH: 				"Folder Path",
			MEDIA_SHARING: 				"Media Sharing",
			STATUS: 					"Active",

			GUEST_ACCESS: 				"Allow Guest Network Access",
			ENABLE_AUTHENTICATION: 		"Enable Authentication",
			ENABLE_WRITE_ACCESS: 		"Enable Write Access",
			ENABLE_MEDIA_SHARE: 		"Enable Media Sharing",
			
			BROWSE: 					"Browse",
			BROWSE_TITLE: 				"Select a folder",

			NO_VOLUMN:					"No Volume",
			
			NOTICE: 					"Are you sure you want to leave for the Dynamic DNS page? Click Save to save and leave. Click Leave to leave without saving. Click Cancel to stay.",
			NO_CHANGE_NOTICE: 			"Are you sure you want to leave for the Dynamic DNS page?",

			SAVE_FAILED_NOTICE: 		"Save failed",
			CONTINUE: 					"Leave",
			CONTINUE_SAVE: 				"Save",
			CANCLE: 					"Cancel",

			ENABLE: 					"Enable"

		},

		PRINT:{
			TITLE: 						"Print Server",
			NAME: 						"Printer Name",
			ENABLE_PRINT_SERVER: 		"Print Server",
			NONE: 						"None",
			
			NOTE_TITLE: 				"Note:",
			STEP1: 						"Step1:",
			STEP2: 						"Step2:",
			STEP3: 						"Step3:",

			NOTE1: 						"Install the printer driver on your computer.",
			NOTE2: 						"Connect the USB printer to the USB port of the router via a USB cable.",
			NOTE3: 						"Install the TP-LINK USB Printer Controller Utility. Please download it from our official website: <a class=\"link\" href=\"http://www.tp-link.com/en/Support/\" target=\"_blank\">http://www.tp-link.com/en/Support/</a>."
		},

		PARENTAL_CTR:{
			TITLE: 						"Parental Controls",
			STATUS: 					"Parental Controls",
			UNKNOWN: 					"Unknown",

			DEVICE_CTR: 				"Devices Under Parental Controls",
			DEVICE: 					"Device Name",
			MAC_ADDRESS: 				"MAC Address",
			TIME: 						"Internet Access Time",
			DESCRIPTION: 				"Description",
			
			ENABLE_THIS_ENTRY: 			"Enable This Entry",
			OPTIONAL: 					"(Optional)",
			BTN_VIEW: 					"View Existing Devices",
			
			DEVICE_LIST: 				"Device List",
			SYSTEM_TIME: 				"System Time",
			
			RESTR: 						"Content Restriction",
			MODE: 						"Restriction Policy",
			BLACKMODE: 					"Blacklist",
			WHITEMODE: 					"Whitelist",
			ACCESS_DEVICES_LIST: 		"Access Devices List",
			
			CHOOSE: 					"Choose",
			ADD_A_NEW_KEYWORD: 			"Add a New Keyword",
			ADD_A_NEW_DOMAIN_NAME: 		"Add a New Domain Name"
		},

		STREAMBOOST: {
			TITLE: 						"Streamboost",
			INTERNET: 					"Internet",
			ROUTER: 					"Router",
			WINDOWS: 					"Windows",
			ANDROID: 					"Android",
			MAC_IOS: 					"MAC/IOS",
			UNIX: 						"Unix",
			LINUX: 						"Linux",
			OTHERS: 					"Others",

			DEVICE: 					"Device",
			RATE: 						"Rate",
			APPLICATION: 				"Application",

			NAME: 						"Name",
			MAC_ADDRESS: 				"MAC Address",
			IP_ADDRESS: 				"IP Address",


			DEVICES: 					"Devices"

			
		},

		BANDWIDTH:{
			BANDWIDTH: 					"Bandwidth",
			TEST_BANDWIDTH: 			"&gt;Test Bandwidth",
			STREAMBOOST: 				"streamboost",
			ENABLE_STREAMBOOST: 		"Enable streamboost",
			UP_LIMITATION: 				"Up Limitation(Mbps)",
			DOWN_LIMITATION: 			"Down Limitation(Mbps)",
			RUN_BANDWIDTH_TEST: 		"Run Bandwidth Test",
			TESTING: 					"Testing",
			TEST_FAILED: 				"Test Failed",
			TEST_SUCCEED: 				"Test Succeed",
			ENABLE_AUTOMATIC_TEST: 		"Enable Automatic Test",
			KEEP_UP_TO_DATE: 			"Keep StreamBoost up to date",
			ENABLE_AUTOMATIC_UPDATE: 	"Enable Automatic Update"

		},

		PRIORITY:{
			PRIORITY: 					"Priority",
			PRIORITY_TIPS: 				"Priority allows you to change the importance of one device over another. This is useful when the device are competing for limited bandwidth with application of the same classification.",
			ALL_DEVICE: 				"All Device",
			ACTIVE_DEVICE: 				"Active Device",
			SAVE: 						"Save",
			ID: 						"ID",
			DEVICE: 					"Device",
			TYPE: 						"Type",
			MAC_ADDRESS: 				"MAC Address",
			STICK: 						"Stick"
		},

		STREAMBOOST_STATISTICS: {
			STATISTICS: 				"Statistics",
			UP_TIME: 					"Up Time",
			DOWNLOADS: 					"Downloads",
			LAST_DAY: 					"Last Day",
			LAST_WEEK: 					"Last Week",
			LAST_MONTH: 				"Last Month",
			ALL_LAN_HOSTS: 				"All LAN Hosts",
			OTHER: 						"Other"

		},

		SECURITY_SETTING: {
			FIREWALL: 					"Firewall",
			ENABLE_SPI: 				"SPI Firewall",

			DOS_PROTECTION: 			"DoS Protection",
			ENABLE_DOS: 				"DoS Protection",
			
			OFF: 						"Off",
			LOW: 						"Low",
			MIDDLE: 					"Middle",
			HIGH: 						"High",

			ICMP: 						"ICMP-FLOOD Attack Filtering",
			UDP: 						"UDP-FLOOD Attack Filtering",
			TCP: 						"TCP-SYN-FLOOD Attack Filtering",
			ENABLE_DOS_TIP:             "DoS protection and Traffic Statistics must be enabled at the same time.",

			IGNORE: 					"Ignore Ping Packet From WAN Port",
			FORBID: 					"Forbid Ping Packet From LAN Port",

			BLOCK_DOS: 					"Blocked DoS Host List",
			HOST_NUMBER: 				"Host Number",
			IP_ADDRESS: 				"IP Address",
			MAC_ADDRESS: 				"MAC Address"
		},

		ACCESS_CTR:{
			ACCESSCTR: 					"Access Control",
			ENABLE_ACCESS: 				"Access Control",

			ACCESS_MODE: 				"Access Mode",
			DEFAULT_ACCESS_MODE: 		"Default Access Mode",
			BLACK_LIST: 				"Blacklist",
			WHITE_LIST: 				"Whitelist",
			
			WIRED:						"Wired",
			WIRELESS:					"Wireless",

			DEVICE_ONLINE: 				"Online Devices",
			NAME: 						"Device Name",
			IP_ADDRESS: 				"IP Address",
			MAC_ADDRESS: 				"MAC Address",
			CONN_TYPE: 					"Connection Type",

			BLOCK: 						"Block",

			DEVICE_IN_WHITE: 			"Devices in Whitelist",
			DEVICE_IN_BLACK: 			"Devices in Blacklist"
		},

		IP_MAC:{
			TITLE: 						"Settings",
			ENABLE_ARP: 				"ARP Binding",

			ARP_LIST: 					"ARP List",
			ARP_NUM: 					"ARP Entry Number",

			MAC_ADDRESS: 				"MAC Address",
			IP_ADDRESS: 				"IP Address",
			BOUND: 						"Bound",
			UNBOUND: 					"Unbound",

			BINDING_LIST: 				"Binding List",
			DESCRIPTION: 				"Description",
			OPTIONAL: 					"(Optional)",
			ENABLE_THIS_ENTRY: 			"Enable This Entry"
		},

		TIMESET: {
			TITLE: 						"Time Settings",
			ZONE: 						"Time Zone",
			DATE: 						"Date",
			DATEFORMAT: 				"MM/DD/YYYY",
			TIME: 						"Time",
			TIMEFORMAT: 				"(HH/MM/SS)",
			NTP1: 						"NTP Server I",
			NTP2: 						"NTP Server II",
			OPTIONAL: 					"(Optional)",

			CURRENT_TIME:  				"Current Time",
			SET_TIME: 					"Set Time",
			AUTOMATIC: 					"Get automatically from the Internet",
			MANUAL: 					"Manually",
			AUTOMATIC_BTN: 				"Obtain",


			GETGMT: 					"Get GMT",

			
			GETGMT_SUCCESS: 			"Get Time From NTP Server Success",
			GETGMT_TIMEOUT: 			"Get Time From NTP Server Time Out",
			GETGMT_WAIT: 				"Waiting",
			
			M: 							"M",
			W: 							"W",
			D: 							"D",
			H: 							"H",
			
			DAYLIGHT_SAVING: 			"Daylight Saving Time",
			ENABLE_DAYLIGHT: 			"Enable Daylight Saving Time",
			START: 						"Start",
			END: 						"End",

			RUNNING_STATUS: 			"Running Status",
			DOWN: 						"Daylight Saving Time is off.",
			UP: 						"Daylight Saving Time is on."
		},

		DIAG:{
			TITLE: 						"Diagnostics",
			DIAGNOSTIC_TOOL: 			"Diagnostic Tool",
			PING: 						"Ping",
			TRACE: 						"Traceroute",

			IPADDR: 					"IP Address/Domain Name",
			COUNT: 						"Ping Count",
			
			BASIC: 						"Basic",
			ADVANCED: 					"Advanced",
			
			COUNTUNIT: 					"(1-50)",

			PKT: 						"Ping Packet Size",
			PKTUNIT: 					"(4-1472 Bytes)",

			TIMEOUT: 					"Ping Timeout",
			TIMOUTUNIT: 				"(100-2000 Milliseconds)",

			TTL: 						"Traceroute Max TTL",
			TTLUNIT: 					"(1-30)",
			
			START: 						"Start",
			STOP: 						"Stop"
		},

		FIMWARE:{
			TITLE: 						"Firmware Upgrade",
			FIRMWARE_INFO:  			"Your firmware is up to date",

			NEWFILE: 					"New Firmware File",
			FIMWAREVERSION: 			"Firmware Version",
			HARDWAREVERSION: 			"Hardware Version",
			CONFIRM_CONTENT:			"Are you sure you want to upgrade the firmware?",
			WARNING:					"Firmware Upgrading... <br/>Please do NOT operate during the process.",
			REBOOTING: 					"Rebooting... <br/>Please do NOT operate during the process.",
			DO_NOT_OPERATE: 			"Upgrading... <br/>Please do NOT operate during the process.",
			FIRMWARE_UPDATING_NOTE: 	"Firmware upgrading...",
			REBOOTING_NOTE: 			"Rebooting...",
			SCREEN_UPDATING_NOTE: 		"Screen upgrading...",
			UPGRADE_FAILED: 			"Upgrade failed.",
			UPGRADE: 					"Upgrade",
			CHECK: 						"Check"
		},
		FIRMWARE:{
			FIRMWARE_INFO:  			"Your firmware is up to date",
			TITLE: 						"Firmware Upgrade",
			INFO: 						"Device Information",
			REMOTE_TITLE: 				"Online Upgrade",
			LOCAL_TITLE: 				"Local Upgrade",

			NEWFILE: 					"New Firmware File",
			FIRMWAREVERSION: 			"Firmware Version",
			HARDWAREVERSION: 			"Hardware Version",
			LATESTVERSION: 				"Latest Version",
			CONFIRM_CONTENT:			"Do you want to upgrade the firmware?",
			WARNING:					"Firmware upgrading... <br/> To avoid any damage, please keep the router powered on and do not operate the router during the firmware upgrade process.",
			REBOOTING: 					"Rebooting... <br/> To avoid any damage, please keep the router powered on and do not operate the router during the firmware upgrade process.",
			DO_NOT_OPERATE: 			"Upgrading... <br/> To avoid any damage, please keep the router powered on and do not operate the router during the firmware upgrade process.",
			FIRMWARE_UPDATING_NOTE: 	"1.Firmware Updating...<br/> To avoid any damage, please keep the router powered on and do not operate the router during the firmware upgrade process.",
			REBOOTING_NOTE: 			"2.Rebooting...<br/> To avoid any damage, please keep the router powered on and do not operate the router during the firmware upgrade process.",
			SCREEN_UPDATING_NOTE: 		"3.Screen Updating...<br/> To avoid any damage, please keep the router powered on and do not operate the router during the firmware upgrade process.",
			UPGRADE_FAILED: 			"Upgrade failed.",
			UPGRADE: 					"Upgrade",
			CHECK: 						"Check",
			DOWNLOADING: 				"Downloading...<br/> To avoid any damage, please keep the router powered on and do not operate the router during the firmware upgrade process.",
			UPGRADE_INOF: 				"To avoid any damage, please keep the router powered on.",
			NOTE: 						"Note: ",
			NO_UPGRADE: 				"This is the latest version",

			UPGRADING: 					"Upgrading...<br/> To avoid any damage, please keep the router powered on and do not operate the router during the firmware upgrade process.",
			RETRY: 						"Retry",
			CANCEL: 					"Cancel",
			ILEGAL_DEVICE:              "Unable to identify the device. Please contact TP-LINK technical support.",
			UPGRADE_FAIL: 				"Unable to upgrade. Please try again later.",
                        CONFIG_RESET_NOTE:                      "Your current settings will be lost after upgrading to this version.",
			CHECK_UPGRADE: 				"Check for upgrade"
		},

		BACKUP:{
			BACKUP: 					"Backup",
			BACKUPTIP: 					"Save a copy of your current settings.",

			RESTORE: 					"Restore",
			RESTORETIP: 				"Restore saved settings from a file.",
			
			RESTORE_WARN:				"Router Restoring... <br/>Please do NOT operate during the process.",
			RESTORE_CONFIRM_CONTENT: 	"Are you sure you want to restore the router from the backup file?",
			
			FILE: 						"File",

			FACTORY: 					"Factory Default Restore",
			FACTORYTIP: 				"Revert all the configuration settings to their default values.",
			FACTORY_CONFIRM_CONTENT:	"Are you sure you want to restore the router to its factory defaults?",
			FACTORY_WARN:				"Router Restoring... <br/>Please do NOT operate during the process.",
			
			BACKUPBTN: 					"Backup",
			RESTOREBTN: 				"Restore",
			FACTORYBTN: 				"Factory Restore"
		},

		ACCOUNT:{
			ACCOUNT_TITLE: 				"Account Management",
			
			OLDUSER: 					"Old Username",
			OLDPWD: 					"Old Password",

			NEWUSER: 					"New Username",
			NEWPWD: 					"New Password",
			CONFIRM: 					"Confirm New Password",

			RECOVERYINFO: 				"Password Recovery",
			ENABLE_PASSWORD_RECOVERY: 	"Enable Password Recovery",
			FROM: 						"From",
			TO: 						"To",
			SMTP_SERVER: 				"SMTP Server",
			
			ENABLE_AUTHENTICATION: 		"Enable Authentication",
			USERNAME: 					"Username",
			PASSWORD: 					"Password",

			TEST_MAIL: 					"Test Email",

			LOCAL:						"Local Management",
			LOCAL_MAC_AUTH: 			"Local MAC Authentication",
			ACCESS: 					"Access for All LAN Connected Devices",
			ACCESS_TIPS: 				"Toggle On to enable the management for all devices on LAN or keep it Off to enable the management for a specific device.",
			
			MAC_ADDRESS: 				"MAC Address",
			VIEW_BTN: 					"View Existing Devices",
			DESCRIPTION: 				"Description",

			EXIST_DEVICE:               "Existing Devices",

			OPTIONAL: 					"(Optional)",
			ENABLE_THIS_ENTRY: 			"Enable This Entry",

			DEVICE_NAME:				"Device Name",
			IP_ADDRESS:					"IP Address",
			

			REMOTE: 					"Remote Management",
			DISABLE_REMOTE_MANAGEMENR: 	"Disable Remote Management",
			ENABLE_REMOTE_MANAGEMENR_ALL: 	"Enable Remote Management for All Devices",
			ENABLE_REMOTE_MANAGEMENR: 	"Enable Remote Management for Specified Devices",
			WEB: 						"Web Management Port",

			NOTICE:						"In conflict with the virtual server port. Are you sure you want to continue?",

			REMOTEIP: 					"Remote Management IP Address",
			REMOTEIPNOTE: 				"(Enter 255.255.255.255 for all)"
			
		},

		SYSLOG:{
			TITLE: 						"System Log",
			LOG_FILTER: 				"Log Filter:",
			
			TYPE_EQ: 					"Type=",
			
			ALL: 						"ALL",

			FIREWALL: 					"Firewall", 
			NAT: 						"NAT",
			DDNS: 						"Dynamic DNS",
			UPNP:						"UPnP",
			IMB:            			"IP & MAC Binding",
			IPTV:						"IPTV",
			DHCPS:						"DHCP Server",
			IGMP_PROXY:					"IGMP Proxy",
			DOMAIN_LOGIN:				"Domain Login",
			BASIC_SECURITY: 			"Basic Security",
			PARENTAL_CONTROL: 			"Parental Controls",
			ACCESS_CONTROL: 			"Access Control",
			DOS_PROTECTION: 			"DoS Protection",
			QOS: 						"QoS",
			TRAFFIC_STATS: 				"Traffic Statistics",
			TIME_SETTINGS: 				"Time Settings",
			ACCOUNT_MANAGEMENT: 		"Account Management",
			LOCAL_MANAGEMENT: 			"Local Management",
			REMOTE_MANAGEMENT: 			"Remote Management",
			LOCALE: 					"Locale",
			FACTORY_RESET: 				"Factory Reset",
			LED_CONTROLLER: 			"Led Controller",
			NETWORK: 					"Network",
			USBSHARE: 					"USB share",
			AND: 						"and",
			LEVEL: 						"Level",
			EMERGENCY:					"EMERGENCY",
			ALERT:						"ALERT",
			CRITICAL:					"CRITICAL",
			ERROR: 						"ERROR",
			WARNING: 					"WARNING",
			NOTICE: 					"NOTICE",
			INFO: 						"INFO",
			DEBUG: 						"DEBUG",

			INDEX: 						"Index",
			TYPE: 						"Type",
			TIME: 						"Time",
			LEVEL_COL:					"Level",

			CONTENT: 					"Log Content",
			
			MAIL_LOG: 					"Mail Log",
			SAVE_LOG: 					"Save Log",

			SEND_OK: 					"Send OK",
			SEND_FAILED: 				"Send Failed",

			MAIL_SETTING: 				"Mail Settings",

 			FROM: 						"From",
 			TO: 						"To",
 			SMTP_SERVER: 				"SMTP Server",
 			ENABLE_AUTHENTICATION: 		"Enable Authentication",

 			USERNAME: 					"Username",
 			PASSWORD: 					"Password",
 			CONFIRM_PASSWORD: 			"Confirm Password",

 			AUTO_MAIL: 					"Enable Auto Mail",
 			LOG_AT: 					"Log at",
 			HH_MM: 						"(HH:MM) everyday",

 			LOG_EVERY: 					"Log every",
 			HOURS: 						"hours"
		},

		QOS:{
			TITLE: 						"QoS",
			QOS: 						"QoS",
			SETTINGS: 					"Settings",
			STATUS: 					"Enable QoS",
			UPBANDWIDTH: 				"Upload Bandwidth",
			DOWNBANDWIDTH: 				"Download Bandwidth",
			SPEED_M: 					"Mbps",
			SPEED_K: 					"Kbps",
			TEST: 						"Speed Test",
			RULE_LIST: 					"QoS Rule List",
			RULE: 						"QoS Rule",
			ID: 						"ID",
			NAME: 						"Name",
			TYPE: 						"Type",
			DETAIL: 					"Detail",
			PRIORITY: 					"Priority",

			APPLICATION: 				"Application",
			APPLICATION_LIST: 			"Application List",
			CUSTOM_APP: 				"Custom Application",
			MAC_ADDR: 					"MAC Address",
			MAC_ADDR_P: 				"MAC:",
			IP_ADDR: 					"IP Address",
			IP_P: 						"IP:",
			PHYSICAL_PORT: 				"Physical Port",

			LOW: 						"Low",
			MIDDLE: 					"Middle",
			HIGH: 						"High",

			PROTO: 						"Protocol",
			PORT: 						"Port",
			PROTO_P: 					"Protocol:",
			PORT_P: 					"Port:",
			PORT_TIPS: 					"(XX or XX-XX,1-65535,at most 5 pairs)",

			ALL: 						"ALL",
			TCP: 						"TCP",
			UDP: 						"UDP",
			TCP_UDP: 					"TCP/UDP",
			CUSTOM: 					"Custom",

			WIFI_HOME: 					"WIFI-HOST",
			WIFI_GUEST: 				"WIFI-GUEST",
			PORT1: 						"LAN1",
			PORT2: 						"LAN2",
			PORT3: 						"LAN3",
			PORT4: 						"LAN4",

			DATABASE_UPGRADE: 			"Database Upgrade",

			NEWFILE: 					"New Database File",
			FIMWAREVERSION: 			"Database Version",
			CONFIRM_CONTENT:			"Are you sure you want to update the database?",
			WARNING:					"Database Updating... <br/>Please do NOT operate during the process.",
			
			UPGRADE: 					"Upgrade",

			SERVICE_RESTART: 			"QoS restarting",
			
			TYPE: 						"Type",
			BY_DEVICE: 					"By Device",
			BY_APP: 					"By Application",
			BY_PHY: 					"By Physical Port",

			HIGH_PRIORITY_LBL: 			"High Priority:",
			MIDDLE_PRIORITY_LBL: 		"Middle Priority:",
			LOW_PRIORITY_LBL: 			"Low Priority:",

			HIGH_PRIORITY: 				"High Priority",
			MIDDLE_PRIORITY: 			"Middle Priority",
			LOW_PRIORITY: 				"Low Priority"

		},

		APPLICATION:{
			APP_LIST: 					"Application List",
			GAME_LIST: 					"Game List",

			QQ: 						"QQ"
			
		},

		STATISTICS: {
			TRIFFIC_STATISTICS: 		"Traffic Statistics",
			ENABLE_STATISTICS: 			"Traffic Statistics",

			TITLE: 						"Traffic Statistics List",
			IP_MAC: 					"IP Address/MAC Address",
			TPKT: 						"Total Packets",
			TBYTE: 						"Total Bytes",
			CPKT: 						"Current Packets",
			CBYTE: 						"Current Bytes",
			CICMP: 						"Current ICMP Tx",
			CUDP: 						"Current UDP Tx",
			CSYN: 						"Current SYN Tx",
			
			DELETE_CONFIRM: 			"Are you sure you want to delete the traffic statistics?",
			DELETE_ALL_CONFIRM: 		"Are you sure you want to delete all the traffic statistics?",

			RESET_ALL: 					"Reset All"
		},

		SYSPARA:{
			W24G: 						"2.4GHz Wireless",
			W5G: 						"5GHz Wireless",
			SWITCH_NOTICE:  			"Your wireless function is turned off. If you want to use this function, please press the Wi-Fi button to turn on your Wi-Fi.",

			ENABLE_TIPS: 				"Your wireless function is disabled.",

			BEACON: 					"Beacon Interval",
			BEACONUNIT: 				"(40-1000)",

			RTS: 						"RTS Threshold",
			RTSUNIT: 					"(1-2346)",
			
			FRAG: 						"Fragmentation Threshold",
			FRAGUNIT: 					"(256-2346)",

			DTIM: 						"DTIM Interval",
			DTIMUNIT: 					"(1-15)",

			GROUP: 						"Group Key Update Period",
			GROUPUNIT: 					"seconds",
			
			
			WMM_FEATURE: 				"WMM Feature",
			WMM: 						"Enable WMM",

			GI_FEATURE: 				"Short GI Feature",
			GI: 						"Enable Short GI",

			AP_FEATURE: 				"AP Isolation Feature",
			AP: 						"Enable AP Isolation",

			WDS_FEATURE: 				"WDS Bridging",
			WDS: 						"Enable WDS Bridging",
			
			SSID_BRIDEGE: 				"SSID (to be bridged)",
			SURVEY: 					"Survey",

			SN: 						"SN",
			MAC_ADDRESS: 				"MAC Address",
			SSID: 						"SSID",
			SIGNAL: 					"Signal",
			CHANNEL: 					"Channel",
			SECURITY: 					"Security",
			CHOSEN: 					"Chosen",
			AP_NUMBER:					"AP Number",

			TOTAL: 						"Total items",

			MAC: 						"MAC Address (to be bridged)",
			MACUNIT: 					"Example: 00-1D-0F-11-22-33",

			SECURITY: 					"Security",
			NO: 						"No",
			NONE: 						"None",
			WPA: 						"WPA-PSK/WPA2-PSK",
			WEP: 						"WEP",
			
			PASSWORD: 					"Password",
			
			AUTH_TYPE: 					"Auth. Type",
			AUTO: 						"Auto",
			OPEN: 						"Open System",
			SHARED: 					"Shared Key",

			WEP_INDEX: 					"WEP Index",
			KEY1: 						"Key1",
			KEY2: 						"Key2",
			KEY3: 						"Key3",
			KEY4: 						"Key4",

			WEP_KEY_FORMAT: 			"WEP Key Format",
			ASC: 						"ASCII",
			HEX: 						"Hexadecimal",			

			WPS: 						"WPS",
			ENABLE_WPS: 				"Enable WPS",

			NAT: 						"NAT",
			ENABLE_NAT: 				"Enable NAT",
			
			NAT_BOOST: 					"NAT Boost",
			ENABLE_NAT_BOOST: 			"Enable NAT Boost",
			
			MEDIA_SERVER: 				"Media Server",
			SCAN_INTERVAL: 				"Auto Scan Interval (Hours)",
			SCAN_UNIT: 					"(2-48)",

			DOS_PROTECTION: 			"DoS Protection Level Settings",

			ICMP: 						"ICMP-FLOOD Packets Level",
			UDP: 						"UDP-FLOOD Packets Level",
			TCP: 						"TCP-FLOOD Packets Level",

			WDS_MODE: 					"WDS Mode",
			WDS1: 						"WDS1",
			WDS2: 						"WDS2",

			LOW: 						"Low",
			MIDDLE: 					"Middle",
			HIGH: 						"High",

			TO: 						"to",
			NOTICE_NAT_REBOOT: 			"Rebooting... <br/>Please do NOT operate during the process.",

			NOTICE_NAT_BOOST: 			"Modification of NAT Boost will result in the rebooting of this device, are you sure you want to continue?",
			NOTICE:						"Your router's channel is not the same with the bridged AP's channel. Please check.",

			UNIT: 						"(5-7200)Packets/Secs",
			LED: 						"LED Control",
			NIGHT_MODE: 				"Night mode",
			PERIOD_NIGHT_TIME: 			"Night Mode Period",
			ENABLE: 					"Enable Night Mode",
			HH_MM: 						"(HH:MM)",
			NIGHT_MODE_NOTE:            "Your LED is turned off. If you want to use this function, please click the LED on the upper right of the page.",
			NOTE2:                      "The night mode period takes effect based on the router's system time. Please make sure you have already set up the Time of the router."
		},
		VPN:{
			OPEN_VPN: 					"OpenVPN",
			NO_CERT_NOTE: 				"No certificate currently, please <b>Generate</b> one before enabling VPN Server.",
			NO_CERT_NOTE2: 				"No certificate currently, please <b>Generate</b> one before export the configuration.",
			ENABLE_VPN_SERVER: 			"Enable VPN Server",
			SERVICE_TYPE: 				"Service Type",
			SERVICE_PORT: 				"Service Port",
			VPN_SUBNET: 				"VPN Subnet/Netmask",
			CLIENTS_ACCESS: 			"Client Access",
			UDP: 						"UDP",
			TCP: 						"TCP",
			HOME_NETWORK_ONLY: 			"Home Network Only",
			INTERNET_HOME: 				"Internet and Home Network",
			CERT_STR: 					"No certificate currently, click OK to generate one and save your configuration.",
			CERT_STR2: 					"No certificate currently, click OK to generate one and export your configuration.",
			CONF_FILE: 					"Configuration File", 
			EXPORT_CONF_FILE: 			"Export the configuration.",
			EXPORT: 					"Export",

			INSTALL_GUIDE: 				"VPN Client Installation Guide",
			INSTALL_STEP: 				"To enable and connect your client devices to the OpenVPN server:",
			INSTALL_NOTICE:				"Before you configure the OpenVPN server, please configure Dynamic DNS Service (recommended) or assign a static IP address for Wan port. And make sure System Time is correct.",
			INSTALL_NOTE1: 				"Select Enable VPN Server.",
			INSTALL_NOTE2: 				"Configure the OpenVPN server parameters (Service Type, Service Port and Client Access) and click Save. And make sure your external port of NAT settings is not the service port and DMZ is disabled.",
			INSTALL_NOTE3: 				"Click Export to save the configuration file.",
			INSTALL_NOTE4: 				"On your client devices, download and install the OpenVPN client utility from <a class=\"link\" href=\"http://openvpn.net/index.php/download/community-downloads.html\">http://openvpn.net/index.php/download/community-downloads.html</a><br> The official supported platforms include Windows, Mac OSX, Linux.",
			INSTALL_NOTE5: 				"Launch the OpenVPN client utility and add a new VPN connection using the saved configuration file to connect your client device to the VPN server.",
			NOTE: 						"To learn more about OpenVPN clients, visit <a class=\"link\" href=\"http://openvpn.net/index.php/open-source/documentation/howto.html#quick\">http://openvpn.net/index.php/open-source/documentation/howto.html#quick</a>",

			PPTPVPN: 					"PPTP VPN",
			CLIENT_IP_ADDRESS: 			"Client IP Address",
			ACCOUNT_USERNAME: 			"Username",
			ACCOUNT_PASSWORD: 			"Password",
			CLIENT_IP_NOTE: 			"(up to 10 clients)",
			SAME_SUBNET_NOTE: 			"The client IP address and LAN IP address cannot be in the same subnet. <br/>Please enter another one.",
			CONFLICT_WITH_DHCP: 		"The client IP address conflicts with the DHCP IP address pool. <br/>Please enter again.",
			CONFLICT_WITH_RESERVED: 	"The client IP address conflicts with the reserved IP address. <br/>Please enter again.",
			CONFLICT_WITH_OPENVPN: 		"The client IP address and OpenVPN IP address cannot be in the same subnet. <br/>Please enter again.",
			SAME_SUBNET_NOTE2: 			"The  VPN Subnet/Netmask and LAN IP address cannot be in the same subnet. <br/>Please enter another one.",
			CONFLICT_WITH_DHCP2: 		"The VPN Subnet/Netmask conflicts with the DHCP IP address pool. <br/>Please enter again.",
			CONFLICT_WITH_RESERVED2: 	"The VPN Subnet/Netmask conflicts with the reserved IP address. <br/>Please enter again.",
			CONFLICT_WITH_PPTPVPN: 		"The VPN Subnet/Netmask and PPTP VPN IP address cannot be in the same subnet. <br/>Please enter again.",
			VPN_MASK_ERROR: 			"The netmask cannot be greater than 255.255.255.248. <br/>Please enter again.",
			ACCOUNT_LIST: 				"Account List (up to 16 users)",
			ADVANCED_SETTING: 			"Advanced",
			ALLOW_SAMBA: 				"Allow Samba (Network Place) access",
			ALLOW_NETBIOS: 				"Allow NetBIOS passthrough",
			ALLOW_UNENCRYPTED_CONN: 	"Allow Unencrypted connections",
			USERNAME_CONFLICT: 			"This username already exists. Please enter another one.",
			
			CONNECT_PPTP: 				"To enable and connect your client devices to the PPTP VPN server:",
			CONNECT_NOTICE:				"Before you configure the PPTP VPN server, please configure Dynamic DNS Service (recommended) or assign a static IP address for WAN port. And make sure your external port of NAT settings is not 1723 and DMZ is disabled, and also your System Time is correct.",
			CONNECT_NOTE1: 				"Select Enable VPN Server.",
			CONNECT_NOTE2: 				"Configure the PPTP VPN server parameters and click Save.",
			CONNECT_NOTE3: 				"On your client devices, create a PPTP VPN connection. The official supported platforms include Windows, Mac OSX, Linux, iOS, and Android.",
			CONNECT_NOTE4: 				"Launch the PPTP VPN program, add a new connection and enter the domain name of the registered DDNS service or the static IP address that assigned to the WAN port, to connect your client device to the PPTP VPN server.",
			
			GENERATE_CERT: 				"Certificate",
			GENERATE_NEW_CERT: 			"Generate the certificate.",
			GENERATE: 					"Generate",
			GENERATE_TIPS: 				"Certificate generating...<br/>It will take a few minutes, please wait.",
			CERT_SUCCESS: 				"Success",
			CERT_FAIL: 					"Failed, please try again.",
			
			VPN_CONNECTIONS: 			"VPN Connections",
			OPEN_VPN_CONNECTIONS: 		"OpenVPN Connection",
			PPTP_VPN_CONNECTIONS: 		"PPTP VPN Connection",
			USER:				"User",
			REMOTE_IP:			"Remote IP",
			ASSIGNED_IP:			"Assigned IP"
		}
	};
})(jQuery);
