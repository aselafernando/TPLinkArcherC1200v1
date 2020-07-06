#!/bin/sh

. /usr/share/libubox/jshn.sh

WL=/usr/sbin/wl
NVRAM=/usr/sbin/nvram
WLCONF=/usr/sbin/wlconf
IFCONFIG=/sbin/ifconfig
BRCTL=/usr/sbin/brctl
FW=/sbin/fw
BRCM_COUNTRYCODE=/lib/wifi/brcm_countrycode.txt
BRCM_WLAN_CONFIG=/lib/wifi/brcm_wlan_config.txt

STDOUT=/dev/null
#DEBUG=1
[ -n "${DEBUG}" ] && STDOUT="/dev/console"
CONSOLE="/dev/console"

NO_EXPORT=1
MACFILTER_ENABLE="off"
MACFILTER_ACTION=""
MAC_LIST=""
COUNTRYCODE_EU=80
COUNTRYCODE_Q2=79
HOME_2G_VIF=""
HOME_5G_VIF=""
state="off"
remove_ko="0"

nvram() {
	[ -n "${DEBUG}" ] && echo nvram "$@" >$STDOUT
	$NVRAM "$@"
}

wl() {
	[ -n "${DEBUG}" ] && echo wl "$@" >$STDOUT
	$WL "$@"
}

wlconf() {
	[ -n "${DEBUG}" ] && echo wlconf "$@" > $STDOUT
	$WLCONF "$@"
}

ifconfig() {
	[ -n "${DEBUG}" ] && echo ifconfig "$@" > $STDOUT
	$IFCONFIG "$@"
}

brctl() {
	[ -n "${DEBUG}" ] && echo brctl "$@" > $STDOUT
	$BRCTL "$@"
}

fw() {
	[ -n "${DEBUG}" ] && echo fw "$@" > $STDOUT
	$FW "$@"
}

get_brname() {
	json_init
	json_load "`ubus call network.interface.lan status`"
	json_get_var l3_device l3_device
	export ${NO_EXPORT:+-n} "$1=$l3_device"
}

kick_sta() {
	ifname=$1
	sta_list=`wl -i $ifname assoclist | cut -d ' ' -f 2`

	if [ "$MACFILTER_ACTION" = "deny" ]; then
		for sta in ${sta_list}; do
			echo ${MAC_LIST} | grep -q ${sta} && wl -i $ifname deauthenticate "$sta"
		done
	elif [ "$MACFILTER_ACTION" = "allow" ]; then
		for sta in ${sta_list}; do
			echo ${MAC_LIST} | grep -q ${sta} || wl -i $ifname deauthenticate "$sta"
		done
	else
		echo "macfilter action is not correct!" >$CONSOLE
	fi
}

wifi_fixup_mode() {
	local dev="$1"
	local hwmode
	local abg_mode
	config_get hwmode $dev hwmode
	case "$hwmode" in
		a_5)
			hwmode=11a
			abg_mode="1"
		;;
		b)
			hwmode=11b
			abg_mode="1"
		;;
		g)
			hwmode=11g
			abg_mode="1"
		;;
		bg)
			hwmode=11bg
			abg_mode="1"
		;;
		n)
			hwmode=11n
		;;
		gn)
			hwmode=11gn
		;;
		n_5)
			hwmode=11n
		;;
		an_5)
			hwmode=11an
		;;
		bgn)
			hwmode=11bgn
		;;
		ac_5)
			hwmode=11ac
		;;
		nac_5)
			hwmode=11nac
		;;
		anac_5)
			hwmode=11anac
		;;
	esac
	config_set $dev hwmode $hwmode
	if [ "$abg_mode" = "1" ]; then
		config_set $dev htmode 20
	fi
}

wifi_fixup_config() {
	local enable macfilter
	#config_get_bool enable filter enable 0
	#config_get macfilter filter action "deny"

	#get config from access control
	if [ $(ac get_enable) = "on" ]; then
		enable="1"
	elif [ $(ac get_enable) = "off" ]; then
		enable="0"
	else
		echo "bad ac enable" >$STDOUT
		enable="0"
	fi

	if [ $enable = "1" ]; then
		#get config from access control
		MACLIST=$(ac get_maclist)
		MACLIST=${MACLIST//-/:}
		#replace \n with space
		MACLIST=`echo $MACLIST | tr '\n' ' '`

		#config_get macfilter filter action "deny"
		#get config from access control
		if [ $(ac get_mode) = "black" ]; then
			macfilter="deny"
		elif [ $(ac get_mode) = "white" ]; then
			macfilter="allow"
		else
			echo "bad ac mode" >$STDOUT
			macfilter="0"
		fi 

		MAC_LIST=$MACLIST
		MACFILTER_ENABLE="on"
		MACFILTER_ACTION=$macfilter
	else
		MAC_LIST=""
		MACFILTER_ENABLE="off"
		MACFILTER_ACTION=""
		unset MACLIST
	fi

	for dev in ${1:-DEVICES}; do
		wifi_fixup_mode "$dev"
	done
}

wifi_default() {
	echo "wifi_default" >$STDOUT
}

wifi_vap() {
	echo "wifi_vap" >$STDOUT
	wifi_reload
}

wifi_mode(){
	echo "wifi_mode" >$STDOUT
	wifi_reload
}

wifi_radio() {
	echo "wifi_radio" >$STDOUT
	wifi_reload
}

#wifi_country() {
#	echo "wifi_country" >$STDOUT
#	wifi_reload
#}
wifi_country() {
	echo "wifi_country" >$STDOUT
	local cmd_flag=1

	local eth_enable=""
	local guest_enable=""
	local wds_enable=""
	local ifnames_enabled=""
	local ifnames_disabled=""

	if [ "$cmd_flag" = "0" ]; then
		wifi_reload
	else
		for dev in ${DEVICES}; do
			ifnames_enabled=""
			ifnames_disabled=""
			config_get_bool wifi_disabled $dev disabled       # hardware switch
			config_get_bool soft_disabled $dev disabled_all   # software switch
			if [ "$wifi_disabled" = "0" -a "$soft_disabled" = "0" ]; then
				config_get vifs $dev vifs
				for vif in $vifs; do    # vifs is wl01/wl02/wl03, home/guest/wds
					config_get_bool enable $vif enable
					if [ "$enable" = "1" ]; then
						config_get mode $vif mode
						config_get guest $vif guest
						config_get ifname $vif ifname
						append ifnames_enabled "$ifname"
						if [ "$mode" = "ap" -a -z "$guest" ]; then
							eth_enable="1"
						elif [ "$mode" = "ap" -a "$guest" = "on" ]; then
							eth_enable="1"
							guest_enable="1"
						elif [ "$mode" = "sta" ]; then
							eth_enable="1"
							wds_enable="1"
						else
							echo "=====>>>>> $dev: vif $vif is disabled or $vif is a guest network" >$STDOUT
						fi
					fi
				done
			else
				append ifnames_disabled $dev
			fi

			config_get band $dev band
			case $band in
				2g)
					HOME_WIFI="wl1"
					GUEST_WIFI="wl1.1"
				;;
				5g)
					HOME_WIFI="wl0"
					GUEST_WIFI="wl0.1"
				;;
			esac

			local tmpCode=""
			local country_code=""
			local country_rev=""
			config_get country $dev country
			tmpCode=`cat $BRCM_COUNTRYCODE | awk '$1=="'"$country"'" {print $2}'`
			if [ "$tmpCode" = "EU" ]; then
				nvram set ${HOME_WIFI}_country_code="$tmpCode"
				nvram set ${HOME_WIFI}_country_rev="$COUNTRYCODE_EU"
				country_code="$tmpCode"
				country_rev="$COUNTRYCODE_EU"
			elif [ "$tmpCode" = "Q2" ]; then
				nvram set ${HOME_WIFI}_country_code="$tmpCode"
				nvram set ${HOME_WIFI}_country_rev="$COUNTRYCODE_Q2"
				country_code="$tmpCode"
				country_rev="$COUNTRYCODE_Q2"
			else
				nvram set ${HOME_WIFI}_country_code="$country"
				nvram set ${HOME_WIFI}_country_rev="$tmpCode"
				country_code="$country"
				country_rev="$tmpCode"
			fi

			# set nmode
			config_get hwmode $dev hwmode
			if [ "$hwmode" = "11b" -o "$hwmode" = "11g" -o "$hwmode" = "11bg" -o "$hwmode" = "11a" ]; then
				nvram set ${HOME_WIFI}_nmode="0"
			else
				nvram set ${HOME_WIFI}_nmode="-1"
			fi

			# set gmode
			if [ "$hwmode" = "11b" ]; then
				nvram set ${HOME_WIFI}_gmode="0"
			elif [ "$hwmode" = "11g" -o "$hwmode" = "11gn" ]; then
				nvram set ${HOME_WIFI}_gmode="2"
			else
				nvram set ${HOME_WIFI}_gmode="1"
			fi

			# set bss_opmode_cap_reqd
			if [ "$hwmode" = "11n" -o "$hwmode" = "11nac" ]; then
				nvram set ${HOME_WIFI}_bss_opmode_cap_reqd="2"
			elif [ "$hwmode" = "11ac" ]; then
				nvram set ${HOME_WIFI}_bss_opmode_cap_reqd="3"
			else
				nvram set ${HOME_WIFI}_bss_opmode_cap_reqd="0"
			fi

			# set bw_cap
			config_get htmode $dev htmode
			if [ "$htmode" = "auto" ]; then
				if [ "$hwmode" = "11ac" -o "$hwmode" = "11anac" -o "$hwmode" = "11nac" ]; then
					nvram set ${HOME_WIFI}_bw_cap="7"
				else
					nvram set ${HOME_WIFI}_bw_cap="3"
				fi
			elif [ "$htmode" = "80" ]; then
				nvram set ${HOME_WIFI}_bw_cap="7"
			elif [ "$htmode" = "40" ]; then
				nvram set ${HOME_WIFI}_bw_cap="3"
			else
				nvram set ${HOME_WIFI}_bw_cap="1"
			fi

			# set obss_coex
			if [ "$htmode" = "auto" ]; then
				nvram set ${HOME_WIFI}_obss_coex="1"
			else
				nvram set ${HOME_WIFI}_obss_coex="0"
			fi

			# set chanspec (wlanChspec)
			config_get channel $dev channel
			wifi_chanspec_config chanspec $htmode $channel
			if [ "$band" = "5g" -a "$wds_enable" = "1" ]; then
				nvram set ${HOME_WIFI}_chanspec="0"
			else
				nvram set ${HOME_WIFI}_chanspec="$chanspec"
			fi

			# set radio
			vap_name="${HOME_WIFI}.1"
			if [ "$eth_enable" = "1" ]; then
				nvram set ${HOME_WIFI}_radio="1"
				if [ "$wds_enable" = "1" ]; then
					nvram set ${vap_name}_radio="1"
				fi
				if [ "$guest_enable" = "1" ]; then
					nvram set ${GUEST_WIFI}_radio="1"
				fi
			fi

			for ifname in $ifnames_enabled; do 
				wl -i $ifname country "$country_code"/"$country_rev"
				wlconf $ifname down
				wlconf $ifname up
				wlconf $ifname start
			done

			for ifname in $ifnames_disabled; do 
				wl -i $ifname country "$country_code"/"$country_rev"
			done
		done
	fi
}

wifi_wps() { 
	local vif="$1"
	local wps
	local wps_cmd
	
	wps_cmd="/usr/sbin/wps-socket -c "

	config_get_bool wps $vif wps 0
	if [ "$wps" = "1" ]; then
		case $2 in
		wps_ap_pin)
			#wifi_reload
			for dev in ${DEVICES}; do  # eth1 eth2
				wifi_wps_config $dev
			done

			killall wps_monitor
			sleep 1
			wps_monitor &
			;;
		status)
			local status method peerAddr

			status=`nvram get wps_proc_status`
			method=`nvram get wps_current_method`
			peerAddr=`nvram get wps_sta_mac`

			#method==1:pin; method==2:pbc
			if [ "$status" = "1" ]; then 
				if [ "$method" = "1" ]; then 
					echo -e "PBC Status: Unknown\nPIN Status: Active\nLast WPS result: None"
				else
					echo -e "PBC Status: Active\nPIN Status: Unknown\nLast WPS result: None"
				fi
			elif [ "$status" = "2" ] || [ "$status" = "7" ]; then
				echo -e "PBC Status: Unknown\nPIN Status: Unknown\nLast WPS result: Success\nPeer Address: $peerAddr"
			elif [ "$status" = "4" ]; then
				if [ "$method" = "1" ]; then 
					echo -e "PBC Status: Unknown\nPIN Status: Timed-out\nLast WPS result: Failed"
				else
					echo -e "PBC Status: Timed-out\nPIN Status: Unknown\nLast WPS result: Failed"
				fi
			elif [ "$status" = "8" ]; then
				echo -e "PBC Status: Overlap\nPIN Status: Unknown\nLast WPS result: Failed"
			else
				echo -e "PBC Status: Unknown\nPIN Status: Unknown\nLast WPS result: None"
			fi
			;;
		pin_lock)
			local lock

			lock=`nvram get wps_aplockdown`
			if [ "$lock" = "1" ]; then
				echo "LockDown: Lock"
			else
				echo "LockDown: Unlock"
			fi
			;;
		pin | pbc)
			local cmd
			local cmdSet cmdAction
			local ssid cmdSsid
			local authType
			local secSubType cmdAkm
			local pskCipher cmdCrypto
			local pskKey cmdPsk
			local cmdSecurity
			local method pin ifname cmdPbcMethod cmdOther

			cmdSet="SET "
			cmdAction="wps_action=\"3\" "

			config_get ssid $vif ssid
			cmdSsid="wps_ssid=\"$ssid\" "

			config_get authType $vif encryption	
			if [ "$authType" = "psk" ]; then
				config_get secSubType $vif psk_version
				if [ "$secSubType" = "wpa" ]; then
					cmdAkm="wps_akm=\"\" "
				elif [ "$secSubType" = "rsn" ]; then
					cmdAkm="wps_akm=\"psk2\" "
				else
					cmdAkm="wps_akm=\"psk psk2\" "
				fi

				config_get pskCipher $vif psk_cipher
				if [ "$pskCipher" = "auto" ]; then
					cmdCrypto="wps_crypto=\"tkip+aes\" "
				elif [ "$pskCipher" = "aes" ]; then
					cmdCrypto="wps_crypto=\"aes\" "
				else
					cmdCrypto="wps_crypto=\"\" "
				fi

				config_get pskKey $vif psk_key
				cmdPsk="wps_psk=\"$pskKey\" "

				cmdSecurity="$cmdAkm""$cmdCrypto""$cmdPsk"
			else
				cmdAkm="wps_akm=\"\" "

				cmdSecurity="$cmdAkm"
			fi

			if [ "$2" = "pin" ]; then
				method="1"
				pin="$3"
				cmdPbcMethod=""
			elif [ "$2" = "pbc" ]; then
				method="2"
				pin="00000000"
				cmdPbcMethod="wps_pbc_method=\"2\" "
			fi
			config_get ifname $vif ifname
			cmdOther="wps_sta_pin=\"$pin\" wps_method=\"$method\" wps_config_command=\"1\" wps_ifname=\"$ifname\" "

			nvram set wps_proc_status="0"
			nvram set wps_current_method="$method"

			cmd="$cmdSet""$cmdAction""$cmdSsid""$cmdSecurity""$cmdPbcMethod""$cmdOther"

			$wps_cmd"$cmd"
			;;
		cancel)
			local cmd

			cmd="SET wps_config_command=\"2\" wps_action=\"0\" "

			$wps_cmd"$cmd"
			;;
		*)
			echo "it is other" >$STDOUT
			echo $* >$STDOUT
			;;
		esac

		echo -e "\n"
		echo "wps_shell_over"
	fi
}

wifi_vlan() {
	local wifi_state=""
	wifi_state=`cat /tmp/wifi_state`

	local isaddif=""
	if [ $1 = "notaddif" ];then
		isaddif=0
	else
		isaddif=1
	fi

	local brname;
	local hvlan=$((0x3)) gvlan=0;
	get_brname brname
	for port in $(brctl show "$brname" | grep eth | cut -f 6-8); do
		brctl setifvlan "$brname" "$port" "$hvlan" 1
	done

	if [ "$wifi_state" = "inited" -o "$isaddif" = "0" ]; then 
		echo "=====>>>>> wifi_vlan" >$STDOUT

		local module_name=br_filter
		local block_ifaces=""
		grep -q '^'$module_name /proc/modules || insmod $module_name

		for dev in $DEVICES; do
			config_get_bool wifi_disabled $dev disabled
			if [ "$wifi_disabled" = "0" ]; then
				config_get vifs $dev vifs
				config_get band $dev band
				for vif in $vifs; do
					config_get_bool enable $vif enable
					config_get mode $vif mode
					config_get guest $vif guest
					config_get ifname $vif ifname
					config_get_bool isolate "$vif" isolate 1
					config_get_bool access "$vif" access 1

					#local fw_action="unblock"
					if [ "$enable" = "1" -a "$mode" = "ap" -a -z "$guest" ]; then
						[ "$isaddif" = "1" ] && brctl addif "$brname" "$ifname"
						brctl setifvlan "$brname" "$ifname" "$hvlan" 1
					elif [ "$enable" = "1" -a "$mode" = "ap" -a "$guest" = "on" ]; then
						#fw_action="block"
						if [ "$access" = "0" ]; then
							if [ "$gvlan" = 0 ]; then
								gvlan=$((0x4))
							else
								[ "$isolate" = 1 ] && gvlan="$(($gvlan << 1))"
							fi
						else
							if [ "$gvlan" = 0 ]; then
								gvlan=$((0x1))
							else
								[ "$isolate" = 1 ] && gvlan="$(($gvlan << 1))"
							fi
						fi
						[ "$isaddif" = "1" ] && brctl addif "$brname" "$ifname"
						brctl setifvlan "$brname" "$ifname" "$gvlan" 1
						#fw "$fw_action"_rt_access dev "$ifname" &
						
						if [ "$block_ifaces" = "" ]; then
							block_ifaces="$ifname"
						else
							block_ifaces="$block_ifaces"" $ifname"
						fi

						# kick all sta
						wl -i $ifname deauthenticate

					elif [ "$enable" = "1" -a "$mode" = "sta" ]; then
						case $band in
							2g)
								IFNAME="wl0.1"
							;;
							5g)
								IFNAME="wl1.1"
							;;
						esac
						[ "$isaddif" = "1" ] && brctl addif "$brname" "$IFNAME"
						brctl setifvlan "$brname" "$IFNAME" 15 1
					else
						echo "=====>>>>> $dev: vif $vif is disabled" >$STDOUT
					fi
				done
			fi
		done

		#set bridge_filter
		echo "$block_ifaces" > /proc/bridge_filter/device_list
	fi
}

wifi_macfilter() {
	local wifi_state=""
	wifi_state=`cat /tmp/wifi_state`
	if [ "$wifi_state" = "inited" ]; then
		echo "=====>>>>> in wifi_macfilter" >$STDOUT
		for dev in ${DEVICES}; do  
			wifi_macfilter_config $dev dynamic
		done
	fi
}

# DUT special feature cfg
init_feature_config() {
	local wlan_support_11ac=""
	wlan_support_11ac=`cat $BRCM_WLAN_CONFIG | awk '$1=="'wlan_support_11ac'" {print $2}'`
	if [ "$wlan_support_11ac" = "yes" ]; then
		nvram set wl_txchain=3
		nvram set wl_rxchain=3
		nvram set wl0_phytype=v
		nvram set wl1_phytype=n
	else
		nvram set wl_txchain=3
		nvram set wl_rxchain=3
		nvram set wl0_phytype=n
		nvram set wl1_phytype=n
	fi
}

# mac cfg
init_nvram_mac() {
	#lanmacaddr=`network_get_firm lan`
	#lanmacaddr=${lanmacaddr//-/:}
	#lanmacaddr=` echo $lanmacaddr | tr '[A-F]' '[a-f]' `
	#nvram set et0macaddr="$lanmacaddr"

	#wanmacaddr=`network_get_firm wan`
	#wanmacaddr=${wanmacaddr//-/:}
	#wanmacaddr=` echo $wanmacaddr | tr '[A-F]' '[a-f]' `
	#nvram set et1macaddr="$wanmacaddr"

	for dev in $DEVICES; do
		config_get band "$dev" band
		case "$band" in
			2g)
				config_get macaddr "$dev" macaddr
				macaddr=${macaddr//-/:}
				nvram set 0:macaddr="${macaddr}"
			;;
			5g)
				config_get macaddr "$dev" macaddr
				macaddr=${macaddr//-/:}
				nvram set "sb/1/macaddr"="${macaddr}"
			;;
		esac
	done

	#nvram set wl_txq_thresh="1024"
	#nvram set wl0_nband="2"
	#nvram set wl1_nband="1"
	#nvram set wl0_phytype="h"
	#nvram set wl1_phytype="v"
	#nvram set is_modified="0"
}

wifi_chanspec_config() {
	local tmpChanspec
	local cw
	htmode=$2
	channel=$3
	if [ $channel = "auto" ]; then
		tmpChanspec="0"
	else
		case $channel in
		1|2|3|4|5)
			if [ $htmode = "auto" ]; then
				cw=40
				tmpChanspec="${channel}l"
			elif [ $htmode = "20" ]; then
				tmpChanspec="$channel"
			elif [ $htmode = "40" ]; then
				tmpChanspec="${channel}l"
			fi
		;;
		6|7|8|9|10|11|12|13)
			if [ $htmode = "auto" ]; then
				cw=40
				tmpChanspec="${channel}u"
			elif [ $htmode = "20" ]; then
				tmpChanspec="$channel"
			elif [ $htmode = "40" ]; then
				tmpChanspec="${channel}u"
			fi
		;;
		36|44|52|60|100|108|132|140|149|157)
			if [ $htmode = "auto" ]; then
				cw=80
				tmpChanspec="${channel}/80"
			elif [ $htmode = "20" ]; then
				tmpChanspec="$channel"
			elif [ $htmode = "40" ]; then
				tmpChanspec="${channel}l"
			elif [ $htmode = "80" ]; then
				tmpChanspec="${channel}/80"
			fi
		;;
		40|48|56|64|104|112|136|144|153|161)
			if [ $htmode = "auto" ]; then
				cw=80
				tmpChanspec="${channel}/80"
			elif [ $htmode = "20" ]; then
				tmpChanspec="$channel"
			elif [ $htmode = "40" ]; then
				tmpChanspec="${channel}u"
			elif [ $htmode = "80" ]; then
				tmpChanspec="${channel}/80"
			fi
		;;
		165)
			tmpChanspec="$channel"
		;;
		*)
			echo "ERROR: the channel you set is not correct!!!" >$CONSOLE
		;;
		esac
	fi

	export ${NO_EXPORT:+-n} "$1=$tmpChanspec"
}

# set home wifi
wifi_basic_config() {
	local dev="$1"
	local wds_enable="0"
	local eth_enable="0"
	local home_vif=""
	local wds_vif=""
	local vif=""
	echo "=====>>>>> $dev: wifi_basic_config" >$STDOUT

	config_get country $dev country
	config_get band $dev band
	config_get hwmode $dev hwmode
	config_get htmode $dev htmode
	config_get channel $dev channel
	echo "=====>>>>> $dev: hwmode is $hwmode, htmode is $htmode" >$STDOUT

	config_get_bool wifi_disabled $dev disabled
	if [ "$wifi_disabled" = "0" ]; then
		config_get vifs $dev vifs
		for vif in $vifs; do # vifs is wl01/wl02/wl03, home/guest/wds
			config_get_bool enable $vif enable
			config_get mode $vif mode
			config_get guest $vif guest
			if [ "$enable" = "1" -a "$mode" = "ap" -a -z "$guest" ]; then
				eth_enable="1"
				home_vif=$vif
				echo "=====>>>>> $dev: HOME WIFI is on" >$STDOUT
			elif [ "$enable" = "1" -a "$mode" = "sta" ]; then
				eth_enable="1"
				wds_enable="1"
				wds_vif=$vif
				echo "=====>>>>> $dev: WDS is on" >$STDOUT
			else
				echo "=====>>>>> $dev: vif $vif is disabled or $vif is a guest network" >$STDOUT
			fi
		done
	fi

	case $band in
		2g)
			vif="$home_vif"
			HOME_WIFI="wl1"
			if [ "$wds_enable" = "1" ]; then
				HOME_2G_VIF="$home_vif"
			fi
		;;
		5g)
			vif="$home_vif"
			HOME_WIFI="wl0"
			if [ "$wds_enable" = "1" ]; then
				HOME_5G_VIF="$home_vif"
			fi
		;;
	esac

	if [ "$eth_enable" = "1" ]; then
		config_get ssid $vif ssid
		nvram set ${HOME_WIFI}_ssid="$ssid"
		local tmpCode=""
		tmpCode=`cat $BRCM_COUNTRYCODE | awk '$1=="'"$country"'" {print $2}'`
		if [ "$tmpCode" = "EU" ]; then
			nvram set ${HOME_WIFI}_country_code="$tmpCode"
			nvram set ${HOME_WIFI}_country_rev="$COUNTRYCODE_EU"
		elif [ "$tmpCode" = "Q2" ]; then
			nvram set ${HOME_WIFI}_country_code="$tmpCode"
			nvram set ${HOME_WIFI}_country_rev="$COUNTRYCODE_Q2"
		else
			nvram set ${HOME_WIFI}_country_code="$country"
			nvram set ${HOME_WIFI}_country_rev="$tmpCode"
		fi

		# set nmode
		if [ "$hwmode" = "11b" -o "$hwmode" = "11g" -o "$hwmode" = "11bg" -o "$hwmode" = "11a" ]; then
			nvram set ${HOME_WIFI}_nmode="0"
		else
			nvram set ${HOME_WIFI}_nmode="-1"
		fi

		# set gmode
		if [ "$hwmode" = "11b" ]; then
			nvram set ${HOME_WIFI}_gmode="0"
		elif [ "$hwmode" = "11g" -o "$hwmode" = "11gn" ]; then
			nvram set ${HOME_WIFI}_gmode="2"
		else
			nvram set ${HOME_WIFI}_gmode="1"
		fi

		# set bss_opmode_cap_reqd
		if [ "$hwmode" = "11n" -o "$hwmode" = "11nac" ]; then
			nvram set ${HOME_WIFI}_bss_opmode_cap_reqd="2"
		elif [ "$hwmode" = "11ac" ]; then
			nvram set ${HOME_WIFI}_bss_opmode_cap_reqd="3"
		else
			nvram set ${HOME_WIFI}_bss_opmode_cap_reqd="0"
		fi

		# set bw_cap
		if [ "$htmode" = "auto" ]; then
			if [ "$hwmode" = "11ac" -o "$hwmode" = "11anac" -o "$hwmode" = "11nac" ]; then
				nvram set ${HOME_WIFI}_bw_cap="7"
			else
				nvram set ${HOME_WIFI}_bw_cap="3"
			fi
		elif [ "$htmode" = "80" ]; then
			nvram set ${HOME_WIFI}_bw_cap="7"
		elif [ "$htmode" = "40" ]; then
			nvram set ${HOME_WIFI}_bw_cap="3"
		else
			nvram set ${HOME_WIFI}_bw_cap="1"
		fi

		# set obss_coex
		if [ "$htmode" = "auto" ]; then
			nvram set ${HOME_WIFI}_obss_coex="1"
		else
			nvram set ${HOME_WIFI}_obss_coex="0"
		fi

		# set chanspec (wlanChspec)
		wifi_chanspec_config chanspec $htmode $channel
		if [ "$band" = "5g" -a "$wds_enable" = "1" ]; then
			nvram set ${HOME_WIFI}_chanspec="0"
		else
			nvram set ${HOME_WIFI}_chanspec="$chanspec"
		fi

		# set radio
		nvram set ${HOME_WIFI}_radio="1"

		config_get mode $vif mode
		nvram set ${HOME_WIFI}_mode="$mode"

		config_get_bool hidden $vif hidden
		if [ "$hidden" = "1" ]; then
			nvram set ${HOME_WIFI}_closed="1"
		else
			nvram set ${HOME_WIFI}_closed="0"
		fi
	else
		nvram set ${HOME_WIFI}_radio="0"
	fi
}

wifi_security_config() {
	local dev="$1"
	local close11N
	local wds_enable="0"
	local eth_enable="0"
	local home_vif=""
	local wds_vif=""
	local vif=""
	local WIFI_PRE=""
	echo "=====>>>>> $dev: wifi_security_config" >$STDOUT

	config_get_bool wifi_disabled $dev disabled
	if [ "$wifi_disabled" = "0" ]; then
		config_get vifs $dev vifs
		for vif in $vifs; do # vifs is wl01/wl02/wl03, home/guest/wds
			config_get_bool enable $vif enable
			config_get mode $vif mode
			config_get guest $vif guest
			if [ "$enable" = "1" -a "$mode" = "ap" -a -z "$guest" ]; then
				eth_enable="1"
				home_vif="$vif"
				echo "=====>>>>> $dev: HOME WIFI is on" >$STDOUT
			elif [ "$enable" = "1" -a "$mode" = "sta" ]; then
				eth_enable="1"
				wds_enable="1"
				wds_vif="$vif"
				echo "=====>>>>> $dev: WDS is on" >$STDOUT
			else
				echo "=====>>>>> $dev: vif $vif is disabled or $vif is a guest network" >$STDOUT
			fi
		done
	fi

	config_get band $dev band
	case $band in
		2g)
			if [ "$wds_enable" = "1" ]; then
				vif="$home_vif"
				WIFI_PRE="wl1.1"
			else
				vif="$home_vif"
				WIFI_PRE="wl1"
			fi
		;;
		5g)
			if [ "$wds_enable" = "1" ]; then
				vif="$home_vif"
				WIFI_PRE="wl0.1"
			else
				vif="$home_vif"
				WIFI_PRE="wl0"
			fi
		;;
	esac

	if [ "$eth_enable" = "1" ]; then
		config_get mode $vif mode
		config_get encryption $vif encryption
		config_get psk_version $vif psk_version
		config_get psk_cipher $vif psk_cipher
		config_get wpa_version $vif wpa_version
		config_get wpa_cipher $vif wpa_cipher
		echo "=====>>>>> $dev: $vif, encryption $encryption, psk_version $psk_version, psk_cipher $psk_cipher" >$STDOUT
		close11N="0"

		if [ -z "$WIFI_PRE" ]; then
			echo "=====>>>>> $dev: dont set security for vif $vif" >$STDOUT
		else
			if [ "$encryption" == "none" ]; then
				nvram set ${WIFI_PRE}_wep="disabled"
				nvram set ${WIFI_PRE}_auth="0"
				nvram set ${WIFI_PRE}_akm=""
			elif [ "$encryption" == "psk" ]; then
				if [ "$psk_version" == "wpa" ]; then
					nvram set ${WIFI_PRE}_akm="psk"
				elif [ "$psk_version" == "rsn" ]; then
					nvram set ${WIFI_PRE}_akm="psk2"
				else
					nvram set ${WIFI_PRE}_akm="psk psk2"
				fi
				if [ "$psk_cipher" == "aes" ]; then
					nvram set ${WIFI_PRE}_crypto="aes"
				elif [ "$psk_cipher" == "tkip" ]; then
					nvram set ${WIFI_PRE}_crypto="tkip"
					close11N="1"
				else
					nvram set ${WIFI_PRE}_crypto="tkip+aes"
				fi

				config_get psk_key $vif psk_key
				nvram set ${WIFI_PRE}_wpa_psk="$psk_key"
				config_get wpa_group_rekey $dev wpa_group_rekey
				if [ -z "$wpa_group_rekey" ]; then
					nvram set ${WIFI_PRE}_wpa_gtk_rekey="0"
				else
					nvram set ${WIFI_PRE}_wpa_gtk_rekey="$wpa_group_rekey"
				fi
				nvram set ${WIFI_PRE}_wep="disabled"
				nvram set ${WIFI_PRE}_auth="0"
			elif [ "$encryption" == "wpa" ]; then
				if [ "$wpa_version" == "wpa" ]; then
					nvram set ${WIFI_PRE}_akm="wpa"
				elif [ "$wpa_version" == "rsn" ]; then
					nvram set ${WIFI_PRE}_akm="wpa2"
				else
					nvram set ${WIFI_PRE}_akm="wpa wpa2"
				fi
				if [ "$wpa_cipher" == "aes" ]; then
					nvram set ${WIFI_PRE}_crypto="aes"
				elif [ "$wpa_cipher" == "tkip" ]; then
					nvram set ${WIFI_PRE}_crypto="tkip"
					close11N="1"
				else
					nvram set ${WIFI_PRE}_crypto="tkip+aes"
				fi

				config_get server $vif server
				nvram set ${WIFI_PRE}_radius_ipaddr="$server"
				config_get port $vif port
				nvram set ${WIFI_PRE}_radius_port="$port"
				config_get wpa_key $vif wpa_key
				nvram set ${WIFI_PRE}_radius_key="$wpa_key"
				config_get wpa_group_rekey $vif wpa_group_rekey
				if [ -z "$wpa_group_rekey" ]; then
					nvram set ${WIFI_PRE}_wpa_gtk_rekey="0"
				else
					nvram set ${WIFI_PRE}_wpa_gtk_rekey="$wpa_group_rekey"
				fi
				nvram set ${WIFI_PRE}_wep="disabled"
				nvram set ${WIFI_PRE}_auth="0"
			elif [ "$encryption" == "wep" ]; then
				nvram set ${WIFI_PRE}_wep="enabled"
				config_get wep_mode $vif wep_mode
				if [ "$wep_mode" == "shared" ]; then
					nvram set ${WIFI_PRE}_auth="1"
				else
					nvram set ${WIFI_PRE}_auth="0"
				fi

				config_get wep_key1 $vif wep_key1
				nvram set ${WIFI_PRE}_key1="$wep_key1"
				nvram set ${WIFI_PRE}_key="1"
				nvram set ${WIFI_PRE}_akm=""
				close11N="1"
			else
				echo "ERROR: the encryption type error" >$CONSOLE
			fi
			if [ "$close11N" == "1" ]; then
				nvram set ${WIFI_PRE}_nmode="0"
				config_get channel $dev channel
				if [ "$channel" = "auto" ]; then
					nvram set ${WIFI_PRE}_chanspec="0"
				else
					nvram set ${WIFI_PRE}_chanspec="$channel"
				fi
			fi
		fi
	fi
}

wifi_macfilter_config() {
	local dev=$1
	local dynamic=$2
	echo "=====>>>>> $dev: wifi_macfilter_config" >$STDOUT
	#record home guest and bridge status 
	local home_enable guest_enable bridge_enable
	local home_vif guest_vif
	local home_ifname guest_ifname

	config_get band $dev band
	config_get vifs $dev vifs

	case $band in
		2g)
			WIFI_UNIT="1"
		;;
		5g)
			WIFI_UNIT="0"
		;;
	esac

	case $MACFILTER_ACTION in
	allow)
		macmode="2"
	;;
	deny)
		macmode="1"
	;;
	*)
		macmode="0"
		;;
	esac

	config_get_bool wifi_disabled $dev disabled
	if [ "$wifi_disabled" = "0" ]; then
		for vif in $vifs; do
			config_get_bool enable  $vif enable
			config_get      mode    $vif mode
			config_get      guest   $vif guest
			config_get      ifname  $vif ifname

			if [ "$mode" = "ap" ] && [ -z "$guest" ]; then
				home_enable=$enable
				home_vif=$vif
				home_ifname=$ifname
			elif [ "$mode" = "ap" ] && [ ! -z "$guest" ]; then
				guest_enable=$enable
				guest_vif=$vif
				guest_ifname=$ifname
			elif [ "$mode" = "sta" ] ; then
				bridge_enable=$enable
			else
				echo "bad if type." >$STDOUT ;
			fi      
		done
	fi
	
	#echo home=$home_enable guest=$guest_enable bridge=$bridge_enable >$STDOUT ;
	#config home acl
	if [ "$home_enable" = "1" -a "$bridge_enable" = "1" ]; then
		HOME_WIFI="wl"${WIFI_UNIT}".1"
	else
		HOME_WIFI="wl"${WIFI_UNIT}
	fi 

	if [ "$guest_enable" = "1" ]; then
		GUEST_WIFI="wl"${WIFI_UNIT}".1"
	fi

	if [ "$MACFILTER_ENABLE" = "on" ]; then
		if [ "$MACFILTER_ACTION" = "allow" -o "$MACFILTER_ACTION" = "deny" ]; then
			nvram set "${HOME_WIFI}_macmode=$MACFILTER_ACTION"
			[ -z "$dynamic" ] || wl -i $home_ifname macmode "$macmode"
			if [ "$guest_enable" == 1 ]; then 
				nvram set "${GUEST_WIFI}_macmode=$MACFILTER_ACTION"
				[ -z "$dynamic" ] || wl -i $guest_ifname macmode "$macmode"
			fi
		else
			echo "bad MACFILTER_ACTION." >$STDOUT ;
		fi
	elif [ "$MACFILTER_ENABLE" = "off" ]; then
		nvram set "${HOME_WIFI}_macmode=disabled"
		[ -z "$dynamic" ] || wl -i $home_ifname macmode 0
		if [ "$guest_enable" = 1 ] ; then 
			nvram set "${GUEST_WIFI}_macmode=disabled"
			[ -z "$dynamic" ] || wl -i $guest_ifname macmode 0
		fi
	else
		echo "bad MACFILTER_ENABLE." >$STDOUT ;
	fi

	if [ "$MACFILTER_ENABLE" = "on" ]; then
		nvram set "${HOME_WIFI}_maclist=${MAC_LIST}"
		if [ "$home_enable" = 1 ] ; then 
			[ -z "$dynamic" ] || {
				wl -i $home_ifname mac none
				wl -i $home_ifname mac ${MAC_LIST}
				kick_sta $home_ifname
			}
		fi
		if [ "$guest_enable" = 1 ] ; then 
				nvram set "${GUEST_WIFI}_maclist=${MAC_LIST}"
				[ -z "$dynamic" ] || {
					wl -i $guest_ifname mac none
					wl -i $guest_ifname mac ${MAC_LIST}
					kick_sta $guest_ifname
				}
		fi
	fi
}

# set advanced config for home wifi
wifi_advanced_config() {
	local dev="$1"
	local wds_enable="0"
	local eth_enable="0"
	local home_vif=""
	local wds_vif=""
	local vif=""
	echo "=====>>>>> $dev: wifi_advanced_config" >$STDOUT

	config_get band $dev band
	config_get_bool wifi_disabled $dev disabled
	if [ "$wifi_disabled" = "0" ]; then
		config_get vifs $dev vifs
		for vif in $vifs; do # vifs is wl01/wl02/wl03, home/guest/wds
			config_get_bool enable $vif enable
			config_get mode $vif mode
			config_get guest $vif guest
			if [ "$enable" = "1" -a "$mode" = "ap" -a -z "$guest" ]; then
				eth_enable="1"
				home_vif="$vif"
				echo "=====>>>>> $dev: HOME WIFI is on" >$STDOUT
			elif [ "$enable" = "1" -a "$mode" = "sta" ]; then
				eth_enable="1"
				wds_enable="1"
				wds_vif="$vif"
				echo "=====>>>>> $dev: WDS is on" >$STDOUT
			else
				echo "=====>>>>> $dev: vif $vif is disabled or $vif is a guest network" >$STDOUT
			fi
		done
	fi

	case $band in
		2g)
			if [ "$wds_enable" = "1" ]; then
				vif="$home_vif"
				HOME_WIFI="wl1"
			else
				vif="$home_vif"
				HOME_WIFI="wl1"
			fi
		;;
		5g)
			if [ "$wds_enable" = "1" ]; then
				vif="$home_vif"
				HOME_WIFI="wl0"
			else
				vif="$home_vif"
				HOME_WIFI="wl0"
			fi
		;;
	esac

	if [ "$eth_enable" = "1" ]; then
		config_get beacon_int $dev beacon_int
		config_get rts $dev rts
		config_get frag $dev frag
		config_get dtim_period $dev dtim_period
		config_get wmm $dev wmm
		config_get_bool isolate $dev isolate
		config_get_bool shortgi $dev shortgi
		if [ "$shortgi" = "1" ]; then
			shortgiStr="short"
		else
			shortgiStr="long"
		fi

		nvram set ${HOME_WIFI}_bcn="$beacon_int"
		nvram set ${HOME_WIFI}_rts="$rts"
		nvram set ${HOME_WIFI}_frag="$frag"
		nvram set ${HOME_WIFI}_dtim="$dtim_period"
		nvram set ${HOME_WIFI}_wme="$wmm" #need to check
		nvram set ${HOME_WIFI}_plcphdr="$shortgiStr"
		nvram set ${HOME_WIFI}_ap_isolate="$isolate"

		# disable txbf
		nvram set ${HOME_WIFI}_txbf_imp="0"
		nvram set ${HOME_WIFI}_txbf_bfr_cap="0"
		nvram set ${HOME_WIFI}_txbf_bfe_cap="0"
	fi
}

wifi_wps_config() {
	local dev=$1
	local wdsIf
	local wpsIf
	echo "=====>>>>> $dev: wifi_wps_config" >$STDOUT

	config_get band $dev band
	case $band in
		2g)
			HOME_WIFI=wl1
		;;
		5g)
			HOME_WIFI=wl0
		;;
	esac

	for vif in $vifs; do
		config_get mode  $vif mode
		#find the wds iface
		if [ -n "$mode" -a "sta" = "$mode" ]; then
			wdsIf=$vif
			break
		fi
	done

	for vif in $vifs; do
		config_get mode  $vif mode
		config_get guest $vif guest

		#find the wps iface
		if [ "ap" = "$mode" -a -z "$guest" ]; then
			wpsIf=$vif
			break
		fi
	done

	config_get_bool wifi_disabled $dev disabled
	config_get_bool disabled_all $dev disabled_all
	config_get_bool wds_enable $wdsIf enable

	if [ "$wifi_disabled" = "0" -a "$disabled_all" = "0" -a "$wds_enable" = "1" ]; then
		nvram set ${HOME_WIFI}_wps_mode="disabled"
		case $band in
			2g)
				HOME_WIFI=wl1.1
			;;
			5g)
				HOME_WIFI=wl0.1
			;;
		esac
	fi

	config_get_bool wps $wpsIf wps
	config_get encryption $wpsIf encryption
	config_get_bool hidden $wpsIf hidden
	config_get psk_version $wpsIf psk_version
	config_get psk_cipher $wpsIf psk_cipher
	if [ "$wps" = "1" ]; then
		if [ "$encryption" = "wpa" -o "$hidden" = "1" -o "$encryption" = "wep" ]; then
			nvram set ${HOME_WIFI}_wps_mode="disabled"
		elif [ "$encryption" = "psk" -a "$psk_version" = "wpa" ]; then
			nvram set ${HOME_WIFI}_wps_mode="disabled"
		elif [ "$encryption" = "psk" -a "$psk_cipher" = "tkip" ]; then
			nvram set ${HOME_WIFI}_wps_mode="disabled"
		else
			nvram set ${HOME_WIFI}_wps_mode="enabled"
		fi
	else
		nvram set ${HOME_WIFI}_wps_mode="disabled"
	fi

	config_get_bool wps_label $wpsIf wps_label
	if [ "$wps_label" = "1" ]; then
		nvram set wps_aplockdown_forceon="0"
	else
		nvram set wps_aplockdown_forceon="1"
	fi

	config_get wps_pin $wpsIf wps_pin
	nvram set wps_device_pin="$wps_pin"

	config_get dev_name wps wps_device_name
	nvram set wps_device_name="$dev_name"

	config_get model_name wps model_name
	nvram set wps_modelname="$model_name"

	config_get manufacturer wps wps_manufacturer
	nvram set wps_mfstring="$manufacturer"

	nvram set wps_modelnum="123456"

	nvram set lan_wps_oob="disabled"
}

wifi_wds_config() {
	local dev="$1"
	config_get vifs $dev vifs
	local eth_enable="0"
	local guest_enable="0"
	local wds_enable="0"
	local wds_vif=""
	local br0_ifnames_tmp=""
	local lan_ifnames_tmp=""
	local vifs_tmp=""
	echo "=====>>>>> $dev: wifi_wds_config" >$STDOUT

	config_get_bool wifi_disabled $dev disabled
	if [ "$wifi_disabled" = "0" ]; then
		config_get vifs $dev vifs
		for vif in $vifs; do
			config_get_bool enable $vif enable
			config_get mode $vif mode
			config_get guest $vif guest
			if [ "$enable" = "1" -a "$mode" = "ap" -a -z "$guest" ]; then
				eth_enable="1"
			elif [ "$enable" = "1" -a "$mode" = "ap" -a "$guest" = "on" ]; then
				eth_enable="1"
				guest_enable="1"
			elif [ "$enable" = "1" -a "$mode" = "sta" ]; then
				eth_enable="1"
				wds_enable="1"
				wds_vif="$vif"
				#when wds is on, we need to re-insmod wl.ko
				remove_ko="1" 
			else
				echo "=====>>>>> $dev: vif $vif is disabled" >$STDOUT
			fi
		done
	fi

	config_get band $dev band
	case $band in
		2g)
			WIFI_PRE="wl1"
			WIFI_UNIT="1.1"
		;;
		5g)
			WIFI_PRE="wl0"
			WIFI_UNIT="0.1"
		;;
	esac
	
	if [ "$eth_enable" = "1" -a "$wds_enable" = "1" ]; then
		config_get ssid $wds_vif ssid
		nvram set ${WIFI_PRE}_ssid="$ssid"
		
		config_get encryption $wds_vif encryption
		if [ "$encryption" = "wep" ]; then
			config_get wep_format1 $wds_vif wep_format1
			if [ "$wep_format1" = "asic" -o "$wep_format1" = "hex" ]; then
				nvram set ${WIFI_PRE}_wep="enabled"
				config_get wep_mode $wds_vif wep_mode
				if [ "$wep_mode" = "shared" ]; then
					nvram set ${WIFI_PRE}_auth="1"
				else
					nvram set ${WIFI_PRE}_auth="0"
				fi
				nvram set ${WIFI_PRE}_key="1"
				config_get wep_key1 $wds_vif wep_key1
				nvram set ${WIFI_PRE}_key1="$wep_key1"
				nvram set ${WIFI_PRE}_key2=""
				nvram set ${WIFI_PRE}_key3=""
				nvram set ${WIFI_PRE}_key4=""
				nvram set ${WIFI_PRE}_akm=""
				nvram set ${WIFI_PRE}_nmode="0"
			fi
		elif [ "$encryption" = "psk" ]; then
			nvram set ${WIFI_PRE}_akm="psk psk2"
			config_get psk_key $wds_vif psk_key
			nvram set ${WIFI_PRE}_wpa_psk="$psk_key"
			nvram set ${WIFI_PRE}_wep="disabled"
			nvram set ${WIFI_PRE}_auth="0"
		elif [ "$encryption" = "none" ]; then
			nvram set ${WIFI_PRE}_akm=""
			nvram set ${WIFI_PRE}_wep="disabled"
			nvram set ${WIFI_PRE}_auth="0"
		else
			echo "ERROR: $dev, unknown wds security" >$CONSOLE
		fi

		nvram set ${WIFI_PRE}_ure="0"
		nvram set ${WIFI_PRE}_mode="psr"
		nvram set ${WIFI_PRE}_vifs="${WIFI_PRE}.1"
		nvram set ${WIFI_PRE}_wps_oob="disabled"
		nvram set ${WIFI_PRE}_ap_isolate="0"
		br0_ifnames_tmp="`nvram get br0_ifnames`"
		echo ${br0_ifnames_tmp} | grep -q ${WIFI_PRE}.1 || nvram set br0_ifnames="$br0_ifnames_tmp ${WIFI_PRE}.1"
		lan_ifnames_tmp="`nvram get lan_ifnames`"
		echo ${lan_ifnames_tmp} | grep -q ${WIFI_PRE}.1 || nvram set lan_ifnames="$lan_ifnames_tmp ${WIFI_PRE}.1"

		for idx in `seq 2 15`; do
			nvram set ${WIFI_PRE}.${idx}_bss_enabled="0"
		done

		vap_name="${WIFI_PRE}.1"
		if [ "$band" = "5g" ]; then
			home_vif="$HOME_5G_VIF"
		else
			home_vif="$HOME_2G_VIF"
		fi
		config_get ssid $home_vif ssid
		nvram set ${vap_name}_ssid="$ssid"
		nvram set ${vap_name}_mode="ap"
		nvram set ${vap_name}_radio="1"
		nvram set ${vap_name}_closed="0"
		config_get_bool ap_isolate $dev isolate 
		nvram set ${vap_name}_ap_isolate="$ap_isolate"
		nvram set ${vap_name}_bss_enabled="1"
		nvram set ${vap_name}_sta_retry_time="5"
		nvram set ${vap_name}_infra="1"
		nvram set ${vap_name}_unit="${WIFI_UNIT}"
		nvram set ${vap_name}_ifname="${vap_name}"
		nvram set ${vap_name}_bss_maxassoc="64"
		nvram set ${vap_name}_wmf_bss_enable="1"
	else
		vap_name="${WIFI_PRE}.1"
		nvram set ${vap_name}_radio="0"
		nvram set ${vap_name}_bss_enabled="0"
		nvram unset ${vap_name}_unit
		nvram unset ${vap_name}_ifname
		for idx in `seq 1 15`; do
			nvram set ${WIFI_PRE}.${idx}_hwaddr=""
		done

		br0_ifnames_tmp="`nvram get br0_ifnames`"
		echo ${br0_ifnames_tmp} | grep -q ${vap_name} && br0_ifnames_tmp="${br0_ifnames_tmp/ ${vap_name}/""}"
		nvram set br0_ifnames="$br0_ifnames_tmp"
		lan_ifnames_tmp="`nvram get lan_ifnames`"
		echo ${lan_ifnames_tmp} | grep -q ${vap_name} && lan_ifnames_tmp="${lan_ifnames_tmp/ ${vap_name}/""}"
		nvram set lan_ifnames="$lan_ifnames_tmp"
		vifs_tmp="`nvram get ${WIFI_PRE}_vifs`"
		echo ${vifs_tmp} | grep -q "${WIFI_PRE}.1" && nvram set ${WIFI_PRE}_vifs=""
	fi
}

wifi_guest_config() {
	local dev="$1"
	local close11N
	local vif=""
	local eth_enable="0"
	local guest_enable="0"
	local wds_enable="0"
	local guest_vif=""
	echo "=====>>>>> $dev: wifi_guest_config" >$STDOUT

	config_get_bool wifi_disabled $dev disabled
	if [ "$wifi_disabled" = "0" ]; then
		config_get vifs $dev vifs
		for vif in $vifs; do
			config_get_bool enable $vif enable
			config_get mode $vif mode
			config_get guest $vif guest
			if [ "$enable" = "1" -a "$mode" = "ap" -a -z "$guest" ]; then
				eth_enable="1"
			elif [ "$enable" = "1" -a "$mode" = "ap" -a "$guest" = "on" ]; then
				eth_enable="1"
				guest_enable="1"
				guest_vif="$vif"
			elif [ "$enable" = "1" -a "$mode" = "sta" ]; then
				eth_enable="1"
				wds_enable="1"
			else
				echo "=====>>>>> $dev: vif $vif is disabled" >$STDOUT
			fi
		done
	fi

	config_get band $dev band
	case $band in
		2g)
			GUEST_WIFI="wl1.1"
			WIFI_DEV="wl1"
			WIFI_UNIT="1.1"
		;;
		5g)
			GUEST_WIFI="wl0.1"
			WIFI_DEV="wl0"
			WIFI_UNIT="0.1"
		;;
	esac

	if [ "$eth_enable" = "1" -a "$guest_enable" = "1" ]; then
		vif="$guest_vif"
		config_get ssid $vif ssid
		config_get encryption $vif encryption
		config_get wds $vif wds
		config_get psk_version $vif psk_version
		config_get psk_cipher $vif psk_cipher
		nvram set ${GUEST_WIFI}_ssid="$ssid"
		nvram set ${GUEST_WIFI}_radio="1"

		config_get_bool hidden $vif hidden
		if [ "$hidden" == "1" ]; then
			nvram set ${GUEST_WIFI}_closed="1"
		else
			nvram set ${GUEST_WIFI}_closed="0"
		fi
		nvram set ${GUEST_WIFI}_bss_maxassoc="64" #need to check guestNet num

		if [ "$encryption" == "none" ]; then
			nvram set ${GUEST_WIFI}_wep="disabled"
			nvram set ${GUEST_WIFI}_auth="0"
			nvram set ${GUEST_WIFI}_akm=""
		elif [ "$encryption" == "psk" ]; then
			if [ "$psk_version" == "wpa" ]; then
				nvram set ${GUEST_WIFI}_akm="psk"
			elif [ "$psk_version" == "rsn" ]; then
				nvram set ${GUEST_WIFI}_akm="psk2"
			else
				nvram set ${GUEST_WIFI}_akm="psk psk2"
			fi
			if [ "$psk_cipher" == "aes" ]; then
				nvram set ${GUEST_WIFI}_crypto="aes"
			elif [ "$psk_cipher" == "tkip" ]; then
				nvram set ${GUEST_WIFI}_crypto="tkip"
				nvram set ${WIFI_DEV}_nmode="0"
				config_get channel $dev channel
				if [ "$channel" = "auto" ]; then
					nvram set ${WIFI_DEV}_chanspec="0"
				else
					nvram set ${WIFI_DEV}_chanspec="$channel"
				fi
			else
				nvram set ${GUEST_WIFI}_crypto="tkip+aes"
			fi

			config_get psk_key $vif psk_key
			nvram set ${GUEST_WIFI}_wpa_psk="$psk_key"
			config_get wpa_group_rekey $dev wpa_group_rekey
			if [ -z "$wpa_group_rekey" ]; then
				nvram set ${GUEST_WIFI}_wpa_gtk_rekey="0"
			else
				nvram set ${GUEST_WIFI}_wpa_gtk_rekey="$wpa_group_rekey"
			fi
			nvram set ${GUEST_WIFI}_wep="disabled"
			nvram set ${GUEST_WIFI}_auth="0"
		else
			nvram set ${GUEST_WIFI}_wep="disabled"
			nvram set ${GUEST_WIFI}_auth="0"
			nvram set ${GUEST_WIFI}_akm=""
			echo "ERROR: $dev, encryption is not correct" >$CONSOLE
		fi
		config_get_bool isolate $vif isolate
		nvram set ${GUEST_WIFI}_ap_isolate="$isolate"
		nvram set ${GUEST_WIFI}_wme="on"
		nvram set ${GUEST_WIFI}_bss_enabled="1"
		nvram set ${GUEST_WIFI}_mode="ap"
		nvram set ${GUEST_WIFI}_infra="1"
		nvram set ${GUEST_WIFI}_unit="$WIFI_UNIT"
		nvram set ${GUEST_WIFI}_ifname="${GUEST_WIFI}"
		nvram set ${GUEST_WIFI}_wps_mode="disabled"
		nvram set ${GUEST_WIFI}_sta_retry_time="5"
		nvram set ${GUEST_WIFI}_wmf_bss_enable="1"
		nvram set ${WIFI_DEV}_vifs="${GUEST_WIFI}"
		br0_ifnames_tmp="`nvram get br0_ifnames`"
		echo ${br0_ifnames_tmp} | grep -q ${GUEST_WIFI} || nvram set br0_ifnames="${br0_ifnames_tmp} ${GUEST_WIFI}"
		lan_ifnames_tmp="`nvram get lan_ifnames`"
		echo ${lan_ifnames_tmp} | grep -q ${GUEST_WIFI} || nvram set lan_ifnames="${lan_ifnames_tmp} ${GUEST_WIFI}"
	elif [ "$eth_enable" = "1" -a "$wds_enable" = "0" ]; then
		nvram set ${GUEST_WIFI}_radio="0"
		nvram set ${GUEST_WIFI}_bss_enabled="0"
		nvram set ${GUEST_WIFI}_wps_mode="disabled"
		br0_ifnames_tmp="`nvram get br0_ifnames`"
		echo ${br0_ifnames_tmp} | grep -q ${GUEST_WIFI} && br0_ifnames_tmp="${br0_ifnames_tmp/ ${GUEST_WIFI}/""}"
		nvram set br0_ifnames="$br0_ifnames_tmp"
		lan_ifnames_tmp="`nvram get lan_ifnames`"
		echo ${lan_ifnames_tmp} | grep -q ${GUEST_WIFI} && lan_ifnames_tmp="${lan_ifnames_tmp/ ${GUEST_WIFI}/""}"
		nvram set lan_ifnames="$lan_ifnames_tmp"
	else
		echo "=====>>>>> $dev: the $dev is off or the guest is off" >$STDOUT
	fi
}

start_eapd(){
	eapd 
}

start_nas(){
	nas 
}

start_wps(){
	wps_restart=`nvram get wps_restart`
	if [ "$wps_restart" == "1" ]; then
		nvram set "wps_restart=0"
	else
		nvram set "wps_restart=0"
		nvram set "wps_proc_status=0"
	fi

	nvram set "wps_sta_pin=00000000"
	killall wps_monitor
	wps_monitor &
}

start_acsd(){
	acsd 
}

stop_eapd(){
	killall eapd
}

stop_nas(){
	killall nas
}

stop_wps(){
	killall wps_monitor
	#wps_gpio_set 0
	echo 0 > /sys/class/leds/blue:wps/brightness
}

stop_acsd(){
	killall acsd
}

start_service(){
	start_eapd
	start_nas
	start_wps
	start_acsd
}

stop_service(){
	stop_wps
	stop_nas
	stop_eapd
	stop_acsd
}

get_if_var(){
	#get interface and status
	for dev in ${1:-$DEVICES}; do
		config_get_bool wifi_disabled $dev disabled
		config_get vifs "$dev" vifs
		config_get band "$dev" band
		config_get mac_$band "$dev"  macaddr
		for vif in $vifs; do 
			config_get ifname "$vif" ifname
			config_get enable "$vif" enable
			config_get guest  "$vif" guest
			config_get mode   "$vif" mode

			if [ "$mode" = "ap" ] && [ -z "$guest" ]; then
				eval "vap0_$band"="$ifname"
				eval "home_$band"="$enable"
				eval "wifi_disabled_$band"="$wifi_disabled"
			elif [ "$mode" = "ap" ] && [ ! -z "$guest" ]; then
				eval "vap1_$band"="$ifname"
				eval "guest_$band"="$enable"
			elif [ "$mode" = "sta" ] ; then
				eval "bridge_$band"="$enable"
			else
				echo "ERROR: bad if type." >$CONSOLE
			fi
		done
	done
}

wifi_start_calibrate(){
	local app_name="rftest"
	local brname
	local host_ip
	
	cd /tmp/
	# get bridge name
	get_brname brname
	
	#tftp host ip is as same as bridge ip except last part, such as br_ip=192.168.0.1 then host_ip=192.168.0.100
	host_ip=`ifconfig $brname | grep -o 'inet addr:[^ ]*' | grep -o '[^:]*$' | sed -n 's/\(^[^\.]*\.[^\.]*\.[^\.]*\)\..*$/\1\.100/p'`
	echo "INFO:TFTP FROM HOST $host_ip" >$STDOUT
	tftp -gr "$app_name" "$host_ip"
	tftp_status=$?
	sleep 1
	echo "tftp_status=$tftp_status" >$STDOUT
	while [ $tftp_status -ne 0 ]
	do
		echo "WARNING:TFTP $app_name FROM PC ERROR!" >$CONSOLE
		rm -rf "$app_name"
		tftp -gr "$app_name" "$host_ip"
		tftp_status=$?
		sleep 1
		echo "tftp_status=$tftp_status" >$STDOUT
	done
	chmod u+x "$app_name"
	./"$app_name"
}

wifi_driver_startup(){
	echo "=====>>>>> wifi_driver_startup" >$STDOUT
	# insmod wl.ko if not exist
	local module_name=wl
	grep -q '^'$module_name /proc/modules || insmod $module_name 
	#grep -q '^'$module_name /proc/modules || echo "insmod wl.ko failed." >$STDOUT ;

	# get bridge name
	get_brname brname

	# get if and status
	get_if_var
	nvram unset acs_ifnames
	for band in "2g" "5g"; do
		eval local vap0=\${vap0_$band} vap1=\${vap1_$band}
		eval local home=\${home_$band} guest=\${guest_$band} bridge=\${bridge_$band}
		eval local mac=\${mac_$band}
		eval local wifi_disabled=\${wifi_disabled_$band}
		#echo "DEBUG:" $band $vap0 $vap1 $home $guest $bridge >$STDOUT
		if [ "$home" = "on" -a "$wifi_disabled" = "0" ]; then 
			#if [ "$band" = "2g" ]; then
				# set vht feature for 2g
			#	wl -i "$vap0" vht_features 0x3 # let 2g support 256QAM
			#fi

			config_get hwmode $vap0 hwmode
			if [ "$band" = "2g" ]; then
				if [ "$hwmode" = "11g" -o "$hwmode" = "11bg" ]; then
					wl -i "$vap0" vhtmode 0 # 256QAM is off, default is on(1)
				fi
			else
				if [ "$hwmode" = "11a" -o "$hwmode" = "11n" -o "$hwmode" = "11an" ]; then
					wl -i "$vap0" vhtmode 0
				fi
			fi

			brctl addif "$brname" "$vap0"
			ifconfig "$vap0" up
			wlconf "$vap0" up

			# wds bridge
			if [ "$bridge" = "on" ]; then 
				#format xx:xx:xx:xx:xx:xx 
				ifconfig $vap1 hw ether ${mac//-/:}
				brctl addif "$brname" $vap1
				ifconfig "$vap1" up
				# enable recv all multicast traffic
				wl -i "$vap0" allmulti 1
			fi

			# guest network
			if [ "$guest" = "on" ]; then 
				ifconfig $vap1 hw ether $(fix_vif_mac $mac "init")
				brctl addif "$brname" "$vap1"
				ifconfig "$vap1" up  
				#TODO: set guest network in firewall
			fi

			config_get txpower $vap0 txpower
			if [ "$txpower" = "high" ]; then
				wl -i $vap0 pwr_percent 100
			elif [ "$txpower" = "middle" ]; then
				wl -i $vap0 pwr_percent 80
			else
				wl -i $vap0 pwr_percent 60
			fi

			config_get_bool shortgi $vap0 shortgi
			if [ "$shortgi" = "1" ]; then
				wl -i $vap0 sgi_tx -1
			else
				wl -i $vap0 sgi_tx 0
			fi

			if [ "$band" = "5g" ]; then
				wl -i "$vap0" down
				wl -i "$vap0" spect 1
				wl -i "$vap0" up
			fi

		fi
	done

	#start programs
	start_service 

	for band in "2g" "5g"; do
		eval local vap0=\${vap0_$band}
		eval local home=\${home_$band}
		if [ "$home" = "on" -a "$wifi_disabled" = "0" ]; then 
			wlconf "$vap0" start
		fi
	done

	wifi_vlan notaddif
}

wifi_driver_stop(){
	echo "=====>>>>> wifi_driverStop" >$STDOUT

	# stop programs
	stop_service

	# get bridge name
	get_brname brname

	# get if and status
	get_if_var

	# br-lan down
	ifconfig "$brname" down

	for band in "2g" "5g"; do
		eval local vap0=\${vap0_$band} vap1=\${vap1_$band}
		eval local home=\${home_$band} guest=\${guest_$band} bridge=\${bridge_$band}
		eval local mac=\${mac_$band}

		for ifd in "$vap0" "$vap1"; do 
			wlconf "$ifd" down
			ifconfig "$ifd" down
			brctl  delif "$brname" "$ifd" 
		done
		
		#TODO set only for wds ?
		ifconfig "$vap1" hw ether $(fix_vif_mac $mac "stop")
	done

	#remove wl.ko
	if [ "$remove_ko" = "1" ];then
		local module_name=wl
		grep -q '^'$module_name /proc/modules && rmmod $module_name 
	fi
	# br-lan up
	ifconfig "$brname" up
	/etc/init.d/imb restart
	ubus call network.interface.lan static_routes_recovery
}

# fix_guest_mac $1=xx-xx-xx-xx-xx-xx $2=init/stop
fix_vif_mac(){
	idx=0
	if [ -z $2 ];then
		echo $1
		return;
	fi
	for m in ${1//-/ }
	do 
		m=0x$m
		if [ $idx = 0 ]; then
			 if [ $2 = "init" ]; then
				 a=$(($m & 0xe3))
				 b=$(($m & 0x1c))
				 c=$(( $((1<<2)) ^ $b ))
				 m=$(($a | $c))
				 m=$(($m | 0x2))
			 elif [ $2 = "stop" ]; then
				 m=$(($m | 0x2))
			 fi
		elif [ $idx = 5 ]; then
			if [ $2 = "init" ]; then
				a=$(($m & 0xf0))
				b=$(($m + 1))
				c=$((0xf & $b))
				m=$(($a | $c))
			fi
		fi
		printf "%02x" $m
		if [ ! $idx = 5 ]; then
			printf ":"
		fi
		idx=$((idx+1))
	done 
}

wifi_nvram_config() {
	for dev in ${DEVICES}; do  # eth3 eth2
		wifi_basic_config $dev
		wifi_security_config $dev
		wifi_macfilter_config $dev
		wifi_advanced_config $dev
		wifi_wps_config $dev
		wifi_wds_config $dev
		wifi_guest_config $dev
	done
}

wifi_led_set() {
	for dev in ${DEVICES}; do
		config_get disabled $dev disabled
		config_get disabled_all $dev disabled_all
		config_get band $dev band
		local postfix=`echo $band | tr 'g' 'G'`
		if [ "$disabled" = "off" -a "$disabled_all" = "off" ]; then
			ledcli WLAN"$postfix"_ON
		else
			ledcli WLAN"$postfix"_OFF
		fi
	done
}

wifi_reload() {
	/etc/init.d/minidlna stop
	echo 3 >/proc/sys/vm/drop_caches
	wifi_led_set
	wifi_nvram_config
	wifi_driver_stop
	wifi_driver_startup
	if [ `wps_led_set status` = "off" ];then
		sleep 1
		#wps_gpio_set 0
		echo 0 > /sys/class/leds/blue:wps/brightness
	fi
	echo "=====>>>>> wireless setting is finished" >$CONSOLE
	/etc/init.d/minidlna start
}

wifi_init_common() {
	echo 14336 >/proc/sys/vm/min_free_kbytes
	wlan_nvram_init
	init_feature_config
	init_nvram_mac
	wifi_nvram_config
	echo /sbin/hotplug > /proc/sys/kernel/hotplug # config for wds, need to check
} 

wifi_init_art() {
	local pro_id=`getfirm PRODUCT_ID`
	nvrammanager -r /tmp/par_tbl -p partition-table
	/usr/bin/cal_cbr /tmp/par_tbl "$pro_id" radio_bk
	if [ "$?" = "0" ]; then
		{
			wifi_driver_startup
			echo "!!! no rftestflag, need to calibrate! start to calibrate ..." >$CONSOLE
			ledcli WLAN2G_SAN
			ledcli WLAN5G_SAN
			/etc/init.d/phyport start
			wifi_start_calibrate &
		}&
	fi
	rm -rf /tmp/par_tbl
} 

wifi_init_start() {
	local module_name=wl
	grep -q '^'$module_name /proc/modules || ( wifi_driver_startup ; wifi_led_set )

	echo "inited" >/tmp/wifi_state #tell others wifi is inited
	echo "=====>>>>> wireless setting is finished" >$CONSOLE
} 

wifi_init() {
	if [ "$1" == "common" ]; then
		wifi_init_common
	elif [ "$1" == "art" ]; then
		wifi_init_art
	elif [ "$1" == "start" ]; then
		wifi_init_start &
	fi
}
