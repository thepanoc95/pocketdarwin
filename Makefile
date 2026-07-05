#
# PocketDarwin Top-Level Build System
#
# Targets:
#   all              Build everything: kernel + loader + dtb
#   kernel           Build the XNU kernel (ARM64)
#   kernldr          Build the kernel loader (boot/kernldr)
#   maloader         Build maloader with XNU kernel patch applied
#   dtbs             Compile device tree blobs
#   drivers          Build IOKit drivers for Android
#   bootimg          Package kernel + dtb + loader into Android boot.img
#   clean            Clean all build artifacts
#   distclean        Full clean including toolchain downloads
#

SHELL := /bin/bash

# ---------------------------------------------------------------------------
# Board configuration — include per-board settings
# ---------------------------------------------------------------------------
BOARD ?= harpia
-include board.mk

# Overridable from env / command line
ARCH            ?= arm64
CROSS_COMPILE   ?= aarch64-linux-gnu-
CC              ?= $(CROSS_COMPILE)gcc
AS              ?= $(CROSS_COMPILE)as
LD              ?= $(CROSS_COMPILE)ld
OBJCOPY         ?= $(CROSS_COMPILE)objcopy
DTC             ?= dtc
MKBOOTIMG       ?= mkbootimg

# ---------------------------------------------------------------------------
# Directories
# ---------------------------------------------------------------------------
ROOTDIR         := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
BUILDDIR        := $(ROOTDIR)/build
KERNEL_DIR      := $(ROOTDIR)/Kernel
KERNELDR_DIR    := $(ROOTDIR)/boot/kernldr
MALOADER_DIR    := $(ROOTDIR)/boot/maloader
DEVICETREE_DIR  := $(ROOTDIR)/devicetree
DRIVERS_DIR     := $(ROOTDIR)/drivers

KERNEL_BUILDDIR := $(BUILDDIR)/kernel
KERNELDR_BUILDDIR := $(BUILDDIR)/kernldr
DTB_BUILDDIR    := $(BUILDDIR)/dtb

# ---------------------------------------------------------------------------
# XNU kernel build configuration
# ---------------------------------------------------------------------------
# The XNU build system expects these env vars set by xnu-build.
# We set up the minimal set for an ARM64 cross-compile.
KERNEL_CONFIG   ?= RELEASE
ARCH_CONFIG     ?= ARM64
MACHINE_CONFIG  ?= DEFAULT
XNU_TARGET      ?= all

XNU_MAKE_ARGS   := ARCH_CONFIG=$(ARCH_CONFIG) \
                   KERNEL_CONFIG=$(KERNEL_CONFIG) \
                   MACHINE_CONFIG=$(MACHINE_CONFIG) \
                   SRCROOT=$(KERNEL_DIR) \
                   OBJROOT=$(KERNEL_BUILDDIR)/obj \
                   SYMROOT=$(KERNEL_BUILDDIR)/sym \
                   DSTROOT=$(KERNEL_BUILDDIR)/dst \
                   CC=$(CC) \
                   LD=$(LD)

# ---------------------------------------------------------------------------
# Boot image config
# ---------------------------------------------------------------------------
KERNEL_LOAD_ADDR    ?= 0x80080000
RAMDISK_ADDR        ?= 0x84000000
TAGS_ADDR           ?= 0x80000100
BOOTIMG_OUT         := $(BUILDDIR)/boot.img

.PHONY: all kernel kernldr maloader dtbs drivers bootimg clean distclean help

# ===========================================================================
# Default target
# ===========================================================================
all: kernel kernldr dtbs drivers

help:
	@echo "PocketDarwin Build System"
	@echo ""
	@echo "Usage: make [BOARD=<board>] [ARCH=<arch>] <target>"
	@echo ""
	@echo "Boards:"
	@echo "  harpia    Moto G4 Play (MSM8916, ARM64)"
	@echo ""
	@echo "Targets:"
	@echo "  all       Build kernel + loader + device tree + drivers"
	@echo "  kernel    Build XNU kernel for ARM64"
	@echo "  kernldr   Build kernel loader (boot/kernldr)"
	@echo "  maloader  Build maloader with XNU kernel patch"
	@echo "  dtbs      Compile device tree blobs"
	@echo "  drivers   Build IOKit Android drivers"
	@echo "  bootimg   Package into Android boot.img"
	@echo "  clean     Clean build artifacts"
	@echo "  distclean Full clean"
	@echo ""
	@echo "Variables:"
	@echo "  CROSS_COMPILE  Toolchain prefix (default: aarch64-linux-gnu-)"
	@echo "  BOARD          Target board name (default: harpia)"
	@echo "  KERNEL_CONFIG  Release|DEVELOP|DEBUG (default: RELEASE)"

# ===========================================================================
# XNU Kernel
# ===========================================================================
kernel: $(KERNEL_DIR)/Makefile
	@mkdir -p $(KERNEL_BUILDDIR)/obj $(KERNEL_BUILDDIR)/sym $(KERNEL_BUILDDIR)/dst
	$(MAKE) -C $(KERNEL_DIR) $(XNU_TARGET) $(XNU_MAKE_ARGS)
	@echo "Kernel built: $(KERNEL_BUILDDIR)/dst/mach_kernel"

kernel_clean:
	$(MAKE) -C $(KERNEL_DIR) clean $(XNU_MAKE_ARGS) || true
	rm -rf $(KERNEL_BUILDDIR)

# ===========================================================================
# Kernel Loader (kernldr)
# ===========================================================================
kernldr: $(KERNELDR_DIR)/Makefile
	@mkdir -p $(KERNELDR_BUILDDIR)
	$(MAKE) -C $(KERNELDR_DIR) \
		CC=$(CC) AS=$(AS) LD=$(LD) OBJCOPY=$(OBJCOPY) \
		O=$(KERNELDR_BUILDDIR)
	@cp $(KERNELDR_DIR)/kernldr.bin $(KERNELDR_BUILDDIR)/
	@echo "Kernel loader built: $(KERNELDR_BUILDDIR)/kernldr.bin"

kernldr_clean:
	$(MAKE) -C $(KERNELDR_DIR) clean
	rm -rf $(KERNELDR_BUILDDIR)

# ===========================================================================
# maloader
# ===========================================================================
maloader: $(MALOADER_DIR)/Makefile
	@mkdir -p $(BUILDDIR)/maloader
	@if [ -f $(ROOTDIR)/boot/maloader.patch ]; then \
		cd $(MALOADER_DIR) && \
		git apply $(ROOTDIR)/boot/maloader.patch 2>/dev/null; \
	fi
	$(MAKE) -C $(MALOADER_DIR) all \
		CC=$(CC) CXX=$(CROSS_COMPILE)g++
	@echo "maloader built: $(MALOADER_DIR)/ld-mac"

maloader_clean:
	$(MAKE) -C $(MALOADER_DIR) clean || true
	cd $(MALOADER_DIR) && git checkout -- . 2>/dev/null || true

# ===========================================================================
# Device Tree Blobs
# ===========================================================================
dtbs: $(DEVICETREE_DIR)/$(BOARD)
	@mkdir -p $(DTB_BUILDDIR)
	@for dts in $(wildcard $(DEVICETREE_DIR)/$(BOARD)/*.dtsi); do \
		base=$$(basename $$dts .dtsi); \
		echo "  DTSI  $$base.dtsi"; \
	done
	@if [ -f $(DEVICETREE_DIR)/$(BOARD)/main.dts ]; then \
		$(DTC) -I dts -O dtb -o $(DTB_BUILDDIR)/$(BOARD).dtb \
			$(DEVICETREE_DIR)/$(BOARD)/main.dts; \
		echo "  DTC   $(DTB_BUILDDIR)/$(BOARD).dtb"; \
	fi
	@if [ -z "$$(ls $(DTB_BUILDDIR)/*.dtb 2>/dev/null)" ]; then \
		echo "  NOTE: No main.dts found for $(BOARD). Create"; \
		echo "        devicetree/$(BOARD)/main.dts to compile dtbs."; \
	fi

dtbs_clean:
	rm -rf $(DTB_BUILDDIR)

# ===========================================================================
# IOKit Drivers
# ===========================================================================
drivers:
	@echo "Drivers: $(DRIVERS_DIR)"
	@echo "  Note: IOKit drivers are compiled as part of the XNU kernel."
	@echo "  AndroidPlatformDevice.kext sources are at:"
	@echo "    $(DRIVERS_DIR)/AndroidPlatformDevice.cpp"
	@echo "    $(DRIVERS_DIR)/AndroidPlatformDevice.h"
	@echo "  To build as a kext bundle, run: make drivers_kext"

drivers_kext:
	@mkdir -p $(BUILDDIR)/drivers/AndroidPlatformDevice.kext
	$(CC) -I$(KERNEL_DIR)/iokit \
	      -I$(KERNEL_DIR)/libkern \
	      -I$(KERNEL_DIR)/osfmk \
	      -I$(KERNEL_DIR)/bsd \
	      -I$(DRIVERS_DIR) \
	      -c -o $(BUILDDIR)/drivers/AndroidPlatformDevice.o \
	      $(DRIVERS_DIR)/AndroidPlatformDevice.cpp
	@cp -r $(DRIVERS_DIR)/AndroidPlatformDevice.kext \
		$(BUILDDIR)/drivers/
	@echo "Driver built: $(BUILDDIR)/drivers/AndroidPlatformDevice.kext"

drivers_clean:
	rm -rf $(BUILDDIR)/drivers

# ===========================================================================
# Android Boot Image
# ===========================================================================
bootimg: kernel kernldr dtbs
	@mkdir -p $(BUILDDIR)/bootimg
	@kernel="$(KERNEL_BUILDDIR)/dst/mach_kernel"; \
	if [ ! -f "$$kernel" ]; then \
		cp $(KERNEL_DIR)/BUILD/dst/mach_kernel $$kernel 2>/dev/null || true; \
	fi; \
	if [ -f $$kernel ]; then \
		echo "Creating boot image with kernel at 0x$(KERNEL_LOAD_ADDR)..."; \
		$(MKBOOTIMG) \
			--kernel $$kernel \
			--base 0x80000000 \
			--kernel_offset 0x00008000 \
			--ramdisk_offset $(RAMDISK_ADDR) \
			--tags_offset $(TAGS_ADDR) \
			--pagesize 2048 \
			--output $(BOOTIMG_OUT); \
		echo "Boot image: $(BOOTIMG_OUT)"; \
	else \
		echo "No kernel found. Build the kernel first: make kernel"; \
	fi

bootimg_clean:
	rm -rf $(BUILDDIR)/bootimg

# ===========================================================================
# Convenience: flash (requires fastboot on Android device)
# ===========================================================================
flash: bootimg
	@echo "Flashing boot image via fastboot..."
	fastboot flash boot $(BOOTIMG_OUT)
	fastboot reboot

# ===========================================================================
# Utility targets
# ===========================================================================
clean: kernel_clean kernldr_clean dtbs_clean drivers_clean bootimg_clean
	rm -rf $(BUILDDIR)/maloader

distclean: clean maloader_clean
	rm -rf $(BUILDDIR)

# ===========================================================================
# Maintainer: update submodules
# ===========================================================================
submodules:
	git submodule update --init --recursive

submodule_patch:
	@cd $(MALOADER_DIR) && git apply $(ROOTDIR)/boot/maloader.patch
	@echo "maloader patch applied."
