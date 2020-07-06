#
# pppd makefile for Linux
# $Id: Makefile.linux,v 1.66 2004/11/13 12:02:22 paulus Exp $
#

# Default installation locations
DESTDIR = $(PKG_INSTALL_DIR)/usr
BINDIR = $(DESTDIR)/sbin
MANDIR = $(DESTDIR)/share/man/man8
INCDIR = $(DESTDIR)/include

TARGETS = pppd

PPPDSRCS = main.c magic.c fsm.c lcp.c ipcp.c upap.c chap-new.c md5.c ccp.c \
	   ecp.c auth.c options.c sys-linux.c md4.c chap_ms.c tty.c \
	   demand.c utils.c  eap.c chap-md5.c

HEADERS = ccp.h chap-new.h ecp.h fsm.h ipcp.h \
	lcp.h magic.h md5.h patchlevel.h pathnames.h pppd.h \
	upap.h eap.h

MANPAGES = pppd.8
PPPDOBJS = main.o magic.o fsm.o lcp.o ipcp.o upap.o chap-new.o md5.o ccp.o \
	   ecp.o auth.o options.o demand.o utils.o sys-linux.o tty.o\
	   eap.o chap-md5.o

#
# include dependencies if present
ifeq (.depend,$(wildcard .depend))
include .depend
endif

#CC = gcc
#CC = /home/yang/wan/pb42/build/gcc-3.4.4-2.16.1/build_mips/bin/mips-linux-gcc
COPTS = $(TARGET_CFLAGS) -O -fomit-frame-pointer -pipe -Wall
LIBS =

# add by yangxv, 2007.12.10
# PPPD_DEBUG=y
#TP_WEB_LOG=n

# add by yangxv
PPPOEOBJS = pppoe/discovery.o pppoe/if.o pppoe/common.o pppoe/debug.o pppoe/plugin.o
PPPOERCS  = pppoe/discovery.c pppoe/if.c pppoe/common.c pppoe/debug.c pppoe/plugin.c
CFLAGSPPPOE = -O2 -fomit-frame-pointer  -I ../include/linux -I . -I ..


ifdef PPPD_DEBUG
CFLAGSPPPOE += -DPPPD_DEBUG
COPTS += -DPPPD_DEBUG
endif

ifdef TP_WEB_LOG
CFLAGSPPPOE += -DTP_WEB_LOG
COPTS += -DTP_WEB_LOG
endif

ifeq ($(MODULE_HWNAT), y)
CFLAGSPPPOE += -DMODULE_HWNAT
COPTS += -DMODULE_HWNAT
endif

# pptp protocol is compile into pppd, by lqt, 2010-3-29
INCLUDE_PPTP=y

ifdef INCLUDE_PPTP
PPTPOBJ = pptp/pptp.o pptp/pptp_callmgr.o pptp/pptp_ctrl.o pptp/pptp_quirks.o \
		pptp/util.o pptp/vector.o pptp/orckit_quirks.o pptp/dirutil.o
endif

# add by lqt, 2010-3-22
PPPOL2TP=y

ifdef PPPOL2TP
PPPDOBJS += l2tp/pppol2tp.o
endif

# Uncomment the next 2 lines to include support for Microsoft's
# MS-CHAP authentication protocol.  Also, edit plugins/radius/Makefile.linux.
CHAPMS=y
USE_CRYPT=y
# Don't use MSLANMAN unless you really know what you're doing.
#MSLANMAN=y
# Uncomment the next line to include support for MPPE.  CHAPMS (above) must
# also be enabled.  Also, edit plugins/radius/Makefile.linux.
MPPE=y

# Uncomment the next line to include support for PPP packet filtering.
# This requires that the libpcap library and headers be installed
# and that the kernel driver support PPP packet filtering.
# FILTER=y

# Uncomment the next line to enable multilink PPP (enabled by default)
# Linux distributions: Please leave multilink ENABLED in your builds
# of pppd!
# HAVE_MULTILINK=y

# Uncomment the next line to enable the TDB database (enabled by default.)
# If you enable multilink, then TDB is automatically enabled also.
# Linux distributions: Please leave TDB ENABLED in your builds.
# USE_TDB=y

HAS_SHADOW=y
#USE_PAM=y

# add explain by yangxv, 2007.11.15
# I have deleted ipv6cp.c ipv6cp.h eui64.c eui64.h,but I have not delete the macro
# HAVE_INET6.So,if you want to add the support for ipv6 only add the files that I mentioned.
# You can find four files from the open source software pppd. 
#HAVE_INET6=y
HAVE_INET6=y

# Enable plugins
# PLUGIN=y

# Enable Microsoft proprietary Callback Control Protocol
#CBCP=y

# Enable EAP SRP-SHA1 authentication (requires libsrp)
#USE_SRP=y

MAXOCTETS=y

INCLUDE_DIRS= -I../include

COMPILE_FLAGS= -DHAVE_PATHS_H  -DHAVE_MMAP

CFLAGS += $(COPTS) $(COMPILE_FLAGS) $(INCLUDE_DIRS)

# added by yangxv for web log, 2008.10.31
ifdef TP_WEB_LOG
CFLAGS  += -I ../../../web_server/msglogd/
LDFLAGS += -L ../../../web_server/msglogd/ -lmsglog
endif

ifdef CHAPMS
CFLAGS   += -DCHAPMS=1
NEEDDES=y
PPPDOBJS += md4.o chap_ms.o
HEADERS	+= md4.h chap_ms.h
ifdef MSLANMAN
CFLAGS   += -DMSLANMAN=1
endif
ifdef MPPE
CFLAGS   += -DMPPE=1
endif
endif

ifdef INCLUDE_NEW_MD4
CFLAGS += -DINCLUDE_NEW_MD4=1
PPPDOBJS += md4-new.o
PPPDSRCS += md4-new.c
HEADERS += md4-new.h
endif

# EAP SRP-SHA1
ifdef USE_SRP
CFLAGS	+= -DUSE_SRP -DOPENSSL -I/usr/local/ssl/include
LIBS	+= -lsrp -L/usr/local/ssl/lib -lcrypto
TARGETS	+= srp-entry
EXTRAINSTALL = $(INSTALL) -s -c -m 555 srp-entry $(BINDIR)/srp-entry
MANPAGES += srp-entry.8
EXTRACLEAN += srp-entry.o
NEEDDES=y
else
# OpenSSL has an integrated version of SHA-1, and its implementation
# is incompatible with this local SHA-1 implementation.  We must use
# one or the other, not both.
PPPDSRCS += sha1.c
HEADERS += sha1.h
PPPDOBJS += sha1.o
endif

ifdef HAS_SHADOW
CFLAGS   += -DHAS_SHADOW
#LIBS     += -lshadow $(LIBS)
endif

#ifneq ($(wildcard /usr/include/crypt.h),)
CFLAGS   += -DHAVE_CRYPT_H=1
#endif
#ifneq ($(wildcard /usr/lib/libcrypt.*),)
LIBS	+= -lcrypt
#endif

ifdef NEEDDES
ifndef USE_CRYPT
LIBS     += -ldes $(LIBS)
else
CFLAGS   += -DUSE_CRYPT=1
endif
PPPDOBJS += pppcrypt.o
HEADERS += pppcrypt.h
endif

# For "Pluggable Authentication Modules", see ftp.redhat.com:/pub/pam/.
ifdef USE_PAM
CFLAGS   += -DUSE_PAM
LIBS     += -lpam -ldl
endif

# Multi-linnk
ifdef HAVE_MULTILINK
	# Multilink implies the use of TDB
	USE_TDB=y

	CFLAGS += -DHAVE_MULTILINK
	PPPDSRCS += multilink.c
	PPPDOBJS += multilink.o
endif

# TDB
ifdef USE_TDB
	CFLAGS += -DUSE_TDB=1
	PPPDSRCS += tdb.c spinlock.c
	PPPDOBJS += tdb.o spinlock.o
	HEADERS += tdb.h spinlock.h
endif

#pptp & l2tp
ifdef PPPOL2TP
	CFLAGS += -DINCLUDE_PPPOL2TP
endif

ifdef INCLUDE_PPTP
	CFLAGS += -DINCLUDE_PPTP_PLUGIN
endif

# Lock library binary for Linux is included in 'linux' subdirectory.
ifdef LOCKLIB
LIBS     += -llock
CFLAGS   += -DLOCKLIB=1
endif

ifdef PLUGIN
CFLAGS	+= -DPLUGIN
LDFLAGS	+= -Wl,-E
LIBS	+= -ldl
endif

ifdef FILTER
ifneq ($(wildcard /usr/include/pcap-bpf.h),)
LIBS    += -lpcap
CFLAGS  += -DPPP_FILTER
endif
endif

ifdef HAVE_INET6
     PPPDSRCS += ipv6cp.c eui64.c
     HEADERS  += ipv6cp.h eui64.h
     PPPDOBJS += ipv6cp.o eui64.o
     CFLAGS   += -DINET6=1
endif

ifdef CBCP
     PPPDSRCS += cbcp.c
     PPPDOBJS += cbcp.o
     CFLAGS += -DCBCP_SUPPORT
     HEADERS += cbcp.h
endif

ifdef MAXOCTETS
     CFLAGS += -DMAXOCTETS
endif

INSTALL= install

all: $(TARGETS)

install: pppd
	mkdir -p $(BINDIR) $(MANDIR)
	$(EXTRAINSTALL)
	$(INSTALL) -s -c -m 555 pppd $(BINDIR)/pppd
	if chgrp pppusers $(BINDIR)/pppd 2>/dev/null; then \
	  chmod o-rx,u+s $(BINDIR)/pppd; fi
	$(INSTALL) -c -m 444 pppd.8 $(MANDIR)

# add pptpobj, by lqt, 2010-3-29	
pppd: $(PPPOEOBJS) $(PPPDOBJS) $(PPTPOBJ) 
	$(CC) $(CFLAGS) $(LDFLAGS) -o pppd  $(PPPDOBJS) $(PPPOEOBJS) $(PPTPOBJ) $(LIBS)

srp-entry:	srp-entry.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ srp-entry.c $(LIBS)

install-devel:
	mkdir -p $(INCDIR)/pppd
	$(INSTALL) -c -m 644 $(HEADERS) $(INCDIR)/pppd

# add pptp obj, by lqt 
clean:
#	rm -f $(PPPDOBJS) $(PPPOEOBJS) $(EXTRACLEAN) $(TARGETS) *~ #* core
	rm -f $(PPPDOBJS) $(PPPOEOBJS) $(PPTPOBJ) $(EXTRACLEAN) $(TARGETS) *~ #* core

depend:
	$(CPP) -M $(CFLAGS) $(PPPDSRCS) >.depend

#$(PPPOEOBJS):
#	$(CC) $(CFLAGSPPPOE) -c $(PPPOERCS)
# add by yangxv
pppoe/plugin.o: pppoe/plugin.c
	$(CC) $(CFLAGSPPPOE) '-DRP_VERSION="3.3"' -c $< -o $@

pppoe/debug.o: pppoe/debug.c
	$(CC) $(CFLAGSPPPOE) '-DVERSION="3.3"' -c $< -o $@

pppoe/common.o: pppoe/common.c
	$(CC) $(CFLAGSPPPOE) '-DVERSION="3.3"' -c $< -o $@

pppoe/if.o: pppoe/if.c
	$(CC) $(CFLAGSPPPOE) '-DVERSION="3.3"' -c $< -o $@

pppoe/discovery.o: pppoe/discovery.c
	$(CC) $(CFLAGSPPPOE) '-DVERSION="3.3"' -c $< -o $@

#add by lqt, 2010-3-22, use pppoe cflag
l2tp/pppol2tp.o: l2tp/pppol2tp.c
	$(CC) $(CFLAGSPPPOE) -c $< -o $@

# for pptp, by lqt, 2010-3-29
#PPTPOBJ = pptp/pptp.o pptp/pptp_callmgr.o pptp/pptp_ctrl.o pptp/pptp_quirks.o \
#	pptp/util.o pptp/vector.o pptp/orckit_quirks.o pptp/dirutil.o
CFLAGSPPTP = -O2 -fomit-frame-pointer -I ..
ifdef PPPD_DEBUG
CFLAGSPPTP += -DPPPD_DEBUG
endif

ifdef TP_WEB_LOG
CFLAGSPPTP += -DTP_WEB_LOG
endif

pptp/pptp.o: pptp/pptp.c
	$(CC) $(CFLAGSPPTP) -c	pptp/pptp.c -o pptp/pptp.o
	
pptp/pptp_callmgr.o: pptp/pptp_callmgr.c
#	$(CC) $(CFLAGSPPTP) '-DPPPD_VERSION="2.4.3"' -c	pptp/pptp_callmgr.c -o pptp/pptp_callmgr.o
	$(CC) $(CFLAGSPPTP) -c	pptp/pptp_callmgr.c -o pptp/pptp_callmgr.o

pptp/pptp_ctrl.o: pptp/pptp_ctrl.c
	$(CC) $(CFLAGSPPTP) -c	pptp/pptp_ctrl.c -o pptp/pptp_ctrl.o

pptp/util.o: pptp/util.c
	$(CC) $(CFLAGSPPTP) -c	pptp/util.c -o pptp/util.o

pptp/vector.o: pptp/vector.c
	$(CC) $(CFLAGSPPTP) -c	pptp/vector.c -o pptp/vector.o

pptp/orckit_quirks.o: pptp/orckit_quirks.c
	$(CC) $(CFLAGSPPTP) -c	pptp/orckit_quirks.c -o pptp/orckit_quirks.o

pptp/dirutil.o: pptp/dirutil.c
	$(CC) $(CFLAGSPPTP) -c pptp/dirutil.c	-o pptp/dirutil.o
	
pptp/pptp_quirks.o: pptp/pptp_quirks.c
	$(CC) $(CFLAGSPPTP) -c pptp/pptp_quirks.c -o pptp/pptp_quirks.o
	
