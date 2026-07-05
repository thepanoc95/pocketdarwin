#include "../AndroidPlatformDevice.h"

#define BLSP_UART_BASE           0x078AF000

#define UART_DM_MR1              0x0000
#define UART_DM_MR2              0x0004
#define UART_DM_IPR              0x0008
#define UART_DM_CR               0x0010
#define UART_DM_SR               0x0014
#define UART_DM_IMRL             0x0020
#define UART_DM_IMRH             0x0024
#define UART_DM_ISR              0x0028
#define UART_DM_DMRX             0x0034
#define UART_DM_DM_DREG          0x0038
#define UART_DM_BADR             0x0050
#define UART_DM_SNPD             0x0054
#define UART_DM_DM_TXFS          0x0058
#define UART_DM_RXFS             0x005C
#define UART_DM_TF               0x0100
#define UART_DM_RF               0x0140
#define UART_DM_MISR             0x0180

#define UART_CR_RESET_RX         (1U << 0)
#define UART_CR_RESET_TX         (1U << 1)
#define UART_CR_RESET_ERR        (1U << 2)
#define UART_CR_RESET_BREAK      (1U << 3)
#define UART_CR_START_BREAK      (1U << 4)
#define UART_CR_STOP_BREAK       (1U << 5)
#define UART_CR_ENA_TX           (1U << 6)
#define UART_CR_ENA_RX           (1U << 7)
#define UART_CR_RESET_TX_RX      (UART_CR_RESET_RX | UART_CR_RESET_TX)

#define UART_SR_RX_READY         (1U << 0)
#define UART_SR_TX_EMPTY         (1U << 3)
#define UART_SR_TX_READY         (1U << 4)
#define UART_SR_RX_FULL          (1U << 5)
#define UART_SR_UART_OVERRUN     (1U << 6)
#define UART_SR_PARITY_ERROR     (1U << 7)

#define UART_MR1_PARITY_NONE     (0U << 1)
#define UART_MR1_PARITY_EVEN     (1U << 1)
#define UART_MR1_PARITY_ODD      (3U << 1)
#define UART_MR1_PARITY_SPACE    (5U << 1)
#define UART_MR1_PARITY_MARK     (7U << 1)

#define UART_MR2_STOP_BITS_1     (0U << 1)
#define UART_MR2_STOP_BITS_2     (1U << 1)
#define UART_MR2_CHAR_LEN_5      (0U << 5)
#define UART_MR2_CHAR_LEN_6      (1U << 5)
#define UART_MR2_CHAR_LEN_7      (2U << 5)
#define UART_MR2_CHAR_LEN_8      (3U << 5)

#define UART_IMRL_TX_READY       (1U << 5)
#define UART_IMRL_RX_READY       (1U << 2)
#define UART_IMRL_RX_FULL        (1U << 4)
#define UART_IMRL_RX_STALE       (1U << 7)
#define UART_IMRL_TX_ERROR       (1U << 8)

#define UART_CR_CMD_STALE        (0x1E << 1)

#define UART_DM_TXFS_BURST_SIZE_MASK 0x3F
#define UART_DM_TXFS_BURST_SIZE_SHIFT 8

#define UART_DM_RXFS_BURST_ENA   (1U << 7)

enum MSM8916UartBaud {
    kUartBaud115200  = 115200,
    kUartBaud230400  = 230400,
    kUartBaud921600  = 921600,
    kUartBaud1000000 = 1000000,
};

class MSM8916UART : public AndroidPlatformDevice
{
    OSDeclareDefaultStructors(MSM8916UART)

private:
    UInt32      fBaudRate;
    UInt32      fInstanceId;

    IOReturn calcBaudDivisor(UInt32 baud, UInt32 clkFreq, UInt16* divisor)
    {
        if (baud == 0) return kIOReturnBadArgument;
        UInt32 d = (clkFreq * 4) / baud;
        *divisor = (UInt16)(d & 0xFFFF);
        return kIOReturnSuccess;
    }

public:
    bool start(IOService* provider) override
    {
        if (!AndroidPlatformDevice::start(provider))
            return false;

        fInstanceId = getBusAddress();

        OSNumber* baudProp = OSDynamicCast(OSNumber,
            provider->getProperty("uart-baud"));
        fBaudRate = baudProp ? baudProp->unsigned32BitValue() : kUartBaud115200;

        kprintf("[MSM8916UART] UART%u starting baud=%u\n",
                fInstanceId, fBaudRate);

        write32(UART_DM_CR, UART_CR_RESET_TX_RX);
        IOSleep(1);
        write32(UART_DM_CR, 0);

        write32(UART_DM_MR1, UART_MR1_PARITY_NONE);
        write32(UART_DM_MR2, UART_MR2_STOP_BITS_1 | UART_MR2_CHAR_LEN_8);

        UInt16 baudDiv = 0;
        calcBaudDivisor(fBaudRate, 7372800, &baudDiv);
        write32(UART_DM_BADR, (UInt32)baudDiv);

        write32(UART_DM_SNPD, 0x0);

        write32(UART_DM_CR, UART_CR_ENA_TX | UART_CR_ENA_RX);

        write32(UART_DM_TXFS, (1 << UART_DM_TXFS_BURST_SIZE_SHIFT) & UART_DM_TXFS_BURST_SIZE_MASK);
        write32(UART_DM_RXFS, UART_DM_RXFS_BURST_ENA | 0x04);

        UInt32 imrl = UART_IMRL_TX_READY | UART_IMRL_RX_READY | UART_IMRL_RX_STALE;
        write32(UART_DM_IMRL, imrl);

        registerService();
        kprintf("[MSM8916UART] UART%u ready\n", fInstanceId);
        return true;
    }

    void uartPutc(char c)
    {
        int timeout = 10000;
        while (timeout--) {
            UInt32 sr = read32(UART_DM_SR);
            if (sr & UART_SR_TX_EMPTY) {
                write32(UART_DM_TF, (UInt32)(unsigned char)c);
                return;
            }
        }
    }

    void uartPuts(const char* str)
    {
        while (*str) {
            if (*str == '\n') uartPutc('\r');
            uartPutc(*str++);
        }
    }

    int uartGetc(void)
    {
        int timeout = 100000;
        while (timeout--) {
            UInt32 sr = read32(UART_DM_SR);
            if (sr & UART_SR_RX_READY) {
                return (int)(read32(UART_DM_RF) & 0xFF);
            }
        }
        return -1;
    }

    IOReturn setBaudRate(UInt32 newBaud)
    {
        if (newBaud == 0) return kIOReturnBadArgument;
        fBaudRate = newBaud;
        UInt16 baudDiv = 0;
        IOReturn ret = calcBaudDivisor(fBaudRate, 7372800, &baudDiv);
        if (ret != kIOReturnSuccess) return ret;
        write32(UART_DM_BADR, (UInt32)baudDiv);
        return kIOReturnSuccess;
    }
};

OSDefineMetaClassAndStructors(MSM8916UART, AndroidPlatformDevice)
