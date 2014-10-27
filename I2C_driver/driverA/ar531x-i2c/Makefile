# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 6562 2007-03-14 01:19:24Z nbd $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ar531x-i2c
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/ar531x-i2c
  SUBMENU:=I2C Bus
  DEPENDS:=
  TITLE:=Driver for AR531X I2C using GPIO
  VERSION:=$(PKG_RELEASE)+$(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE)
  FILES:=$(PKG_BUILD_DIR)/ar531x_gpio_i2c.$(LINUX_KMOD_SUFFIX)
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

BUILDFLAGS=-I$(LINUX_DIR)/arch/mips/atheros

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(BUILDFLAGS)" \
		modules
endef
	
$(eval $(call KernelPackage,ar531x-i2c))
