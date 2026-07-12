# minlibc - Minimal C Runtime for XNU (macOS 10.7)
# ARM EABI (eabihf) support

# Toolchain configuration
CC = clang -target arm-apple-darwin11 -arch armv7
AS = arm-apple-darwin11-as
AR = arm-apple-darwin11-ar
RANLIB = arm-apple-darwin11-ranlib

# Architecture flags
ARCH_FLAGS = -marm -mfloat-abi=hard
CFLAGS = $(ARCH_FLAGS) -Os -fno-stack-protector -fno-asynchronous-unwind-tables \
         -I./include -Wno-builtin-declaration-mismatch
ASFLAGS = $(ARCH_FLAGS)
LDFLAGS = -nostdlib

# Source files
CRT_OBJ = crt0.o
LIBC_OBJ = libc.o

# Library targets
MINLIBC_A = libSystem.a
CRT_O = crt0.o

# Default target
all: $(CRT_O) $(MINLIBC_A)

# Build crt0.o
crt0.o: crt.s
	$(AS) $(ASFLAGS) -o $@ $<

# Build libc.o
libc.o: libc.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Create static library
$(MINLIBC_A): libc.o
	$(AR) rcs $@ $^
	$(RANLIB) $@

# Clean build artifacts
clean:
	rm -f *.o *.a

# Phony targets
.PHONY: all clean

# Build rules
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

.s.o:
	$(AS) $(ASFLAGS) -o $@ $<