#
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
PKG_NAME:=accel-pptpd
PKG_VERSION:=0.8.5
PKG_RELEASE:=1
PKG_DRIVER_NAME:=pptp
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
# PKG_SOURCE_URL:=@SF/poptop
PKG_MD5SUM:=36f9f45c6ffa92bc3b6e24ae2d053505

PKG_INSTALL:=1
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=net
  CATEGORY:=TP-LINK iplatform apps
  TITLE:=ACCEL-PPTP pptp server
  DEPENDS:=+kmod-ppp +kmod-mppe +pppd
  URL:=http://www.poptop.org/
  SUBMENU:=VPN
endef

CONFIGURE_ARGS += \
	--enable-bcrelay \

CONFIGURE_VARS += \
	ac_cv_header_libintl_h=no \
	

MAKE_FLAGS += \
	COPTS="$(TARGET_CFLAGS)" \
	INSTALL="install" \
	PPPD_PPTP_INCLUDED=1
	

define Package/$(PKG_NAME)/conffiles
/etc/pptpd.conf
/etc/ppp/options.pptpd
/etc/config/pptpd
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef


define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/sbin/bcrelay \
		$(PKG_INSTALL_DIR)/usr/sbin/pptpctrl \
		$(PKG_INSTALL_DIR)/usr/sbin/pptpd \
		$(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib/pptpd
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pptpd/* $(1)/usr/lib/pptpd/
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) ./filesystem/etc/pptpd.conf $(1)/etc/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./filesystem/etc/init.d/pptpd $(1)/etc/init.d/pptpd
	$(INSTALL_DIR) $(1)/etc/ppp
	$(INSTALL_DATA) ./filesystem/etc/ppp/options.pptpd $(1)/etc/ppp/options.pptpd.sample
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./filesystem/etc/config/pptpd $(1)/etc/config/pptpd
endef

$(eval $(call BuildPackage,$(PKG_NAME)))