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

# Architecture
ARCH            := arm64
CROSS_COMPILE   ?= aarch64-linux-gnu-

# Kernel config
KERNEL_CONFIG   ?= RELEASE
ARCH_CONFIG     := ARM64
MACHINE_CONFIG  := DEFAULT

# Memory map (from msm8916-memory.dtsi)
PHYS_MEM_BASE   := 0x80000000
PHYS_MEM_SIZE   := 0x80000000  # 2 GB

# Boot image layout (typical for MSM8916)
KERNEL_LOAD_ADDR  := 0x80080000
RAMDISK_ADDR      := 0x84000000
TAGS_ADDR         := 0x80000100

# UART for early debug
UART_BASE         := 0x78b0000   # BLSP UART2

# Device tree
DEVICETREE_DTS    := devicetree/harpia/main.dts
DEVICETREE_INCLUDES := devicetree/harpia/

endif

# ---------------------------------------------------------------------------
# Board: placeholder (add more boards below)
# ---------------------------------------------------------------------------
# ifeq ($(BOARD),some_other_board)
#   ARCH := arm64
#   CROSS_COMPILE ?= aarch64-linux-gnu-
#   ...
# endif
