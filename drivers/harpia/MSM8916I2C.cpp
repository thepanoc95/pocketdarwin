#include "../AndroidPlatformDevice.h"

#define BLSP_I2C_REGS_BASE      0x078B000

#define I2C_CTRL_REG            0x000
#define I2C_STATUS_REG          0x004
#define I2C_DATA_REG            0x008
#define I2C_CNTL_REG            0x00C
#define I2C_CONFIG_REG          0x010
#define I2C_I2C_CLK_CTL         0x100
#define I2C_I2C_RESET           0x104
#define I2C_DEVID_REG           0x108
#define I2C_QUEUE_CMD_REG       0x110
#define I2C_QUEUE_STATUS_REG    0x114
#define I2C_QUEUE_THRESHOLD     0x118
#define I2C_DMA_CTRL_REG        0x120
#define I2C_DMA_STATUS_REG      0x124

#define I2C_CTRL_ENA            (1U << 0)
#define I2C_CTRL_MASTER         (1U << 1)
#define I2C_CTRL_RESET          (1U << 7)

#define I2C_STATUS_BUSY         (1U << 0)
#define I2C_STATUS_DATA_DONE    (1U << 1)
#define I2C_STATUS_NACK         (1U << 2)
#define I2C_STATUS_ARB_LOST     (1U << 3)

#define I2C_CNTL_READ           (1U << 0)
#define I2C_CNTL_WRITE          (0U << 0)
#define I2C_CNTL_START          (1U << 1)
#define I2C_CNTL_STOP           (1U << 2)
#define I2C_CNTL_PRE_STOP       (1U << 3)
#define I2C_CNTL_QUEUED         (1U << 4)

#define I2C_QUEUE_CMD_TX        (0U << 14)
#define I2C_QUEUE_CMD_RX        (1U << 14)
#define I2C_QUEUE_CMD_STOP      (2U << 14)
#define I2C_QUEUE_CMD_RESTART   (3U << 14)

enum MSM8916I2CBusSpeed {
    kI2CSpeedStandard  = 100000,
    kI2CSpeedFast      = 400000,
};

class MSM8916I2CController : public AndroidPlatformDevice
{
    OSDeclareDefaultStructors(MSM8916I2CController)

private:
    uint32_t    fI2CClkCtlOffset;
    UInt32      fBusSpeed;
    UInt32      fInstanceId;

    IOReturn waitForIdle(void)
    {
        int timeout = 10000;
        while (timeout--) {
            UInt32 status = read32(I2C_STATUS_REG);
            if (!(status & I2C_STATUS_BUSY))
                return kIOReturnSuccess;
        }
        kprintf("[MSM8916I2C] timeout waiting for idle\n");
        return kIOReturnTimeout;
    }

    IOReturn resetController(void)
    {
        write32(I2C_CTRL_REG, I2C_CTRL_RESET);
        IOSleep(1);
        write32(I2C_CTRL_REG, I2C_CTRL_ENA | I2C_CTRL_MASTER);
        return kIOReturnSuccess;
    }

public:
    bool start(IOService* provider) override
    {
        if (!AndroidPlatformDevice::start(provider))
            return false;

        fInstanceId = getBusAddress();

        OSNumber* speedProp = OSDynamicCast(OSNumber,
            provider->getProperty("i2c-speed"));
        fBusSpeed = speedProp ? speedProp->unsigned32BitValue() : kI2CSpeedStandard;

        kprintf("[MSM8916I2C] starting I2C controller %u speed=%u\n",
                fInstanceId, fBusSpeed);

        write32(I2C_CONFIG_REG, 0x00000000);
        IOSleep(1);

        write32(I2C_CTRL_REG, I2C_CTRL_ENA | I2C_CTRL_MASTER);
        IOSleep(1);

        resetController();

        registerService();

        kprintf("[MSM8916I2C] controller %u ready\n", fInstanceId);
        return true;
    }

    IOReturn i2cTransfer(UInt8 slaveAddr, UInt8* txBuf, UInt32 txLen,
                         UInt8* rxBuf, UInt32 rxLen)
    {
        IOReturn ret = waitForIdle();
        if (ret != kIOReturnSuccess)
            return ret;

        if (txLen > 0) {
            write32(I2C_DATA_REG, (uint32_t)(slaveAddr << 1) | 0);
            write32(I2C_QUEUE_CMD_REG, I2C_QUEUE_CMD_TX | 1);

            for (UInt32 i = 0; i < txLen; i++) {
                write32(I2C_DATA_REG, txBuf[i]);
                write32(I2C_QUEUE_CMD_REG, I2C_QUEUE_CMD_TX | 1);
            }
        }

        if (rxLen > 0) {
            if (txLen > 0) {
                write32(I2C_QUEUE_CMD_REG, I2C_QUEUE_CMD_RESTART);
            }

            write32(I2C_DATA_REG, (uint32_t)(slaveAddr << 1) | 1);
            write32(I2C_QUEUE_CMD_REG, I2C_QUEUE_CMD_TX | 1);

            write32(I2C_QUEUE_CMD_REG, I2C_QUEUE_CMD_RX | rxLen);
        }

        write32(I2C_QUEUE_CMD_REG, I2C_QUEUE_CMD_STOP);

        ret = waitForIdle();
        if (ret != kIOReturnSuccess)
            return ret;

        UInt32 status = read32(I2C_STATUS_REG);
        if (status & I2C_STATUS_NACK) {
            kprintf("[MSM8916I2C] NACK from slave 0x%02x\n", slaveAddr);
            resetController();
            return kIOReturnIOError;
        }

        for (UInt32 i = 0; i < rxLen; i++) {
            rxBuf[i] = (UInt8)read32(I2C_DATA_REG);
        }

        return kIOReturnSuccess;
    }

    UInt32 busReadReg(UInt8 regAddr, UInt8 regSize) override
    {
        UInt8 slaveAddr = (UInt8)fBusAddress;
        UInt8 txData[1] = { regAddr };
        UInt8 rxData[4] = { 0, 0, 0, 0 };
        UInt32 rxBytes = (regSize == 2) ? 2 : (regSize == 4) ? 4 : 1;

        IOReturn ret = i2cTransfer(slaveAddr, txData, 1, rxData, rxBytes);
        if (ret != kIOReturnSuccess)
            return 0xFFFFFFFF;

        UInt32 result = 0;
        switch (rxBytes) {
            case 1: result = rxData[0]; break;
            case 2: result = (UInt32)rxData[0] | ((UInt32)rxData[1] << 8); break;
            case 4: result = (UInt32)rxData[0] | ((UInt32)rxData[1] << 8) |
                             ((UInt32)rxData[2] << 16) | ((UInt32)rxData[3] << 24); break;
        }
        return result;
    }

    IOReturn busWriteReg(UInt8 regAddr, UInt32 value, UInt8 regSize) override
    {
        UInt8 slaveAddr = (UInt8)fBusAddress;
        UInt8 txBuf[5];
        UInt32 txLen;
        txBuf[0] = regAddr;

        switch (regSize) {
            case 1: txBuf[1] = (UInt8)(value & 0xFF); txLen = 2; break;
            case 2: txBuf[1] = (UInt8)(value & 0xFF);
                    txBuf[2] = (UInt8)((value >> 8) & 0xFF); txLen = 3; break;
            case 4: txBuf[1] = (UInt8)(value & 0xFF);
                    txBuf[2] = (UInt8)((value >> 8) & 0xFF);
                    txBuf[3] = (UInt8)((value >> 16) & 0xFF);
                    txBuf[4] = (UInt8)((value >> 24) & 0xFF); txLen = 5; break;
            default: return kIOReturnBadArgument;
        }

        return i2cTransfer(slaveAddr, txBuf, txLen, nullptr, 0);
    }
};

OSDefineMetaClassAndStructors(MSM8916I2CController, AndroidPlatformDevice)
