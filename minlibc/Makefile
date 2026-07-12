# minlibc - Minimal C Runtime
# Supports: ARM EABI, i386, x86_64 on Linux and macOS/Darwin
#
# Usage:
#   make ARCH=x86_64                  # native build for host
#   make ARCH=x86_64 PLATFORM=darwin  # cross-target macOS
#   make ARCH=arm   PLATFORM=darwin   # cross-target ARM Darwin
#
#   make wrappers                     # wrapper for current ARCH/PLATFORM
#   make all-wrappers                 # wrappers for all combos

# Configuration
ARCH     ?= $(shell uname -m | sed 's/aarch64/arm64/')
PLATFORM ?= $(shell uname -s | tr '[:upper:]' '[:lower:]')

# Sysroot
SYSROOT = $(CURDIR)/sysroot

# Toolchain
CC = clang
AS = as
AR = ar
RANLIB = ranlib

# Per-platform flags
ifeq ($(PLATFORM),darwin)
    ifeq ($(ARCH),arm)
        TARGET_TRIPLE = arm-apple-darwin11
        ARCH_FLAGS = -arch armv7
        AS = $(TARGET_TRIPLE)-as
        AR = $(TARGET_TRIPLE)-ar
        RANLIB = $(TARGET_TRIPLE)-ranlib
    else ifeq ($(ARCH),arm64)
        TARGET_TRIPLE = arm64-apple-darwin
        ARCH_FLAGS = -arch arm64
    else ifeq ($(ARCH),x86_64)
        TARGET_TRIPLE = x86_64-apple-darwin
        ARCH_FLAGS = -arch x86_64
    else ifeq ($(ARCH),i386)
        TARGET_TRIPLE = i386-apple-darwin
        ARCH_FLAGS = -arch i386
    endif
    CC += -target $(TARGET_TRIPLE) $(ARCH_FLAGS)
else ifeq ($(PLATFORM),linux)
    ifeq ($(ARCH),i386)
        ARCH_FLAGS = -m32
        LDFLAGS_EXTRA = -m32
    endif
    CC += $(ARCH_FLAGS)
endif

CFLAGS = -Os -fno-stack-protector -fno-asynchronous-unwind-tables \
         -I$(SYSROOT)/include -Wno-implicit-function-declaration
ASFLAGS = $(ARCH_FLAGS)
LDFLAGS = -nostdlib -L$(SYSROOT)/lib $(LDFLAGS_EXTRA)

# Library targets
MINLIBC_A = libSystem.a
CRT_O = crt0.o

# Default target
all: $(CRT_O) $(MINLIBC_A) install wrappers

# Build crt0.o
crt0.o: crt.S
	$(CC) $(ASFLAGS) -c -o $@ $<
# Build libc.o
libc.o: libc.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Create static library
$(MINLIBC_A): libc.o
	$(AR) rcs $@ $^
	$(RANLIB) $@

# Install to sysroot
install: $(MINLIBC_A) $(CRT_O)
	mkdir -p $(SYSROOT)/lib $(SYSROOT)/bin
	cp $(MINLIBC_A) $(SYSROOT)/lib/
	cp $(CRT_O) $(SYSROOT)/lib/

# Generate wrappers for current ARCH/PLATFORM (both clang and gcc)
wrappers: install
	@echo "Generating wrappers for $(ARCH)-$(PLATFORM)..."
	@./gen-wrappers.sh $(SYSROOT) $(ARCH) $(PLATFORM) clang $(ARCH) $(PLATFORM) gcc

# Generate wrappers for ALL arch/platform/compiler combos
all-wrappers: install
	@echo "Generating all wrapper combinations..."
	@./gen-wrappers.sh $(SYSROOT)

# Clean
clean:
	rm -f *.o *.a
	rm -rf $(SYSROOT)/lib/* $(SYSROOT)/bin/*

.PHONY: all clean install wrappers all-wrappers

# Build rules
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

.s.o:
	$(AS) $(ASFLAGS) -o $@ $<
