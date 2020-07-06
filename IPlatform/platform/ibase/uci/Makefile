#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

UCI_VERSION=2013-01-04
UCI_RELEASE=1

PKG_NAME:=uci
PKG_VERSION:=$(UCI_VERSION)$(if $(UCI_RELEASE),.$(UCI_RELEASE))
PKG_RELEASE:=1
PKG_REV:=e4516d01a7d2b0a5a8def7b5791c7d4032138287

PKG_LICENSE:=LGPLv2.1 GPLv2
PKG_LICENSE_FILES:=

PKG_MAINTAINER:=Felix Fietkau <nbd@openwrt.org>

PKG_BUILD_PARALLEL:=0

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

# set to 1 to enable debugging
DEBUG=

define Package/libuci
  SECTION:=libs
  SUBMENU:=Uci
  CATEGORY:=TP-LINK ibase
  TITLE:=C library for the Unified Configuration Interface (UCI)
endef

define Package/uci
  SECTION:=base
  SUBMENU:=Uci
  CATEGORY:=TP-LINK ibase
  DEPENDS:=+libuci
  TITLE:=Utility for the Unified Configuration Interface (UCI)
endef

define Package/libuci-lua
  SECTION=libs
  SUBMENU:=Uci
  CATEGORY=TP-LINK ibase
  DEPENDS:=+libuci +liblua
  TITLE:=Lua plugin for UCI
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include
TARGET_LDFLAGS += -L$(STAGING_DIR)/usr/lib

CMAKE_OPTIONS = \
	-DLUAPATH=/usr/lib/lua \
	$(if $(DEBUG),-DUCI_DEBUG=ON)

define Package/libuci/install
	$(INSTALL_DIR) $(1)/lib
	$(CP) $(PKG_BUILD_DIR)/libuci.so* $(1)/lib/
endef

define Package/libuci-lua/install
	$(INSTALL_DIR) $(1)/usr/lib/lua
	$(CP) $(PKG_BUILD_DIR)/lua/uci.so $(1)/usr/lib/lua/
endef

define Package/uci/install
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/uci $(1)/sbin/
	$(CP) ./filesystem/* $(1)/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/uci{,_config,map}.h $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libuci.so* $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libucimap.a $(1)/usr/lib
endef

$(eval $(call BuildPackage,uci))
$(eval $(call BuildPackage,libuci))
$(eval $(call BuildPackage,libuci-lua))
