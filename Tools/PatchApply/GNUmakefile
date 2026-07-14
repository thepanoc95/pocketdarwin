# GNUmakefile for PatchApply GUI application, SysBuild GUI app, and clipatche CLI tool

include $(GNUSTEP_MAKEFILES)/common.make

# GUI Application - PatchApply
APP_NAME = PatchApply
PatchApply_OBJC_FILES = PatchApply.m
PatchApply_RESOURCE_FILES =
PatchApply_FRAMEWORKS = AppKit Foundation
PatchApply_INSTALL_PATH = /Applications
include $(GNUSTEP_MAKEFILES)/application.make

# GUI Application - SysBuild
APP_NAME = SysBuild
SysBuild_OBJC_FILES = SysBuild.m
SysBuild_RESOURCE_FILES =
SysBuild_FRAMEWORKS = AppKit Foundation
SysBuild_INSTALL_PATH = /Applications
include $(GNUSTEP_MAKEFILES)/application.make

# CLI Tool
TOOL_NAME = clipatche
clipatche_OBJC_FILES = pdpatch.m
clipatche_FRAMEWORKS = Foundation
clipatche_INSTALL_PATH = /usr/local/bin
include $(GNUSTEP_MAKEFILES)/tool.make