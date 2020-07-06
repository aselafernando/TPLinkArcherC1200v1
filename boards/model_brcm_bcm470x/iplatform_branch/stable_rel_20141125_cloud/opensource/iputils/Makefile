#
# Copyright (C) 2010-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=iputils
PKG_VERSION:=20101006
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_PARALLEL:=1

ifeq ($(CONFIG_EX_SSL),y)
  USEEXLIBSSL=-L$(STAGING_DIR)/usr/lib -Wl,-rpath,$(TOOLCHAIN_LIB_DIRS)
endif

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)/Default
  SUBMENU:=IPv6
  SECTION:=ipv6
  CATEGORY:=TP-LINK iplatform apps
  TITLE:=iputils
  DEPENDS:=+libopenssl
endef

TARGET_CFLAGS += \
	-I$(STAGING_DIR)/usr/include \
	$(USEEXLIBSSL)

define Package/$(PKG_NAME)-ping6
  $(call Package/$(PKG_NAME)/Default)
  TITLE+= ping6
endef

define Package/$(PKG_NAME)-ping6/description
	ping6 is a icmp tool for IPv6.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile	
	$(MAKE) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS) \
		TARGET_CFLAGS="$(TARGET_CFLAGS)" \
		all 
endef

define Package/$(PKG_NAME)-ping6/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ping6 $(1)/usr/sbin
endef

$(eval $(call BuildPackage,iputils-ping6))
