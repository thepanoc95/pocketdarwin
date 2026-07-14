#
# kext.make - Makefile rules for building GNUKext-based Kexts
#
# This makefile provides rules and variables for building kernel extensions
# (kexts) that use the GNUKext framework. It supports building kexts with
# Objective-C, Objective-C++, and Kotlin sources.
#

ifndef GNUKext_VERSION
GNUKext_VERSION := 0.1
endif

# Default kext extension
ifndef KEXT_EXT
ifeq ($(findstring Darwin,$(shell uname -s)),Darwin)
KEXT_EXT := kext
else
KEXT_EXT := ko
endif
endif

# Default architectures
ifndef KEXT_ARCHS
KEXT_ARCHS := $(ARCH)
endif

# Default Info.plist location
ifndef KEXT_INFO_PLIST
KEXT_INFO_PLIST := Info.plist
endif

# Variables for kext building
KEXT_LDFLAGS := -framework GNUKext -framework Foundation
KEXT_INCLUDE_DIRS := -I$(GNUSTEP_MAKEFILES)/Makefiles

# Kotlin support for kexts
ifeq ($(ENABLE_KOTLIN_KEXT),yes)
    KOTLIN_KEXT_DIR := $(GNUSTEP_LIBRARIES)/KotlinKit/Kotlin
    KEXT_LDFLAGS += -framework KotlinKit
endif

# Rule to build a kext from sources
define build-kext
$(KEXT_OBJC_FILES): %.m: 
	$(CC) $(KEXT_INCLUDE_DIRS) $(ADDITIONAL_CPPFLAGS) $(OBJCFLAGS) -c $< -o $@.o
	
$(KEXT_OBJCC_FILES): %.mm:
	$(CXX) $(KEXT_INCLUDE_DIRS) $(ADDITIONAL_CPPFLAGS) $(OBJCXXFLAGS) -c $< -o $@.o
	
$(KEXT_C_FILES): %.c:
	$(CC) $(KEXT_INCLUDE_DIRS) $(ADDITIONAL_CPPFLAGS) $(CFLAGS) -c $< -o $@.o
endef

# Rule for Kotlin kexts
define build-kotlin-kext
$(KEXT_KOTLIN_FILES): %.kt:
	@if test -z "$$(KOTLIN_HOME)"; then \
		echo "Error: KOTLIN_HOME not set. Please set KOTLIN_HOME to your Kotlin compiler."; \
		exit 1; \
	fi
	$$(KOTLIN_HOME)/bin/kotlinc -d $(OBJ_DIR) $<
endef

# Convenience functions for kext development
KextInclude := include GNUmakefile.preamble
KextFrameworks := $(KEXT_LDFLAGS)
KotlinKextSupport := ENABLE_KOTLIN_KEXT=yes KEXT_LDFLAGS += -framework KotlinKit

# Example kext template variables
KEXT_BUNDLE_ID ?= com.example.kext
KEXT_BUNDLE_NAME ?= ExampleKext
KEXT_BUNDLE_VERSION ?= 1.0

# Helper to generate a default Info.plist
define generate-kext-plist
	@echo '<?xml version="1.0" encoding="UTF-8"?>' > $(KEXT_INFO_PLIST)
	@echo '<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">' >> $(KEXT_INFO_PLIST)
	@echo '<plist version="1.0">' >> $(KEXT_INFO_PLIST)
	@echo '<dict>' >> $(KEXT_INFO_PLIST)
	@echo '	<key>CFBundleDevelopmentRegion</key>' >> $(KEXT_INFO_PLIST)
	@echo '	<string>en</string>' >> $(KEXT_INFO_PLIST)
	@echo '	<key>CFBundleIdentifier</key>' >> $(KEXT_INFO_PLIST)
	@echo '	<string>$(KEXT_BUNDLE_ID)</string>' >> $(KEXT_INFO_PLIST)
	@echo '	<key>CFBundleName</key>' >> $(KEXT_INFO_PLIST)
	@echo '	<string>$(KEXT_BUNDLE_NAME)</string>' >> $(KEXT_INFO_PLIST)
	@echo '	<key>CFBundleVersion</key>' >> $(KEXT_INFO_PLIST)
	@echo '	<string>$(KEXT_BUNDLE_VERSION)</string>' >> $(KEXT_INFO_PLIST)
	@echo '	<key>IOKitPersonalities</key>' >> $(KEXT_INFO_PLIST)
	@echo '	<dict/>' >> $(KEXT_INFO_PLIST)
	@echo '</dict>' >> $(KEXT_INFO_PLIST)
	@echo '</plist>' >> $(KEXT_INFO_PLIST)
endef