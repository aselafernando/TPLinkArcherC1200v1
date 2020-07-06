# <<Broadcom-WL-IPTag/Proprietary:>>


AUTOMAKE_OPTIONS = foreign

SRCDIR = $(BASEDIR)/src
VISSHARED=../shared
BCMWIFIDIR=$(SRCDIR)/shared/bcmwifi/src
SHAREDDIR=$(SRCDIR)/shared

CFLAGS=-Wall -Werror -I$(SRCDIR)/shared/bcmwifi/include/ \
	-I$(SRCDIR)/common/include/ \
	-I$(SRCDIR)/include/ -I../include/ \
	-I$(SRCDIR)/../components/opensource/libxml2/include/ \
	-I$(SRCDIR)/../components/shared/ \
	-DVERSION=\"$(VERSION)\" -DPROG="\"$(PACKAGE)\""

LDFLAGS=-L$(SRCDIR)/../components/opensource/libxml2/installbin/lib \
		-lxml2 $(USRLIBDIR)/libm.a -L$(SRCBASE_ROUTER)/nvram -L$(INSTALLDIR)/nvram/usr/lib -lnvram
if ISBCMINTERNAL
CFLAGS  += -DBCMINTERNAL
endif
CFLAGS  += -DBCMDBG

bin_PROGRAMS = vis-datacollector
vis_datacollector_SOURCES = $(SHAREDDIR)/bcmutils.c \
			$(SHAREDDIR)/bcmxtlv.c $(SHAREDDIR)/bcm_app_utils.c \
			$(BCMWIFIDIR)/bcmwifi_channels.c vis_common.c \
			$(VISSHARED)/vis_xmlshared.c $(VISSHARED)/vis_sock_util.c \
			$(VISSHARED)/vis_shared_util.c \
			vis_socketclient.c vis_xmlutility.c vis_savedata.c\
			vis_wl.c vis_wlmetrics.c vis_wlcongestion.c vis_linux_main.c
