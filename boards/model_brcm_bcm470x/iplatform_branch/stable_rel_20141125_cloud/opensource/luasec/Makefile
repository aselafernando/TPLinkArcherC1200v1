#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luasec
PKG_VERSION:=0.4
PKG_RELEASE:=1

# PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
# PKG_SOURCE_URL:=http://luaforge.net/frs/download.php/4255
# PKG_MD5SUM:=712158d60207bdbb6215fc7e07d8db24

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

# =====================================================================================
ifneq ($(CONFIG_EX_GCC),)
  ifneq ($(CONFIG_LIB_GCC_PATH),)
    export USEEXTERNLIBGCC:=-L$(TOOLCHAIN_ROOT_DIR)$(call qstrip,$(CONFIG_LIB_GCC_PATH))
  endif
endif
# =====================================================================================

define Package/luasec
  SUBMENU:=administration
  SECTION:=TP-LINK
  CATEGORY:=TP-LINK iplatform apps
  TITLE:=LuaSec
  URL:=http://luasec.luaforge.net/
  DEPENDS:=+lua +libopenssl +luasocket
endef

define Package/luasec/description
  LuaSec is a binding for OpenSSL library to provide TLS/SSL communication.
endef

define Build/Configure
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

MAKE_PATH = ./src

MAKE_FLAGS += \
	INCDIR="$(TARGET_CPPFLAGS)" \
	LIBDIR="$(if $(TARGET_LDFLAGS_FOR_LD),$(TARGET_LDFLAGS_FOR_LD),$(TARGET_LDFLAGS))" \
	CC="$(TARGET_CC) $(TARGET_CFLAGS) -std=gnu99" \
	LD="$(TARGET_CROSS)ld -shared" \
	LUACPATH="$(PKG_INSTALL_DIR)/usr/lib/lua" \
	LUAPATH="$(PKG_INSTALL_DIR)/usr/lib/lua" \
	linux \

define Package/luasec/install
	$(INSTALL_DIR) $(1)/usr/lib/lua
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/lua/ssl.so $(1)/usr/lib/lua/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/lua/ssl.lua $(1)/usr/lib/lua/
	$(INSTALL_DIR) $(1)/usr/lib/lua/ssl
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/lua/ssl/https.lua $(1)/usr/lib/lua/ssl/
endef

$(eval $(call BuildPackage,luasec))
