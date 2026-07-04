#include "AndroidPlatformDevice.h"

/* Generic I2C touch panel driver — subclasses AndroidPlatformDevice
 * for Synaptics RMI4 or Goodix touch controllers. */
class AndroidI2CTouchDriver : public AndroidPlatformDevice
{
    OSDeclareDefaultStructors(AndroidI2CTouchDriver)

private:
    UInt32 mTouchI2CAddr;

public:
    bool start(IOService* provider) override
    {
        if (!AndroidPlatformDevice::start(provider))
            return false;

        /* Get I2C slave address from bus address property. */
        mTouchI2CAddr = getBusAddress();

        /* Read device ID register to identify controller. */
        UInt32 deviceId = busReadReg(0x00, 1);
        kprintf("[AndroidI2CTouch] device ID=0x%02x addr=0x%02x\n",
                deviceId, mTouchI2CAddr);

        /* Register interrupt for touch events. */
        registerInterrupt(fInterruptIndex,
            [](OSObject* target, void* refcon,
               IOInterruptEventSource* src, int intCount)
            {
                /* Read touch data from RMI4 data registers
                 * and dispatch as IOKit HID events. */
            },
            this, nullptr);

        return true;
    }
};

OSDefineMetaClassAndStructors(AndroidI2CTouchDriver, AndroidPlatformDevice)