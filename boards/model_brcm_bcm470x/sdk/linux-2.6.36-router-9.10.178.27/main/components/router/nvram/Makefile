#
# Linux NVRAM Makefile
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
# $Id: Makefile 511542 2014-10-30 04:58:39Z $
#

include $(TOP)/.config

CFLAGS	+= -I. -I$(SRCBASE)/include ${WLAN_StdIncPathA} -I$(SRCBASE)/../components/shared -I$(SRCBASE)/common/include -Wall
#CFLAGS	+= -g -DDEBUG
CFLAGS	+= -s
ifeq ($(CONFIG_RTR_OPTIMIZE_SIZE),y)
CFLAGS  += -Os
else
CFLAGS  += -O2
endif
LDFLAGS += -L.

vpath %.c $(SRCBASE)/shared/nvram

all: libnvram.so $(if $(CONFIG_NVRAM_UTILITY),nvram)

install: all
	install -D libnvram.so $(INSTALLDIR)/usr/lib/libnvram.so
	$(STRIP) $(INSTALLDIR)/usr/lib/libnvram.so
ifneq ($(CONFIG_NVRAM_UTILITY),)
	install -D nvram $(INSTALLDIR)/usr/sbin/nvram
	$(STRIP) $(INSTALLDIR)/usr/sbin/nvram
else
	# Utility not wanted: get rid of any leftover one
	rm -f $(INSTALLDIR)/usr/sbin/nvram
endif


clean:
	rm -f *.o *.a *.so nvram

libnvram.so: nvram_linux.o
	$(LD) -shared -o $@ $^

nvram: main.o libnvram.so
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lnvram
