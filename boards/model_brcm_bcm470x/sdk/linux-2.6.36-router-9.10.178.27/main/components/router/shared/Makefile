#
# Linux router shared code Makefile
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
# $Id: Makefile 520342 2014-12-11 05:39:44Z $
#

include $(SRCBASE_ROUTER)/.config

CFLAGS	+= -I. -I../httpd -I$(SRCBASE)/include ${WLAN_StdIncPathA} -I$(SRCBASE)/../components/shared -I$(SRCBASE)/common/include -Wall -fPIC
CFLAGS	+= $(WLAN_ComponentIncPath)
ifneq (2_4,$(LINUX_VERSION))
CFLAGS += -I$(LINUXDIR)/include
CFLAGS += -DLINUX26
ifeq (2_6_36,$(LINUX_VERSION))
CFLAGS += -DAUTOFW_PORT_DEPRECATED
endif
endif
#CFLAGS	+= -g -DDEBUG
CFLAGS	+= -s -O2
ifeq ($(CONFIG_RTR_OPTIMIZE_SIZE),y)
CFLAGS  += -Os
else
CFLAGS  += -O2
endif
LDFLAGS += -L.


OBJS := nvparse.o shutils.o wl.o wl_linux.o linux_timer.o defaults.o wlif_utils.o bcmwifi_channels.o et_linux.o confmtd_utils.o common_utils.o

# Adding passpoint_nvparse.c dependency
ifeq ($(CONFIG_HSPOT),y)
INCLUDE += -I$(PASSPOINT_DIR)/pkt
INCLUDE += -I$(PASSPOINT_ROUTER)/shared/
CFLAGS  += $(INCLUDE)
vpath %.c $(PASSPOINT_ROUTER)/shared/
OBJS += passpoint_nvparse.o
endif

# Actually all builds use the new 11ac ratespec/chanspec
CFLAGS  += -DD11AC_IOTYPES
vpath %.c $(WLAN_StdSrcDirsA) $(WLAN_ComponentSrcDirs)

all: libshared.so

install: all
	install -d $(INSTALLDIR)/usr/lib
	install -m 755 libshared.so $(INSTALLDIR)/usr/lib
	$(STRIP) $(INSTALLDIR)/usr/lib/libshared.so

clean:
	rm -f *.o *.so

libshared.so: $(OBJS)
	$(LD) -shared -o $@ $^
