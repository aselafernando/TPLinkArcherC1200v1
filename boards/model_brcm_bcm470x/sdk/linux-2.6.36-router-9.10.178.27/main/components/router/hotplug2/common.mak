# vim:set sw=8 nosta:
.NOTPARALLEL:

PREFIX=
CFLAGS=-Os -DHAVE_RULES -Wall -g
LDFLAGS=-g

INSTALL=install -c -m 644
INSTALL_BIN=install -c -m 755
INSTALL_DIR=install -d


.PHONY: install install-recursive

.PHONY: all all-recursive
all: all-recursive
all-recursive: ${SUBDIRS}

.PHONY: ${SUBDIRS}
${SUBDIRS} :
	+$(MAKE) -C $@ all

MAKEDEP=-gcc $(CFLAGS) -MM $(wildcard *.c *.cc) > .depend
.PHONY: dep
dep: dep-recursive .depend
	-gcc $(CFLAGS) -MM $(wildcard *.c *.cc) > .depend

.PHONY: dep-recursive
dep-recursive: $(addprefix dep-,${SUBDIRS})
dep-%:
	+$(MAKE) -C $(patsubst dep-%,%,$@) dep
	
-include .depend

.PHONY: clean clean-recursive
clean: clean-recursive
	$(RM) $(wildcard *.o *.so *.a $(BINS)) .depend
clean-recursive: $(addprefix clean-,${SUBDIRS})

clean-%:
	+$(MAKE) -C $(patsubst clean-%,%,$@) clean

.PHONY: install install-recursive
install: all install-recursive
install-recursive: $(addprefix install-,${SUBDIRS})

install-%:
	+$(MAKE) -C $(patsubst install-%,%,$@) install

