#
# Miscellaneous utilities Makefile
#
# Copyright (C) 2009, Broadcom Corporation
# All Rights Reserved.
# 
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
#
# $Id: Makefile 518163 2014-11-28 15:48:31Z $
#

include $(SRCBASE_ROUTER)/.config

ifneq (2_6_36,$(LINUX_VERSION))
WLU_OBJS := wlu.o wlu_cmd.o wlu_iov.o wlu_linux.o bcmutils.o wlu_client_shared.o wlu_pipe.o wlu_pipe_linux.o miniopt.o bcm_app_utils.o wlu_rates_matrix.o wlc_ppr.o bcmxtlv.o
ifneq ($(wildcard $(SRCBASE)/wl/exe/wlu_common.c),)
WLU_OBJS += wlu_common.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_phy.c),)
WLU_OBJS += wluc_phy.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_wnm.c),)
WLU_OBJS += wluc_wnm.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_cac.c),)
WLU_OBJS += wluc_cac.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_relmcast.c),)
WLU_OBJS += wluc_relmcast.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_rrm.c),)
WLU_OBJS += wluc_rrm.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_fbt.c),)
WLU_OBJS += wluc_fbt.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_wowl.c),)
WLU_OBJS += wluc_wowl.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_pkt_filter.c),)
WLU_OBJS += wluc_pkt_filter.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_mfp.c),)
WLU_OBJS += wluc_mfp.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_ota_test.c),)
WLU_OBJS += wluc_ota_test.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_bssload.c),)
WLU_OBJS += wluc_bssload.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_stf.c),)
WLU_OBJS += wluc_stf.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_offloads.c),)
WLU_OBJS += wluc_offloads.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_tpc.c),)
WLU_OBJS += wluc_tpc.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_toe.c),)
WLU_OBJS += wluc_toe.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_arpoe.c),)
WLU_OBJS += wluc_arpoe.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_keep_alive.c),)
WLU_OBJS += wluc_keep_alive.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_ap.c),)
WLU_OBJS += wluc_ap.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_ampdu.c),)
WLU_OBJS += wluc_ampdu.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_ampdu_cmn.c),)
WLU_OBJS += wluc_ampdu_cmn.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_bmac.c),)
WLU_OBJS += wluc_bmac.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_ht.c),)
WLU_OBJS += wluc_ht.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_wds.c),)
WLU_OBJS += wluc_wds.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_keymgmt.c),)
WLU_OBJS += wluc_keymgmt.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_scan.c),)
WLU_OBJS += wluc_scan.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_obss.c),)
WLU_OBJS += wluc_obss.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_prot_obss.c),)
WLU_OBJS += wluc_prot_obss.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_lq.c),)
WLU_OBJS += wluc_lq.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_seq_cmds.c),)
WLU_OBJS += wluc_seq_cmds.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_btcx.c),)
WLU_OBJS += wluc_btcx.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_led.c),)
WLU_OBJS += wluc_led.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_interfere.c),)
WLU_OBJS += wluc_interfere.o
endif
ifneq ($(wildcard $(SRCBASE)/wl/exe/wluc_ltecx.c),)
WLU_OBJS += wluc_ltecx.o
endif
ifneq ($(findstring WLTDLS, $(CFLAGS)),)
WLU_OBJS += wluc_tdls.o
endif
ifneq ($(findstring WL_PROXDETECT, $(CFLAGS)),)
WLU_OBJS += wlu_proxd.o
endif

ifneq ($(findstring WL_PROXDETECT, $(CFLAGS)),)
WLU_OBJS += wlu_proxd.o
endif

ifeq ($(RWL),1)
WL_OBJS := $(WLU_OBJS)
SERVER_OBJS := wlu_server_linux.o wlu_pipe_linux.o wlu_pipe.o wlu_server_shared.o shellproc_linux.o
APUT_OBJS := wfa_apcmdproc.o wfa_apcmdlib.o wfa_apcs.o wfa_apcmdresp.o wfa_apcmdvalid.o wfa_apcmdtbls.o wfa_apcmdcvt.o wfa_val2str.o wfa_apdiy.o wfa_sock.o wfa_aput_all.o

SOCKET_OBJS := $(SERVER_OBJS:%.o=socket/%.o)
WIFI_OBJS := $(SERVER_OBJS:%.o=wifi/%.o)
WL_OBJS := $(WL_OBJS:%.o=wl_obj/%.o)
WFA_APUT_OBJS := $(APUT_OBJS:%.o=aput/%.o)

SERVER_SOCKET  := wl_server_socket
SERVER_WIFI  := wl_server_wifi
WFA_APUT_ALL := wfa_aput_all
WL_EXE  := wl
endif
endif

UTILS := epi_ttcp
CFLAGS += -s -I$(SRCBASE)/include -I$(SRCBASE)/../components/shared -I$(SRCBASE)/common/include
ifeq ($(CONFIG_RTR_OPTIMIZE_SIZE),y)
CFLAGS  += -Os
else
CFLAGS  += -O2
endif
CFLAGS += -DBCMINTERNAL -DBCMDBG

CFLAGS += -DLINUX

ifneq (2_6_36,$(LINUX_VERSION))

CFLAGS += ${WLAN_StdIncPathA}
CFLAGS += $(if $(WLAN_ComponentIncPath),$(WLAN_ComponentIncPath),$(addprefix -I,$(wildcard $(SRCBASE)/shared/bcmwifi/include)))

CFLAGS += -I$(SRCBASE)/wl/exe
CFLAGS += -DPPR_API
#ifdef WLCNT
CFLAGS += -DWLCNT
#endif

ifeq ($(CONFIG_TRAFFIC_MGMT),y)
CFLAGS += -DTRAFFIC_MGMT
endif
endif

## Enable DMA RX overflow fix
ifeq ($(ET_INGRESS_QOS),1)
CFLAGS += -DET_INGRESS_QOS
endif

ifeq (2_6_36,$(LINUX_VERSION))
vpath %.c $(SRCBASE)/tools/misc
else
vpath %.c $(SRCBASE)/tools/misc $(WLAN_StdSrcDirsA) $(WLAN_ComponentSrcDirs)
endif

ifneq ($(wildcard $(SRCBASE)/et/linux/et.c),)
UTILS += et
vpath %.c $(SRCBASE)/et/linux
endif

ifneq (2_6_36,$(LINUX_VERSION))
ifneq ($(wildcard $(SRCBASE)/wl/exe),)
CFLAGS += -I$(SRCBASE)/wl/exe
UTILS += wl
vpath %.c $(SRCBASE)/wl/exe $(SRCBASE)/shared
endif

ifeq ($(RWL),1)
ifeq ($(BUILD_WFA_APUT),1)
ifneq ($(wildcard $(SRCBASE_ROUTER)/aput),)
CFLAGS += -I$(SRCBASE_ROUTER)/aput
UTILS += $(WFA_APUT_ALL)
vpath %.c $(SRCBASE_ROUTER)/aput
endif
endif

ifneq ($(wildcard $(SRCBASE)/wl/exe),)
UTILS += $(SERVER_SOCKET)
vpath %.c $(SRCBASE)/wl/exe $(SRCBASE)/shared
endif

ifneq ($(wildcard $(SRCBASE)/wl/exe),)
UTILS += $(SERVER_WIFI)
vpath %.c $(SRCBASE)/wl/exe $(SRCBASE)/shared
endif
endif
endif

LDFLAGS += -L$(TOP)/nvram -L$(INSTALLDIR)/nvram/usr/lib -lnvram
LDFLAGS += -L$(TOP)/shared -L$(INSTALLDIR)/shared/usr/lib -lshared
LDFLAGS += $(EXTRA_LDFLAGS)

all: $(UTILS)

ifneq (2_6_36,$(LINUX_VERSION))
wl: $(WLU_OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

ifeq ($(RWL),1)
$(WFA_APUT_ALL): $(WFA_APUT_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

aput/%.o: %.c
	@mkdir -p aput
	$(CC) -c $(CFLAGS) -o $@ $^
$(SERVER_SOCKET): $(SOCKET_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

socket/%.o: %.c
	@mkdir -p socket
	$(CC) -c $(CFLAGS) -DRWL_SOCKET -o $@ $^

$(SERVER_WIFI): $(WIFI_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

wifi/%.o: %.c
	@mkdir -p wifi
	$(CC) -c $(CFLAGS) -DRWL_WIFI -o $@ $^
endif
endif

clean:
	rm -f *.o *~ $(UTILS)
	rm -rf socket
ifneq (2_6_36,$(LINUX_VERSION))
	rm -rf wifi
endif

install: all
	install -d $(INSTALLDIR)/usr/sbin
	install $(UTILS) $(INSTALLDIR)/usr/sbin
	$(STRIP) $(foreach file,$(UTILS),$(INSTALLDIR)/usr/sbin/$(file))
