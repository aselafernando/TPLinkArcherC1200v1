include $(TOPDIR)/rules.mk

PKG_NAME:=libubox
PKG_VERSION:=2013-07-04
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://nbd.name/luci2/libubox.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=11e8afea0f7eb34f8c23a8e589ee659c46f3f8aa
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_MIRROR_MD5SUM:=
CMAKE_INSTALL:=1

PKG_LICENSE:=ISC BSD-3c
PKG_LICENSE_FILES:=

PKG_MAINTAINER:=Felix Fietkau <nbd@openwrt.org>

PKG_BUILD_DEPENDS:=lua
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/libubox
  SECTION:=libs
  SUBMENU:=Ubox
  CATEGORY:=TP-LINK ibase
  TITLE:=Basic utility library
  DEPENDS:=
endef

define Package/libblobmsg-json
  SECTION:=libs
  SUBMENU:=Ubox
  CATEGORY:=TP-LINK ibase
  TITLE:=blobmsg <-> json conversion library
  DEPENDS:=+libjson +libubox
endef

define Package/jshn
  SECTION:=utils
  SUBMENU:=Ubox
  CATEGORY:=TP-LINK ibase
  DEPENDS:=+libjson
  TITLE:=JSON SHell Notation
endef

define Package/jshn/description
  Library for parsing and generating JSON from shell scripts
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include
CMAKE_OPTIONS = \
	-DLUAPATH=/usr/lib/lua

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

define Package/libubox/install
	$(INSTALL_DIR) $(1)/lib/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/libubox.so $(1)/lib/
endef

define Package/libblobmsg-json/install
	$(INSTALL_DIR) $(1)/lib/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/libblobmsg_json.so $(1)/lib/
endef

define Package/jshn/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/usr/share/libubox
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/jshn $(1)/usr/bin
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/share/libubox/jshn.sh $(1)/usr/share/libubox
endef

$(eval $(call BuildPackage,libubox))
$(eval $(call BuildPackage,libblobmsg-json))
$(eval $(call BuildPackage,jshn))

