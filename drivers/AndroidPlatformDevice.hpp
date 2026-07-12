#ifndef _ANDROID_PLATFORM_DEVICE_HPP
#define _ANDROID_PLATFORM_DEVICE_HPP

// Include shim BEFORE any IOKit headers to provide our definitions
#include "IOKitShim.h"

// Skip the real IOKit headers entirely - we have our own shims
// The real headers require XNU kernel headers that aren't available on Linux

/*
 * Property keys used in the Apple Device Tree for Android peripherals.
 * These are set by the dtc-AppleDeviceTree converter or the baremetal
 * bootstrap's FDT-to-ADT translation layer.
 */
#define kAndroidDeviceNameKey      "android,device-name"
#define kAndroidCompatibleKey     "android,compatible"
#define kAndroidRegBaseKey        "android,reg-base"
#define kAndroidRegSizeKey        "android,reg-size"
#define kAndroidInterruptKey      "android,interrupt"
#define kAndroidClockKey           "android,clock"
#define kAndroidBusTypeKey        "android,bus-type"
#define kAndroidBusAddrKey        "android,bus-addr"

/*
 * Bus types for I2C/SPI peripherals commonly found on Android SoCs.
 */
enum AndroidBusType {
    kAndroidBusTypeNone   = 0,
    kAndroidBusTypeI2C    = 1,
    kAndroidBusTypeSPI    = 2,
    kAndroidBusTypeUART   = 3,
    kAndroidBusTypeMMIO   = 4,
};

/*
 * Generic platform device wrapper for Android peripherals.
 *
 * This class represents a single hardware peripheral discovered via
 * the device tree. It maps register space, sets up interrupts, and
 * provides a base for specialized drivers (display, touch, audio, etc.)
 * to subclass.
 */
class AndroidPlatformDevice : public IOService
{
    OSDeclareDefaultStructors(AndroidPlatformDevice)

public:
    /* --- IOService lifecycle --- */

    /* Probe: called when the matching system finds a candidate device.
     * Return a non-zero score; higher = more confident match. */
    IOService* probe(IOService* provider, SInt32* score);

    /* Start: called when this driver is selected to drive the device.
     * Map register space, register interrupts, enable clocks. */
    bool start(IOService* provider);

    /* Stop: called before this driver is detached. Cleanup here. */
    void stop(IOService* provider);

    /* Free: release all resources. Called when retain count hits zero. */
    void free(void);

    /* --- Hardware access helpers --- */

    /* Read a 32-bit register from the mapped peripheral space. */
    UInt32 read32(IOByteOffset offset);

    /* Write a 32-bit register to the mapped peripheral space. */
    void write32(IOByteOffset offset, UInt32 value);

    /* Read a 16-bit register (some peripherals use 16-bit registers). */
    UInt16 read16(IOByteOffset offset);

    /* Write a 16-bit register. */
    void write16(IOByteOffset offset, UInt16 value);

    /* Read an 8-bit register. */
    UInt8 read8(IOByteOffset offset);

    /* Write an 8-bit register. */
    void write8(IOByteOffset offset, UInt8 value);

    /* --- Interrupt handling --- */

    /* Register an interrupt handler for this device's IRQ.
     * Called from start(). Subclasses provide the actual handler. */
    IOReturn registerInterrupt(int index,
                                IOInterruptActionHandler handler,
                                void* target,
                                void* refcon);

    /* Enable the interrupt at the GIC level. */
    void enableInterrupt(int index);

    /* Disable the interrupt at the GIC level. */
    void disableInterrupt(int index);

    /* --- Clock & power management --- */

    /* Enable the peripheral clock(s) for this device.
     * Stub implementation — subclass or platform expert fills in. */
    virtual IOReturn enableClock(void);

    /* Disable the peripheral clock(s). */
    virtual IOReturn disableClock(void);

    /* --- I2C/SPI bus interface (optional) --- */

    /* For I2C/SPI peripherals: send a register read transaction.
     * Returns the read value, or 0xFFFFFFFF on error. */
    virtual UInt32 busReadReg(UInt8 regAddr, UInt8 regSize);

    /* For I2C/SPI peripherals: write a register value. */
    virtual IOReturn busWriteReg(UInt8 regAddr, UInt32 value, UInt8 regSize);

    /* --- Accessors --- */

    /* Return the device name from the device tree. */
    const char* getDeviceName(void);

    /* Return the bus type (I2C, SPI, MMIO, etc.). */
    AndroidBusType getBusType(void);

    /* Return the bus address (I2C slave addr, SPI chip-select, etc.). */
    UInt32 getBusAddress(void);

protected:
    /* Register space memory descriptor and mapping. */
    IOMemoryDescriptor*  fRegDescriptor;
    IOMemoryMap*         fRegMap;
    IOVirtualAddress     fRegBase;
    IOByteCount           fRegSize;

    /* Device tree properties. */
    OSString*            fDeviceName;
    OSString*            fCompatible;
    AndroidBusType       fBusType;
    UInt32               fBusAddress;

    /* Interrupt source index (from device tree). */
    int                  fInterruptIndex;
    bool                 fInterruptRegistered;

    /* Clock name(s) from device tree. */
    OSArray*             fClockNames;
};

#endif /* _ANDROID_PLATFORM_DEVICE_HPP */