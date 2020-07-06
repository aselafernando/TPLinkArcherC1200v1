#
# Copyright (c) 2013 The Linux Foundation. All rights reserved.
#

include	$(TOPDIR)/rules.mk

PKG_NAME:=fdk-aac
PKG_VERSION:=0.1.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://downloads.sourceforge.net/opencore-amr/
PKG_MD5SUM:=87ed9dd60e933b5cb4fe72bdc749480e

include	$(INCLUDE_DIR)/package.mk

define Package/fdk-aac
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Open Source Fraunhofer AAC Codec
  URL:=https://github.com/mstorsjo/fdk-aac
endef

define Package/fdk-aac/description
  The Open Source Fraunhofer FDK AAC library is purported to be a "high quality"
  aac implementation.
endef

ifeq ($(CONFIG_LINUX_KARCH),"mips")
  ARCH_RELATED_OPTIONS=-mips32
endif

define Build/Configure
	$(call Build/Configure/Default, \
		--disable-asm --enable-static=no \
	)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		CPPFLAGS=$(ARCH_RELATED_OPTIONS) \
		all install
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/fdk-aac
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/include/fdk-aac/* \
		$(1)/usr/include/fdk-aac/
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/libfdk-aac.{la,so*} \
		$(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/fdk-aac.pc \
		$(1)/usr/lib/pkgconfig
endef

define Package/fdk-aac/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/libfdk-aac.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,fdk-aac))
