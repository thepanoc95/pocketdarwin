#
# PocketDarwin per-board configuration
#
# Usage: make BOARD=<name>
#        make BOARD=harpia kernel
#

# ---------------------------------------------------------------------------
# Board: harpia (Moto G4 Play / MSM8916)
# ---------------------------------------------------------------------------
ifeq ($(BOARD),harpia)
	ARCH            := arm
	CROSS_COMPILE   ?= arm-linux-gnueabi-
	KERNEL_CONFIG   ?= DEBUG
	ARCH_CONFIG     := ARM
	MACHINE_CONFIG  := MSM8916_HARPIA
	PHYS_MEM_BASE   := 0x80000000
	PHYS_MEM_SIZE   := 0x80000000  # 2 GB
	KERNEL_LOAD_ADDR  := 0x80080000
	RAMDISK_ADDR      := 0x84000000
	TAGS_ADDR         := 0x80000100
	UART_BASE         := 0x78b0000   # BLSP UART2
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
