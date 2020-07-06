#
# Basic targets for special product
#

LUAC?=$(PLATFORM_DIR)/staging_dir/host/bin/luac
LUA_PATH?=$(IMAGE_ROOTFS_DIR)/usr/lib/lua

firmware:
	@# copy sdk rootfs and kernel
	@install -d $(IMAGE_ROOTFS_DIR)
	@-find $(SDK_ROOTFS_DIR) -name 'CVS' -o -name '.svn' -o -name '.#*' -o -name '*~' | xargs rm -rf
	@cp -af $(SDK_ROOTFS_DIR)/* $(IMAGE_ROOTFS_DIR)
	@cp -f $(SDK_BIN_DIR)/vmlinuz $(IMAGE_DIR)

	@# copy model_brcm_bcm4709 rootfs
	@cp -af $(PLATFORM_DIR)/build_dir/target-arm-openwrt-linux-uclibc/root-model_brcm_bcm470x/* $(IMAGE_ROOTFS_DIR)
	@# remove redundance
	@for mod in `ls $(IMAGE_ROOTFS_DIR)/lib/modules/iplatform/*.ko` ; \
	do \
	  find $(IMAGE_ROOTFS_DIR)/lib/modules/2.6.36.4brcmarm/ -type f -name `basename $$mod` | xargs rm -f;\
	done 

	@# copy board rootfs
	@cp -af $(CONFIG_DIR)/filesystems/ $(CONFIG_DIR)/filesystems_tmp/ 
	@-find $(CONFIG_DIR)/filesystems_tmp/ -name 'CVS' -o -name '.svn' -o -name '.#*' -o -name '*~' | xargs rm -rf
	@cp -af $(CONFIG_DIR)/filesystems_tmp/* $(IMAGE_ROOTFS_DIR)
	@rm -rf $(CONFIG_DIR)/filesystems_tmp/ 

	@# precompile lua files
	@mv -f $(LUA_PATH)/luci/debug.lua $(LUA_PATH)/luci/debug.tmp
	@for i in $$(find $(LUA_PATH) -name '*.lua'); do \
		$(LUAC) -s -o $$i $$i; \
		chmod +x $$i; \
	done
	@mv $(LUA_PATH)/luci/debug.tmp $(LUA_PATH)/luci/debug.lua

	@# make firmware
	@cp -af $(IMAGE_ROOTFS_DIR) $(IMAGE_TMP_DIR)
	@-find $(IMAGE_TMP_DIR) -name 'CVS' -o -name '.svn' -o -name '.#*' -o -name '*~' | xargs rm -rf
	@$(SDK_BUILD_DIR)/squashfs-4.2/mksquashfs $(IMAGE_TMP_DIR) $(IMAGE_DIR)/target.squashfs -noappend -all-root
	@$(SDK_TOOLS_PATH)/trx -o $(IMAGE_DIR)/vmlinux.trx $(IMAGE_DIR)/vmlinuz
	@#$(SDK_TOOLS_PATH)/trx -o $(IMAGE_DIR)/linux.trx $(IMAGE_DIR)/vmlinuz $(IMAGE_DIR)/target.squashfs
	@rm -rf $(IMAGE_TMP_DIR)
	@echo make $@ done.

sdk_post:
	[ ! -d "$(SDK_KERNEL_DIR)/modules" ] || rm -rf $(SDK_KERNEL_DIR)/modules
	mkdir -p $(SDK_KERNEL_DIR)/modules
	for node in `find $(SDK_KERNEL_DIR) -name "*.ko" -type f`; do \
		cp -f $$node $(SDK_KERNEL_DIR)/modules; \
	done

sdk.config:
	@cp -f $(CONFIG_DIR)/sdk.config $(SDK_BUILD_DIR)/.config

kernel.config:
	@cp -f $(CONFIG_DIR)/kernel.config $(SDK_KERNEL_DIR)/.config

sdk: all install sdk_post
sdk_menuconfig: menuconfig 
sdk_clean: clean

uboot:uboot_clean
	@echo uboot: making ...
	$(MAKE) -C $(UBOOT_BUILD_DIR)/build/broadcom/bcm947xx/ $(UBOOT_FLAGS)
	@cp $(UBOOT_BUILD_DIR)/build/broadcom/bcm947xx/compressed/cfez.bin $(UBOOT_BUILD_DIR)/cfe-a7.bin
	@chmod +x $(SDK_TOOLS_PATH)/nvserial
	@$(SDK_TOOLS_PATH)/nvserial -i $(UBOOT_BUILD_DIR)/cfe-a7.bin -o $(UBOOT_BUILD_DIR)/second_boot.bin -s 001 $(UBOOT_CALIBRATE_TEXT) -z
	@[ -d $(IMAGE_DIR) ] || mkdir -p $(IMAGE_DIR)
	cp $(UBOOT_BUILD_DIR)/second_boot.bin $(IMAGE_DIR)/second_boot.bin

uboot_clean:
	@echo uboot: clean ...
	$(MAKE) -C $(UBOOT_BUILD_DIR)/build/broadcom/bcm947xx/ clean

factory_boot:factory_boot_clean
	@echo factory_boot: making ...
	$(MAKE) -C $(FACTORY_BUILD_DIR)/build/broadcom/bcm947xx/ $(FACTORY_FLAGS)
	@cp $(FACTORY_BUILD_DIR)/build/broadcom/bcm947xx/compressed/cfez.bin $(FACTORY_BUILD_DIR)/cfe-a7.bin
	@chmod +x $(SDK_TOOLS_PATH)/nvserial
	@$(SDK_TOOLS_PATH)/nvserial -i $(FACTORY_BUILD_DIR)/cfe-a7.bin -o $(FACTORY_BUILD_DIR)/factory_boot.bin -s 001 $(FACTORY_CALIBRATE_TEXT) -z
	@[ -d $(IMAGE_DIR) ] || mkdir -p $(IMAGE_DIR)
	cp $(FACTORY_BUILD_DIR)/factory_boot.bin $(IMAGE_DIR)/factory_boot.bin

factory_boot_clean:
	@echo factory_boot: clean ...
	$(MAKE) -C $(FACTORY_BUILD_DIR)/build/broadcom/bcm947xx/ clean

