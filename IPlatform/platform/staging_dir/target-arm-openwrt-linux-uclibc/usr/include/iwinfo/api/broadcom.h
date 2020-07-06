/*
 * Custom OID/ioctl definitions for
 * Broadcom 802.11abg Networking Device Driver
 *
 * Definitions subject to change without notice.
 *
 * Copyright 2006, Broadcom Corporation
 * All Rights Reserved.
 *
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 */

#ifndef _BROADCOM_H
#define _BROADCOM_H

#define WL_MCSSET_LEN				16
#define WL_MAX_STA_COUNT			64

#define WL_BSS_RSSI_OFFSET			82
#define WL_BSS_NOISE_OFFSET			84

#define WLC_IOCTL_MAGIC				0x14e46c77
#define	WLC_IOCTL_MAXLEN			8192

#define WLC_CNTRY_BUF_SZ        	4

#define WLC_GET_MAGIC				0
#define WLC_GET_RATE				12
#define WLC_GET_INFRA				19
#define WLC_GET_AUTH				21
#define WLC_GET_BSSID				23
#define WLC_GET_SSID				25
#define WLC_GET_CHANNEL				29
#define WLC_GET_PASSIVE 			48
#define WLC_GET_COUNTRY				83
#define WLC_GET_REVINFO				98
#define WLC_GET_AP					117
#define WLC_GET_RSSI				127
#define WLC_GET_WSEC				133
#define WLC_GET_PHY_NOISE			135
#define WLC_GET_BSS_INFO			136
#define WLC_GET_ASSOCLIST			159
#define WLC_GET_WPA_AUTH			164
#define WLC_GET_COUNTRY_LIST		261
#define WLC_GET_VAR					262

#define WLC_GET_RADIO				37
#define WLC_GET_CURR_RATESET			114	/* current rateset */
#define WLC_GET_VALID_CHANNELS		217
#define WLC_GET_PHYTYPE				39
#define	WLC_PHY_TYPE_SSN	6
#define	WLC_PHY_TYPE_LCN	8
#define	WLC_PHY_TYPE_LCN40	10
#define WLC_GET_BCNPRD				75

#define	WLC_PHY_TYPE_A		0
#define DOT11_MAX_DEFAULT_KEYS	4	/* number of default keys */

/* Key related defines */
#define DOT11_MAX_DEFAULT_KEYS	4	/* number of default keys */
#define DOT11_MAX_KEY_SIZE	32	/* max size of any key */
#define DOT11_MAX_IV_SIZE	16	/* max size of any IV */
#define DOT11_EXT_IV_FLAG	(1<<5)	/* flag to indicate IV is > 4 bytes */
#define DOT11_WPA_KEY_RSC_LEN   8       /* WPA RSC key len */

#define WEP1_KEY_SIZE		5	/* max size of any WEP key */
#define WEP1_KEY_HEX_SIZE	10	/* size of WEP key in hex. */
#define WEP128_KEY_SIZE		13	/* max size of any WEP key */
#define WEP128_KEY_HEX_SIZE	26	/* size of WEP key in hex. */
#define TKIP_MIC_SIZE		8	/* size of TKIP MIC */
#define TKIP_EOM_SIZE		7	/* max size of TKIP EOM */
#define TKIP_EOM_FLAG		0x5a	/* TKIP EOM flag byte */
#define TKIP_KEY_SIZE		32	/* size of any TKIP key */
#define TKIP_MIC_AUTH_TX	16	/* offset to Authenticator MIC TX key */
#define TKIP_MIC_AUTH_RX	24	/* offset to Authenticator MIC RX key */
#define TKIP_MIC_SUP_RX		TKIP_MIC_AUTH_TX	/* offset to Supplicant MIC RX key */
#define TKIP_MIC_SUP_TX		TKIP_MIC_AUTH_RX	/* offset to Supplicant MIC TX key */
#define AES_KEY_SIZE		16	/* size of AES key */
#define AES_MIC_SIZE		8	/* size of AES MIC */
#define BIP_KEY_SIZE		16	/* size of BIP key */
#define BIP_MIC_SIZE		8   /* sizeof BIP MIC */

/* Override bit for WLC_SET_TXPWR.  if set, ignore other level limits */
#define WL_TXPWR_OVERRIDE	(1U<<31)
#define WL_TXPWR_NEG   (1U<<30)

/* Bit masks for radio disabled status - returned by WL_GET_RADIO */
#define WL_RADIO_SW_DISABLE		(1<<0)
#define WL_RADIO_HW_DISABLE		(1<<1)
#define WL_RADIO_MPC_DISABLE		(1<<2)
#define WL_RADIO_COUNTRY_DISABLE	(1<<3)	/* some countries don't support any channel */

/*
 * One doesn't need to include this file explicitly, gets included automatically if
 * typedefs.h is included.
 */

/* Use BCM_REFERENCE to suppress warnings about intentionally-unused function
 * arguments or local variables.
 */
#define BCM_REFERENCE(data)	((void)(data))

struct wl_ether_addr {
	uint8_t					octet[6];
};

struct wl_maclist {
	uint					count;
	struct wl_ether_addr 	ea[1];
};

typedef struct wl_sta_rssi {
	int						rssi;
	char					mac[6];
	uint16_t				foo;
} wl_sta_rssi_t;

#define WL_NUMRATES     16 /* max # of rates in a rateset */
typedef struct wl_rateset {
    uint32_t  				count;          /* # rates in this set */
    uint8_t   				rates[WL_NUMRATES]; /* rates in 500kbps units w/hi bit set if basic */
} wl_rateset_t;

#define WL_STA_ANT_MAX		4		/* max possible rx antennas */

typedef struct wl_sta_info {
	uint16_t			ver;		/* version of this struct */
	uint16_t			len;		/* length in bytes of this structure */
	uint16_t			cap;		/* sta's advertised capabilities */
	uint32_t			flags;		/* flags defined below */
	uint32_t			idle;		/* time since data pkt rx'd from sta */
	unsigned char			ea[6];      /* Station address */
	wl_rateset_t		rateset;	/* rateset in use */
	uint32_t			in;		/* seconds elapsed since associated */
	uint32_t			listen_interval_inms; /* Min Listen interval in ms for this STA */
	uint32_t			tx_pkts;	/* # of user packets transmitted (unicast) */
	uint32_t			tx_failures;	/* # of user packets failed */
	uint32_t			rx_ucast_pkts;	/* # of unicast packets received */
	uint32_t			rx_mcast_pkts;	/* # of multicast packets received */
	uint32_t			tx_rate;	/* Rate used by last tx frame */
	uint32_t			rx_rate;	/* Rate of last successful rx frame */
	uint32_t			rx_decrypt_succeeds;	/* # of packet decrypted successfully */
	uint32_t			rx_decrypt_failures;	/* # of packet decrypted unsuccessfully */
	uint32_t			tx_tot_pkts;	/* # of user tx pkts (ucast + mcast) */
	uint32_t			rx_tot_pkts;	/* # of data packets recvd (uni + mcast) */
	uint32_t			tx_mcast_pkts;	/* # of mcast pkts txed */
	uint64_t			tx_tot_bytes;	/* data bytes txed (ucast + mcast) */
	uint64_t			rx_tot_bytes;	/* data bytes recvd (ucast + mcast) */
	uint64_t			tx_ucast_bytes;	/* data bytes txed (ucast) */
	uint64_t			tx_mcast_bytes;	/* # data bytes txed (mcast) */
	uint64_t			rx_ucast_bytes;	/* data bytes recvd (ucast) */
	uint64_t			rx_mcast_bytes;	/* data bytes recvd (mcast) */
	int8_t			rssi[WL_STA_ANT_MAX]; /* average rssi per antenna
										   * of data frames
										   */
	int8_t			nf[WL_STA_ANT_MAX];	/* per antenna noise floor */
	uint16_t			aid;		/* association ID */
	uint16_t			ht_capabilities;	/* advertised ht caps */
	uint16_t			vht_flags;		/* converted vht flags */
	uint32_t			tx_pkts_retried;	/* # of frames where a retry was
							 * necessary
							 */
	uint32_t			tx_pkts_retry_exhausted; /* # of user frames where a retry
							  * was exhausted
							  */
	int8_t			rx_lastpkt_rssi[WL_STA_ANT_MAX]; /* Per antenna RSSI of last
								  * received data frame.
								  */
	/* TX WLAN retry/failure statistics:
	 * Separated for host requested frames and WLAN locally generated frames.
	 * Include unicast frame only where the retries/failures can be counted.
	 */
	uint32_t			tx_pkts_total;		/* # user frames sent successfully */
	uint32_t			tx_pkts_retries;	/* # user frames retries */
	uint32_t			tx_pkts_fw_total;	/* # FW generated sent successfully */
	uint32_t			tx_pkts_fw_retries;	/* # retries for FW generated frames */
	uint32_t			tx_pkts_fw_retry_exhausted;	/* # FW generated where a retry
								 * was exhausted
								 */
	uint32_t			rx_pkts_retried;	/* # rx with retry bit set */
	uint32_t			tx_rate_fallback;	/* lowest fallback TX rate */
} wl_sta_info_t;

typedef struct wlc_ssid {
	uint32_t				ssid_len;
	unsigned char			ssid[32];
} wlc_ssid_t;

/* Linux network driver ioctl encoding */
typedef struct wl_ioctl {
	uint32_t				cmd;	/* common ioctl definition */
	void					*buf;	/* pointer to user buffer */
	uint32_t				len;	/* length of user buffer */
	uint8_t					set;	/* get or set request (optional) */
	uint32_t				used;	/* bytes read or written (optional) */
	uint32_t				needed;	/* bytes needed (optional) */
} wl_ioctl_t;

/* Revision info */
typedef struct wlc_rev_info {
	uint		vendorid;	/* PCI vendor id */
	uint		deviceid;	/* device id of chip */
	uint		radiorev;	/* radio revision */
	uint		chiprev;	/* chip revision */
	uint		corerev;	/* core revision */
	uint		boardid;	/* board identifier (usu. PCI sub-device id) */
	uint		boardvendor;	/* board vendor (usu. PCI sub-vendor id) */
	uint		boardrev;	/* board revision */
	uint		driverrev;	/* driver version */
	uint		ucoderev;	/* microcode version */
	uint		bus;		/* bus type */
	uint		chipnum;	/* chip number */
} wlc_rev_info_t;

typedef struct wl_country_list {
	uint32_t buflen;
	uint32_t band_set;
	uint32_t band;
	uint32_t count;
	char country_abbrev[1];
} wl_country_list_t;


#endif
