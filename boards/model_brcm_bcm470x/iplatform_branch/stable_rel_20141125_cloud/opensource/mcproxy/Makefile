#
# Copyright (c) 2014 The Linux Foundation. All rights reserved.
# Copyright (C) 2010-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mcproxy
PKG_VERSION:=1.1.0.y
PKG_RELEASE:=1
PKG_REV:=39632a422d2d277664a4e63e6eba43d2601db62b

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://github.com/mcproxy/mcproxy.git
PKG_BRANCH:=OpenWRT/uClibc
PKG_SOURCE_VERSION:=$(PKG_REV)

include $(INCLUDE_DIR)/package.mk

define Package/mcproxy
  SECTION:=net
  CATEGORY:=TP-LINK iplatform apps
  SUBMENU:=Routing and Redirection
  DEPENDS:=+libpthread +libstdcpp
  TITLE:=Multicast Proxy for IGMP/MLD
  URL:=http://mcproxy.realmv6.org/
endef

define Package/mcproxy/description
	mcproxy is a free & open source implementation of the IGMP/MLD proxy function (see  RFC 4605) for Linux systems.
endef

define Package/mcproxy/conffiles
/etc/config/mcproxy
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/mcproxy \
		CC="$(TARGET_CC)" \
		CXX="$(TARGET_CXX)" \
		CFLAGS="$(TARGET_CFLAGS)"
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) -u ./src/* $(PKG_BUILD_DIR)/
endef

define Package/mcproxy/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./filesystem/etc/config/mcproxy.config $(1)/etc/config/mcproxy
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./filesystem/etc/init.d/mcproxy.init $(1)/etc/init.d/mcproxy
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mcproxy/mcproxy $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_DATA) ./filesystem/etc/hotplug.d/iface/mcproxy.hotplug $(1)/etc/hotplug.d/iface/50-mcproxy
endef

$(eval $(call BuildPackage,mcproxy))
