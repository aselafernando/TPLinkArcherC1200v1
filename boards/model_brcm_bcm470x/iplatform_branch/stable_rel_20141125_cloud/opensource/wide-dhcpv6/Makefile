#
# Copyright (C) 2010-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=wide-dhcpv6
PKG_VERSION:=20080615
PKG_RELEASE:=11

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)/Default
  SUBMENU:=IPv6
  SECTION:=ipv6
  CATEGORY:=TP-LINK iplatform apps
  TITLE:=WIDE-DHCPv6
  # SUBMENU:=wide-dhcpv6
  DEPENDS:=+kmod-ipv6 
endef

define Package/$(PKG_NAME)-client
  $(call Package/$(PKG_NAME)/Default)
  TITLE+= client
endef

define Package/$(PKG_NAME)-client/description
	WIDE-DHCPv6 is an open-source implementation of Dynamic Host Configuration Protocol
	for IPv6 (DHCPv6) originally developed by the KAME project. The implementation
	mainly conforms to the following standards: RFC3315,3319,3633,3646,4075,4272,etc.

	This package installs the client component.
endef

define Package/$(PKG_NAME)-server
  $(call Package/$(PKG_NAME)/Default)
  TITLE+= server
endef

define Package/$(PKG_NAME)-server/description
	WIDE-DHCPv6 is an open-source implementation of Dynamic Host Configuration Protocol
	for IPv6 (DHCPv6) originally developed by the KAME project. The implementation
	mainly conforms to the following standards: RFC3315,3319,3633,3646,4075,4272,etc.

	This package installs the server component.
endef

define Package/$(PKG_NAME)-relay
  $(call Package/$(PKG_NAME)/Default)
  TITLE+= relay
endef

define Package/$(PKG_NAME)-relay/description
	WIDE-DHCPv6 is an open-source implementation of Dynamic Host Configuration Protocol
	for IPv6 (DHCPv6) originally developed by the KAME project. The implementation
	mainly conforms to the following standards: RFC3315,3319,3633,3646,4075,4272,etc.

	This package installs the relay component.
endef

define Package/$(PKG_NAME)-control
  $(call Package/$(PKG_NAME)/Default)
  TITLE+= client and server control utility
endef

define Package/$(PKG_NAME)-control/description
	WIDE-DHCPv6 is an open-source implementation of Dynamic Host Configuration Protocol
	for IPv6 (DHCPv6) originally developed by the KAME project. The implementation
	mainly conforms to the following standards: RFC3315,3319,3633,3646,4075,4272,etc.

	This package installs the client and server control utility.
endef

TARGET_CFLAGS+=-D_GNU_SOURCE

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
	$(call Build/Configure/Default,--with-localdbdir=/var)
endef

define Build/Compile	
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		EXTRA_CFLAGS="-include debug.h" \
		all 
endef

define Package/$(PKG_NAME)-client/conffiles
/etc/config/dhcp6c
endef

define Package/$(PKG_NAME)-client/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dhcp6c $(1)/usr/sbin
	# $(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	# $(INSTALL_BIN) ./filesystem/02-ipv6 $(1)/etc/hotplug.d/iface
	$(INSTALL_DIR) $(1)/lib/netifd
	$(INSTALL_DIR) $(1)/lib/netifd/proto
	$(INSTALL_BIN) ./filesystem/dhcp6c.sh $(1)/lib/netifd/proto/dhcp6c.sh
	$(INSTALL_BIN) ./filesystem/dhcp6c.script $(1)/lib/netifd/dhcp6c.script
endef

define Package/$(PKG_NAME)-server/conffiles
/etc/config/dhcp6s
endef

define Package/$(PKG_NAME)-server/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dhcp6s $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/dhcp6s.conf.sample $(1)/etc/dhcp6s.conf
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./filesystem/dhcp6s.config $(1)/etc/config/dhcp6s
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./filesystem/dhcp6s.init $(1)/etc/init.d/dhcp6s
	# $(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	# $(INSTALL_DATA) ./filesystem/dhcp6s.hotplug $(1)/etc/hotplug.d/iface/50-dhcp6s
	$(INSTALL_CONF) ./filesystem/dhcp6sctlkey $(1)/etc
	$(INSTALL_CONF) ./filesystem/dhcp6cctlkey $(1)/etc
endef

define Package/$(PKG_NAME)-relay/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dhcp6relay $(1)/usr/sbin
endef

define Package/$(PKG_NAME)-control/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dhcp6ctl $(1)/usr/sbin
endef

$(eval $(call BuildPackage,wide-dhcpv6-client))
$(eval $(call BuildPackage,wide-dhcpv6-server))
# $(eval $(call BuildPackage,wide-dhcpv6-relay))
# $(eval $(call BuildPackage,wide-dhcpv6-control))
