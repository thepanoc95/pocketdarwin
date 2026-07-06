#
# Makefile for AndroidPlatformDevice.kext
#
# Standalone build for the AndroidPlatformDevice IOKit kext bundle and
# its board-specific subclass drivers.
#
# Usage:
#   make                    Build the kext bundle
#   make KERNEL_DIR=/path   Point to XNU kernel source (default: ../Kernel)
#   make BOARD=harpia       Select board (default: harpia)
#   make clean              Remove build artifacts
#
# Overridable variables:
#   KERNEL_DIR     XNU kernel source path
#   BOARD          Target board name
#   CROSS_COMPILE  Toolchain prefix (default: aarch64-linux-gnu-)
#   CC, CXX, LD    Compiler / linker
#   BUILDDIR       Artifact output directory
#

SHELL       := /bin/bash
ROOTDIR     := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/..)
KERNEL_DIR  ?= $(ROOTDIR)/Kernel
BUILDDIR    ?= $(ROOTDIR)/build
BOARD       ?= harpia

CROSS_COMPILE ?= aarch64-linux-gnu-
CC  ?= $(CROSS_COMPILE)gcc
CXX ?= $(CROSS_COMPILE)g++
LD  ?= $(CROSS_COMPILE)ld

# Board-specific drivers (extend this for new boards)
ifeq ($(BOARD),harpia)
BOARD_DRIVERS := MSM8916GCC MSM8916TLMM MSM8916I2C MSM8916SPI \
                 MSM8916UART MSM8916SDHCI MSM8916USB MSM8916SPMI \
                 Display I2CTouch
else
$(warning Unknown board "$(BOARD)"; no board drivers will be built)
BOARD_DRIVERS :=
endif

DRIVERS_DIR     := $(ROOTDIR)/drivers
KEXT_SRC        := $(DRIVERS_DIR)/AndroidPlatformDevice.cpp
KEXT_HEADER     := $(DRIVERS_DIR)/AndroidPlatformDevice.h
BOARD_SRC_DIR   := $(DRIVERS_DIR)/$(BOARD)
KEXT_BUNDLE_DIR := $(BUILDDIR)/drivers/AndroidPlatformDevice.kext
KEXT_BINARY     := $(KEXT_BUNDLE_DIR)/AndroidPlatformDevice
KEXT_INFO       := $(DRIVERS_DIR)/AndroidPlatformDevice.kext/Info.plist

INCLUDES := -I$(KERNEL_DIR)/iokit \
            -I$(KERNEL_DIR)/libkern \
            -I$(KERNEL_DIR)/osfmk \
            -I$(KERNEL_DIR)/bsd \
            -I$(DRIVERS_DIR)

CFLAGS := -O2 $(INCLUDES)

OBJS := $(BUILDDIR)/drivers/AndroidPlatformDevice.o \
        $(foreach drv,$(BOARD_DRIVERS),$(BUILDDIR)/drivers/$(drv).o)

.PHONY: all clean

all: $(KEXT_BINARY)

$(BUILDDIR)/drivers/AndroidPlatformDevice.o: $(KEXT_SRC) $(KEXT_HEADER)
	@mkdir -p $(BUILDDIR)/drivers
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/drivers/%.o: $(BOARD_SRC_DIR)/%.cpp
	@mkdir -p $(BUILDDIR)/drivers
	$(CC) $(CFLAGS) -c -o $@ $<

$(KEXT_BINARY): $(OBJS) $(KEXT_INFO)
	@mkdir -p $(KEXT_BUNDLE_DIR)
	$(LD) -r -o $@ $(OBJS)
	@cp $(KEXT_INFO) $(KEXT_BUNDLE_DIR)/
	@echo "Built: $(KEXT_BUNDLE_DIR)"

clean:
	rm -rf $(BUILDDIR)/drivers
	@echo "Clean: removed $(BUILDDIR)/drivers"
