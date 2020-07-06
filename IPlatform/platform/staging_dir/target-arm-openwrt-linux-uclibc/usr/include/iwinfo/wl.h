/*
 * iwinfo - Wireless Information Library - Broadcom wl.o Headers
 *
 *   Copyright (C) 2009-2010 Jo-Philipp Wich <xm@subsignal.org>
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
 */

#ifndef __IWINFO_WL_H_
#define __IWINFO_WL_H_

#include <fcntl.h>

#include "iwinfo.h"
#include "iwinfo/utils.h"
#include "iwinfo/api/broadcom.h"

int wl_probe(const char *ifname);
int wl_get_mode(const char *ifname, int *buf);
int wl_get_ssid(const char *ifname, char *buf);
int wl_get_bssid(const char *ifname, char *buf);
int wl_get_country(const char *ifname, char *buf);
int wl_get_channel(const char *ifname, int *buf);
int wl_get_frequency(const char *ifname, int *buf);
int wl_get_frequency_offset(const char *ifname, int *buf);
int wl_get_txpower(const char *ifname, int *buf);
int wl_get_txpower_offset(const char *ifname, int *buf);
int wl_get_bitrate(const char *ifname, int *buf);
int wl_get_signal(const char *ifname, int *buf);
int wl_get_noise(const char *ifname, int *buf);
int wl_get_quality(const char *ifname, int *buf);
int wl_get_quality_max(const char *ifname, int *buf);
int wl_get_enctype(const char *ifname, char *buf);
int wl_get_encryption(const char *ifname, char *buf);
int wl_get_assoclist(const char *ifname, char *buf, int *len);
int wl_get_txpwrlist(const char *ifname, char *buf, int *len);
int wl_get_scanlist(const char *ifname, char *buf, int *len);
int wl_get_freqlist(const char *ifname, char *buf, int *len);
int wl_get_countrylist(const char *ifname, char *buf, int *len);
int wl_get_hwmodelist(const char *ifname, int *buf);
int wl_get_mbssid_support(const char *ifname, int *buf);
int wl_get_hardware_id(const char *ifname, char *buf);
int wl_get_hardware_name(const char *ifname, char *buf);
int wl_get_beacon_int(const char *ifname, int *buf);
void wl_close(void);

static const struct iwinfo_ops wl_ops = {
	.channel          = wl_get_channel,
	.frequency        = wl_get_frequency,
	.frequency_offset = wl_get_frequency_offset,
	.txpower          = wl_get_txpower,
	.txpower_offset   = wl_get_txpower_offset,
	.bitrate          = wl_get_bitrate,
	.signal           = wl_get_signal,
	.noise            = wl_get_noise,
	.quality          = wl_get_quality,
	.quality_max      = wl_get_quality_max,
	.mbssid_support   = wl_get_mbssid_support,
	.hwmodelist       = wl_get_hwmodelist,
	.mode             = wl_get_mode,
	.ssid             = wl_get_ssid,
	.bssid            = wl_get_bssid,
	.country          = wl_get_country,
	.hardware_id      = wl_get_hardware_id,
	.hardware_name    = wl_get_hardware_name,
	.encryption       = wl_get_encryption,
	.assoclist        = wl_get_assoclist,
	.txpwrlist        = wl_get_txpwrlist,
	.scanlist         = wl_get_scanlist,
	.freqlist         = wl_get_freqlist,
	.countrylist      = wl_get_countrylist,
    .beacon_int       = wl_get_beacon_int,
	.close            = wl_close
};

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

typedef signed char  int8;
typedef signed short int16;
typedef signed int   int32;

#define htod32(i) i
#define htod16(i) i
#define dtoh32(i) i
#define dtoh16(i) i

/* channel encoding */
typedef struct channel_info {
	int hw_channel;
	int target_channel;
	int scan_channel;
} channel_info_t;

/* uint32 list */
typedef struct wl_uint32_list {
	/* in - # of elements, out - # of entries */
	uint32 count;
	/* variable length uint32 list */
	uint32 element[1];
} wl_uint32_list_t;

/* channel defines */
#define CH_UPPER_SB				0x01
#define CH_LOWER_SB				0x02
#define CH_EWA_VALID			0x04
#define CH_80MHZ_APART			16
#define CH_40MHZ_APART			8
#define CH_20MHZ_APART			4
#define CH_10MHZ_APART			2
#define CH_5MHZ_APART			1	/* 2G band channels are 5 Mhz apart */
#define	MAXCHANNEL				224
#define CH_MAX_2G_CHANNEL		14	/* Max channel in 2G band */

/**
 * Channel Factor for the starting frequence of 2.4 GHz channels.
 * The value corresponds to 2407 MHz.
 */
#define WF_CHAN_FACTOR_2_4_G		4814	/* 2.4 GHz band, 2407 MHz */

/**
 * Channel Factor for the starting frequence of 5 GHz channels.
 * The value corresponds to 5000 MHz.
 */
#define WF_CHAN_FACTOR_5_G		10000	/* 5   GHz band, 5000 MHz */

/* Defines from wlc_pub.h */
#define	WL_IW_RSSI_MINVAL		-200	/* Low value, e.g. for forcing roam */
#define	WL_IW_RSSI_NO_SIGNAL	-91	/* NDIS RSSI link quality cutoffs */
#define	WL_IW_RSSI_VERY_LOW		-80	/* Very low quality cutoffs */
#define	WL_IW_RSSI_LOW			-70	/* Low quality cutoffs */
#define	WL_IW_RSSI_GOOD			-68	/* Good quality cutoffs */
#define	WL_IW_RSSI_VERY_GOOD	-58	/* Very good quality cutoffs */
#define	WL_IW_RSSI_EXCELLENT	-57	/* Excellent quality cutoffs */
#define	WL_IW_RSSI_INVALID		0	/* invalid RSSI value */

/* size of wl_scan_params not including variable length array */
#define WL_SCAN_PARAMS_FIXED_SIZE 64

/* maximum channels returned by the get valid channels iovar */
#define WL_NUMCHANNELS		64

/* max number of chanspecs (used by the iovar to calc. buf space) */
#define WL_NUMCHANSPECS		110

#define WLC_SCAN			50
#define WLC_SCAN_RESULTS	51

#define ETHER_ADDR_LEN		6

/* MLME Enumerations */
#define DOT11_BSSTYPE_INFRASTRUCTURE	0	/* d11 infrastructure */
#define DOT11_BSSTYPE_INDEPENDENT		1	/* d11 independent */
#define DOT11_BSSTYPE_ANY				2	/* d11 any BSS type */
#define DOT11_SCANTYPE_ACTIVE			0	/* d11 scan active */
#define DOT11_SCANTYPE_PASSIVE			1	/* d11 scan passive */

#define	WL_BSS_INFO_VERSION				109		/* current version of wl_bss_info struct */

/* Capability Information Field */
#define DOT11_CAP_ESS				0x0001	/* d11 cap. ESS */
#define DOT11_CAP_IBSS				0x0002	/* d11 cap. IBSS */
#define DOT11_CAP_POLLABLE			0x0004	/* d11 cap. pollable */
#define DOT11_CAP_POLL_RQ			0x0008	/* d11 cap. poll request */
#define DOT11_CAP_PRIVACY			0x0010	/* d11 cap. privacy */
#define DOT11_CAP_SHORT				0x0020	/* d11 cap. short */
#define DOT11_CAP_PBCC				0x0040	/* d11 cap. PBCC */
#define DOT11_CAP_AGILITY			0x0080	/* d11 cap. agility */
#define DOT11_CAP_SPECTRUM			0x0100	/* d11 cap. spectrum */
#define DOT11_CAP_SHORTSLOT			0x0400	/* d11 cap. shortslot */
#define DOT11_CAP_RRM				0x1000	/* d11 cap. 11k radio measurement */
#define DOT11_CAP_CCK_OFDM			0x2000	/* d11 cap. CCK/OFDM */

/*
 * Structure of a 48-bit Ethernet address
 */
struct ether_addr {
	uint8_t octet[ETHER_ADDR_LEN];
} __attribute__((packed));

/* A chanspec holds the channel number, band, bandwidth and control sideband */
typedef uint16 chanspec_t;

typedef unsigned int uintptr;

/* ************* HT definitions. ************* */
#define MCSSET_LEN	16		/* 16-bits per 8-bit set to give 128-bits bitmap of MCS Index */
#define MAX_MCS_NUM	128		/* max mcs number = 128 */

/* BSS info structure
 * Applications MUST CHECK ie_offset field and length field to access IEs and
 * next bss_info structure in a vector (in wl_scan_results_t)
 */
typedef struct wl_bss_info {
	uint32		version;		/* version field */
	uint32		length;			/* byte length of data in this record,
								 * starting at version and including IEs
								 */
	struct ether_addr BSSID;
	uint16		beacon_period;	/* units are Kusec */
	uint16		capability;		/* Capability information */
	uint8		SSID_len;
	uint8		SSID[32];
	struct {
		uint	count;			/* # rates in this set */
		uint8	rates[16];		/* rates in 500kbps units w/hi bit set if basic */
	} rateset;					/* supported rates */
	chanspec_t	chanspec;		/* chanspec for bss */
	uint16		atim_window;	/* units are Kusec */
	uint8		dtim_period;	/* DTIM period */
	int16		RSSI;			/* receive signal strength (in dBm) */
	int8		phy_noise;		/* noise (in dBm) */

	uint8		n_cap;			/* BSS is 802.11N Capable */
	uint32		nbss_cap;		/* 802.11N BSS Capabilities (based on HT_CAP_*) */
	uint8		ctl_ch;			/* 802.11N BSS control channel number */
	uint32		reserved32[1];	/* Reserved for expansion of BSS properties */
	uint8		flags;			/* flags */
	uint8		reserved[3];	/* Reserved for expansion of BSS properties */
	uint8		basic_mcs[MCSSET_LEN];	/* 802.11N BSS required MCS set */

	uint16		ie_offset;		/* offset at which IEs start, from beginning */
	uint32		ie_length;		/* byte length of Information Elements */
	int16		SNR;			/* average SNR of during frame reception */
	/* Add new fields here */
	/* variable length Information Elements */
} wl_bss_info_t;

typedef struct wl_scan_params {
	wlc_ssid_t ssid;			/* default: {0, ""} */
	struct ether_addr bssid;	/* default: bcast */
	int8 bss_type;				/* default: any,
								 * DOT11_BSSTYPE_ANY/INFRASTRUCTURE/INDEPENDENT
								 */
	uint8 scan_type;			/* flags, 0 use default */
	int32 nprobes;				/* -1 use default, number of probes per channel */
	int32 active_time;			/* -1 use default, dwell time per channel for
								 * active scanning
								 */
	int32 passive_time;			/* -1 use default, dwell time per channel
								 * for passive scanning
								 */
	int32 home_time;			/* -1 use default, dwell time for the home channel
								 * between channel scans
								 */
	int32 channel_num;			/* count of channels and ssids that follow
								 *
								 * low half is count of channels in channel_list, 0
								 * means default (use all available channels)
								 *
								 * high half is entries in wlc_ssid_t array that
								 * follows channel_list, aligned for int32 (4 bytes)
								 * meaning an odd channel count implies a 2-byte pad
								 * between end of channel_list and first ssid
								 *
								 * if ssid count is zero, single ssid in the fixed
								 * parameter portion is assumed, otherwise ssid in
								 * the fixed portion is ignored
								 */
	uint16 channel_list[1];		/* list of chanspecs */
} wl_scan_params_t;

typedef struct wl_scan_results {
	uint32 buflen;
	uint32 version;
	uint32 count;
	wl_bss_info_t bss_info[1];
} wl_scan_results_t;

/* bandstate array indices */
#define BAND_2G_INDEX		0	/* wlc->bandstate[x] index */
#define BAND_5G_INDEX		1	/* wlc->bandstate[x] index */

#define WL_CHANSPEC_CHAN_MASK		0x00ff
#define WL_CHANSPEC_CHAN_SHIFT		0
#define WL_CHANSPEC_CHAN1_MASK		0x000f
#define WL_CHANSPEC_CHAN1_SHIFT		0
#define WL_CHANSPEC_CHAN2_MASK		0x00f0
#define WL_CHANSPEC_CHAN2_SHIFT		4

#define WL_CHANSPEC_CTL_SB_MASK		0x0700
#define WL_CHANSPEC_CTL_SB_SHIFT	8
#define WL_CHANSPEC_CTL_SB_LLL		0x0000
#define WL_CHANSPEC_CTL_SB_LLU		0x0100
#define WL_CHANSPEC_CTL_SB_LUL		0x0200
#define WL_CHANSPEC_CTL_SB_LUU		0x0300
#define WL_CHANSPEC_CTL_SB_ULL		0x0400
#define WL_CHANSPEC_CTL_SB_ULU		0x0500
#define WL_CHANSPEC_CTL_SB_UUL		0x0600
#define WL_CHANSPEC_CTL_SB_UUU		0x0700
#define WL_CHANSPEC_CTL_SB_LL		WL_CHANSPEC_CTL_SB_LLL
#define WL_CHANSPEC_CTL_SB_LU		WL_CHANSPEC_CTL_SB_LLU
#define WL_CHANSPEC_CTL_SB_UL		WL_CHANSPEC_CTL_SB_LUL
#define WL_CHANSPEC_CTL_SB_UU		WL_CHANSPEC_CTL_SB_LUU
#define WL_CHANSPEC_CTL_SB_L		WL_CHANSPEC_CTL_SB_LLL
#define WL_CHANSPEC_CTL_SB_U		WL_CHANSPEC_CTL_SB_LLU
#define WL_CHANSPEC_CTL_SB_LOWER 	WL_CHANSPEC_CTL_SB_LLL
#define WL_CHANSPEC_CTL_SB_UPPER	WL_CHANSPEC_CTL_SB_LLU

#define WL_CHANSPEC_BW_MASK		0x3800
#define WL_CHANSPEC_BW_SHIFT		11
#define WL_CHANSPEC_BW_5		0x0000
#define WL_CHANSPEC_BW_10		0x0800
#define WL_CHANSPEC_BW_20		0x1000
#define WL_CHANSPEC_BW_40		0x1800
#define WL_CHANSPEC_BW_80		0x2000
#define WL_CHANSPEC_BW_160		0x2800
#define WL_CHANSPEC_BW_8080		0x3000

#define WL_CHANSPEC_BAND_MASK	0xc000
#define WL_CHANSPEC_BAND_SHIFT	14
#define WL_CHANSPEC_BAND_2G		0x0000
#define WL_CHANSPEC_BAND_3G		0x4000
#define WL_CHANSPEC_BAND_4G		0x8000
#define WL_CHANSPEC_BAND_5G		0xc000
#define INVCHANSPEC				255

/* channel defines */
#define LOWER_20_SB(channel)		(((channel) > CH_10MHZ_APART) ? \
					((channel) - CH_10MHZ_APART) : 0)
#define UPPER_20_SB(channel)		(((channel) < (MAXCHANNEL - CH_10MHZ_APART)) ? \
					((channel) + CH_10MHZ_APART) : 0)

#define LL_20_SB(channel)	(((channel) > 3 * CH_10MHZ_APART) ? ((channel) - 3 * CH_10MHZ_APART) : 0)
#define UU_20_SB(channel)	(((channel) < (MAXCHANNEL - 3 * CH_10MHZ_APART)) ? \
					((channel) + 3 * CH_10MHZ_APART) : 0)
#define LU_20_SB(channel) LOWER_20_SB(channel)
#define UL_20_SB(channel) UPPER_20_SB(channel)

#define LOWER_40_SB(channel)		((channel) - CH_20MHZ_APART)
#define UPPER_40_SB(channel)		((channel) + CH_20MHZ_APART)
#define CHSPEC_WLCBANDUNIT(chspec)	(CHSPEC_IS5G(chspec) ? BAND_5G_INDEX : BAND_2G_INDEX)
#define CH20MHZ_CHSPEC(channel)		(chanspec_t)((chanspec_t)(channel) | WL_CHANSPEC_BW_20 | \
					(((channel) <= CH_MAX_2G_CHANNEL) ? \
					WL_CHANSPEC_BAND_2G : WL_CHANSPEC_BAND_5G))
#define NEXT_20MHZ_CHAN(channel)	(((channel) < (MAXCHANNEL - CH_20MHZ_APART)) ? \
					((channel) + CH_20MHZ_APART) : 0)
#define CH40MHZ_CHSPEC(channel, ctlsb)	(chanspec_t) \
					((channel) | (ctlsb) | WL_CHANSPEC_BW_40 | \
					((channel) <= CH_MAX_2G_CHANNEL ? WL_CHANSPEC_BAND_2G : \
					WL_CHANSPEC_BAND_5G))
#define CH80MHZ_CHSPEC(channel, ctlsb)	(chanspec_t) \
					((channel) | (ctlsb) | \
					WL_CHANSPEC_BW_80 | WL_CHANSPEC_BAND_5G)
#define CH160MHZ_CHSPEC(channel, ctlsb)	(chanspec_t) \
					((channel) | (ctlsb) | \
					WL_CHANSPEC_BW_160 | WL_CHANSPEC_BAND_5G)

/* simple MACROs to get different fields of chanspec */
#define CHSPEC_CHANNEL(chspec)		((uint8)((chspec) & WL_CHANSPEC_CHAN_MASK))
#define CHSPEC_CHAN1(chspec)		((chspec) & WL_CHANSPEC_CHAN1_MASK)
#define CHSPEC_CHAN2(chspec)		((chspec) & WL_CHANSPEC_CHAN2_MASK)
#define CHSPEC_BAND(chspec)			((chspec) & WL_CHANSPEC_BAND_MASK)
#define CHSPEC_CTL_SB(chspec)		((chspec) & WL_CHANSPEC_CTL_SB_MASK)
#define CHSPEC_BW(chspec)			((chspec) & WL_CHANSPEC_BW_MASK)

#ifdef WL11N_20MHZONLY

#define CHSPEC_IS10(chspec)		0
#define CHSPEC_IS20(chspec)		1
#ifndef CHSPEC_IS40
#define CHSPEC_IS40(chspec)		0
#endif
#ifndef CHSPEC_IS80
#define CHSPEC_IS80(chspec)		0
#endif
#ifndef CHSPEC_IS160
#define CHSPEC_IS160(chspec)	0
#endif
#ifndef CHSPEC_IS8080
#define CHSPEC_IS8080(chspec)	0
#endif

#else /* !WL11N_20MHZONLY */

#define CHSPEC_IS10(chspec)		(((chspec) & WL_CHANSPEC_BW_MASK) == WL_CHANSPEC_BW_10)
#define CHSPEC_IS20(chspec)		(((chspec) & WL_CHANSPEC_BW_MASK) == WL_CHANSPEC_BW_20)
#ifndef CHSPEC_IS40
#define CHSPEC_IS40(chspec)		(((chspec) & WL_CHANSPEC_BW_MASK) == WL_CHANSPEC_BW_40)
#endif
#ifndef CHSPEC_IS80
#define CHSPEC_IS80(chspec)		(((chspec) & WL_CHANSPEC_BW_MASK) == WL_CHANSPEC_BW_80)
#endif
#ifndef CHSPEC_IS160
#define CHSPEC_IS160(chspec)	(((chspec) & WL_CHANSPEC_BW_MASK) == WL_CHANSPEC_BW_160)
#endif
#ifndef CHSPEC_IS8080
#define CHSPEC_IS8080(chspec)	(((chspec) & WL_CHANSPEC_BW_MASK) == WL_CHANSPEC_BW_8080)
#endif
#endif

#define CHSPEC_IS5G(chspec)	(((chspec) & WL_CHANSPEC_BAND_MASK) == WL_CHANSPEC_BAND_5G)
#define CHSPEC_IS2G(chspec)	(((chspec) & WL_CHANSPEC_BAND_MASK) == WL_CHANSPEC_BAND_2G)

#define WL_CHAN_VALID_HW	(1 << 0)	/* valid with current HW */
#define WL_CHAN_VALID_SW	(1 << 1)	/* valid with current country setting */
#define WL_CHAN_BAND_5G		(1 << 2)	/* 5GHz-band channel */
#define WL_CHAN_RADAR		(1 << 3)	/* radar sensitive  channel */
#define WL_CHAN_INACTIVE	(1 << 4)	/* temporarily inactive due to radar */
#define WL_CHAN_PASSIVE		(1 << 5)	/* channel is in passive mode */
#define WL_CHAN_RESTRICTED	(1 << 6)	/* restricted use channel */

#endif
