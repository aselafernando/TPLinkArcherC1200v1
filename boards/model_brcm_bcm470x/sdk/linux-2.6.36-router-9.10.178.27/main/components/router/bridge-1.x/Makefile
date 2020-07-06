.NOTPARALLEL:

DESTDIR=
KERNEL_HEADERS=-I$(LINUXDIR)

INSTALL=/tools/bin/install -c

prefix=/usr
exec_prefix=${prefix}
bindir=${exec_prefix}/bin
sbindir=${exec_prefix}/sbin
mandir=${prefix}/man
distdir = $(PACKAGE)-$(VERSION)

SUBDIRS=libbridge brctl

.PHONY: all
all: ${SUBDIRS}

.PHONY: ${SUBDIRS}
brctl : libbridge
${SUBDIRS} :
	+$(MAKE) ${MFLAGS} -C $@

.PHONY: clean
clean: $(addprefix clean-,${SUBDIRS})

clean-%:
	+$(MAKE) ${MFLAGS} -C $(patsubst clean-%,%,$@) clean

.PHONY: distclean
distclean:	clean
	rm -f config.log
	rm -f config.cache
	rm -f config.status

.PHONY: maintainer-clean
maintainer-clean: distclean
	rm -f Makefile
	rm -f brctl/Makefile
	rm -f libbridge/Makefile
	rm -f doc/Makefile

.PHONY: install
install: all $(addprefix install-,${SUBDIRS})

install-%:
	+$(MAKE) ${MFLAGS} -C $(patsubst install-%,%,$@) install

