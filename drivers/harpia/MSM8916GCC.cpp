#include "../AndroidPlatformDevice.h"

#define GCC_MAP_START           0x1800000
#define GCC_MAP_SIZE            0xD000

#define GCC_PLL_CTL             0x3000
#define GCC_PLL_STATUS          0x3004
#define GCC_PLL_CONFIG          0x3008

#define GCC_APCS_CLOCK_BRANCH   0x2020
#define GCC_MMSS_CLOCK_BRANCH   0x5014
#define GCC_BLSP1_AHB_CLOCK     0x5808
#define GCC_BLSP1_SLEEP_CLOCK   0x580C
#define GCC_BLSP2_AHB_CLOCK     0x5818
#define GCC_BLSP2_SLEEP_CLOCK   0x581C
#define GCC_SDCC1_AHB_CLOCK     0x5868
#define GCC_SDCC1_APPS_CLOCK    0x586C
#define GCC_SDCC2_AHB_CLOCK     0x5878
#define GCC_SDCC2_APPS_CLOCK    0x587C
#define GCC_USB_AHB_CLOCK       0x58E8
#define GCC_USB_SLEEP_CLOCK     0x58EC
#define GCC_USB_HS_SYSTEM_CLOCK 0x58F0
#define GCC_USB_PHY_CFG_AHB     0x58F8
#define GCC_GPIO_CLOCK          0x5900
#define GCC_MDP_AHB_CLOCK       0x5A04
#define GCC_MDP_AXI_CLOCK       0x5A08
#define GCC_MDP_VSYNC_CLOCK     0x5A0C
#define GCC_MDP_MDP_CLOCK       0x5A10

#define GCC_CLK_BRANCH_ENA      (1U << 0)
#define GCC_CLK_BRANCH_ROOT_ENA (1U << 1)
#define GCC_CLK_BRANCH_OFF      0

struct MSM8916ClockBranch {
    const char* name;
    uint32_t    ctlOffset;
    bool        isCritical;
};

class MSM8916GCC : public AndroidPlatformDevice
{
    OSDeclareDefaultStructors(MSM8916GCC)

public:
    bool start(IOService* provider) override
    {
        if (!AndroidPlatformDevice::start(provider))
            return false;

        kprintf("[MSM8916GCC] starting Global Clock Controller\n");

        UInt32 pllCtl = read32(GCC_PLL_CTL);
        if (!(pllCtl & (1U << 31))) {
            kprintf("[MSM8916GCC] PLL not yet configured, enabling\n");
            write32(GCC_PLL_CONFIG, 0x00000001);
            write32(GCC_PLL_CTL, pllCtl | (1U << 31));
            IOSleep(5);
        }

        write32(GCC_BLSP1_AHB_CLOCK, GCC_CLK_BRANCH_ENA);
        write32(GCC_BLSP1_SLEEP_CLOCK, GCC_CLK_BRANCH_ENA);
        write32(GCC_BLSP2_AHB_CLOCK, GCC_CLK_BRANCH_ENA);
        write32(GCC_BLSP2_SLEEP_CLOCK, GCC_CLK_BRANCH_ENA);
        write32(GCC_GPIO_CLOCK, GCC_CLK_BRANCH_ENA);
        write32(GCC_MDP_AHB_CLOCK, GCC_CLK_BRANCH_ENA);
        write32(GCC_MDP_AXI_CLOCK, GCC_CLK_BRANCH_ENA);
        write32(GCC_MDP_MDP_CLOCK, GCC_CLK_BRANCH_ENA);

        registerService();

        kprintf("[MSM8916GCC] core clocks enabled\n");
        return true;
    }

    virtual IOReturn enableBranchClock(const char* name, uint32_t clockCtlOffset)
    {
        write32(clockCtlOffset, GCC_CLK_BRANCH_ENA);
        return kIOReturnSuccess;
    }

    virtual IOReturn disableBranchClock(uint32_t clockCtlOffset)
    {
        write32(clockCtlOffset, GCC_CLK_BRANCH_OFF);
        return kIOReturnSuccess;
    }
};

OSDefineMetaClassAndStructors(MSM8916GCC, AndroidPlatformDevice)
