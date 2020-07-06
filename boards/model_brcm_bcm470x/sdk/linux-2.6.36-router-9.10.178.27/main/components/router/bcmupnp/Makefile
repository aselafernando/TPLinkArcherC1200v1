#
# Linux upnp Makefile support WFA device
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

ifeq "$(SRCBASE)" ""
	SRCBASE=$(shell cd ../.. ; pwd)
endif

include $(SRCBASE_ROUTER)/.config

ifeq ($(CONFIG_IGD),y)
CFG_BCMIGD = 0
else
CFG_BCMIGD ?= 1
endif

UPNP	:= $(SRCBASE_ROUTER)/bcmupnp/upnp/linux
CFLAGS += -s -include bcmconfig.h
ifeq ($(CONFIG_RTR_OPTIMIZE_SIZE),y)
CFLAGS  += -Os
else
CFLAGS  += -O2
endif
#DEBUG	:= 1

.PHONY: all
ifneq ($(wildcard ./prebuilt),)
all: docopy upnp

.PHONY: docopy
docopy:
	install -d $(INSTALLDIR)/usr/sbin
	install -m 755 prebuilt/upnp $(INSTALLDIR)/usr/sbin/upnp
else
all: upnp
endif

.PHONY: install
install: all
	$(if $(wildcard ${UPNP}/Makefile),+$(MAKE) -C ${UPNP} CFG_BCMIGD=${CFG_BCMIGD} install)
	+$(MAKE) -C $(UPNP) -f Makefile.nat CFG_BCMIGD=$(CFG_BCMIGD) install

.PHONY: clean
clean:
	[ ! -f $(UPNP)/Makefile ] || $(MAKE) -C $(UPNP) clean
	$(MAKE) -C $(UPNP) -f Makefile.nat clean

.PHONY: upnp
upnp: FORCE
	$(if $(wildcard ${UPNP}/Makefile),+$(MAKE) -C ${UPNP} DEBUG=${DEBUG} CFG_BCMIGD=${CFG_BCMIGD})
	+$(MAKE) -C $(UPNP) -f Makefile.nat DEBUG=$(DEBUG) CFG_BCMIGD=$(CFG_BCMIGD)

.PHONY: FORCE
FORCE:
