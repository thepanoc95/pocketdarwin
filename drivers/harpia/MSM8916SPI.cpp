#include "../AndroidPlatformDevice.h"

#define BLSP_SPI_BASE            0x078B8000

#define SPI_CONFIG_REG           0x0000
#define SPI_STATUS_REG           0x0004
#define SPI_DATA_REG             0x0008
#define SPI_DATA_WORD_0          0x0018
#define SPI_DATA_WORD_1          0x001C
#define SPI_DATA_WORD_2          0x0020
#define SPI_DATA_WORD_3          0x0024
#define SPI_IO_CONTROL_REG       0x004C
#define SPI_IO_MODES_REG         0x0050
#define SPI_SW_RESET_REG         0x0054
#define SPI_FIFO_CTRL_REG        0x0058
#define SPI_TRANSFER_CFG         0x0070
#define SPI_DEASSERT_WAIT        0x0074
#define SPI_QUEUE_CTRL           0x0078

#define SPI_CONFIG_LOOPBACK      (1U << 0)
#define SPI_CONFIG_INPUT_FIRST   (1U << 1)
#define SPI_CONFIG_SLAVE_MSB     (1U << 2)

#define SPI_STATUS_TX_FIFO_FULL  (1U << 0)
#define SPI_STATUS_TX_FIFO_EMPTY (1U << 1)
#define SPI_STATUS_RX_FIFO_FULL  (1U << 2)
#define SPI_STATUS_RX_FIFO_EMPTY (1U << 3)
#define SPI_STATUS_CS_ACTIVE     (1U << 4)
#define SPI_STATUS_ERROR         (1U << 5)
#define SPI_STATUS_XFER_COMPLETE (1U << 6)

#define SPI_IO_MODE_MUX          (1U << 16)
#define SPI_IO_MODE_CLK_IDLE_HIGH (1U << 8)
#define SPI_IO_MODE_CLK_ALWAYS    (1U << 4)
#define SPI_IO_MODE_CS_CTRL      (1U << 12)

#define SPI_TRANSFER_CFG_BIT_LEN_MASK 0x1F

enum MSM8916SpiMode {
    kSpiMode0 = 0,
    kSpiMode1 = 1,
    kSpiMode2 = 2,
    kSpiMode3 = 3,
};

class MSM8916SPIController : public AndroidPlatformDevice
{
    OSDeclareDefaultStructors(MSM8916SPIController)

private:
    UInt32      fSpiClkRate;
    UInt32      fChipSelect;
    UInt32      fInstanceId;

    IOReturn resetCore(void)
    {
        write32(SPI_SW_RESET_REG, 1);
        IOSleep(1);
        write32(SPI_SW_RESET_REG, 0);
        return kIOReturnSuccess;
    }

    IOReturn waitForTransfer(void)
    {
        int timeout = 100000;
        while (timeout--) {
            UInt32 status = read32(SPI_STATUS_REG);
            if (status & SPI_STATUS_XFER_COMPLETE)
                return kIOReturnSuccess;
            if (status & SPI_STATUS_ERROR) {
                kprintf("[MSM8916SPI] transfer error status=0x%x\n", status);
                return kIOReturnIOError;
            }
        }
        kprintf("[MSM8916SPI] transfer timeout\n");
        return kIOReturnTimeout;
    }

public:
    bool start(IOService* provider) override
    {
        if (!AndroidPlatformDevice::start(provider))
            return false;

        fInstanceId = getBusAddress();

        OSNumber* rateProp = OSDynamicCast(OSNumber,
            provider->getProperty("spi-max-frequency"));
        fSpiClkRate = rateProp ? rateProp->unsigned32BitValue() : 50000000;

        OSNumber* csProp = OSDynamicCast(OSNumber,
            provider->getProperty("spi-cs"));
        fChipSelect = csProp ? csProp->unsigned32BitValue() : 0;

        kprintf("[MSM8916SPI] SPI controller %u clk=%u cs=%u\n",
                fInstanceId, fSpiClkRate, fChipSelect);

        resetCore();

        write32(SPI_CONFIG_REG, SPI_CONFIG_INPUT_FIRST);

        write32(SPI_IO_MODES_REG, SPI_IO_MODE_MUX | fChipSelect);

        UInt32 txFifoThreshold = 4;
        UInt32 rxFifoThreshold = 4;
        write32(SPI_FIFO_CTRL_REG, (rxFifoThreshold << 8) | txFifoThreshold);

        registerService();
        kprintf("[MSM8916SPI] controller %u ready\n", fInstanceId);
        return true;
    }

    IOReturn spiTransfer(UInt8* txBuf, UInt8* rxBuf, UInt32 len, UInt8 bitsPerWord)
    {
        if (len == 0 || len > 4096)
            return kIOReturnBadArgument;

        write32(SPI_TRANSFER_CFG, (bitsPerWord - 1) & SPI_TRANSFER_CFG_BIT_LEN_MASK);

        write32(SPI_DEASSERT_WAIT, 1);

        UInt32 ioModes = read32(SPI_IO_MODES_REG);
        ioModes |= SPI_IO_MODE_MUX;
        write32(SPI_IO_MODES_REG, ioModes);

        write32(SPI_QUEUE_CTRL, len);

        for (UInt32 i = 0; i < len; i++) {
            write32(SPI_DATA_REG, txBuf ? txBuf[i] : 0);
        }

        IOReturn ret = waitForTransfer();
        if (ret != kIOReturnSuccess)
            return ret;

        if (rxBuf) {
            for (UInt32 i = 0; i < len; i++) {
                rxBuf[i] = (UInt8)(read32(SPI_DATA_REG) & 0xFF);
            }
        }

        return kIOReturnSuccess;
    }
};

OSDefineMetaClassAndStructors(MSM8916SPIController, AndroidPlatformDevice)
