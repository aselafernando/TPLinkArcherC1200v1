#
# Copyright (c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.
#
# Author  : Ye Qianchuan <yeqianchuan@tp-link.net>
# Version : 1.0
# Date    : 9 Apr, 2014
#

include $(TOPDIR)/rules.mk

PKG_NAME:=pppoe-discovery
PKG_VERSION:=3.8p

include $(INCLUDE_DIR)/package.mk

define Package/pppoe-discovery
  SECTION:=TP-LINK
  CATEGORY:=TP-LINK iplatform apps
  MAINTAINER:=Ye Qianchuan <yeqianchuan@tp-link.net>
  DEPENDS:=
  TITLE:=pppoe discovery
endef

define Package/pppoe-discovery/Description
	Perform PPPOE discovery
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) -u ./src/* $(PKG_BUILD_DIR)/
endef

MAKE_VARS += RP_VERSION="$(PKG_VERSION)"

define Package/pppoe-discovery/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/pppoe-discovery $(1)/usr/sbin
endef

$(eval $(call BuildPackage,pppoe-discovery))
