#include "../AndroidPlatformDevice.h"

#define SPMI_BASE               0x0200F000
#define SPMI_SIZE               0x00001000

#define SPMI_CTRL_REG           0x000
#define SPMI_STATUS_REG         0x004
#define SPMI_IRQ_STATUS         0x008
#define SPMI_IRQ_ENABLE         0x00C
#define SPMI_IRQ_CLEAR          0x010
#define SPMI_CMD_REG            0x020
#define SPMI_ADDR_REG           0x024
#define SPMI_DATA_REG           0x028
#define SPMI_RD_DATA_REG        0x02C
#define SPMI_OWNER_REG          0x030
#define SPMI_ARB_CTRL           0x100
#define SPMI_ARB_STATUS         0x104
#define SPMI_ARB_CONFIG         0x108
#define SPMI_ARB_CHANNEL_CFG(n) (0x200 + (n) * 0x20)
#define SPMI_ARB_CHANNEL_CMD(n) (0x204 + (n) * 0x20)
#define SPMI_ARB_CHANNEL_STATUS(n) (0x208 + (n) * 0x20)
#define SPMI_ARB_CHANNEL_DATA(n) (0x20C + (n) * 0x20)

#define SPMI_CTRL_ENA           (1U << 0)
#define SPMI_CTRL_IRQ_EN        (1U << 1)
#define SPMI_CTRL_ARB_EN        (1U << 2)

#define SPMI_STATUS_BUSY        (1U << 0)
#define SPMI_STATUS_ARB_BUSY    (1U << 1)
#define SPMI_STATUS_ERROR       (1U << 2)
#define SPMI_STATUS_DONE        (1U << 3)

#define SPMI_CMD_EXT_READ       0x04
#define SPMI_CMD_EXT_WRITE      0x05
#define SPMI_CMD_EXT_READ_LONG  0x06
#define SPMI_CMD_EXT_WRITE_LONG 0x07
#define SPMI_CMD_RESET          0x10
#define SPMI_CMD_SLEEP          0x11
#define SPMI_CMD_SHUTDOWN       0x12
#define SPMI_CMD_WAKEUP         0x13

#define SPMI_PMIC_ARB_CHANNEL   0
#define PMIC_ARB_MAX_CHANNELS   16

#define SPMI_PMIC_SID           0
#define SPMI_PMIC_PERIPH_BASE   0x1000

class MSM8916SPMI : public AndroidPlatformDevice
{
    OSDeclareDefaultStructors(MSM8916SPMI)

private:
    UInt32 fNumChannels;

    IOReturn waitForIdle(void)
    {
        int timeout = 10000;
        while (timeout--) {
            UInt32 status = read32(SPMI_STATUS_REG);
            if (!(status & (SPMI_STATUS_BUSY | SPMI_STATUS_ARB_BUSY)))
                return kIOReturnSuccess;
        }
        return kIOReturnTimeout;
    }

public:
    bool start(IOService* provider) override
    {
        if (!AndroidPlatformDevice::start(provider))
            return false;

        kprintf("[MSM8916SPMI] starting SPMI controller\n");

        write32(SPMI_CTRL_REG, SPMI_CTRL_ENA | SPMI_CTRL_ARB_EN);
        IOSleep(1);

        write32(SPMI_ARB_CONFIG, 0x00000001);

        fNumChannels = PMIC_ARB_MAX_CHANNELS;
        for (UInt32 i = 0; i < fNumChannels; i++) {
            write32(SPMI_ARB_CHANNEL_CFG(i), (0x00 << 8) | (i & 0xFF));
        }

        write32(SPMI_IRQ_ENABLE, 0x00000000);

        write32(SPMI_OWNER_REG, 0x00000001);

        registerService();
        kprintf("[MSM8916SPMI] SPMI ready, %u channels\n", fNumChannels);
        return true;
    }

    IOReturn pmicRead(UInt8 slaveId, UInt16 addr, UInt8* buf, UInt32 len)
    {
        IOReturn ret = waitForIdle();
        if (ret != kIOReturnSuccess)
            return ret;

        if (SPMI_PMIC_SID != 0) return kIOReturnBadArgument;

        UInt32 cmd = SPMI_CMD_EXT_READ_LONG;
        if (len == 1) cmd = SPMI_CMD_EXT_READ;

        write32(SPMI_CMD_REG, cmd);
        write32(SPMI_ADDR_REG, (UInt32)(slaveId << 16) | addr);
        write32(SPMI_DATA_REG, len);

        int timeout = 10000;
        while (timeout--) {
            UInt32 status = read32(SPMI_STATUS_REG);
            if (status & SPMI_STATUS_DONE) {
                write32(SPMI_STATUS_REG, SPMI_STATUS_DONE);
                break;
            }
            if (status & SPMI_STATUS_ERROR) {
                write32(SPMI_STATUS_REG, SPMI_STATUS_ERROR);
                return kIOReturnIOError;
            }
        }

        if (timeout <= 0) return kIOReturnTimeout;

        for (UInt32 i = 0; i < len; i++) {
            buf[i] = (UInt8)read32(SPMI_RD_DATA_REG + (i * 4));
        }

        return kIOReturnSuccess;
    }

    IOReturn pmicWrite(UInt8 slaveId, UInt16 addr, UInt8* buf, UInt32 len)
    {
        IOReturn ret = waitForIdle();
        if (ret != kIOReturnSuccess)
            return ret;

        UInt32 cmd = SPMI_CMD_EXT_WRITE_LONG;
        if (len == 1) cmd = SPMI_CMD_EXT_WRITE;

        write32(SPMI_CMD_REG, cmd);
        write32(SPMI_ADDR_REG, (UInt32)(slaveId << 16) | addr);

        for (UInt32 i = 0; i < len; i++) {
            write32(SPMI_DATA_REG + (i * 4), buf[i]);
        }
        write32(SPMI_DATA_REG + 4, len);

        int timeout = 10000;
        while (timeout--) {
            UInt32 status = read32(SPMI_STATUS_REG);
            if (status & SPMI_STATUS_DONE) {
                write32(SPMI_STATUS_REG, SPMI_STATUS_DONE);
                return kIOReturnSuccess;
            }
            if (status & SPMI_STATUS_ERROR) {
                write32(SPMI_STATUS_REG, SPMI_STATUS_ERROR);
                return kIOReturnIOError;
            }
        }

        return kIOReturnTimeout;
    }

    IOReturn pmicRegulatorSetEnabled(UInt8 periphId, bool enable)
    {
        UInt8 buf = enable ? 0x80 : 0x00;
        return pmicWrite(SPMI_PMIC_SID,
                         SPMI_PMIC_PERIPH_BASE | (periphId << 8) | 0x46,
                         &buf, 1);
    }

    IOReturn pmicRegulatorSetVoltage(UInt8 periphId, UInt16 mV)
    {
        UInt16 vStep;
        UInt8 buf[2];

        if (mV >= 2300) {
            vStep = (UInt16)((mV - 2300 + 7) / 8);
            if (vStep > 255) vStep = 255;
            buf[0] = (UInt8)(vStep >> 4);
            buf[1] = (UInt8)((vStep & 0xF) << 4);
        } else if (mV >= 1500) {
            vStep = (UInt16)((mV - 1500 + 4) / 5);
            if (vStep > 255) vStep = 255;
            buf[0] = (UInt8)(vStep >> 4);
            buf[1] = (UInt8)((vStep & 0xF) << 4);
        } else {
            vStep = (UInt16)((mV - 375 + 2) / 3);
            if (vStep > 255) vStep = 255;
            buf[0] = (UInt8)(vStep >> 4);
            buf[1] = (UInt8)((vStep & 0xF) << 4);
        }

        return pmicWrite(SPMI_PMIC_SID,
                         SPMI_PMIC_PERIPH_BASE | (periphId << 8) | 0x40,
                         buf, 2);
    }
};

OSDefineMetaClassAndStructors(MSM8916SPMI, AndroidPlatformDevice)
