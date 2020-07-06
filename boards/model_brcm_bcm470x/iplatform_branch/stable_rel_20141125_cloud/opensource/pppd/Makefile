#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=pppd
PKG_VERSION:=2.4.3
PKG_RELEASE:=8

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

PKG_BUILD_PARALLEL:=1
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=TP-LINK
  CATEGORY:=TP-LINK iplatform apps
  SUBMENU:=Network
  DEPENDS:=+kmod-ppp +kmod-pptp +kmod-pppoe +resolveip
  TITLE:=PPP daemon (PPPoE/PPTP/L2TP plugin included)
endef

define Package/$(PKG_NAME)/description
This package contains the PPP (Point-to-Point Protocol) daemon for PPPoE, L2TP and PPTP.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
$(call Build/Configure/Default,, \
	UNAME_S="Linux" \
	UNAME_R="$(LINUX_VERSION)" \
	UNAME_M="$(ARCH)" \
)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/pppd $(TARGET_CONFIGURE_OPTS) \
		TARGET_CFLAGS="$(TARGET_CFLAGS)  -DQSDK=1"
endef

define Build/Install
	$(MAKE) -C $(PKG_BUILD_DIR)/pppd install-devel PKG_INSTALL_DIR="$(PKG_INSTALL_DIR)"
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/pppd $(1)/usr/include/
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/pppd/pppd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/ppp
	$(INSTALL_CONF) ./filesystem/etc/ppp/chap-secrets $(1)/etc/ppp/
	$(INSTALL_DATA) ./filesystem/etc/ppp/filter $(1)/etc/ppp/
	$(INSTALL_DATA) ./filesystem/etc/ppp/options $(1)/etc/ppp/
	ln -sf /tmp/resolv.conf.ppp $(1)/etc/ppp/resolv.conf
	$(INSTALL_DIR) $(1)/lib/netifd/proto
	$(INSTALL_BIN) ./filesystem/lib/netifd/proto/ppp.sh $(1)/lib/netifd/proto/
	$(INSTALL_BIN) ./filesystem/lib/netifd/proto/pppv6.sh $(1)/lib/netifd/proto/
	$(INSTALL_BIN) ./filesystem/lib/netifd/ppp-up $(1)/lib/netifd/
	$(INSTALL_BIN) ./filesystem/lib/netifd/pppv6-up $(1)/lib/netifd/
	$(INSTALL_BIN) ./filesystem/lib/netifd/ppp-dhcp6c.script $(1)/lib/netifd/
	$(INSTALL_BIN) ./filesystem/lib/netifd/ppp-down $(1)/lib/netifd/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))

