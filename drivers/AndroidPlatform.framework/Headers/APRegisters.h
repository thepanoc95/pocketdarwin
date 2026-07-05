#ifndef _AP_REGISTERS_H_
#define _AP_REGISTERS_H_

#include <AndroidPlatform/APTypes.h>

struct APRegField {
    uint32_t offset;
    uint8_t  bitHigh;
    uint8_t  bitLow;
    const char* name;
};

struct APRegBlock {
    const char*    name;
    ap_addr_t      physBase;
    ap_size_t      size;
    const struct APRegField* fields;
    uint32_t       numFields;
};

#define AP_REG(block, offset) ((block) + (offset))
#define AP_FIELD(reg, field)  ap_extract((reg), (field).bitHigh, (field).bitLow)
#define AP_INSERT(reg, field, val) ap_insert((reg), (val), (field).bitHigh, (field).bitLow)
#define AP_BIT(offset, bit)   { (offset), (bit), (bit), #bit }
#define AP_FLD(offset, hi, lo, n) { (offset), (hi), (lo), (n) }

#define AP_READ32(base, offset)     ap_reg_read32((volatile void*)((uintptr_t)(base) + (offset)))
#define AP_WRITE32(base, offset, v) ap_reg_write32((volatile void*)((uintptr_t)(base) + (offset)), (v))
#define AP_SET32(base, offset, m)   ap_reg_set32((volatile void*)((uintptr_t)(base) + (offset)), (m))
#define AP_CLR32(base, offset, m)   ap_reg_clear32((volatile void*)((uintptr_t)(base) + (offset)), (m))

struct APRegBlock APRegBlockMake(const char* name, ap_addr_t base, ap_size_t size);

/* ------------------------------------------------------------------ *
 * MSM8916 SoC register blocks
 * ------------------------------------------------------------------ */

#define AP_MSM8916_GCC_BASE       0x1800000
#define AP_MSM8916_GCC_SIZE       0xD000
#define AP_MSM8916_TLMM_BASE      0x1000000
#define AP_MSM8916_TLMM_SIZE      0x300000
#define AP_MSM8916_BLSP1_BASE     0x078B0000
#define AP_MSM8916_BLSP1_SIZE     0x10000
#define AP_MSM8916_BLSP2_BASE     0x07AC0000
#define AP_MSM8916_BLSP2_SIZE     0x10000
#define AP_MSM8916_SDCC1_BASE     0x07860000
#define AP_MSM8916_SDCC2_BASE     0x07864000
#define AP_MSM8916_SDCC_SIZE      0x4000
#define AP_MSM8916_USB_BASE       0x07800000
#define AP_MSM8916_USB_SIZE       0x1000000
#define AP_MSM8916_MDP_BASE       0x01A00000
#define AP_MSM8916_MDP_SIZE       0x100000
#define AP_MSM8916_HDMI_BASE      0x0A00000
#define AP_MSM8916_HDMI_SIZE      0x100000
#define AP_MSM8916_SPMI_BASE      0x0200F000
#define AP_MSM8916_SPMI_SIZE      0x1000
#define AP_MSM8916_RPM_BASE       0x200000
#define AP_MSM8916_RPM_SIZE       0x1000

/* --- GCC register offsets --- */
#define AP_GCC_PLL_CTL            0x3000
#define AP_GCC_PLL_STATUS         0x3004
#define AP_GCC_PLL_CONFIG         0x3008
#define AP_GCC_APCS_CLK_BR        0x2020
#define AP_GCC_MMSS_CLK_BR        0x5014
#define AP_GCC_BLSP1_AHB_CLK      0x5808
#define AP_GCC_BLSP1_SLP_CLK      0x580C
#define AP_GCC_BLSP2_AHB_CLK      0x5818
#define AP_GCC_BLSP2_SLP_CLK      0x581C
#define AP_GCC_SDCC1_AHB_CLK      0x5868
#define AP_GCC_SDCC1_APPS_CLK     0x586C
#define AP_GCC_SDCC2_AHB_CLK      0x5878
#define AP_GCC_SDCC2_APPS_CLK     0x587C
#define AP_GCC_USB_AHB_CLK        0x58E8
#define AP_GCC_USB_SLP_CLK        0x58EC
#define AP_GCC_USB_HS_SYS_CLK     0x58F0
#define AP_GCC_USB_PHY_CFG_AHB    0x58F8
#define AP_GCC_GPIO_CLK           0x5900
#define AP_GCC_MDP_AHB_CLK        0x5A04
#define AP_GCC_MDP_AXI_CLK        0x5A08
#define AP_GCC_MDP_VSYNC_CLK      0x5A0C
#define AP_GCC_MDP_MDP_CLK        0x5A10
#define AP_GCC_OXILI_GFX3D_CLK    0x5908
#define AP_GCC_OXILI_AHB_CLK      0x5904
#define AP_GCC_OXILI_AXI_CLK      0x590C

/* --- TLMM register offsets --- */
#define AP_TLMM_GPIO_CTL(n)       (0x1000 + (n) * 0x1000)
#define AP_TLMM_GPIO_CFG(n)       (0x1004 + (n) * 0x1000)
#define AP_TLMM_GPIO_INOUT(n)     (0x1008 + (n) * 0x1000)
#define AP_TLMM_GPIO_INTR_CFG(n)  (0x100C + (n) * 0x1000)
#define AP_TLMM_INTR_EN           0x0604
#define AP_TLMM_INTR_STATUS       0x0600

/* --- SDHCI register offsets --- */
#define AP_SDHCI_SDMA_ADDR       0x000
#define AP_SDHCI_BLOCK_SIZE      0x004
#define AP_SDHCI_BLOCK_COUNT     0x006
#define AP_SDHCI_ARGUMENT        0x008
#define AP_SDHCI_TRANSFER_MODE   0x00C
#define AP_SDHCI_COMMAND         0x00E
#define AP_SDHCI_RESPONSE        0x010
#define AP_SDHCI_BUFFER          0x020
#define AP_SDHCI_PRESENT_STATE   0x024
#define AP_SDHCI_HOST_CTRL       0x028
#define AP_SDHCI_POWER_CTRL      0x029
#define AP_SDHCI_CLOCK_CTRL      0x02C
#define AP_SDHCI_TIMEOUT_CTRL    0x02E
#define AP_SDHCI_SOFT_RESET      0x02F
#define AP_SDHCI_INT_STATUS      0x030
#define AP_SDHCI_INT_ENABLE      0x034
#define AP_SDHCI_SIGNAL_EN       0x038
#define AP_SDHCI_HOST_CTRL2      0x03E
#define AP_SDHCI_CAPABILITIES    0x040
#define AP_SDHCI_CAPABILITIES2   0x044

/* --- USB CI register offsets --- */
#define AP_USB_REVISION          0x0000
#define AP_USB_CTRL              0x0004
#define AP_USB_STATUS            0x0008
#define AP_USB_FRAME_INDEX       0x000C
#define AP_USB_PORTSC            0x0044
#define AP_USB_CONFIG_FLAG       0x0050
#define AP_USB_OTGSC             0x0064
#define AP_USB_ASYNCLISTADDR     0x0018
#define AP_USB_PERIODICLISTBASE  0x0014

/* --- SPMI register offsets --- */
#define AP_SPMI_CTRL             0x000
#define AP_SPMI_STATUS           0x004
#define AP_SPMI_IRQ_STATUS       0x008
#define AP_SPMI_IRQ_ENABLE       0x00C
#define AP_SPMI_CMD              0x020
#define AP_SPMI_ADDR             0x024
#define AP_SPMI_DATA             0x028
#define AP_SPMI_RD_DATA          0x02C
#define AP_SPMI_ARB_CTRL         0x100
#define AP_SPMI_ARB_CHAN_CFG(n)  (0x200 + (n) * 0x20)

/* --- MDP register offsets --- */
#define AP_MDP_CTRL              0x00000
#define AP_MDP_DMA_P_CONFIG      0x02004
#define AP_MDP_DMA_P_BASE        0x02008
#define AP_MDP_DMA_P_SRC_STRIDE  0x0200C
#define AP_MDP_DMA_P_DST_STRIDE  0x02010
#define AP_MDP_DISPLAY_CMD_TRIG  0x0003C
#define AP_MDP_VSYNC_CTL         0x00040
#define AP_MDP_INTR_EN           0x00050
#define AP_MDP_INTR_STATUS       0x00054

#endif /* _AP_REGISTERS_H_ */
