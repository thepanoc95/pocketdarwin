#
# GNUmakefile - Example UIKit Application with Kotlin
#
ifeq ($(GNUSTEP_MAKEFILES),)
 GNSTEP_MAKEFILES := $(shell gnustep-config --variable=GNUSTEP_MAKEFILES 2>/dev/null)
endif

ifeq ($(GNUSTEP_MAKEFILES),)
  $(error You need to set GNUSTEP_MAKEFILES before compiling!)
endif

include $(GNUSTEP_MAKEFILES)/common.make

#
# Build an application
#
PACKAGE_NAME = KotlinUIExample

#
# Enable Kotlin support
#
ENABLE_KOTLIN_UIKIT = yes

#
# Source files
#
KotlinUIExample_OBJC_FILES = \
main.m

#
# Frameworks
#
KotlinUIExample_FRAMEWORKS_DEPEND_UPON = -framework UIKit -framework KotlinKit

-include GNUmakefile.preamble
include $(GNUSTEP_MAKEFILES)/tool.make
-include GNUmakefile.postamble