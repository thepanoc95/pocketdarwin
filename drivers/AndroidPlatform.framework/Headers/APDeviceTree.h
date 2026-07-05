#ifndef _AP_DEVICETREE_H_
#define _AP_DEVICETREE_H_

#include <AndroidPlatform/APTypes.h>
#include <IOKit/IOService.h>
#include <libkern/c++/OSDictionary.h>
#include <libkern/c++/OSString.h>
#include <libkern/c++/OSNumber.h>
#include <libkern/c++/OSArray.h>
#include <libkern/c++/OSData.h>

/* ------------------------------------------------------------------ *
 * APDeviceTree — helpers to parse device-tree properties from an
 * IOService provider's property dictionary.
 *
 * The property keys are set by the FDT-to-ADT converter in the
 * PocketDarwin bootstrap loader.  The naming convention mirrors
 * the Linux device-tree bindings with an "android," prefix.
 * ------------------------------------------------------------------ */

#define AP_PROP_COMPATIBLE     "android,compatible"
#define AP_PROP_DEVICE_NAME    "android,device-name"
#define AP_PROP_REG_BASE       "android,reg-base"
#define AP_PROP_REG_SIZE       "android,reg-size"
#define AP_PROP_INTERRUPT      "android,interrupt"
#define AP_PROP_CLOCK          "android,clock"
#define AP_PROP_BUS_TYPE       "android,bus-type"
#define AP_PROP_BUS_ADDR       "android,bus-addr"
#define AP_PROP_I2C_SPEED      "i2c-speed"
#define AP_PROP_SPI_FREQ       "spi-max-frequency"
#define AP_PROP_SPI_CS         "spi-cs"
#define AP_PROP_UART_BAUD      "uart-baud"

enum APBusType {
    kAPBusNone = 0,
    kAPBusI2C  = 1,
    kAPBusSPI  = 2,
    kAPBusUART = 3,
    kAPBusMMIO = 4,
};

struct APDeviceInfo {
    ap_addr_t    regBasePhys;
    ap_size_t    regSize;
    int          interruptIndex;
    APBusType    busType;
    uint32_t     busAddress;
    const char*  deviceName;
    const char*  compatible;
    uint32_t     numClocks;
};

/* Read a string property from the provider. */
AP_INLINE OSString* ap_dt_string(IOService* provider, const char* key)
{
    return OSDynamicCast(OSString, provider->getProperty(key));
}

/* Read a number property from the provider. */
AP_INLINE OSNumber* ap_dt_number(IOService* provider, const char* key)
{
    return OSDynamicCast(OSNumber, provider->getProperty(key));
}

/* Parse the standard Android device properties into a struct. */
AP_EXPORT
bool ap_dt_parse(IOService* provider, struct APDeviceInfo* info);

/* Match a device by compatible string (Linux-style binding). */
AP_EXPORT
bool ap_dt_is_compatible(IOService* provider, const char* compatible);

/* Count the number of interrupts listed for this device. */
AP_EXPORT
uint32_t ap_dt_interrupt_count(IOService* provider);

/* Get a specific interrupt number from the device tree. */
AP_EXPORT
int ap_dt_interrupt_at(IOService* provider, uint32_t index);

/* Look up a clock name from the clock array property. */
AP_EXPORT
const char* ap_dt_clock_name(IOService* provider, uint32_t index);

/* ------------------------------------------------------------------ *
 * Convenience helpers for commonly-used property lookups.
 * ------------------------------------------------------------------ */

AP_INLINE uint64_t ap_dt_reg_base(IOService* provider)
{
    OSNumber* n = ap_dt_number(provider, AP_PROP_REG_BASE);
    return n ? n->unsigned64BitValue() : 0;
}

AP_INLINE uint64_t ap_dt_reg_size(IOService* provider)
{
    OSNumber* n = ap_dt_number(provider, AP_PROP_REG_SIZE);
    return n ? n->unsigned64BitValue() : 0;
}

AP_INLINE int ap_dt_interrupt(IOService* provider)
{
    OSNumber* n = ap_dt_number(provider, AP_PROP_INTERRUPT);
    return n ? (int)n->unsigned32BitValue() : -1;
}

AP_INLINE uint32_t ap_dt_bus_address(IOService* provider)
{
    OSNumber* n = ap_dt_number(provider, AP_PROP_BUS_ADDR);
    return n ? n->unsigned32BitValue() : 0;
}

#endif /* _AP_DEVICETREE_H_ */
