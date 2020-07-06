#
# Copyright (C) 2015, Broadcom Corporation
# All Rights Reserved.
# 
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#
#
# <<Broadcom-WL-IPTag/Proprietary:>>
#

CROSS =
CC = $(CROSS)gcc
AR = $(CROSS)ar
STRIP = $(CROSS)strip

ALL_OBJS = igmprt.o proxy.o igmpv2.o util.o filter.o

CCFLAGS += -DHND_FIX -Wall -I../../include ${WLAN_StdIncPathA} -I../../common/include -I../shared -s -Os -fomit-frame-pointer
LDFLAGS += -Wl,-allow-shlib-undefined 
LDFLAGS += -L$(TOP)/netconf -L$(INSTALLDIR)/netconf/usr/lib -lnetconf

ifeq ($(LINUX_VERSION),2_6_36)
CCFLAGS += -DLINUX_2_6_36
endif

ifneq ($(wildcard ./prebuilt),)
all:
	-cp prebuilt/igmp igmp
else
all: igmp
endif

igmp: $(ALL_OBJS)
	$(CC) $(CCFLAGS) $(LDFLAGS) -o $@ $(ALL_OBJS) $(LDLIBS)
	$(STRIP) $@

$(ALL_OBJS): %.o: %.c
	@echo " "
	@echo "Making $<"
	$(CC) $(CCFLAGS) -c $<

install: all
	install -d $(INSTALLDIR)/igmp/usr/sbin
	install -m 755 igmp $(INSTALLDIR)/igmp/usr/sbin

.PHONY:	clean
clean:
	-rm -f *.o *~ core igmp
