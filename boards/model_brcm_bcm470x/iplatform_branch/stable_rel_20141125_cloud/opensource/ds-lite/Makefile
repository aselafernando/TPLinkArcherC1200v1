#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ds-lite
PKG_VERSION:=1
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_PARALLEL:=1

define Package/$(PKG_NAME)/Default
  SUBMENU:=IPv6
  SECTION:=ipv6
  CATEGORY:=TP-LINK iplatform apps
  TITLE:=Dual-Stack Lite (DS-Lite) configuration support
  DEPENDS:=+kmod-ipv6 +kmod-ip6-tunnel
  MAINTAINER:=Steven Barth <steven@midlink.org>
endef

define Package/$(PKG_NAME)
  $(call Package/$(PKG_NAME)/Default)
  TITLE+= Daemon
endef

define Package/$(PKG_NAME)/description
Provides support for Dual-Stack Lite in /etc/config/network.
Refer to http://wiki.openwrt.org/doc/uci/network for
configuration details.
endef

define Build/Compile
endef

define Build/Configure
endef

define Package/ds-lite/install
	$(INSTALL_DIR) $(1)/lib/netifd/proto
	$(INSTALL_BIN) ./files/dslite.sh $(1)/lib/netifd/proto/dslite.sh
endef

$(eval $(call BuildPackage,ds-lite))
