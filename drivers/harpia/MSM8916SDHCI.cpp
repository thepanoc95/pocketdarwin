#include "../AndroidPlatformDevice.h"

#define SDCC1_BASE              0x07860000
#define SDCC2_BASE              0x07864000
#define SDCC_SIZE               0x4000

#define SDHCI_SDMA_ADDR         0x000
#define SDHCI_BLOCK_SIZE        0x004
#define SDHCI_BLOCK_COUNT       0x006
#define SDHCI_ARGUMENT          0x008
#define SDHCI_TRANSFER_MODE     0x00C
#define SDHCI_COMMAND           0x00E
#define SDHCI_RESPONSE_0        0x010
#define SDHCI_RESPONSE_1        0x014
#define SDHCI_RESPONSE_2        0x018
#define SDHCI_RESPONSE_3        0x01C
#define SDHCI_BUFFER            0x020
#define SDHCI_PRESENT_STATE     0x024
#define SDHCI_HOST_CONTROL      0x028
#define SDHCI_POWER_CONTROL     0x029
#define SDHCI_CLOCK_CONTROL     0x02C
#define SDHCI_TIMEOUT_CONTROL   0x02E
#define SDHCI_SOFTWARE_RESET    0x02F
#define SDHCI_INT_STATUS        0x030
#define SDHCI_INT_ENABLE        0x034
#define SDHCI_SIGNAL_ENABLE     0x038
#define SDHCI_ACMD12_ERR        0x03C
#define SDHCI_HOST_CONTROL2     0x03E
#define SDHCI_CAPABILITIES      0x040
#define SDHCI_CAPABILITIES_1    0x044
#define SDHCI_MAX_CURRENT       0x048
#define SDHCI_ADMA_ERR          0x054
#define SDHCI_ADMA_ADDR         0x058
#define SDHCI_PRESET_VAL0       0x060
#define SDHCI_PRESET_VAL1       0x064
#define SDHCI_BOOT_CONTROL      0x06C
#define SDHCI_BOOT_TIMEOUT      0x06E
#define SDHCI_SLOT_INT_STATUS   0x0FC
#define SDHCI_HOST_VERSION      0x0FE

#define SDHCI_INT_RESPONSE      (1U << 0)
#define SDHCI_INT_DATA_END      (1U << 1)
#define SDHCI_INT_DMA_END       (1U << 2)
#define SDHCI_INT_SPACE_AVAIL   (1U << 3)
#define SDHCI_INT_DATA_AVAIL    (1U << 4)
#define SDHCI_INT_CARD_INSERT   (1U << 6)
#define SDHCI_INT_CARD_REMOVE   (1U << 7)
#define SDHCI_INT_CARD_INT      (1U << 8)
#define SDHCI_INT_ERROR         (1U << 15)
#define SDHCI_INT_TIMEOUT       (1U << 16)
#define SDHCI_INT_CRC           (1U << 17)
#define SDHCI_INT_END_BIT       (1U << 18)
#define SDHCI_INT_INDEX         (1U << 19)

#define SDHCI_CMD_INDEX_SHIFT   8
#define SDHCI_CMD_ISDATA        (1U << 5)
#define SDHCI_CMD_CMD23         (1U << 4)
#define SDHCI_CMD_BUSY          (1U << 3)
#define SDHCI_CMD_RESP_48       (1U << 1)
#define SDHCI_CMD_RESP_136      (1U << 2)
#define SDHCI_CMD_RESP_NONE     0

#define SDHCI_TRNS_DMA          (1U << 0)
#define SDHCI_TRNS_BLK_CNT_EN   (1U << 1)
#define SDHCI_TRNS_READ         (1U << 4)
#define SDHCI_TRNS_MULTI        (1U << 5)
#define SDHCI_TRNS_ACMD12       (1U << 2)

#define SDHCI_SRST_CMD          (1U << 1)
#define SDHCI_SRST_DATA         (1U << 2)
#define SDHCI_SRST_ALL          (1U << 0)

#define SDHCI_CLK_INT_EN        (1U << 0)
#define SDHCI_CLK_INT_STABLE    (1U << 1)
#define SDHCI_CLK_SDCLK_EN      (1U << 2)
#define SDHCI_CLK_FREQ_MASK     0xFF
#define SDHCI_CLK_FREQ_SHIFT    8

#define SDHCI_STATE_CMD_INHIBIT   (1U << 0)
#define SDHCI_STATE_DATA_INHIBIT  (1U << 1)

class MSM8916SDHCI : public AndroidPlatformDevice
{
    OSDeclareDefaultStructors(MSM8916SDHCI)

private:
    bool fIsSdcc1;
    bool fCardPresent;

    IOReturn resetCmdData(void)
    {
        write8(SDHCI_SOFTWARE_RESET, SDHCI_SRST_CMD);
        int timeout = 10000;
        while (timeout--) {
            if (!(read8(SDHCI_SOFTWARE_RESET) & SDHCI_SRST_CMD))
                break;
        }
        write8(SDHCI_SOFTWARE_RESET, SDHCI_SRST_DATA);
        timeout = 10000;
        while (timeout--) {
            if (!(read8(SDHCI_SOFTWARE_RESET) & SDHCI_SRST_DATA))
                break;
        }
        return kIOReturnSuccess;
    }

    bool waitCmdInhibit(void)
    {
        int timeout = 100000;
        while (timeout--) {
            UInt32 state = read32(SDHCI_PRESENT_STATE);
            if (!(state & SDHCI_STATE_CMD_INHIBIT))
                return true;
        }
        return false;
    }

    bool waitDataInhibit(void)
    {
        int timeout = 100000;
        while (timeout--) {
            UInt32 state = read32(SDHCI_PRESENT_STATE);
            if (!(state & SDHCI_STATE_DATA_INHIBIT))
                return true;
        }
        return false;
    }

    void setClock(UInt32 freqKHz)
    {
        UInt32 clk = read16(SDHCI_CLOCK_CONTROL);
        clk &= ~(SDHCI_CLK_INT_EN | SDHCI_CLK_SDCLK_EN);
        write16(SDHCI_CLOCK_CONTROL, clk);

        UInt32 baseClkKHz = 400000;
        UInt32 div = (baseClkKHz + freqKHz - 1) / freqKHz;
        if (div > 0) div--;
        if (div > 0xFF) div = 0xFF;

        clk = read16(SDHCI_CLOCK_CONTROL);
        clk &= ~(SDHCI_CLK_FREQ_MASK << SDHCI_CLK_FREQ_SHIFT);
        clk |= (div << SDHCI_CLK_FREQ_SHIFT);
        clk |= SDHCI_CLK_INT_EN;
        write16(SDHCI_CLOCK_CONTROL, clk);

        int timeout = 10000;
        while (timeout--) {
            if (read16(SDHCI_CLOCK_CONTROL) & SDHCI_CLK_INT_STABLE)
                break;
        }

        clk = read16(SDHCI_CLOCK_CONTROL);
        clk |= SDHCI_CLK_SDCLK_EN;
        write16(SDHCI_CLOCK_CONTROL, clk);
    }

    IOReturn sendCommand(UInt32 cmd, UInt32 arg, UInt32 respType, bool isData)
    {
        if (!waitCmdInhibit())
            return kIOReturnTimeout;

        write32(SDHCI_ARGUMENT, arg);

        UInt32 cmdReg = (cmd << SDHCI_CMD_INDEX_SHIFT) | respType;
        if (isData) cmdReg |= SDHCI_CMD_ISDATA;

        write16(SDHCI_COMMAND, cmdReg);

        int timeout = 100000;
        while (timeout--) {
            UInt32 intSt = read32(SDHCI_INT_STATUS);
            if (intSt & SDHCI_INT_RESPONSE) {
                write32(SDHCI_INT_STATUS, SDHCI_INT_RESPONSE);
                return kIOReturnSuccess;
            }
            if (intSt & SDHCI_INT_TIMEOUT) {
                write32(SDHCI_INT_STATUS, SDHCI_INT_TIMEOUT);
                resetCmdData();
                return kIOReturnTimeout;
            }
            if (intSt & SDHCI_INT_ERROR) {
                write32(SDHCI_INT_STATUS, SDHCI_INT_ERROR);
                resetCmdData();
                return kIOReturnIOError;
            }
        }
        return kIOReturnTimeout;
    }

public:
    bool start(IOService* provider) override
    {
        if (!AndroidPlatformDevice::start(provider))
            return false;

        {
            OSString* compatStr = OSDynamicCast(OSString,
                provider->getProperty(kAndroidCompatibleKey));
            if (compatStr) {
                const char* s = compatStr->getCStringNoCopy();
                fIsSdcc1 = (strstr(s, "sdcc1") != nullptr);
            } else {
                fIsSdcc1 = true;
            }
        }

        kprintf("[MSM8916SDHCI] starting %s\n",
                fIsSdcc1 ? "eMMC (SDCC1)" : "SD card (SDCC2)");

        write8(SDHCI_SOFTWARE_RESET, SDHCI_SRST_ALL);
        IOSleep(2);

        setClock(400);

        write8(SDHCI_TIMEOUT_CONTROL, 0x0E);

        write16(SDHCI_BLOCK_SIZE, 0x0200);
        write16(SDHCI_BLOCK_COUNT, 1);

        UInt32 intMask = SDHCI_INT_RESPONSE | SDHCI_INT_DATA_END |
                         SDHCI_INT_ERROR | SDHCI_INT_TIMEOUT |
                         SDHCI_INT_CRC | SDHCI_INT_END_BIT |
                         SDHCI_INT_CARD_INSERT | SDHCI_INT_CARD_REMOVE;
        write32(SDHCI_INT_ENABLE, intMask);
        write32(SDHCI_SIGNAL_ENABLE, intMask);

        write8(SDHCI_POWER_CONTROL, 0x0E);
        IOSleep(1);
        write8(SDHCI_POWER_CONTROL, 0x0F);

        UInt32 caps = read32(SDHCI_CAPABILITIES);
        kprintf("[MSM8916SDHCI] capabilities=0x%x\n", caps);

        registerService();
        kprintf("[MSM8916SDHCI] controller ready\n");
        return true;
    }

    UInt32 getResponse0(void)
    {
        return read32(SDHCI_RESPONSE_0);
    }

    UInt32 getResponse1(void)
    {
        return read32(SDHCI_RESPONSE_1);
    }

    UInt32 getResponse2(void)
    {
        return read32(SDHCI_RESPONSE_2);
    }

    UInt32 getResponse3(void)
    {
        return read32(SDHCI_RESPONSE_3);
    }

    IOReturn sendCmd(UInt8 cmdIndex, UInt32 arg, UInt32 respType)
    {
        return sendCommand(cmdIndex, arg, respType, false);
    }

    bool isCardPresent(void)
    {
        return (read32(SDHCI_PRESENT_STATE) & (1U << 16)) != 0;
    }
};

OSDefineMetaClassAndStructors(MSM8916SDHCI, AndroidPlatformDevice)
