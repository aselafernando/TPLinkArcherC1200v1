#
# Copyright (C) 2010-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=minidlna
PKG_VERSION:=1.1.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/minidlna
PKG_MD5SUM:=65cebffaf9b57bf30fbfcf2a3a3253e6

PKG_BUILD_PARALLEL:=0
PKG_BUILD_DEPENDS:=util-linux
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/minidlna
  SECTION:=multimedia
#  CATEGORY:=Multimedia
  CATEGORY:=TP-LINK iplatform apps
  SUBMENU:=USB Storage Sharing
  TITLE:=UPnP A/V & DLNA Media Server
  URL:=http://minidlna.sourceforge.net/
  DEPENDS:= +libpthread +libexif +libjpeg +libsqlite3 +libffmpeg \
  	+libid3tag +libflac +libvorbis +libuuid +libtiff +libubox +libubus \
  	$(ICONV_DEPENDS) $(INTL_DEPENDS)
endef

define Package/minidlna/description
  MiniDLNA (aka ReadyDLNA) is server software with the aim of
  being fully compliant with DLNA/UPnP-AV clients.
endef

define Package/minidlna/conffiles
/etc/config/minidlna
/etc/minidlna.conf
endef

ifeq ($(CONFIG_LINUX_KARCH),"arm")
  ARCH_RELATED_OPTIONS=-D__ARM__
endif

ifeq ($(CONFIG_LINUX_KARCH),"mips")
  LIBS_SEARCH_OPTIONS=-Wl,-rpath,$(TOOLCHAIN_LIB_DIRS)
endif

TARGET_CPPFLAGS += \
	-I$(STAGING_DIR)/usr/include \
	-I$(STAGING_DIR)/usr/include/FLAC \
	-I$(STAGING_DIR)/usr/include/libavcodec \
	-I$(STAGING_DIR)/usr/include/libavformat \
	-I$(STAGING_DIR)/usr/include/libavutil \
	-I$(STAGING_DIR)/usr/include/libexif \
	-I$(STAGING_DIR)/usr/include/uuid \
	-I$(STAGING_DIR)/usr/include/vorbis \
	-I$(ICONV_PREFIX)/include \
	-I$(INTL_PREFIX)/include \
	-D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 \
	$(ARCH_RELATED_OPTIONS) \

TARGET_LDFLAGS += \
	-L$(ICONV_PREFIX)/lib \
	-L$(INTL_PREFIX)/lib \
	-Wl,-rpath-link=$(STAGING_DIR)/usr/lib \
	$(LIBS_SEARCH_OPTIONS) \

MAKE_FLAGS +=\
	CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)" \
	ICONV_LIBS="-liconv $(if $(INTL_FULL),-lintl)" \

CONFIGURE_ARGS += \
	--program-prefix="$(STAGING_DIR)/usr" \
	--with-libiconv-prefix="$(ICONV_PREFIX)" \
	--with-libintl-prefix="$(INTL_PREFIX)" \
	--with-os-name="TP-LINK Linux" \
	--with-os-version="$(LINUX_VERSION)" \
	--with-os-url="http://www.tp-link.com/" \
	--with-db-path="/var/run/minidlna" \
	--with-log-path="/var/log" \

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

define Package/minidlna/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/minidlnad $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./filesystem/etc/init.d/minidlna $(1)/etc/init.d/minidlna
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./filesystem/etc/config/minidlna $(1)/etc/config/minidlna
endef

define Package/minidlna/conffiles
/etc/config/minidlna
endef

$(eval $(call BuildPackage,minidlna))
