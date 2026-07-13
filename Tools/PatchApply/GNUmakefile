# GNUmakefile for PatchApply GUI application and clipatche CLI tool

include $(GNUSTEP_MAKEFILES)/common.make

# GUI Application
APP_NAME = PatchApply
PatchApply_OBJC_FILES = PatchApply.m
PatchApply_RESOURCE_FILES =
PatchApply_FRAMEWORKS = AppKit Foundation
PatchApply_INSTALL_PATH = /Applications
include $(GNUSTEP_MAKEFILES)/application.mk

# CLI Tool
TOOL_NAME = clipatche
clipatche_OBJC_FILES = pdpatch.m
clipatche_FRAMEWORKS = Foundation
clipatche_INSTALL_PATH = /usr/local/bin
include $(GNUSTEP_MAKEFILES)/tool.mk
