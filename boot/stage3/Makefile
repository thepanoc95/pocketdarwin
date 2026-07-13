# ============================================================================
# PocketDarwin ELF Loader - EBBR UEFI Application
# ============================================================================
#
# Produces: bootaa64.efi  (PE/COFF EFI Application for AArch64)
#
# Usage:
#   make              Build bootaa64.efi
#   make clean        Remove build artifacts
#
# Requires: aarch64-linux-gnu-gcc, aarch64-linux-gnu-ld,
#           aarch64-linux-gnu-objcopy
# ============================================================================

CROSS    ?= aarch64-linux-gnu-
CC        = $(CROSS)gcc
LD        = $(CROSS)ld
OBJCOPY   = $(CROSS)objcopy
OBJDUMP   = $(CROSS)objdump

CFLAGS    = -fpic -fshort-wchar -ffreestanding -fno-stack-protector \
            -Wall -Wextra -O2 -I.
LDFLAGS   = -T efi_ldr.ld -nostdlib
OBJCFLAGS = -I elf64-littleaarch64 -O pei-aarch64-little --subsystem=10 \
            --image-base=0x0

TARGET    = bootaa64.efi
SOBJ      = main.so

SRCS      = main.c
HEADERS   = uefi.h elf.h font.h

# ============================================================================
# Targets
# ============================================================================

all: $(TARGET)

$(SOBJ): $(SRCS) $(HEADERS) efi_ldr.ld
	$(CC) $(CFLAGS) -c main.c -o main.o
	$(LD) $(LDFLAGS) main.o -o $(SOBJ)

$(TARGET): $(SOBJ)
	$(OBJCOPY) $(OBJCFLAGS) $< $@
	@echo "Built: $@"

disasm: $(SOBJ)
	$(OBJDUMP) -d $< > $(SOBJ).asm

size: $(TARGET)
	@ls -la $(TARGET)

clean:
	rm -f *.o *.so *.efi *.asm

.PHONY: all clean disasm size
