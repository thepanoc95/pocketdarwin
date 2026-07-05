#include "../AndroidPlatformDevice.h"

#define TLMM_BASE               0x01000000
#define TLMM_SIZE               0x00300000

#define TLMM_GPIO_CTL(n)        (0x1000 + (n) * 0x1000)
#define TLMM_GPIO_CFG(n)        (0x1004 + (n) * 0x1000)
#define TLMM_GPIO_IN_OUT(n)     (0x1008 + (n) * 0x1000)
#define TLMM_GPIO_INTR_CFG(n)   (0x100C + (n) * 0x1000)
#define TLMM_GPIO_INTR_STATUS(n) (0x1010 + (n) * 0x1000)

#define TLMM_TLMM_GPIO_PIN_INTR  0x0600
#define TLMM_TLMM_GPIO_INTR_EN   0x0604

#define TLMM_GPIO_MAX            122

#define GPIO_CFG_FUNC_MASK       0x7
#define GPIO_CFG_FUNC_SHIFT      2
#define GPIO_CFG_DRV_STR_MASK    0x7
#define GPIO_CFG_DRV_STR_SHIFT   6
#define GPIO_CFG_PULL_MASK       0x3
#define GPIO_CFG_PULL_SHIFT      10
#define GPIO_CFG_OE              (1U << 9)

#define GPIO_PULL_NONE           0
#define GPIO_PULL_DOWN           1
#define GPIO_PULL_KEEPER         2
#define GPIO_PULL_UP             3

#define GPIO_FUNC_GPIO           0
#define GPIO_FUNC_FUNC1          1
#define GPIO_FUNC_FUNC2          2
#define GPIO_FUNC_FUNC3          3

#define GPIO_DRV_STR_2MA         0
#define GPIO_DRV_STR_4MA         1
#define GPIO_DRV_STR_6MA         2
#define GPIO_DRV_STR_8MA         3
#define GPIO_DRV_STR_10MA        4
#define GPIO_DRV_STR_12MA        5
#define GPIO_DRV_STR_14MA        6
#define GPIO_DRV_STR_16MA        7

class MSM8916TLMM : public AndroidPlatformDevice
{
    OSDeclareDefaultStructors(MSM8916TLMM)

public:
    bool start(IOService* provider) override
    {
        if (!AndroidPlatformDevice::start(provider))
            return false;

        kprintf("[MSM8916TLMM] starting TLMM GPIO controller\n");

        for (int i = 0; i < TLMM_GPIO_MAX; i++) {
            UInt32 cfg = read32(TLMM_GPIO_CFG(i));
            cfg &= ~(GPIO_CFG_PULL_MASK << GPIO_CFG_PULL_SHIFT);
            cfg |= (GPIO_PULL_NONE << GPIO_CFG_PULL_SHIFT);
            cfg &= ~(GPIO_CFG_FUNC_MASK << GPIO_CFG_FUNC_SHIFT);
            cfg |= (GPIO_FUNC_GPIO << GPIO_CFG_FUNC_SHIFT);
            write32(TLMM_GPIO_CFG(i), cfg);
        }

        write32(TLMM_TLMM_GPIO_INTR_EN, 0);

        registerService();
        kprintf("[MSM8916TLMM] GPIO controller ready\n");
        return true;
    }

    void gpioSetFunction(UInt32 pin, UInt32 funcSel)
    {
        if (pin >= TLMM_GPIO_MAX) return;
        UInt32 cfg = read32(TLMM_GPIO_CFG(pin));
        cfg &= ~(GPIO_CFG_FUNC_MASK << GPIO_CFG_FUNC_SHIFT);
        cfg |= (funcSel & GPIO_CFG_FUNC_MASK) << GPIO_CFG_FUNC_SHIFT;
        write32(TLMM_GPIO_CFG(pin), cfg);
    }

    void gpioSetOutput(UInt32 pin, bool output)
    {
        if (pin >= TLMM_GPIO_MAX) return;
        UInt32 cfg = read32(TLMM_GPIO_CFG(pin));
        if (output)
            cfg |= GPIO_CFG_OE;
        else
            cfg &= ~GPIO_CFG_OE;
        write32(TLMM_GPIO_CFG(pin), cfg);
    }

    void gpioSet(UInt32 pin, bool high)
    {
        if (pin >= TLMM_GPIO_MAX) return;
        UInt32 io = read32(TLMM_GPIO_IN_OUT(pin));
        if (high)
            io |= 1U;
        else
            io &= ~1U;
        write32(TLMM_GPIO_IN_OUT(pin), io);
    }

    bool gpioGet(UInt32 pin)
    {
        if (pin >= TLMM_GPIO_MAX) return false;
        return (read32(TLMM_GPIO_IN_OUT(pin)) & 1U) != 0;
    }

    void gpioSetPull(UInt32 pin, UInt32 pullMode)
    {
        if (pin >= TLMM_GPIO_MAX) return;
        UInt32 cfg = read32(TLMM_GPIO_CFG(pin));
        cfg &= ~(GPIO_CFG_PULL_MASK << GPIO_CFG_PULL_SHIFT);
        cfg |= (pullMode & GPIO_CFG_PULL_MASK) << GPIO_CFG_PULL_SHIFT;
        write32(TLMM_GPIO_CFG(pin), cfg);
    }

    void gpioSetDriveStrength(UInt32 pin, UInt32 mA)
    {
        if (pin >= TLMM_GPIO_MAX) return;
        UInt32 drvStr = (mA > GPIO_DRV_STR_16MA) ? GPIO_DRV_STR_16MA : mA;
        UInt32 cfg = read32(TLMM_GPIO_CFG(pin));
        cfg &= ~(GPIO_CFG_DRV_STR_MASK << GPIO_CFG_DRV_STR_SHIFT);
        cfg |= (drvStr & GPIO_CFG_DRV_STR_MASK) << GPIO_CFG_DRV_STR_SHIFT;
        write32(TLMM_GPIO_CFG(pin), cfg);
    }
};

OSDefineMetaClassAndStructors(MSM8916TLMM, AndroidPlatformDevice)
