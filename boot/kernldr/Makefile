#============================================================================
# kernldr - XNU Kernel Loader for ARM64 Android devices
#============================================================================
# Overridable variables:
#   CC, AS, LD, OBJCOPY
#   CFLAGS, ASFLAGS, LDFLAGS
#   O          - output directory (default: .)
#   ARCH       - arm64 or arm
#============================================================================

CC        ?= aarch64-linux-gnu-gcc
AS        ?= aarch64-linux-gnu-as
LD        ?= aarch64-linux-gnu-ld
OBJCOPY   ?= aarch64-linux-gnu-objcopy
ARCH      ?= arm64

CFLAGS    ?= -O2 -ffreestanding -mgeneral-regs-only -I. -Wall -Wextra
ASFLAGS   ?= -I.
LDFLAGS   ?= -nostdlib -Ttext=0x80080000 -N

# Output directory
O         ?= .

OBJDIR    := $(O)
OBJS      := $(OBJDIR)/vmldr.o $(OBJDIR)/XNUKernelMachO.o

ifeq ($(ARCH),arm64)
OBJS      += $(OBJDIR)/boot.arm64.o
else
OBJS      += $(OBJDIR)/boot.arm.o
endif

TARGET_ELF  := $(OBJDIR)/kernldr.elf
TARGET_BIN  := $(OBJDIR)/kernldr.bin

.SUFFIXES:

all: $(TARGET_BIN)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(TARGET_ELF): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $< $@
	@echo "  BUILT  $@"

$(OBJDIR)/vmldr.o: vmldr.c XNUKernelMachO.hpp | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/XNUKernelMachO.o: XNUKernelMachO.cpp XNUKernelMachO.hpp | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/boot.arm64.o: boot.arm64.S | $(OBJDIR)
	$(AS) $(ASFLAGS) -o $@ $<

$(OBJDIR)/boot.arm.o: boot.arm.S | $(OBJDIR)
	$(AS) $(ASFLAGS) -o $@ $<

.PHONY: clean distclean
clean:
	rm -f *.o *.elf *.bin
	rm -rf $(OBJDIR)/*.o $(OBJDIR)/*.elf $(OBJDIR)/*.bin
