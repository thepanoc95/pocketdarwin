#
# PocketDarwin per-board configuration
#
# Usage: make BOARD=<name>
#        make BOARD=harpia kernel
#        make BOARD=harpia64 kernel   (ARM64 / AArch64 build)
#

# ---------------------------------------------------------------------------
# Board: harpia (Moto G4 Play / MSM8916) — ARM (AArch32)
# ---------------------------------------------------------------------------
ifeq ($(BOARD),harpia)
	ARCH            := arm
	CROSS_COMPILE   ?= arm-linux-gnueabi-
	KERNEL_CONFIG   ?= DEBUG
	ARCH_CONFIG     := ARM
	MACHINE_CONFIG  := MSM8916_HARPIA
	PHYS_MEM_BASE   := 0x80000000
	PHYS_MEM_SIZE   := 0x80000000  # 2 GB
	KERNEL_LOAD_ADDR  := 0x80008000
	RAMDISK_ADDR      := 0x84000000
	TAGS_ADDR         := 0x80000100
	UART_BASE         := 0x78b0000   # BLSP UART2
	DEVICETREE_DTS    := devicetree/harpia/arch/arm/boot/dts/qcom/msm8916-harpia.dtsi
	DEVICETREE_INCLUDES := devicetree/harpia/include \
							devicetree/harpia/arm/boot/dts/qcom/include
endif

# ---------------------------------------------------------------------------
# Board: harpia64 (Moto G4 Play / MSM8916) — ARM64 (AArch64)
#
# The MSM8916 Snapdragon 410 has quad-core Cortex-A53 which supports
# both AArch32 and AArch64 execution modes.  This target builds the
# XNU kernel as a 64-bit ARMv8-A binary.
#
# The kernel image base matches the ARM64 convention used by Apple's
# FOUNDATION / VEXPRESS platforms: 0xFFFFFF8000202000 (512MB-aligned
# static base).  The bootloader maps physical 0x80000000 to this
# virtual address.
# ---------------------------------------------------------------------------
ifeq ($(BOARD),harpia64)
	ARCH            := arm64
	CROSS_COMPILE   ?= aarch64-linux-gnu-
	KERNEL_CONFIG   ?= DEBUG
	ARCH_CONFIG     := ARM64
	MACHINE_CONFIG  := MSM8916_HARPIA
	PHYS_MEM_BASE   := 0x80000000
	PHYS_MEM_SIZE   := 0x80000000  # 2 GB
	KERNEL_LOAD_ADDR  := 0x80008000
	RAMDISK_ADDR      := 0x84000000
	TAGS_ADDR         := 0x80000100
	UART_BASE         := 0x78b0000
	DEVICETREE_DTS    := devicetree/harpia/arch/arm/boot/dts/qcom/msm8916-harpia.dtsi
	DEVICETREE_INCLUDES := devicetree/harpia/include \
							devicetree/harpia/arm/boot/dts/qcom/include
endif

# ---------------------------------------------------------------------------
# Board: placeholder (add more boards below)
# ---------------------------------------------------------------------------
# ifeq ($(BOARD),some_other_board)
#   ARCH := arm64
#   CROSS_COMPILE ?= aarch64-linux-gnu-
#   ...
# endif
