SHELL := /bin/bash

# ---------------------------------------------------------------------------
# Board configuration — include per-board settings
# ---------------------------------------------------------------------------
BOARD ?= harpia
-include board.mk

# Overridable from env / command line
# Use simple assignment (=) to override make's built-in defaults
# (GNU make defaults CC=cc, LD=ld, AS=as, etc.).
ARCH            ?= arm64
CROSS_COMPILE   ?= aarch64-linux-gnu-
CC              = $(CROSS_COMPILE)gcc
AS              = $(CROSS_COMPILE)as
LD              = $(CROSS_COMPILE)ld
OBJCOPY         = $(CROSS_COMPILE)objcopy
DTC             ?= dtc
MKBOOTIMG       ?= mkbootimg
MIG             ?= $(ROOTDIR)/BuildHost/xnu-deps-linux/out/bin/mig
MIGCOM          ?= $(ROOTDIR)/BuildHost/xnu-deps-linux/out/bin/migcom
BISON           ?= bison
FLEX            ?= flex
NM              ?= $(CROSS_COMPILE)nm
STRIP           ?= $(CROSS_COMPILE)strip
UNIFDEF         ?= unifdef

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
# We set up the minimal set for an ARM/ARM64 cross-compile.
KERNEL_CONFIG   ?= RELEASE
ARCH_CONFIG     ?= ARM64
MACHINE_CONFIG  ?= DEFAULT
XNU_TARGET      ?= all

# Override ARCH_CONFIGS from host arch (x86_64) when cross-compiling
ifeq ($(ARCH_CONFIG),ARM)
ARCH_CONFIGS    := ARM
PLATFORM        := iPhoneOS
endif
ifeq ($(ARCH_CONFIG),ARM64)
ARCH_CONFIGS    := ARM64
PLATFORM        := iPhoneOS
endif

# On Linux, use Clang wrapper scripts that add -target
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
# Add toolchain bin dir and BuildHost mig to PATH so sub-makes find our wrappers
export PATH := $(ROOTDIR)/toolchain/bin:$(ROOTDIR)/BuildHost/xnu-deps-linux/out/bin:$(PATH)
export MIGCOM
CC_PATH := $(or $(CC),/usr/bin/cc)
CXX_PATH := $(or $(CXX),/usr/bin/c++)
export CC_PATH CXX_PATH
ifeq ($(ARCH_CONFIG),ARM)
CC := arm-linux-gnueabi-clang
CXX := arm-linux-gnueabi-clang++
LD := arm-linux-gnueabi-clang++ -nostdlib
endif
ifeq ($(ARCH_CONFIG),ARM64)
CC := aarch64-linux-gnu-clang
CXX := aarch64-linux-gnu-clang++
LD := aarch64-linux-gnu-clang++ -nostdlib
endif
endif

XNU_MAKE_ARGS   := ARCH_CONFIG=$(ARCH_CONFIG) \
                   ARCH_CONFIGS=$(ARCH_CONFIGS) \
                   KERNEL_CONFIG=$(KERNEL_CONFIG) \
                   MACHINE_CONFIG=$(MACHINE_CONFIG) \
                   PLATFORM=$(PLATFORM) \
                   SRCROOT=$(KERNEL_DIR) \
                   OBJROOT=$(KERNEL_BUILDDIR)/obj \
                   SYMROOT=$(KERNEL_BUILDDIR)/sym \
                   DSTROOT=$(KERNEL_BUILDDIR)/dst \
                   HOST_CC=clang \
                   HOST_CXX=clang++ \
                    MIG=$(MIG) \
                    MIGCOM=$(MIGCOM) \
                    HOST_BISON=$(BISON) \
                    HOST_FLEX=$(FLEX) \
                    NM=$(NM) \
                    STRIP=$(STRIP) \
                    UNIFDEF=$(UNIFDEF)

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
	$(MAKE) -C $(KERNEL_DIR) $(XNU_TARGET) \
		CC='$(CC)' CXX='$(CXX)' LD='$(LD)' MIGCC='$(word 1,$(CC))' \
		$(XNU_MAKE_ARGS)
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

HARPIA_DRIVERS := MSM8916GCC MSM8916TLMM MSM8916I2C MSM8916SPI \
                  MSM8916UART MSM8916SDHCI MSM8916USB MSM8916SPMI

drivers_kext:
	@mkdir -p $(BUILDDIR)/drivers/AndroidPlatformDevice.kext
	$(CC) -I$(KERNEL_DIR)/iokit \
	      -I$(KERNEL_DIR)/libkern \
	      -I$(KERNEL_DIR)/osfmk \
	      -I$(KERNEL_DIR)/bsd \
	      -I$(DRIVERS_DIR) \
	      -c -o $(BUILDDIR)/drivers/AndroidPlatformDevice.o \
	      $(DRIVERS_DIR)/AndroidPlatformDevice.cpp
	@for drv in $(HARPIA_DRIVERS); do \
		echo "  CC  harpia/$$drv.cpp"; \
		$(CC) -I$(KERNEL_DIR)/iokit \
		      -I$(KERNEL_DIR)/libkern \
		      -I$(KERNEL_DIR)/osfmk \
		      -I$(KERNEL_DIR)/bsd \
		      -I$(DRIVERS_DIR) \
		      -c -o $(BUILDDIR)/drivers/$$drv.o \
		      $(DRIVERS_DIR)/harpia/$$drv.cpp; \
	done
	@$(LD) -r -o $(BUILDDIR)/drivers/AndroidPlatformDevice.kext/AndroidPlatformDevice \
		$(BUILDDIR)/drivers/AndroidPlatformDevice.o \
		$(foreach drv,$(HARPIA_DRIVERS),$(BUILDDIR)/drivers/$(drv).o)
	@cp -r $(DRIVERS_DIR)/AndroidPlatformDevice.kext \
		$(BUILDDIR)/drivers/
	@echo "Driver built: $(BUILDDIR)/drivers/AndroidPlatformDevice.kext"

FRAMEWORK_DIR := $(DRIVERS_DIR)/AndroidPlatform.framework
FRAMEWORK_BUILDDIR := $(BUILDDIR)/drivers/AndroidPlatform.framework

drivers_framework:
	@mkdir -p $(FRAMEWORK_BUILDDIR)/Headers \
	           $(FRAMEWORK_BUILDDIR)/Modules
	@echo "Building AndroidPlatform.framework..."
	$(CC) -I$(KERNEL_DIR)/iokit \
	      -I$(KERNEL_DIR)/libkern \
	      -I$(KERNEL_DIR)/osfmk \
	      -I$(KERNEL_DIR)/bsd \
	      -I$(FRAMEWORK_DIR) \
	      -nostdlib \
	      -c -o $(FRAMEWORK_BUILDDIR)/AndroidPlatform.o \
	      $(FRAMEWORK_DIR)/AndroidPlatform.cpp
	@$(LD) -r -o $(FRAMEWORK_BUILDDIR)/AndroidPlatform \
		$(FRAMEWORK_BUILDDIR)/AndroidPlatform.o
	@cp $(FRAMEWORK_DIR)/Info.plist $(FRAMEWORK_BUILDDIR)/
	@cp $(FRAMEWORK_DIR)/Modules/module.modulemap $(FRAMEWORK_BUILDDIR)/Modules/
	@for hdr in $(wildcard $(FRAMEWORK_DIR)/Headers/*.h); do \
		cp $$hdr $(FRAMEWORK_BUILDDIR)/Headers/; \
	done
	@echo "Framework built: $(FRAMEWORK_BUILDDIR)"

drivers: drivers_kext drivers_framework

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
