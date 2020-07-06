CC	= gcc
COPTS	= -O2 -g
CFLAGS	= -I$(STAGING_DIR)/usr/include $(COPTS) -I.. -I../../include -fPIC
LDFLAGS	= -L$(STAGING_DIR)/usr/include -L$(STAGING_DIR)/lib -shared
LDADD   = -lutil
INSTALL	= install -o root
prefix  = /usr/local

PLUGINS = pptpd-logwtmp.so

# include dependencies if present
ifeq (.depend,$(wildcard .depend))
include .depend
endif

all:	$(PLUGINS)

%.so: %.c
	$(CC) -o $@ $(LDFLAGS) $(CFLAGS) $^ $(LDADD)

LIBDIR	= $(DESTDIR)$(prefix)/lib/pptpd

install: $(PLUGINS)
	$(INSTALL) -d $(LIBDIR)
	$(INSTALL) $? $(LIBDIR)

uninstall:
	rm -f $(LIBDIR)$(PLUGINS)

clean:
	rm -f *.o *.so *.a

depend:
	$(CPP) -M $(CFLAGS) *.c >.depend
