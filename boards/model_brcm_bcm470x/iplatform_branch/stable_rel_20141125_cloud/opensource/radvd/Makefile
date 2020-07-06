#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=radvd
PKG_VERSION:=1.7
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

#
# add our lib if ifaddrs.h is missing.
# =====================================================================================
#ifneq ($(CONFIG_IFADDRS),)
  export USELIBIFADDRS:=-lifaddrs
#endif
# =====================================================================================

define Package/$(PKG_NAME)/Default
  SUBMENU:=IPv6
  SECTION:=ipv6
  CATEGORY:=TP-LINK iplatform apps
  TITLE:=IPv6 Routing Advertisement
  DEPENDS:=+kmod-ipv6 +libdaemon +IFADDRS:libifaddrs
endef

define Package/$(PKG_NAME)
  $(call Package/radvd/Default)
  TITLE+= Daemon
endef

define Package/$(PKG_NAME)/description
radvd is the router advertisement daemon for IPv6. It listens to router
solicitations and sends router advertisements as described in "Neighbor
Discovery for IP Version 6 (IPv6)" (RFC 4861).  With these advertisements hosts
can automatically configure their addresses and some other parameters. They also
can choose a default router based on these advertisements.
endef

define Package/radvdump
  $(call Package/$(PKG_NAME)/Default)
  TITLE+= Dumper
endef

define Package/radvdump/description
radvdump prints out the contents of incoming router advertisements sent by radvd
or some other software implementing (parts of) "Neighbor Discovery for IP
Version 6 (IPv6)" (RFC 4861).
endef

CONFIGURE_ARGS += \
	--with-configfile=/etc/radvd.conf \
	--with-logfile=/var/log/radvd.log \
	--with-pidfile=/var/run/radvd.pid

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) clean	
	cd $(PKG_BUILD_DIR) && $(CP) scanner_flex.c scanner.c
	$(MAKE) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS) 
endef

define Package/$(PKG_NAME)/conffiles
/etc/config/radvd
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./filesystem/radvd.config $(1)/etc/config/radvd
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./filesystem/radvd.init $(1)/etc/init.d/radvd
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/radvd $(1)/usr/sbin/
endef

define Package/radvdump/install
	mkdir -p $(PKG_INSTALL_DIR)/usr/sbin/radvdump
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/radvdump $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,radvd))
$(eval $(call BuildPackage,radvdump))
