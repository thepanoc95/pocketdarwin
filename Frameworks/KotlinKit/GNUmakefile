#
# GNUmakefile - KotlinKit Framework
#
ifeq ($(GNUSTEP_MAKEFILES),)
 GNSTEP_MAKEFILES := $(shell gnustep-config --variable=GNUSTEP_MAKEFILES 2>/dev/null)
  ifeq ($(GNUSTEP_MAKEFILES),)
    $(warning )
    $(warning Unable to obtain GNUSTEP_MAKEFILES setting from gnustep-config!)
    $(warning Perhaps gnustep-make is not properly installed,)
    $(warning so gnustep-config is not in your PATH.)
    $(warning )
    $(warning Your PATH is currently $(PATH))
    $(warning )
  endif
endif
ifeq ($(GNUSTEP_MAKEFILES),)
 $(error You need to set GNUSTEP_MAKEFILES before compiling!)
endif

include $(GNUSTEP_MAKEFILES)/common.make

#
# Framework
#
VERSION = 0.1
PACKAGE_NAME = KotlinKit
FRAMEWORK_NAME = KotlinKit
KotlinKit_CURRENT_VERSION_NAME = 0.1
KotlinKit_DEPLOY_WITH_CURRENT_VERSION = yes

#
# Public headers (will be installed)
#
KotlinKit_HEADER_FILES_DIR = Headers
KotlinKit_HEADER_FILES_INSTALL_DIR = /KotlinKit
KotlinKit_HEADER_FILES = \
$(KotlinKit_HEADER_FILES_DIR)/KotlinKit.h \
$(KotlinKit_HEADER_FILES_DIR)/KotlinKitTypes.h \
KotlinKext.h \
KotlinUIKit.h

#
# Framework dependencies
#
KotlinKit_FRAMEWORKS_DEPEND_UPON = -framework GNUKext -framework UIKit

#
# Additional include directories
#
ADDITIONAL_INCLUDE_DIRS += -I$(GNUSTEP_MAKEFILES)/Makefiles

#
# Objective-C Class files
#
KotlinKit_OBJC_FILES = \
Kotlin.m \
KotlinArray.m \
KotlinBoolean.m \
KotlinByte.m \
KotlinChar.m \
KotlinDouble.m \
KotlinFloat.m \
KotlinInt.m \
KotlinLong.m \
KotlinShort.m \
KotlinUnit.m \
KotlinFuncs.m \
KotlinKext.m

#
# Objective-C++ Class files (for UIKit bridge)
#
KotlinKit_OBJCC_FILES = \
KotlinUIKit.mm

#
# AutoGSDoc
#
DOCUMENT_NAME = KotlinKit

KotlinKit_AGSDOC_FILES = \
$(KotlinKit_HEADER_FILES) \
$(KotlinKit_OBJC_FILES)

KotlinKit_AGSDOC_FLAGS += -MakeFrames YES

#
# Makefiles
#
-include GNUmakefile.preamble
include $(GNUSTEP_MAKEFILES)/framework.make
-include GNUmakefile.postamble

#Only build documentation if doc=yes was passed on the command line
#
ifeq ($(doc),yes)
include $(GNUSTEP_MAKEFILES)/documentation.make
endif