#
# Samba Makefile
#
# $ Copyright Open Broadcom Corporation $
#
#
# <<Broadcom-WL-IPTag/Open:>>
#
# $Id: Makefile,v 1.6 2010-07-17 02:57:34 kenlo Exp $
#

SAMBA_SRC=samba-3.0.25b/source
PASSWD_SRC=genpasswd

CFLAGS += -DLINUX
ifeq (2_6_36,$(LINUX_VERSION))
CFLAGS += -DHAVE_BCM_RECVFILE
endif

DIRS := ${SAMBA_SRC} ${PASSWD_SRC}

.PHONY: all
all: samba samba_pass

.PHONY: configure
configure:
	[ -f $(SAMBA_SRC)/Makefile ] || \
	(cd $(SAMBA_SRC); \
	 export SMB_BUILD_CC_NEGATIVE_ENUM_VALUES=yes ; \
	 export libreplace_cv_READDIR_GETDIRENTRIES=no ; \
	 export libreplace_cv_READDIR_GETDENTS=no ; \
	 export linux_getgrouplist_ok=no ; \
	 export samba_cv_REPLACE_READDIR=no ; \
	 export samba_cv_HAVE_WRFILE_KEYTAB=yes ; \
	 export samba_cv_HAVE_KERNEL_OPLOCKS_LINUX=yes ; \
	 export samba_cv_HAVE_IFACE_IFCONF=yes ; \
	 export samba_cv_USE_SETRESUID=yes ; \
	 CC=$(CC) ./configure \
	    --target=mipsel-uclibc-linux26 \
	    --host=mipsel-uclibc-linux26 \
	    --build=`/bin/arch`-linux \
	    --enable-shared \
	    --disable-static \
	    --disable-cups \
	    --disable-iprint \
	    --disable-pie \
	    --disable-fam \
	    --localstatedir=/tmp/samba/lib/ \
	    --with-configdir=/tmp/samba/lib/ \
	    --with-privatedir=/tmp/samba/private \
	    --with-lockdir=/tmp/samba/var/locks \
	    --with-piddir=/tmp/samba/var/locks \
	    --without-ldap \
	    --without-sys-quotas \
	    --without-cifsmount \
	    --prefix=/tmp/samba; \
	)

.PHONY: samba
samba: configure headers
	+$(MAKE) -C $(SAMBA_SRC)

.PHONY: headers
headers: configure
	+$(MAKE) -C $(SAMBA_SRC) headers

.PHONY: samba_pass
samba_pass:
	+$(if $(wildcard ${PASSWD_SRC}/passwd),@true,$(MAKE) -C ${PASSWD_SRC})

.PHONY: install
install: all install-${PASSWD_SRC}

	install -d $(INSTALLDIR)/usr/sbin	
	install $(SAMBA_SRC)/bin/smbd $(INSTALLDIR)/usr/sbin
	$(STRIP) $(INSTALLDIR)/usr/sbin/smbd
	install $(SAMBA_SRC)/bin/smbpasswd $(INSTALLDIR)/usr/sbin
	$(STRIP) $(INSTALLDIR)/usr/sbin/smbpasswd
	install -d $(INSTALLDIR)/usr/lib
	install -m 755 $(SAMBA_SRC)/bin/libbigballofmud.so $(INSTALLDIR)/usr/lib/libbigballofmud.so.0

install-%:
	+$(MAKE) -C $(patsubst install-%,%,$@) install

.PHONY: clean
clean: $(addprefix clean-,${DIRS})

.PHONY: $(addprefix clean-,${DIRS})
$(addprefix clean-,${DIRS}):
	+$(MAKE) -C $(patsubst clean-%,%,$@) clean
