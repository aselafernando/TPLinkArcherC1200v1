#
# Copyright (c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.
#
# Author  : Zhang Longwei <zhanglongwei@tp-link.net>
# Version : 1.0
# Date    : 14th Aug, 2014
#

include $(TOPDIR)/rules.mk

PKG_NAME:=iplatform-extern-libs
PKG_VERSION:=1.0

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/libifaddrs
  SECTION:=libs
  SUBMENU:=iplatform-extern-libs
  CATEGORY:=Libraries
  VERSION:=1.0.0
  MAINTAINER:=Zhang Longwei <zhanglongwei@tp-link.net>
  TITLE:=extern libs for API of network interface
  DEPENDS:=@IFADDRS
endef

define Package/libversionsort
  SECTION:=libs
  SUBMENU:=iplatform-extern-libs
  CATEGORY:=Libraries
  VERSION:=1.0.0
  MAINTAINER:=Tristan Xiao <xiaolongde@tp-link.com.cn>
  TITLE:=extern libs for API of network interface
  DEPENDS:=@VERSIONSORT
endef

define Package/libopenat
  SECTION:=libs
  SUBMENU:=iplatform-extern-libs
  CATEGORY:=Libraries
  VERSION:=1.0.0
  MAINTAINER:=Tristan Xiao <xiaolongde@tp-link.com.cn>
  TITLE:=extern libs for API of network interface
  DEPENDS:=@OPENAT
endef

define Package/libunshare
  SECTION:=libs
  SUBMENU:=iplatform-extern-libs
  CATEGORY:=Libraries
  VERSION:=1.0.0
  MAINTAINER:=Tristan Xiao <xiaolongde@tp-link.com.cn>
  TITLE:=extern libs for API of network interface
  DEPENDS:=@UNSHARE
endef

define Package/libfdopendir
  SECTION:=libs
  SUBMENU:=iplatform-extern-libs
  CATEGORY:=Libraries
  VERSION:=1.0.0
  MAINTAINER:=Tristan Xiao <xiaolongde@tp-link.com.cn>
  TITLE:=extern libs for API of network interface
  DEPENDS:=@FDOPENDIR
endef

define Package/libifaddrs/description
 Interface for network address. 
 This is an implementation of libifaddrs.
endef

#MAKE_VARS += FPIC="$(FPIC)"

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/Makefile $(PKG_BUILD_DIR)
        ifeq ($(CONFIG_IFADDRS),y)
		$(CP) -rf ./src/ifaddrs/ $(PKG_BUILD_DIR)
        endif
        ifeq ($(CONFIG_VERSIONSORT),y)
		$(CP) -rf ./src/versionsort/ $(PKG_BUILD_DIR)
        endif
        ifeq ($(CONFIG_OPENAT),y)
		$(CP) -rf ./src/openat/ $(PKG_BUILD_DIR)
        endif
        ifeq ($(CONFIG_UNSHARE),y)
		$(CP) -rf ./src/unshare/ $(PKG_BUILD_DIR)
        endif
        ifeq ($(CONFIG_FDOPENDIR),y)
		$(CP) -rf ./src/fdopendir/ $(PKG_BUILD_DIR)
        endif
endef

define Build/Compile/Default
	$(MAKE) CC=$(TARGET_CROSS)gcc CFLAGS=-I$(TOOLCHAIN_INC_DIRS) -C $(PKG_BUILD_DIR) all
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib
        ifeq ($(CONFIG_IFADDRS),y)
		$(CP) $(PKG_BUILD_DIR)/ifaddrs/ifaddrs.h $(1)/usr/include
		$(CP) $(PKG_BUILD_DIR)/ifaddrs/*.so $(1)/usr/lib/
        endif

        ifeq ($(CONFIG_VERSIONSORT),y)
		$(CP) $(PKG_BUILD_DIR)/versionsort/*.so $(1)/usr/lib/
        endif
        ifeq ($(CONFIG_OPENAT),y)
		$(CP) $(PKG_BUILD_DIR)/openat/*.so $(1)/usr/lib/
        endif
        ifeq ($(CONFIG_UNSHARE),y)
		$(CP) $(PKG_BUILD_DIR)/unshare/*.so $(1)/usr/lib/
        endif
        ifeq ($(CONFIG_FDOPENDIR),y)
		$(CP) $(PKG_BUILD_DIR)/fdopendir/*.so $(1)/usr/lib/
        endif
endef

define Package/libifaddrs/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/ifaddrs/*.so $(1)/usr/lib/
endef

define Package/libversionsort/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/versionsort/*.so $(1)/usr/lib/
endef

define Package/libopenat/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/openat/*.so $(1)/usr/lib/
endef

define Package/libunshare/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/unshare/*.so $(1)/usr/lib/
endef

define Package/libfdopendir/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/fdopendir/*.so $(1)/usr/lib/
endef
$(eval $(call BuildPackage,libifaddrs))
$(eval $(call BuildPackage,libversionsort))
$(eval $(call BuildPackage,libopenat))
$(eval $(call BuildPackage,libunshare))
$(eval $(call BuildPackage,libfdopendir))
