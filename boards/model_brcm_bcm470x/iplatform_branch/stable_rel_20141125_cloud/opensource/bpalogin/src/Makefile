###########################################################################
# Makefile for BPALogin v2.0
##

##
# Install locations as determined by configure
prefix=/usr/local
exec_prefix=${prefix}
SBINPATH=${exec_prefix}/sbin
ETCPATH=${prefix}/etc

##
# Locations of system utilities as determined by configure
CC=/home/yang/wan/pb42/build/gcc-3.4.4-2.16.1/build_mips/bin/mips-linux-gcc
#CC=gcc
CHKCONFIG=/sbin/chkconfig
INSTALL=/usr/bin/install -c
SED=/bin/sed

##
#
CFLAGS= $(TARGET_CFLAGS) -O2 -Wall #-g
LIBS=-lnsl 
LDFLAGS=

# add by yangxv 
BPA_DEBUG = 
#TP_WEB_LOG = y

ifdef BPA_DEBUG
CFLAGS += -DBPA_DEBUG
endif

ifdef TP_WEB_LOG
CFLAGS 	+= -DTP_WEB_LOG
CFLAGS  += -I ../../web_server/msglogd/
LDFLAGS += -L ../../web_server/msglogd/ -lmsglog
endif

# end add

OBJS = bpalogin.o protocol.o transaction.o gen_md5.o unixmain.o

bpalogin: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS) $(LDFLAGS) $(WEB_LOG_OBJ)

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -fr $(OBJS) bpalogin

distclean: clean
	rm -f bpalogin config.log config.cache config.status Makefile bpalogin.h

#bpalogin.h: bpalogin.h.in
#	$(SED) 's#@@BPALOGIN_CONF@@#$(ETCPATH)/bpalogin.conf#' < bpalogin.h.in > bpalogin.h

$(OBJS): bpalogin.h

install: bpalogin bpalogin.init bpalogin.bsd
	case `uname -s` in \
	  Linux*) \
	    mkdir -p $(RPM_BUILD_ROOT)$(ETCPATH)/rc.d/init.d ; \
	    $(INSTALL) -m 755 bpalogin.init \
	               $(RPM_BUILD_ROOT)$(ETCPATH)/rc.d/init.d/bpalogin ; \
	    \
	    if [ -z "$(RPM_BUILD_ROOT)" -a -x "$(CHKCONFIG)" ]; then \
	      $(CHKCONFIG) --add bpalogin ; \
	    fi ; \
	    ;; \
	  \
	  FreeBSD*) \
	    mkdir -p $(ETCPATH)/rc.d ; \
	    $(INSTALL) -m 755 bpalogin.bsd $(ETCPATH)/rc.d/bpalogin.sh ; \
	    ;; \
	    \
	  *) \
	    echo "###" ; \
	    echo "# You will need to add BPALogin to your startup scripts." ; \
	    echo "###" ; \
	    ;; \
	esac ; \
	
	mkdir -p $(RPM_BUILD_ROOT)$(SBINPATH)
	mkdir -p $(RPM_BUILD_ROOT)$(ETCPATH)
	$(INSTALL) -m 755 bpalogin $(RPM_BUILD_ROOT)$(SBINPATH)
	$(INSTALL) -m 600 bpalogin.conf $(RPM_BUILD_ROOT)$(ETCPATH)
