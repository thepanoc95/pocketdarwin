#ifndef _AP_GPIO_H_
#define _AP_GPIO_H_

#include <AndroidPlatform/APTypes.h>
#include <IOKit/IOService.h>

/* ------------------------------------------------------------------ *
 * APGPIO — GPIO abstraction layer
 *
 * This provides a platform-independent interface for toggling GPIO
 * pins, configuring alternate functions, pull resistors, and drive
 * strength.  Each SoC's GPIO driver (e.g. MSM8916TLMM) subclasses
 * IOService and publishes an APGPIOController nub that conforms to
 * this interface.
 *
 * Client drivers find the GPIO controller via the IORegistry and
 * call through this API instead of poking registers directly.
 * ------------------------------------------------------------------ */

#define AP_GPIO_PIN_UNDEFINED   ((uint32_t)-1)
#define AP_GPIO_MAX_PINS        256

enum APGpioFunction {
    kAPGpioFuncGPIO      = 0,
    kAPGpioFuncAlt1      = 1,
    kAPGpioFuncAlt2      = 2,
    kAPGpioFuncAlt3      = 3,
    kAPGpioFuncAlt4      = 4,
    kAPGpioFuncAlt5      = 5,
};

enum APGpioPull {
    kAPGpioPullNone      = 0,
    kAPGpioPullDown      = 1,
    kAPGpioPullUp        = 2,
    kAPGpioPullKeeper    = 3,
};

enum APGpioDriveStrength {
    kAPGpioDrive2mA      = 0,
    kAPGpioDrive4mA      = 1,
    kAPGpioDrive6mA      = 2,
    kAPGpioDrive8mA      = 3,
    kAPGpioDrive10mA     = 4,
    kAPGpioDrive12mA     = 5,
    kAPGpioDrive14mA     = 6,
    kAPGpioDrive16mA     = 7,
};

struct APGpioConfig {
    uint32_t           pin;
    APGpioFunction     function;
    APGpioPull         pull;
    APGpioDriveStrength driveStrength;
    bool               output;
    bool               outputHigh;
};

/* Names for known GPIO lines on harpia / MSM8916. */
enum APGpioHarpia {
    kAPGpioHarpiaUartTx      = 4,
    kAPGpioHarpiaUartRx      = 5,
    kAPGpioHarpiaI2C1SDA     = 6,
    kAPGpioHarpiaI2C1SCL     = 7,
    kAPGpioHarpiaTouchRst    = 12,
    kAPGpioHarpiaTouchIrq    = 13,
    kAPGpioHarpiaDisplayRst  = 25,
    kAPGpioHarpiaKeyVolDn    = 91,
    kAPGpioHarpiaKeyVolUp    = 92,
    kAPGpioHarpiaUsbId       = 115,
    kAPGpioHarpiaSdDetect    = 38,
};

class APGPIOController : public IOService
{
    OSDeclareDefaultStructors(APGPIOController)

public:
    virtual bool gpioConfigure(uint32_t pin, APGpioConfig config) = 0;
    virtual bool gpioSetValue(uint32_t pin, bool high) = 0;
    virtual bool gpioGetValue(uint32_t pin) = 0;
    virtual bool gpioSetOutput(uint32_t pin, bool output) = 0;
    virtual bool gpioSetPull(uint32_t pin, APGpioPull pull) = 0;
    virtual bool gpioSetFunction(uint32_t pin, APGpioFunction func) = 0;
    virtual bool gpioSetDriveStrength(uint32_t pin, APGpioDriveStrength ds) = 0;

    /* Find the GPIO controller in the IORegistry. */
    static APGPIOController* getController(void);
};

#endif /* _AP_GPIO_H_ */
