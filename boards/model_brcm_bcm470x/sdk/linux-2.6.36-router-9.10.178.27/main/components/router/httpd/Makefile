#
# milli_httpd Makefile
#
# Copyright (C) 2015, Broadcom Corporation. All Rights Reserved.
# 
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
# 
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
# SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
# OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
# CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
#
# <<Broadcom-WL-IPTag/Open:>>
#
# $Id: Makefile 537177 2015-02-25 07:33:05Z $
#

include $(TOP)/.config

ifneq (2_4,$(LINUX_VERSION))
CFLAGS += -DLINUX26
# No more Application Specific Port Forwards support after Linux 2.6.36
ifeq (2_6_36,$(LINUX_VERSION))
CFLAGS += -DAUTOFW_PORT_DEPRECATED
endif
endif

CFLAGS	+= -I. -I$(TOP)/shared -I$(TOP)/www/broadcom/cgi -I$(SRCBASE)/include ${WLAN_StdIncPathA} -I$(SRCBASE)/../components/shared -I$(SRCBASE)/common/include -Wall
CFLAGS	+= $(if $(WLAN_ComponentIncPath),$(WLAN_ComponentIncPath),$(addprefix -I,$(wildcard $(SRCBASE)/shared/bcmwifi/include)))
# For customer backwords compatibility, for now we keep CONFIG_WSCCMD here and can be removed when
# all customers migrate to new SDK (no CONFIG_WSCCMD in configration files).
ifneq ($(CONFIG_WPS)$(CONFIG_WSCCMD),)
CFLAGS += -I$(SRCBASE)/../components/apps/wps/brcm_apps/include
endif
#CFLAGS	+= -g -DDEBUG
CFLAGS	+= -s
LDFLAGS	+= -L$(TOP)/nvram -L$(INSTALLDIR)/nvram/usr/lib -lnvram -L$(TOP)/libbcmcrypto -L$(INSTALLDIR)/libbcmcrypto/usr/lib -lbcmcrypto -L$(TOP)/shared -L$(INSTALLDIR)/shared/usr/lib -lshared
LDFLAGS += $(EXTRA_LDFLAGS)


OBJS := cgi.o ej.o httpd.o cgi_common.o cgi_radio.o cgi_index.o cgi_lan.o cgi_wan.o cgi_status.o cgi_filter.o cgi_forward.o cgi_media.o cgi_storage.o cgi_ssid.o cgi_security.o cgi_wps.o cgi_firmware.o cgi_internal.o

PRE_LINK_TARGETS :=
PRE_CLEAN_TARGETS :=
IQOS_GUI_SRC :=
PREBUILLT_OBJS :=
ifeq ($(LINUX_VERSION),2_6_36)
ifeq ($(CONFIG_TREND_IQOS),y)
PRE_LINK_TARGETS += $(IQOS_DIR)/lib/libiqos_interface.a
PRE_CLEAN_TARGETS += iqos_interface_clean
IQOS_SRCS := $(IQOS_DIR)/src/interface
CFLAGS += -I$(IQOS_SRCS)
CFLAGS += -I$(SRCBASE)/../components/opensource/jsonc/installbin/include/json-c
LDFLAGS += -L$(TOP)/../../components/opensource/jsonc/installbin/lib
LDFLAGS += -L$(INSTALLDIR)/libjsonc/usr/lib -ljson-c
ifneq ($(wildcard cgi_iqos.c),)
	OBJS += cgi_iqos.o
	IQOS_GUI_SRC = cgi_iqos.c
else
	PREBUILLT_OBJS += cgi_iqos.o
endif
vpath %.c $(IQOS_SRCS)
endif
endif

# Adding Passpoint_gui.c dependency
ifeq ($(CONFIG_HSPOT),y)
INCLUDE += -I$(PASSPOINT_DIR)/pkt
INCLUDE += -I$(PASSPOINT_ROUTER)/shared/
CFLAGS += $(INCLUDE)
vpath %.c $(PASSPOINT_ROUTER)/shared/
OBJS += passpoint_gui.o
endif

ifeq ($(LINUX_VERSION),2_6_36)
ifeq ($(CONFIG_VISUALIZATION),y)
OBJS += vis_gui.o
endif
endif

ifeq ($(CONFIG_EZC),y)
OBJS += ezc.o
endif

vpath %.c $(TOP)/shared
vpath %.c $(TOP)/www/broadcom/cgi
vpath %.o $(SRCBASE_ROUTER)/httpd/prebuilt

all: httpd

clean: $(PRE_CLEAN_TARGETS)
	rm -f $(OBJS) *~ httpd

cgi_iqos.o: $(IQOS_GUI_SRC)
ifneq ($(wildcard cgi_iqos.c),)
	$(CC) $(CFLAGS) -c -o $@ $<
endif

install: all
	install -D httpd $(INSTALLDIR)/usr/sbin/httpd
	$(STRIP) $(INSTALLDIR)/usr/sbin/httpd

httpd: $(OBJS) $(PREBUILLT_OBJS) $(PRE_LINK_TARGETS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(IQOS_DIR)/lib/libiqos_interface.a:
ifneq ($(wildcard $(IQOS_DIR)/src),)
	$(MAKE) -C $(IQOS_DIR)/src/interface
endif

iqos_interface_clean:
ifneq ($(wildcard $(IQOS_DIR)/src),)
	$(MAKE) -C $(IQOS_DIR)/src/interface clean
endif
