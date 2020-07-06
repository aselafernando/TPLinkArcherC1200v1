/*
 * iwinfo - Wireless Information Library - Broadcom wl.o Backend
 *
 *   Copyright (C) 2009 Jo-Philipp Wich <xm@subsignal.org>
 *
 * The iwinfo library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * The iwinfo library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with the iwinfo library. If not, see http://www.gnu.org/licenses/.
 *
 * This code is based on the wlc.c utility published by OpenWrt.org .
 */

#include "iwinfo/wl.h"
#include "iwinfo/wext.h"

#include <assert.h>

#define ASSERT(x) assert(x)

#define IWINFO_DEBUG(fmt, args...)   \
    printf("[IWINFO_DEBUG]%s %d: "fmt"\n", __FUNCTION__, __LINE__, ##args)

/* Quarter dBm units to mW
 * Table starts at QDBM_OFFSET, so the first entry is mW for qdBm=153
 * Table is offset so the last entry is largest mW value that fits in
 * a uint16.
 */

#define QDBM_OFFSET 153 /* QDBM_OFFSET */
#define QDBM_TABLE_LEN 40 /* QDBM_TABLE_LEN */

/* Smallest mW value that will round up to the first table entry, QDBM_OFFSET.
 * Value is ( mW(QDBM_OFFSET - 1) + mW(QDBM_OFFSET) ) / 2
 */
#define QDBM_TABLE_LOW_BOUND 6493 /* QDBM_TABLE_LOW_BOUND */

/* Largest mW value that will round down to the last table entry,
 * QDBM_OFFSET + QDBM_TABLE_LEN-1.
 * Value is ( mW(QDBM_OFFSET + QDBM_TABLE_LEN - 1) + mW(QDBM_OFFSET + QDBM_TABLE_LEN) ) / 2.
 */
#define QDBM_TABLE_HIGH_BOUND 64938 /* QDBM_TABLE_HIGH_BOUND */

static const uint16 nqdBm_to_mW_map[QDBM_TABLE_LEN] = {
/* qdBm:        +0	+1	+2	+3	+4	+5	+6	+7	*/
/* 153: */      6683,	7079,	7499,	7943,	8414,	8913,	9441,	10000,
/* 161: */      10593,	11220,	11885,	12589,	13335,	14125,	14962,	15849,
/* 169: */      16788,	17783,	18836,	19953,	21135,	22387,	23714,	25119,
/* 177: */      26607,	28184,	29854,	31623,	33497,	35481,	37584,	39811,
/* 185: */      42170,	44668,	47315,	50119,	53088,	56234,	59566,	63096
};

static char* wl_ifname(const char* ifname);

static int wl_ioctl(const char *name, int cmd, void *buf, int len)
{
	struct ifreq ifr;
	wl_ioctl_t ioc;

	/* do it */
	ioc.cmd = cmd;
	ioc.buf = buf;
	ioc.len = len;

	strncpy(ifr.ifr_name, wl_ifname(name), IFNAMSIZ);
	ifr.ifr_data = (caddr_t) &ioc;

	return iwinfo_ioctl(SIOCDEVPRIVATE, &ifr);
}

static int wl_iovar(const char *name, const char *cmd, const char *arg,
					int arglen, void *buf, int buflen)
{
	int cmdlen = strlen(cmd) + 1;

	memcpy(buf, cmd, cmdlen);

	if (arg && arglen > 0)
		memcpy(buf + cmdlen, arg, arglen);

	return wl_ioctl(name, WLC_GET_VAR, buf, buflen);
}

static struct wl_maclist * wl_read_assoclist(const char *ifname)
{
	struct wl_maclist *macs;
	int maclen = 4 + WL_MAX_STA_COUNT * 6;

	if ((macs = (struct wl_maclist *) malloc(maclen)) != NULL)
	{
		memset(macs, 0, maclen);
		macs->count = WL_MAX_STA_COUNT;

		if (!wl_ioctl(ifname, WLC_GET_ASSOCLIST, macs, maclen))
			return macs;

		free(macs);
	}

	return NULL;
}


int wl_probe(const char *ifname)
{
	int magic;
	return (!wl_ioctl(ifname, WLC_GET_MAGIC, &magic, sizeof(magic)) &&
			(magic == WLC_IOCTL_MAGIC));
}

void wl_close(void)
{
	/* Nop */
}

uint8_t wl_restricted(const char *ifname, uint8_t channel)
{
	uint32_t bitmap;
	uint32_t chanspec_arg;
	char buf[WLC_IOCTL_MAXLEN];

	memset(buf, 0, WLC_IOCTL_MAXLEN);

	chanspec_arg = CH20MHZ_CHSPEC((uint32_t)channel);
	chanspec_arg = htod32((uint32_t)chanspec_arg);

	if (!wl_iovar(ifname, "per_chan_info", &chanspec_arg, sizeof(chanspec_arg), buf, 
				  WLC_IOCTL_MAXLEN))
{
		bitmap = dtoh32(*(uint32_t *)buf);
		if (bitmap & WL_CHAN_RESTRICTED)
			return 1;
	}

	return 0;
}

int wl_get_mode(const char *ifname, int *buf)
{
	int ret = -1;
	int ap = 0, infra = 0;

	if ((ret = wl_ioctl(ifname, WLC_GET_AP, &ap, sizeof(ap))))
		return ret;

	if ((ret = wl_ioctl(ifname, WLC_GET_INFRA, &infra, sizeof(infra))))
		return ret;

	*buf = infra ? (ap ? IWINFO_OPMODE_MASTER : IWINFO_OPMODE_CLIENT) : IWINFO_OPMODE_ADHOC;

	return 0;
}

int wl_get_ssid(const char *ifname, char *buf)
{
	int ret = -1;
	wlc_ssid_t ssid;

	if (!(ret = wl_ioctl(ifname, WLC_GET_SSID, &ssid, sizeof(ssid))))
		memcpy(buf, ssid.ssid, ssid.ssid_len);

	return ret;
}

int wl_get_bssid(const char *ifname, char *buf)
{
	int ret = -1;
	char bssid[6];

	if (!(ret = wl_ioctl(ifname, WLC_GET_BSSID, bssid, 6)))
		sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
			(uint8_t)bssid[0], (uint8_t)bssid[1], (uint8_t)bssid[2],
			(uint8_t)bssid[3], (uint8_t)bssid[4], (uint8_t)bssid[5]
		);

	return ret;
}

int wl_get_channel(const char *ifname, int *buf)
{
	chanspec_t chanspec = 0;
	int nbw = 0;
	int target_channel = 0;
	int nctrlsb = 0;
	int home_channel = 0;
	char iobuf[WLC_IOCTL_MAXLEN];
	static const uint16 sidebands[] = {
		WL_CHANSPEC_CTL_SB_LLL, WL_CHANSPEC_CTL_SB_LLU,
		WL_CHANSPEC_CTL_SB_LUL, WL_CHANSPEC_CTL_SB_LUU,
		WL_CHANSPEC_CTL_SB_ULL, WL_CHANSPEC_CTL_SB_ULU,
		WL_CHANSPEC_CTL_SB_UUL, WL_CHANSPEC_CTL_SB_UUU
	};
	int i = 0;

	memset(iobuf, 0 , WLC_IOCTL_MAXLEN);

	if (wl_iovar(ifname, "chanspec", &chanspec, sizeof(chanspec), iobuf, WLC_IOCTL_MAXLEN) < 0)
	{
		fprintf(stderr, "Get chanspec failed.\n");
		return -1;
	}

	/* fetching info we are interested in */
	chanspec = *(chanspec_t *)iobuf;

	if (chanspec != 0)
	{
		nbw = chanspec & WL_CHANSPEC_BW_MASK;
		target_channel = chanspec & WL_CHANSPEC_CHAN_MASK;

		if (nbw == WL_CHANSPEC_BW_80)
		{
			nctrlsb = chanspec & WL_CHANSPEC_CTL_SB_MASK;

			for (i = 0; i < 4; i++)
			{
				if (nctrlsb == sidebands[i])
				{
					home_channel = target_channel - (CH_40MHZ_APART - CH_10MHZ_APART) + (i * CH_20MHZ_APART);
					break;
				}
			}

			if (i == 4)
			{
				fprintf(stderr, "Unexpected nctrlsb.\n");
				return -1;
			}
		}
		else if (nbw == WL_CHANSPEC_BW_40)
		{
			nctrlsb = chanspec & WL_CHANSPEC_CTL_SB_MASK;

			if (nctrlsb == WL_CHANSPEC_CTL_SB_UPPER)
			{
				home_channel = target_channel + 2;
			}
			else
			{
				home_channel = target_channel - 2;
			}
		}
		else
		{
			home_channel = target_channel;
		}
	}

	*buf = home_channel;

	return 0;
}

static inline int
wl_channel2mhz(uint ch, uint start_factor)
{
	int freq;

	if ((start_factor == WF_CHAN_FACTOR_2_4_G && (ch < 1 || ch > 14)) ||
	    (ch > 200))
		freq = -1;
	else if ((start_factor == WF_CHAN_FACTOR_2_4_G) && (ch == 14))
		freq = 2484;
	else
		freq = ch * 5 + start_factor / 2;

	return freq;
}

static inline uint
bcm_mkiovar(char *name, char *data, uint datalen, char *buf, uint buflen)
{
	uint len;

	len = strlen(name) + 1;

	if ((len + datalen) > buflen)
		return 0;

	strncpy(buf, name, buflen);

	/* append data onto the end of the name string */
	memcpy(&buf[len], data, datalen);
	len += datalen;

	return len;
}

/*
get named driver variable to int value and return error indication
calling example: dev_wlc_bufvar_get(dev, "arate", &rate)
*/
#define MAX_WLIW_IOCTL_LEN 1024
#define	WLC_IOCTL_SMLEN		256	

static int
wl_bufvar_get(
	const char* ifname,
	char *name,
	char *buf, int buflen)
{
	char *ioctlbuf;
	int error;

	uint len;

	ioctlbuf = malloc(MAX_WLIW_IOCTL_LEN);
	if (!ioctlbuf)
		return -1;
	len = bcm_mkiovar(name, NULL, 0, ioctlbuf, MAX_WLIW_IOCTL_LEN);
	ASSERT(len);
	BCM_REFERENCE(len);
	error = wl_ioctl(ifname, WLC_GET_VAR, (void *)ioctlbuf, MAX_WLIW_IOCTL_LEN);
	if (!error)
		bcopy(ioctlbuf, buf, buflen);

	free(ioctlbuf);
	return (error);
}

/*
get named driver variable to int value and return error indication
calling example: dev_wlc_intvar_get(dev, "arate", &rate)
*/
static int
wl_intvar_get(
	const char* ifname,
	char *name,
	int *retval)
{
	union {
		char buf[WLC_IOCTL_SMLEN];
		int val;
	} var;
	int error;

	uint len;
	uint data_null;

	len = bcm_mkiovar(name, (char *)(&data_null), 0, (char *)(&var), sizeof(var.buf));
	ASSERT(len);
	error = wl_ioctl(ifname, WLC_GET_VAR, (void *)&var, len);

	*retval = dtoh32(var.val);

	return (error);
}

int wl_get_range(const char *ifname, int *buf)
{
    int ret = -1;
    struct iw_range *range = (struct iw_range *) buf;
    static int channels[MAXCHANNEL+1];
	wl_uint32_list_t *list = (wl_uint32_list_t *) channels;
	wl_rateset_t rateset;
	int error, i, k;
	uint sf, ch;

	int phytype;
	int bw_cap = 0, sgi_tx = 0, nmode = 0;
	channel_info_t ci;
	uint8 nrate_list2copy = 0;
	uint16 nrate_list[4][8] = { {13, 26, 39, 52, 78, 104, 117, 130},
		{14, 29, 43, 58, 87, 116, 130, 144},
		{27, 54, 81, 108, 162, 216, 243, 270},
		{30, 60, 90, 120, 180, 240, 270, 300}};

    if (!range)
    {
		return -1;
    }

    memset(range, 0, sizeof(*range));

    /* We don't use nwids */
	range->min_nwid = range->max_nwid = 0;

	/* Set available channels/frequencies */
	list->count = htod32(MAXCHANNEL);
	if ((error = wl_ioctl(ifname, WLC_GET_VALID_CHANNELS, channels, sizeof(channels))))
    {
		return error;
    }
	for (i = 0; i < dtoh32(list->count) && i < IW_MAX_FREQUENCIES; i++) {
		range->freq[i].i = dtoh32(list->element[i]);

		ch = dtoh32(list->element[i]);
		if (ch <= CH_MAX_2G_CHANNEL)
			sf = WF_CHAN_FACTOR_2_4_G;
		else
			sf = WF_CHAN_FACTOR_5_G;

		range->freq[i].m = wl_channel2mhz(ch, sf);
		range->freq[i].e = 6;
	}
	range->num_frequency = range->num_channels = i;

	/* Link quality (use NDIS cutoffs) */
	range->max_qual.qual = 5;
	/* Signal level (use RSSI) */
	range->max_qual.level = 0x100 - 200;	/* -200 dBm */
	/* Noise level (use noise) */
	range->max_qual.noise = 0x100 - 200;	/* -200 dBm */
	/* Signal level threshold range (?) */
	range->sensitivity = 65535;

#if WIRELESS_EXT > 11
	/* Link quality (use NDIS cutoffs) */
	range->avg_qual.qual = 3;
	/* Signal level (use RSSI) */
	range->avg_qual.level = 0x100 + WL_IW_RSSI_GOOD;
	/* Noise level (use noise) */
	range->avg_qual.noise = 0x100 - 75;	/* -75 dBm */
#endif /* WIRELESS_EXT > 11 */

	/* Set available bitrates */
	if ((error = wl_ioctl(ifname, WLC_GET_CURR_RATESET, &rateset, sizeof(rateset))))
    {
		return error;
    }
	rateset.count = dtoh32(rateset.count);
	range->num_bitrates = rateset.count;
	for (i = 0; i < rateset.count && i < IW_MAX_BITRATES; i++)
		range->bitrate[i] = (rateset.rates[i] & 0x7f) * 500000; /* convert to bps */
    wl_intvar_get(ifname, "nmode", &nmode);
	if ((error = wl_ioctl(ifname, WLC_GET_PHYTYPE, &phytype, sizeof(phytype))))
		return error;

	if (nmode == 1 && ((phytype == WLC_PHY_TYPE_SSN) || (phytype == WLC_PHY_TYPE_LCN) ||
		(phytype == WLC_PHY_TYPE_LCN40))) {
        wl_bufvar_get(ifname, "mimo_bw_cap", &bw_cap, sizeof(bw_cap));
        wl_bufvar_get(ifname, "sgi_tx", &sgi_tx, sizeof(sgi_tx));
		wl_ioctl(ifname, WLC_GET_CHANNEL, &ci, sizeof(channel_info_t));
		ci.hw_channel = dtoh32(ci.hw_channel);

		if (bw_cap == 0 ||
			(bw_cap == 2 && ci.hw_channel <= 14)) {
			if (sgi_tx == 0)
				nrate_list2copy = 0;
			else
				nrate_list2copy = 1;
		}
		if (bw_cap == 1 ||
			(bw_cap == 2 && ci.hw_channel >= 36)) {
			if (sgi_tx == 0)
				nrate_list2copy = 2;
			else
				nrate_list2copy = 3;
		}
		range->num_bitrates += 8;
		ASSERT(range->num_bitrates < IW_MAX_BITRATES);
		for (k = 0; i < range->num_bitrates; k++, i++) {
			/* convert to bps */
			range->bitrate[i] = (nrate_list[nrate_list2copy][k]) * 500000;
		}
	}

	/* Set an indication of the max TCP throughput
	 * in bit/s that we can expect using this interface.
	 * May be use for QoS stuff... Jean II
	 */
	if ((error = wl_ioctl(ifname, WLC_GET_PHYTYPE, &i, sizeof(i))))
    {
		return error;
    }
	i = dtoh32(i);
	if (i == WLC_PHY_TYPE_A)
		range->throughput = 24000000;	/* 24 Mbits/s */
	else
		range->throughput = 1500000;	/* 1.5 Mbits/s */

	/* RTS and fragmentation thresholds */
	range->min_rts = 0;
	range->max_rts = 2347;
	range->min_frag = 256;
	range->max_frag = 2346;

	range->max_encoding_tokens = DOT11_MAX_DEFAULT_KEYS;
	range->num_encoding_sizes = 4;
	range->encoding_size[0] = WEP1_KEY_SIZE;
	range->encoding_size[1] = WEP128_KEY_SIZE;
#if WIRELESS_EXT > 17
	range->encoding_size[2] = TKIP_KEY_SIZE;
#else
	range->encoding_size[2] = 0;
#endif
	range->encoding_size[3] = AES_KEY_SIZE;

	/* Do not support power micro-management */
	range->min_pmp = 0;
	range->max_pmp = 0;
	range->min_pmt = 0;
	range->max_pmt = 0;
	range->pmp_flags = 0;
	range->pm_capa = 0;

	/* Transmit Power - values are in mW */
	range->num_txpower = 2;
	range->txpower[0] = 1;
	range->txpower[1] = 255;
	range->txpower_capa = IW_TXPOW_MWATT;

#if WIRELESS_EXT > 10
	range->we_version_compiled = WIRELESS_EXT;
	range->we_version_source = 19;

	/* Only support retry limits */
	range->retry_capa = IW_RETRY_LIMIT;
	range->retry_flags = IW_RETRY_LIMIT;
	range->r_time_flags = 0;
	/* SRL and LRL limits */
	range->min_retry = 1;
	range->max_retry = 255;
	/* Retry lifetime limits unsupported */
	range->min_r_time = 0;
	range->max_r_time = 0;
#endif /* WIRELESS_EXT > 10 */

#if WIRELESS_EXT > 17
	range->enc_capa = IW_ENC_CAPA_WPA;
	range->enc_capa |= IW_ENC_CAPA_CIPHER_TKIP;
	range->enc_capa |= IW_ENC_CAPA_CIPHER_CCMP;
	range->enc_capa |= IW_ENC_CAPA_WPA2;
#if (defined(BCMSUP_PSK) && defined(WLFBT))
	/* Tell the host (e.g. wpa_supplicant) to let us do the handshake */
	range->enc_capa |= IW_ENC_CAPA_4WAY_HANDSHAKE;
#endif /* (defined (BCMSUP_PSK) && defined(WLFBT)) */

	/* Event capability (kernel) */
	IW_EVENT_CAPA_SET_KERNEL(range->event_capa);
	/* Event capability (driver) */
	IW_EVENT_CAPA_SET(range->event_capa, SIOCGIWAP);
	IW_EVENT_CAPA_SET(range->event_capa, SIOCGIWSCAN);
	IW_EVENT_CAPA_SET(range->event_capa, IWEVTXDROP);
	IW_EVENT_CAPA_SET(range->event_capa, IWEVMICHAELMICFAILURE);
	IW_EVENT_CAPA_SET(range->event_capa, IWEVASSOCREQIE);
	IW_EVENT_CAPA_SET(range->event_capa, IWEVASSOCRESPIE);
	IW_EVENT_CAPA_SET(range->event_capa, IWEVPMKIDCAND);

#if WIRELESS_EXT >= 22 && defined(IW_SCAN_CAPA_ESSID)
	/* FC7 wireless.h defines EXT 22 but doesn't define scan_capa bits */
	range->scan_capa = IW_SCAN_CAPA_ESSID;
#endif
#endif /* WIRELESS_EXT > 17 */

    return 0;
}

static double wext_freq2float(const struct iw_freq *in)
{
	int		i;
	double	res = (double) in->m;
	for(i = 0; i < in->e; i++) res *= 10;
	return res;
}

static inline int wext_freq2mhz(const struct iw_freq *in)
{
	int i;

	if( in->e == 6 )
	{
		return in->m;
	}
	else
	{
		return (int)(wext_freq2float(in) / 1000000);
	}
}

int wl_get_frequency(const char *ifname, int *buf)
{
	int ret = -1;
	int channel = 0;
	uint sf = 0;

	if (!(ret = wl_get_channel(ifname, &channel)))
	{
		if (channel <= CH_MAX_2G_CHANNEL)
			sf = WF_CHAN_FACTOR_2_4_G;
		else
			sf = WF_CHAN_FACTOR_5_G;

		*buf = wl_channel2mhz(channel, sf);
	}

	return ret;
	/* return wext_get_frequency(ifname, buf); */
}

static inline uint16
bcm_qdbm_to_mw(uint8 qdbm)
{
	uint factor = 1;
	int idx = qdbm - QDBM_OFFSET;

	if (idx >= QDBM_TABLE_LEN) {
		/* clamp to max uint16 mW value */
		return 0xFFFF;
	}

	/* scale the qdBm index up to the range of the table 0-40
	 * where an offset of 40 qdBm equals a factor of 10 mW.
	 */
	while (idx < 0) {
		idx += 40;
		factor *= 10;
	}

	/* return the mW value scaled down to the correct factor of 10,
	 * adding in factor/2 to get proper rounding.
	 */
	return ((nqdBm_to_mW_map[idx] + factor/2) / factor);
}

int wl_get_txpower(const char *ifname, int *buf)
{
    struct iwreq wrq;
    struct iw_param *vwrq = &wrq.u.txpower;

    int error, disable, txpwrdbm;
	uint8 result;

    if ((error = wl_ioctl(ifname, WLC_GET_RADIO, &disable, sizeof(disable))) ||
	    (error = wl_intvar_get(ifname, "qtxpower", &txpwrdbm)))
    {
		return error;
    }

	disable = dtoh32(disable);
	result = (uint8)(txpwrdbm & ~WL_TXPWR_OVERRIDE);
	vwrq->value = (int32)bcm_qdbm_to_mw(result);
	vwrq->fixed = 0;
	vwrq->disabled = (disable & (WL_RADIO_SW_DISABLE | WL_RADIO_HW_DISABLE)) ? 1 : 0;
	vwrq->flags = IW_TXPOW_MWATT;

    if(wrq.u.txpower.flags & IW_TXPOW_MWATT)
		*buf = iwinfo_mw2dbm(wrq.u.txpower.value);
	else
		*buf = wrq.u.txpower.value;
    return 0;
	/* WLC_GET_VAR "qtxpower" */
	/*return wext_get_txpower(ifname, buf);*/
}

int wl_get_bitrate(const char *ifname, int *buf)
{
	int ret = -1;
	int rate = 0;

	if( !(ret = wl_ioctl(ifname, WLC_GET_RATE, &rate, sizeof(rate))) && (rate > 0))
		*buf = ((rate / 2) * 1000) + ((rate & 1) ? 500 : 0);

	return ret;
}

int wl_get_signal(const char *ifname, int *buf)
{
	unsigned int ap, rssi, i, rssi_count;
	int ioctl_req_version = 0x2000;
	char tmp[WLC_IOCTL_MAXLEN];
	struct wl_maclist *macs = NULL;
	wl_sta_rssi_t starssi;

	memset(tmp, 0, WLC_IOCTL_MAXLEN);
	memcpy(tmp, &ioctl_req_version, sizeof(ioctl_req_version));

	wl_ioctl(ifname, WLC_GET_BSS_INFO, tmp, WLC_IOCTL_MAXLEN);

	if (!wl_ioctl(ifname, WLC_GET_AP, &ap, sizeof(ap)) && !ap)
	{
		*buf = tmp[WL_BSS_RSSI_OFFSET];
	}
	else
	{
		rssi = rssi_count = 0;

		/* Calculate average rssi from conntected stations */
		if ((macs = wl_read_assoclist(ifname)) != NULL)
		{
			for (i = 0; i < macs->count; i++)
			{
				memcpy(starssi.mac, &macs->ea[i], 6);

				if (!wl_ioctl(ifname, WLC_GET_RSSI, &starssi, 12))
				{
					rssi -= starssi.rssi;
					rssi_count++;
				}
			}

			free(macs);
		}

		*buf = (rssi == 0 || rssi_count == 0) ? 1 : -(rssi / rssi_count);
	}

	return 0;
}

int wl_get_noise(const char *ifname, int *buf)
{
	unsigned int ap, noise;
	int ioctl_req_version = 0x2000;
	char tmp[WLC_IOCTL_MAXLEN];

	memset(tmp, 0, WLC_IOCTL_MAXLEN);
	memcpy(tmp, &ioctl_req_version, sizeof(ioctl_req_version));

	wl_ioctl(ifname, WLC_GET_BSS_INFO, tmp, WLC_IOCTL_MAXLEN);

	if ((wl_ioctl(ifname, WLC_GET_AP, &ap, sizeof(ap)) < 0) || ap)
	{
		if (wl_ioctl(ifname, WLC_GET_PHY_NOISE, &noise, sizeof(noise)) < 0)
			noise = 0;
	}
	else
	{
		noise = tmp[WL_BSS_NOISE_OFFSET];
	}

	*buf = noise;

	return 0;
}

int wl_get_quality(const char *ifname, int *buf)
{
	return wext_get_quality(ifname, buf);
}

int wl_get_quality_max(const char *ifname, int *buf)
{
	return wext_get_quality_max(ifname, buf);
}

int wl_get_encryption(const char *ifname, char *buf)
{
	uint32_t wsec, wauth, wpa;
	struct iwinfo_crypto_entry *c = (struct iwinfo_crypto_entry *)buf;

	if( wl_ioctl(ifname, WLC_GET_WPA_AUTH, &wpa,   sizeof(uint32_t)) ||
	    wl_ioctl(ifname, WLC_GET_WSEC,     &wsec,  sizeof(uint32_t)) ||
		wl_ioctl(ifname, WLC_GET_AUTH,     &wauth, sizeof(uint32_t)) )
			return -1;

	switch(wsec & 0xf)
	{
		case 2:
			c->pair_ciphers |= IWINFO_CIPHER_TKIP;
			break;

		case 4:
			c->pair_ciphers |= IWINFO_CIPHER_CCMP;
			break;

		case 6:
			c->pair_ciphers |= IWINFO_CIPHER_TKIP;
			c->pair_ciphers |= IWINFO_CIPHER_CCMP;
			break;
	}

	switch(wpa)
	{
		case 0:
			if ((wsec & 0xf) && !wauth)
				c->auth_algs |= IWINFO_AUTH_OPEN;

			else if ((wsec & 0xf) && wauth)
				c->auth_algs |= IWINFO_AUTH_SHARED;

			/* ToDo: evaluate WEP key lengths */
			c->pair_ciphers = IWINFO_CIPHER_WEP40 | IWINFO_CIPHER_WEP104;
			c->auth_suites |= IWINFO_KMGMT_NONE;
			break;

		case 2:
			c->wpa_version = 1;
			c->auth_suites |= IWINFO_KMGMT_8021x;
			break;

		case 4:
			c->wpa_version = 1;
			c->auth_suites |= IWINFO_KMGMT_PSK;
			break;

		case 32:
		case 64:
			c->wpa_version = 2;
			c->auth_suites |= IWINFO_KMGMT_8021x;
			break;

		case 66:
			c->wpa_version = 3;
			c->auth_suites |= IWINFO_KMGMT_8021x;
			break;

		case 128:
			c->wpa_version = 2;
			c->auth_suites |= IWINFO_KMGMT_PSK;
			break;

		case 132:
			c->wpa_version = 3;
			c->auth_suites |= IWINFO_KMGMT_PSK;
			break;

		default:
			break;
	}

	c->enabled = (c->wpa_version || c->auth_algs) ? 1 : 0;
	c->group_ciphers = c->pair_ciphers;

	return 0;
}

int wl_get_enctype(const char *ifname, char *buf)
{
	uint32_t wsec, wpa;
	char algo[11];

	if( wl_ioctl(ifname, WLC_GET_WPA_AUTH, &wpa, sizeof(uint32_t)) ||
	    wl_ioctl(ifname, WLC_GET_WSEC, &wsec, sizeof(uint32_t)) )
			return -1;

	switch(wsec)
	{
		case 2:
			sprintf(algo, "TKIP");
			break;

		case 4:
			sprintf(algo, "CCMP");
			break;

		case 6:
			sprintf(algo, "TKIP, CCMP");
			break;
	}

	switch(wpa)
	{
		case 0:
			sprintf(buf, "%s", wsec ? "WEP" : "None");
			break;

		case 2:
			sprintf(buf, "WPA 802.1X (%s)", algo);
			break;

		case 4:
			sprintf(buf, "WPA PSK (%s)", algo);
			break;

		case 32:
			sprintf(buf, "802.1X (%s)", algo);
			break;

		case 64:
			sprintf(buf, "WPA2 802.1X (%s)", algo);
			break;

		case 66:
			sprintf(buf, "mixed WPA/WPA2 802.1X (%s)", algo);
			break;

		case 128:
			sprintf(buf, "WPA2 PSK (%s)", algo);
			break;

		case 132:
			sprintf(buf, "mixed WPA/WPA2 PSK (%s)", algo);
			break;

		default:
			sprintf(buf, "Unknown");
	}

	return 0;
}

static void wl_get_assoclist_cb(const char *ifname, struct iwinfo_assoclist_entry *e)
{
	wl_sta_info_t sta = { 0 };

	if (!wl_iovar(ifname, "sta_info", e->mac, 6, &sta, sizeof(sta)) &&
		(sta.ver >= 2))
	{
		e->inactive     = sta.idle * 1000;
		e->rx_packets   = sta.rx_tot_pkts;
		e->tx_packets   = sta.tx_tot_pkts;
		e->rx_rate.rate = sta.rx_rate;
		e->tx_rate.rate = sta.tx_rate;

		/* ToDo: 11n */
		e->rx_rate.mcs = -1;
		e->tx_rate.mcs = -1;
	}
}

int wl_get_assoclist(const char *ifname, char *buf, int *len)
{
	int i, j, noise;
	int ap, infra, passive;
	char line[128];
	char macstr[18];
	char devstr[IFNAMSIZ];
	struct wl_maclist *macs;
	struct wl_sta_rssi rssi;
	struct iwinfo_assoclist_entry entry;
	FILE *arp;

	ap = infra = passive = 0;

	wl_ioctl(ifname, WLC_GET_AP, &ap, sizeof(ap));
	wl_ioctl(ifname, WLC_GET_INFRA, &infra, sizeof(infra));
	wl_ioctl(ifname, WLC_GET_PASSIVE, &passive, sizeof(passive));

	if (wl_get_noise(ifname, &noise))
		noise = 0;

	if ((ap || infra || passive) && ((macs = wl_read_assoclist(ifname)) != NULL))
	{
		for (i = 0, j = 0; i < macs->count; i++, j += sizeof(struct iwinfo_assoclist_entry))
		{
			memset(&entry, 0, sizeof(entry));
			memcpy(rssi.mac, &macs->ea[i], 6);

			if (!wl_ioctl(ifname, WLC_GET_RSSI, &rssi, sizeof(struct wl_sta_rssi)))
				entry.signal = (rssi.rssi - 0x100);
			else
				entry.signal = 0;

			entry.noise = noise;
			memcpy(entry.mac, &macs->ea[i], 6);
			wl_get_assoclist_cb(ifname, &entry);

			memcpy(&buf[j], &entry, sizeof(entry));
		}

		*len = j;
		free(macs);
		return 0;
	}
	else if ((arp = fopen("/proc/net/arp", "r")) != NULL)
	{
		j = 0;

		while (fgets(line, sizeof(line), arp) != NULL)
		{
			if (sscanf(line, "%*s 0x%*d 0x%*d %17s %*s %s", macstr, devstr) && !strcmp(devstr, ifname))
			{
				rssi.mac[0] = strtol(&macstr[0],  NULL, 16);
				rssi.mac[1] = strtol(&macstr[3],  NULL, 16);
				rssi.mac[2] = strtol(&macstr[6],  NULL, 16);
				rssi.mac[3] = strtol(&macstr[9],  NULL, 16);
				rssi.mac[4] = strtol(&macstr[12], NULL, 16);
				rssi.mac[5] = strtol(&macstr[15], NULL, 16);

				if (!wl_ioctl(ifname, WLC_GET_RSSI, &rssi, sizeof(struct wl_sta_rssi)))
					entry.signal = (rssi.rssi - 0x100);
				else
					entry.signal = 0;

				entry.noise = noise;
				memcpy(entry.mac, rssi.mac, 6);
				memcpy(&buf[j], &entry, sizeof(entry));

				j += sizeof(entry);
			}
		}

		*len = j;
		(void) fclose(arp);
		return 0;
	}

	return -1;
}

int wl_get_txpwrlist(const char *ifname, char *buf, int *len)
{
	struct iwinfo_txpwrlist_entry entry;
	uint8_t dbm[11] = { 0, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24 };
	uint8_t mw[11]  = { 1, 3, 6, 10, 15, 25, 39, 63, 100, 158, 251 };
	int i;

	for (i = 0; i < 11; i++)
	{
		entry.dbm = dbm[i];
		entry.mw  = mw[i];
		memcpy(&buf[i*sizeof(entry)], &entry, sizeof(entry));
	}

	*len = 11 * sizeof(entry);
	return 0;
}

static const struct ether_addr ether_bcast = {{255, 255, 255, 255, 255, 255}};

static int wl_set_scan(const char *ifname)
{
	int params_size = WL_SCAN_PARAMS_FIXED_SIZE + WL_NUMCHANNELS * sizeof(uint16);
	wl_scan_params_t *params;
	int err = 0;

	params = (wl_scan_params_t*)malloc(params_size);
	if(params == NULL)
	{
		fprintf(stderr, "Error allocating %d bytes for scan params\n", params_size);
		return -1;
	}

	memset(params, 0, params_size);
	params->bss_type = DOT11_BSSTYPE_ANY;
	memcpy(params->bssid.octet, &ether_bcast, ETHER_ADDR_LEN);
	params->scan_type = -1;		/* custom scan */
	params->nprobes = -1;
	params->active_time = 200;
	params->passive_time = -1;
	params->home_time = -1;
	params->channel_num = 0;

	err = wl_ioctl(ifname, WLC_SCAN, params, params_size);
	free(params);

	return err;
}

static int rssi_to_qual(int rssi)
{
	if (rssi <= WL_IW_RSSI_NO_SIGNAL)
		return 0;
	else if (rssi <= WL_IW_RSSI_VERY_LOW)
		return 1;
	else if (rssi <= WL_IW_RSSI_LOW)
		return 2;
	else if (rssi <= WL_IW_RSSI_GOOD)
		return 3;
	else if (rssi <= WL_IW_RSSI_VERY_GOOD)
		return 4;
	else
		return 5;
}

static inline void wl_fill_wpa(unsigned char *iebuf, int ielen, struct iwinfo_scanlist_entry *e)
{
	static unsigned char ms_oui[3] = { 0x00, 0x50, 0xf2 };

	while (ielen >= 2 && ielen >= iebuf[1])
	{
		switch (iebuf[0])
		{
		case 48: /* RSN */
			iwinfo_parse_rsn(&e->crypto, iebuf + 2, iebuf[1],
							 IWINFO_CIPHER_CCMP, IWINFO_KMGMT_8021x);
			break;

		case 221: /* Vendor */
			if (iebuf[1] >= 4 && !memcmp(iebuf + 2, ms_oui, 3) && iebuf[5] == 1)
				iwinfo_parse_rsn(&e->crypto, iebuf + 6, iebuf[1] - 4,
								 IWINFO_CIPHER_TKIP, IWINFO_KMGMT_PSK);
			break;
		}

		ielen -= iebuf[1] + 2;
		iebuf += iebuf[1] + 2;
	}
}

static int wl_fill_entry(const char *ifname, struct iw_point *dwrq, char *buf, 
	struct iw_range *range, int *entrylen)
{
	channel_info_t ci;
	wl_scan_results_t *list;
	struct iw_event	iwe;
	wl_bss_info_t *bi = NULL;
	int error, i, j;
	uint buflen = dwrq->length;
	struct iwinfo_scanlist_entry e;

	if(!dwrq->pointer)
		return -EINVAL;

	/* Check for scan in progress */
	if( (error = wl_ioctl(ifname, WLC_GET_CHANNEL, &ci, sizeof(ci))) )
		return error;
	ci.scan_channel = dtoh32(ci.scan_channel);
	if(ci.scan_channel)
		return -EAGAIN;

	/* Get scan results (too large to put on the stack) */
	list = malloc(buflen);
	if(!list)
		return -ENOMEM;

	memset(list, 0, buflen);
	list->buflen = htod32(buflen);
	if( (error = wl_ioctl(ifname, WLC_SCAN_RESULTS, list, buflen)) )
	{
		free(list);
		return error;
	}
	list->buflen = dtoh32(list->buflen);
	list->version = dtoh32(list->version);
	list->count = dtoh32(list->count);

	ASSERT(list->version == WL_BSS_INFO_VERSION);

	for(i = 0; i < list->count && i < IW_MAX_AP; i++)
	{
		bi = bi ? (wl_bss_info_t *)((uintptr)bi + dtoh32(bi->length)) : list->bss_info;
		ASSERT(((uintptr)bi + dtoh32(bi->length)) <= ((uintptr)list + buflen));

		memset(&e, 0, sizeof(struct iwinfo_scanlist_entry));

		/* BSSID */
		memcpy(e.mac, &bi->BSSID, ETHER_ADDR_LEN);

		/* SSID */
		memcpy(e.ssid, (uint8 *)&bi->SSID, bi->SSID_len);

		/* Mode */
		if (dtoh16(bi->capability) & (DOT11_CAP_ESS | DOT11_CAP_IBSS))
		{
			if (dtoh16(bi->capability) & DOT11_CAP_ESS)
				e.mode = IWINFO_OPMODE_MASTER;
			else
				e.mode = IWINFO_OPMODE_ADHOC;
		}
		else
			e.mode = IWINFO_OPMODE_UNKNOWN;

		/* Channel */
		if (bi->ctl_ch == 0)
			e.channel = bi->chanspec;
		else
			e.channel = bi->ctl_ch;

		/* filter band2 & band3 channel */
		if(e.channel > 48 && e.channel < 149) 
		{
			continue;
		}

		/* Channel quality */
		e.signal = 0x100 + dtoh16(bi->RSSI);
		e.quality = rssi_to_qual(dtoh16(bi->RSSI));
		e.quality_max = range->max_qual.qual;

		uint8 *ptr = ((uint8 *)bi) + sizeof(wl_bss_info_t);
		wl_fill_wpa(ptr, bi->ie_length, &e);

		/* Encryption */
		e.crypto.enabled = dtoh16(bi->capability) & DOT11_CAP_PRIVACY;

		if( ((*entrylen) + sizeof(struct iwinfo_scanlist_entry)) <= IWINFO_BUFSIZE )
		{
			/* if encryption is off, clear the crypto strunct */
			if( !e.crypto.enabled )
				memset(&e.crypto, 0, sizeof(struct iwinfo_crypto_entry));

			memcpy(&buf[*entrylen], &e, sizeof(struct iwinfo_scanlist_entry));
			(*entrylen) += sizeof(struct iwinfo_scanlist_entry);
		}
		else
		{
			/* we exceed the callers buffer size, abort here ... */
			break;
		}
	}

	free(list);

	return 0;
}

int wl_get_scanlist(const char *ifname, char *buf, int *len)
{
	struct iw_point wrq;
	struct iw_scan_req scanopt;		/* Options for 'set' */
	unsigned char *buffer = NULL;	/* Results */
	int buflen = IW_SCAN_MAX_DATA;	/* Min for compat WE<17 */
	struct iw_range range;
	int has_range = 1;
	struct timeval tv;				/* Select timeout */
	int timeout = 15000000;			/* 15s */
	int entrylen = 0;
	int error = 0;

	if( wl_get_range(ifname, &range) >= 0 )
	{
		/* Init timeout value -> 250ms between set and first get */
		tv.tv_sec  = 0;
		tv.tv_usec = 250000;

		/* Clean up set args */
		memset(&scanopt, 0, sizeof(scanopt));

		/* Initiate Scanning */
		if( wl_set_scan(ifname) >= 0 )
		{
			timeout -= tv.tv_usec;

			/* Forever */
			while(1)
			{
				fd_set rfds;	/* File descriptors for select */
				int last_fd;	/* Last fd */
				int ret;

				/* Guess what ? We must re-generate rfds each time */
				FD_ZERO(&rfds);
				last_fd = -1;
				/* In here, add the rtnetlink fd in the list */

				/* Wait until something happens */
				ret = select(last_fd + 1, &rfds, NULL, NULL, &tv);

				/* Check if there was an error */
				if(ret < 0)
				{
					if(errno == EAGAIN || errno == EINTR)
						continue;

					return -1;
				}

				/* Check if there was a timeout */
				if(ret == 0)
				{
					unsigned char *newbuf;

		realloc:
					/* (Re)allocate the buffer - realloc(NULL, len) == malloc(len) */
					newbuf = realloc(buffer, buflen);
					if(newbuf == NULL)
					{
						if(buffer)
							free(buffer);

						return -1;
					}

					buffer = newbuf;

					/* Try to read the results */
					wrq.pointer = buffer;
					wrq.flags   = 0;
					wrq.length  = buflen;

					if( error = wl_fill_entry(ifname, &wrq, buf, &range, &entrylen) )
					{
						/* Check if buffer was too small (WE-17 only) */
						if((errno == E2BIG) && (range.we_version_compiled > 16)
									&& (buflen < 0xFFFF))
						{
							/* Some driver may return very large scan results, either
							 * because there are many cells, or because they have many
							 * large elements in cells (like IWEVCUSTOM). Most will
							 * only need the regular sized buffer. We now use a dynamic
							 * allocation of the buffer to satisfy everybody. Of course,
							 * as we don't know in advance the size of the array, we try
							 * various increasing sizes. Jean II */

							/* Check if the driver gave us any hints. */
							if(wrq.length > buflen)
								buflen = wrq.length;
							else
								buflen *= 2;

							/* wrq.u.data.length is 16 bits so max size is 65535 */
							if(buflen > 0xFFFF)
								buflen = 0xFFFF;

							/* Try again */
							goto realloc;
						}

						/* Check if results not available yet */
						if(error == -EAGAIN || errno == EAGAIN)
						{
							/* Restart timer for only 100ms*/
							tv.tv_sec = 0;
							tv.tv_usec = 100000;
							timeout -= tv.tv_usec;

							if(timeout > 0)
								continue;	/* Try again later */
						}

						/* Bad error */
						free(buffer);
						return -1;

					} else {
						/* We have the results */
						*len = entrylen;
						free(buffer);
						return 0;
					}
				}
			}
		}
	}

	return -1;
	/* return wext_get_scanlist(ifname, buf, len); */
}

int wl_get_freqlist(const char *ifname, char *buf, int *len)
{
	struct iw_range range;
	struct iwinfo_freqlist_entry entry;
	int i, bl;

	if(wl_get_range(ifname, &range) >= 0)
	{
		bl = 0;

		for(i = 0; i < range.num_frequency; i++)
		{
			entry.mhz        = wext_freq2mhz(&range.freq[i]);
			entry.channel    = range.freq[i].i;
			entry.restricted = entry.channel <= CH_MAX_2G_CHANNEL ? 0x80 : 0;
			
			/* filter band2 & band3 channel */
			if(entry.channel > 48 && entry.channel < 149) 
			{
				continue;
			}

			memcpy(&buf[bl], &entry, sizeof(struct iwinfo_freqlist_entry));
			bl += sizeof(struct iwinfo_freqlist_entry);
		}

		*len = bl;
		return 0;
	}

	return -1;
	/* return wext_get_freqlist(ifname, buf, len); */
}

int wl_get_country(const char *ifname, char *buf)
{
	char ccode[WLC_CNTRY_BUF_SZ];

	if (!wl_ioctl(ifname, WLC_GET_COUNTRY, ccode, WLC_CNTRY_BUF_SZ))
	{
		/* IL0 -> World */
		if (!strcmp(ccode, "IL0"))
			sprintf(buf, "00");

		/* YU -> RS */
		else if (!strcmp(ccode, "YU"))
			sprintf(buf, "RS");

		else
			memcpy(buf, ccode, 2);

		return 0;
	}

	return -1;
}

int wl_get_countrylist(const char *ifname, char *buf, int *len)
{
	int i, count;
	char cdata[WLC_IOCTL_MAXLEN];
	struct iwinfo_country_entry *c = (struct iwinfo_country_entry *)buf;
	wl_country_list_t *cl = (wl_country_list_t *)cdata;

	cl->buflen = sizeof(cdata);

	if (!wl_ioctl(ifname, WLC_GET_COUNTRY_LIST, cl, cl->buflen))
	{
		for (i = 0, count = 0; i < cl->count; i++, c++)
		{
			sprintf(c->ccode, &cl->country_abbrev[i * WLC_CNTRY_BUF_SZ]);
			c->iso3166 = c->ccode[0] * 256 + c->ccode[1];

			/* IL0 -> World */
			if (!strcmp(c->ccode, "IL0"))
				c->iso3166 = 0x3030;

			/* YU -> RS */
			else if (!strcmp(c->ccode, "YU"))
				c->iso3166 = 0x5253;
		}

		*len = (i * sizeof(struct iwinfo_country_entry));
		return 0;
	}

	return -1;
}

int wl_get_hwmodelist(const char *ifname, int *buf)
{
    char chans[IWINFO_BUFSIZE] = { 0 };
	struct iwinfo_freqlist_entry *e = NULL;
	int len = 0;

	*buf = 0;

	if( !wl_get_freqlist(ifname, chans, &len) )
	{
		for( e = (struct iwinfo_freqlist_entry *)chans; e->channel; e++ )
		{
			if( e->channel <= 14 )
			{
				*buf |= IWINFO_80211_B;
				*buf |= IWINFO_80211_G;
                *buf |= IWINFO_80211_N;
			}
			else
			{
				*buf |= IWINFO_80211_A;
                *buf |= IWINFO_80211_N;
                *buf |= IWINFO_80211_AC; /* add or not? */
			}
		}

		return 0;
	}

	return -1;
	/* return wext_get_hwmodelist(ifname, buf); */
}

int wl_get_mbssid_support(const char *ifname, int *buf)
{
	wlc_rev_info_t revinfo;

	/* Multi bssid support only works on corerev >= 9 */
	if (!wl_ioctl(ifname, WLC_GET_REVINFO, &revinfo, sizeof(revinfo)))
	{
		if (revinfo.corerev >= 9)
		{
			*buf = 1;
			return 0;
		}
	}

	return -1;
}

int wl_get_hardware_id(const char *ifname, char *buf)
{
	wlc_rev_info_t revinfo;
	struct iwinfo_hardware_id *ids = (struct iwinfo_hardware_id *)buf;

	if (wl_ioctl(ifname, WLC_GET_REVINFO, &revinfo, sizeof(revinfo)))
		return -1;

	ids->vendor_id = revinfo.vendorid;
	ids->device_id = revinfo.deviceid;
	ids->subsystem_vendor_id = revinfo.boardvendor;
	ids->subsystem_device_id = revinfo.boardid;

	return 0;
}

int wl_get_hardware_name(const char *ifname, char *buf)
{
	struct iwinfo_hardware_id ids;

	if (wl_get_hardware_id(ifname, (char *)&ids))
		return -1;

	sprintf(buf, "Broadcom BCM%04X", ids.device_id);

	return 0;
}

int wl_get_txpower_offset(const char *ifname, int *buf)
{
	FILE *p;
	char off[8];

	*buf = 0;

	if ((p = popen("/usr/sbin/nvram get opo", "r")) != NULL)
	{
		if (fread(off, 1, sizeof(off), p))
			*buf = strtoul(off, NULL, 16);

		pclose(p);
	}

	return 0;
}

int wl_get_frequency_offset(const char *ifname, int *buf)
{
	/* Stub */
	*buf = 0;
	return -1;
}

int wl_get_beacon_int(const char *ifname, int *buf)
{
    int beacon;

	/* Multi bssid support only works on corerev >= 9 */
	if (!wl_ioctl(ifname, WLC_GET_BCNPRD, &beacon, sizeof(beacon)))
	{
        *buf = beacon;
		return 0;
	}
	return -1;
}

char map[][2][IFNAMSIZ] = 
{
    {"wl01", "eth1"},
    {"wl02", "wl0.1"},
    {"wl11", "eth2"},
    {"wl12", "wl1.1"},
    {"wl21", "eth3"},
    {"wl22", "wl2.1"},
};

/* We made ifname convert here. It's ugly, but ... by mw 09.29.14 */
static char* wl_ifname(const char* ifname)
{
    int idx = 0;
    int num = sizeof(map) / sizeof(char [2][IFNAMSIZ]);
    if(ifname) 
    {
        for(idx = 0; idx < num; idx++) 
        {
            if(!strcmp(ifname, map[idx][0])) 
            {
                return map[idx][1];
            }
        }
    }
    return ifname;
}
