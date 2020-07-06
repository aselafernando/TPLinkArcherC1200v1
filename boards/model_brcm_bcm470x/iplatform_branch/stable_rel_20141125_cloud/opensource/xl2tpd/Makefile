#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=xl2tpd
PKG_VERSION:=1.1.12
PKG_RELEASE:=1


PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_PARALLEL:=1
#PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=TP-LINK
  CATEGORY:=TP-LINK iplatform apps
  SUBMENU:=Network
  DEPENDS:=+kmod-pppol2tp +kmod-ppp +pppd +ip +resolveip
  TITLE:=An L2TP (Layer 2 Tunneling Protocol) daemon
endef

define Package/$(PKG_NAME)/description
l2tpd is the open source implementation of the L2TP tunneling protocol (RFC2661).
It does implement both LAC and LNS role in a L2TP networking architecture. The
main goal of this protocol is to tunnel PPP frame trough an IP network.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

define Build/Configure
$(call Build/Configure/Default,, \
	UNAME_S="Linux" \
	UNAME_R="$(LINUX_VERSION)" \
	UNAME_M="$(ARCH)" \
)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) clean 
	$(MAKE) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS) \
					TARGET_CFLAGS="$(TARGET_CFLAGS) -DQSDK=1"
endef

#define Package/$(PKG_NAME)/conffiles
#/etc/xl2tpd/xl2tpd.conf
#/etc/xl2tpd/xl2tp-secrets
#/etc/ppp/options.xl2tpd
#endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/xl2tpd $(1)/usr/sbin/

	$(INSTALL_DIR) $(1)/lib/netifd/proto
	$(INSTALL_BIN) ./filesystem/lib/netifd/proto/l2tp.sh $(1)/lib/netifd/proto
endef

$(eval $(call BuildPackage,xl2tpd))
