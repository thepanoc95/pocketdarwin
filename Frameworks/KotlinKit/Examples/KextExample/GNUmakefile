#
# GNUmakefile - Example GNUKext-based Kext with Kotlin
#
ifeq ($(GNUSTEP_MAKEFILES),)
 GNSTEP_MAKEFILES := $(shell gnustep-config --variable=GNUSTEP_MAKEFILES 2>/dev/null)
endif

ifeq ($(GNUSTEP_MAKEFILES),)
  $(error You need to set GNUSTEP_MAKEFILES before compiling!)
endif

include $(GNUSTEP_MAKEFILES)/common.make

#
# Build a kext module bundle
#
PACKAGE_NAME = kotlin-example-kext
BUNDLE_EXTENSION = kext

#
# Enable Kotlin support in preambles
#
ENABLE_KOTLIN_KEXT = yes

#
# Bundle resources
#
BUNDLE_INFO_PLIST = Info.plist

#
# Source files
#
BUNDLE_OBJC_FILES = \
MyKextDriver.m

#
# Frameworks
#
ADDITIONAL_LDFLAGS += -framework GNUKext -framework KotlinKit

-include GNUmakefile.preamble
include $(GNUSTEP_MAKEFILES)/bundle.make
-include GNUmakefile.postamble