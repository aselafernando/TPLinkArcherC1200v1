#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=broadcom-wl
PKG_VERSION:=5.10.56.27.3
PKG_RELEASE:=5

#PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)_$(ARCH).tar.bz2
#PKG_SOURCE_URL:=http://downloads.openwrt.org/sources

PKG_MD5SUM.mipsel:=3363e3a6b3d9d73c49dea870c7834eac
PKG_MD5SUM.mips:=f8de63debc75333d6b4e28193eb051ff
PKG_MD5SUM:=$(PKG_MD5SUM.$(ARCH))

include $(INCLUDE_DIR)/package.mk

define Package/broadcom-wl/Default
  SECTION:=kernel
  CATEGORY:=Kernel modules
  #DEPENDS:=@PACKAGE_kmod-brcm-wl||PACKAGE_kmod-brcm-wl-mini
  SUBMENU:=Proprietary BCM43xx WiFi driver
  #SUBMENUDEP:=@TARGET_brcm47xx||TARGET_brcm63xx
endef

define KernelPackage/brcm-wl/Default
  $(call Package/broadcom-wl/Default)
  SECTION:=kernel
  #DEPENDS:=@TARGET_brcm47xx||TARGET_brcm63xx +wireless-tools
  DEPENDS:=
  TITLE:=Kernel driver for BCM43xx chipsets
  #FILES:=$(PKG_BUILD_DIR)/driver$(1)/wl.ko $(PKG_BUILD_DIR)/glue/wl_glue.ko
  #AUTOLOAD:=$(call AutoLoad,30,wl_glue wl)
endef

define KernelPackage/brcm-wl/Default/description
 This package contains the proprietary wireless driver for the Broadcom 
 BCM43xx chipset.
endef

define KernelPackage/brcm-wl
$(call KernelPackage/brcm-wl/Default,)
  TITLE+= (normal version)
endef

define KernelPackage/brcm-wl/description
$(call KernelPackage/brcm-wl/Default/description)
endef

define KernelPackage/brcm-wl-mini
$(call KernelPackage/brcm-wl/Default,-mini)
  TITLE+= (Legacy version)
endef

define KernelPackage/brcm-wl-mini/description
$(call KernelPackage/brcm-wl/Default/description)
endef

define Package/wlc
$(call Package/broadcom-wl/Default)
  TITLE:=wl driver setup utility
endef

define Package/wlc/description
 This package contains an utility for initializing the proprietary Broadcom 
 wl driver.
endef

define Package/wl
$(call Package/broadcom-wl/Default)
  TITLE:=Proprietary Broadcom wl driver config utility
endef

define Package/wl/description
 This package contains the proprietary utility (wl) for configuring the 
 proprietary Broadcom wl driver.
endef

define Package/nas
$(call Package/broadcom-wl/Default)
  TITLE:=Proprietary Broadcom WPA/WPA2 authenticator
endef

define Package/nas/description
 This package contains the proprietary WPA/WPA2 authenticator (nas) for the 
 proprietary Broadcom wl driver.
endef

MAKE_KMOD := $(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		PATH="$(TARGET_PATH)" \
		SUBDIRS="$(PKG_BUILD_DIR)/kmod" \

define Build/Prepare
	true
endef

define Build/Compile
	true
endef

define Build/InstallDev
	true
endef

define Package/wlc/install
	true
endef

define Package/wlc/postinst
	true
endef

define Package/wl/install
	true
endef

define Package/nas/install
	true
endef

$(eval $(call KernelPackage,brcm-wl))
$(eval $(call KernelPackage,brcm-wl-mini))
$(eval $(call BuildPackage,wlc))
$(eval $(call BuildPackage,wl))
$(eval $(call BuildPackage,nas))
