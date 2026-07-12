#
# mindyld - Custom Dynamic Linker/Loader Build System
# Supports: Linux, FreeBSD, OpenBSD, NetBSD, macOS
# Architectures: x86, x86_64, ARM, ARM64
#

# ============================================================================
# Build Configuration
# ============================================================================

# Detect platform
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

ifeq ($(UNAME_S),Darwin)
    PLATFORM := APPLE
    PLATFORM_FLAGS := -DMINDYLD_APPLE -DMINDYLD_MACHO
    SO_EXT := .dylib
else ifeq ($(UNAME_S),Linux)
    PLATFORM := LINUX
    PLATFORM_FLAGS := -DMINDYLD_LINUX -DMINDYLD_ELF
    SO_EXT := .so
else ifeq ($(UNAME_S),FreeBSD)
    PLATFORM := BSD
    PLATFORM_FLAGS := -DMINDYLD_BSD -DMINDYLD_ELF
    SO_EXT := .so
else ifeq ($(UNAME_S),OpenBSD)
    PLATFORM := BSD
    PLATFORM_FLAGS := -DMINDYLD_BSD -DMINDYLD_ELF
    SO_EXT := .so
else ifeq ($(UNAME_S),NetBSD)
    PLATFORM := BSD
    PLATFORM_FLAGS := -DMINDYLD_BSD -DMINDYLD_ELF
    SO_EXT := .so
else
    $(error Unsupported platform: $(UNAME_S))
endif

# Detect architecture
ifeq ($(UNAME_M),x86_64)
    ARCH := x86_64
    ARCH_FLAGS := -m64
else ifeq ($(UNAME_M),i386)
    ARCH := i386
    ARCH_FLAGS := -m32
else ifeq ($(UNAME_M),arm64)
    ARCH := arm64
    ARCH_FLAGS := -march=armv8-a
else ifeq ($(UNAME_M),armv7l)
    ARCH := armv7
    ARCH_FLAGS := -march=armv7-a -mfpu=neon
else
    ARCH := unknown
    ARCH_FLAGS :=
endif

# ============================================================================
# Compiler Configuration
# ============================================================================

CC := clang
CFLAGS := -Wall -Wextra -Werror -fPIC -nostdlib \
          -D_GNU_SOURCE $(ARCH_FLAGS) $(PLATFORM_FLAGS) \
          -I. -I../minlibc/include

LDFLAGS := -shared -nostdlib $(ARCH_FLAGS)
AR := ar
RANLIB := ranlib

# Enable debug symbols if DEBUG=1
ifdef DEBUG
    CFLAGS += -g -DMINDYLD_DEBUG
else
    CFLAGS += -O2
endif

# ============================================================================
# Source Files
# ============================================================================

SOURCES := \
    mindyld_core.c \
    elf_loader.c \
    macho_loader.c

HEADERS := \
    mindyld_common.h \
    mindyld_elf.h \
    mindyld_macho.h

# Platform-specific sources (not used yet, but could be extended)
ifeq ($(PLATFORM),LINUX)
    SOURCES += platform_linux.c
endif

ifeq ($(PLATFORM),APPLE)
    SOURCES += platform_apple.c
endif

# ============================================================================
# Build Targets
# ============================================================================

OBJECTS := $(SOURCES:.c=.o)
LIBRARY := libmindyld.a
SHARED_LIB := libmindyld$(SO_EXT)

.PHONY: all clean install debug release help

# Default target
all: $(LIBRARY) $(SHARED_LIB)

# Debug build
debug: DEBUG := 1
debug: clean all

# Release build
release: DEBUG :=
release: clean all

# ============================================================================
# Compilation Rules
# ============================================================================

%.o: %.c $(HEADERS)
	@echo "[CC] $< ($(PLATFORM) $(ARCH))"
	@$(CC) $(CFLAGS) -c -o $@ $<

$(LIBRARY): $(OBJECTS)
	@echo "[AR] Creating static library: $@"
	@$(AR) rcs $@ $^
	@$(RANLIB) $@

$(SHARED_LIB): $(OBJECTS)
	@echo "[LD] Creating shared library: $@"
	@$(CC) $(LDFLAGS) -o $@ $^

# ============================================================================
# Testing
# ============================================================================

test: all
	@echo "[TEST] Running mindyld tests..."
	@# TODO: Add test execution

# ============================================================================
# Installation
# ============================================================================

install: all
	@echo "[INSTALL] Installing mindyld to /usr/local..."
	@install -d /usr/local/lib
	@install -d /usr/local/include
	@install -m 644 $(LIBRARY) /usr/local/lib/
	@install -m 644 $(SHARED_LIB) /usr/local/lib/
	@install -m 644 mindyld_common.h /usr/local/include/
	@install -m 644 mindyld_elf.h /usr/local/include/
	@install -m 644 mindyld_macho.h /usr/local/include/

# ============================================================================
# Cleaning
# ============================================================================

clean:
	@echo "[CLEAN] Removing build artifacts..."
	@rm -f $(OBJECTS) $(LIBRARY) $(SHARED_LIB) *.o *.a *.so *.dylib

distclean: clean
	@echo "[DISTCLEAN] Complete cleanup"
	@rm -f *~ *.bak *.orig

# ============================================================================
# Help
# ============================================================================

help:
	@echo "mindyld Build System"
	@echo "===================="
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Build static and shared libraries (default)"
	@echo "  debug     - Build with debug symbols"
	@echo "  release   - Build optimized release version"
	@echo "  test      - Run tests"
	@echo "  install   - Install to /usr/local"
	@echo "  clean     - Remove build artifacts"
	@echo "  distclean - Remove all generated files"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Platform: $(PLATFORM)"
	@echo "Architecture: $(ARCH)"
	@echo "Compiler: $(CC)"

# ============================================================================
# Build Information
# ============================================================================

.PHONY: info

info:
	@echo "mindyld Build Information"
	@echo "========================="
	@echo "Platform:         $(PLATFORM)"
	@echo "Architecture:     $(ARCH)"
	@echo "Compiler:         $(CC)"
	@echo "CFLAGS:           $(CFLAGS)"
	@echo "LDFLAGS:          $(LDFLAGS)"
	@echo "Sources:          $(SOURCES)"
	@echo "Objects:          $(OBJECTS)"
	@echo "Library:          $(LIBRARY)"
	@echo "Shared Library:   $(SHARED_LIB)"
