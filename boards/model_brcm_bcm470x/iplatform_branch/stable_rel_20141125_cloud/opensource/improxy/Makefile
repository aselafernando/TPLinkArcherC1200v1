#
# Copyright (c) 2014 The Linux Foundation. All rights reserved.
# Copyright (C) 2010-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=improxy
PKG_VERSION:=1.0
PKG_RELEASE:=1
PKG_REV:=08bf9b8f378409e56df11a4dd796b2d309121ee5

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://github.com/haibbo/improxy.git
PKG_BRANCH:=OpenWRT/uClibc
PKG_SOURCE_VERSION:=$(PKG_REV)

include $(INCLUDE_DIR)/package.mk

define Package/improxy
  SECTION:=TP-LINK
  CATEGORY:=TP-LINK iplatform apps
  SUBMENU:=Routing and Redirection
#  DEPENDS:=+libpthread
  TITLE:=Multicast Proxy for IGMP/MLD
endef

define Package/improxy/description
	Improxy is an IGMP/MLD Proxy daemon written in c for Linux. It is lightweight and do not depend on any thirdparty library.
endef

define Package/improxy/conffiles
/etc/config/improxy
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/improxy \
		CC="$(TARGET_CC)" \
		CXX="$(TARGET_CXX)" \
		CFLAGS="$(TARGET_CFLAGS)"
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)/improxy
	$(CP) -u ./src/* $(PKG_BUILD_DIR)/improxy
endef

define Package/improxy/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./filesystem/etc/config/improxy.config $(1)/etc/config/improxy
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./filesystem/etc/init.d/improxy.init $(1)/etc/init.d/improxy
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/improxy/improxy $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_DATA) ./filesystem/etc/hotplug.d/iface/improxy.hotplug $(1)/etc/hotplug.d/iface/50-improxy
	$(INSTALL_DIR) $(1)/etc/hotplug.d/firewall
	$(INSTALL_DATA) ./filesystem/etc/hotplug.d/firewall/improxy.hotplug $(1)/etc/hotplug.d/firewall/50-improxy
	$(INSTALL_DIR) $(1)/etc/ppp/ip-up.d
	$(INSTALL_BIN) ./filesystem/etc/ppp/ip-up.d/99improxy.sh $(1)/etc/ppp/ip-up.d/99improxy.sh
	$(INSTALL_DIR) $(1)/etc/ppp/ip-down.d
	$(INSTALL_BIN) ./filesystem/etc/ppp/ip-down.d/99improxy.sh $(1)/etc/ppp/ip-down.d/99improxy.sh
endef

$(eval $(call BuildPackage,improxy))
