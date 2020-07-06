#
# Copyright (C) 2007-2012 OpenWrt.org
# Copyright (C) 2010 Vertical Communications
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/version.mk

PKG_NAME:=base-filesystem
PKG_RELEASE:=1
PKG_BUILD_DEPENDS:=opkg/host

include $(INCLUDE_DIR)/package.mk

define Package/base-filesystem
  SECTION:=TP-LINK
  CATEGORY:=TP-LINK iplatform apps
  DEPENDS:=+netifd +libc
  DEFAULT:=y
  TITLE:=Base filesystem for TP-LINK
  VERSION:=$(PKG_RELEASE)-$(REVISION)
endef

define Package/base-filesystem/conffiles
/etc/hosts
/etc/inittab
/etc/group
/etc/passwd
/etc/shadow
/etc/profile
/etc/shells
/etc/sysctl.conf
/etc/rc.local
/etc/sysupgrade.conf
/etc/config/
/etc/dropbear/
/etc/crontabs/
endef

define Package/base-filesystem/description
 This package contains a base filesystem and system scripts for TP-LINK.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile/Default

endef
Build/Compile = $(Build/Compile/Default)

define Package/base-filesystem/install
	$(CP) ./filesystem/* $(1)/
	$(VERSION_SED) \
		$(1)/etc/banner \
		$(1)/etc/openwrt_version \
		$(1)/etc/openwrt_release

	chmod 0600 $(1)/etc/shadow
	$(INSTALL_BIN) ./filesystem/sbin/sysupgrade $(1)/sbin/sysupgrade
endef

$(eval $(call BuildPackage,base-filesystem))
