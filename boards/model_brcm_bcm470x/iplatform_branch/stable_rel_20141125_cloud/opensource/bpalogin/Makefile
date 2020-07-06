#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=bpalogin
PKG_VERSION:=2.0.2
PKG_RELEASE:=1


PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=TP-LINK
  CATEGORY:=TP-LINK iplatform apps
  DEPENDS:=
  TITLE:=for BigPond access type which used in Australia
endef

define Package/$(PKG_NAME)/description
The program is need by BigPond access type to handle authentication with server.
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
	$(MAKE) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS) \
					TARGET_CFLAGS="$(TARGET_CFLAGS) -DQSDK=1"
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bpalogin $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/lib/netifd
	$(INSTALL_BIN) ./filesystem/bpa.script $(1)/lib/netifd
	$(INSTALL_DIR) $(1)/lib/netifd/proto
	$(INSTALL_BIN) ./filesystem/bpa.sh $(1)/lib/netifd/proto
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
