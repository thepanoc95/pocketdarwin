#ifndef _AP_CLOCK_POWER_H_
#define _AP_CLOCK_POWER_H_

#include <AndroidPlatform/APTypes.h>
#include <IOKit/IOService.h>

/* ------------------------------------------------------------------ *
 * APClock — clock control abstraction
 *
 * SoC clocks are managed by the Global Clock Controller (GCC) driver.
 * Peripheral drivers request clock enables / disables / rate changes
 * through this interface without touching GCC registers directly.
 *
 * The GCC driver publishes an APClockController service.
 * ------------------------------------------------------------------ */

#define AP_CLOCK_NAME_MAX   32

enum APClockType {
    kAPClockBranch    = 0,
    kAPClockPLL       = 1,
    kAPClockMux       = 2,
    kAPClockDivider   = 3,
};

class APClockController : public IOService
{
    OSDeclareDefaultStructors(APClockController)

public:
    virtual IOReturn enableClock(const char* name) = 0;
    virtual IOReturn disableClock(const char* name) = 0;
    virtual IOReturn setClockRate(const char* name, uint32_t rateHz) = 0;
    virtual uint32_t getClockRate(const char* name) = 0;
    virtual bool     isClockEnabled(const char* name) = 0;

    static APClockController* getController(void);
};

/* ------------------------------------------------------------------ *
 * APPowerDomain — power domain / regulator abstraction
 *
 * On MSM8916, voltage regulators are managed by the PM8916 PMIC
 * accessed over SPMI.  This interface provides a unified API for
 * setting regulator voltages and enabling/disabling power rails.
 * ------------------------------------------------------------------ */

class APPowerDomain : public IOService
{
    OSDeclareDefaultStructors(APPowerDomain)

public:
    virtual IOReturn powerOn(void) = 0;
    virtual IOReturn powerOff(void) = 0;
    virtual IOReturn setVoltage(uint32_t microvolts) = 0;
    virtual uint32_t getVoltage(void) = 0;

    static APPowerDomain* getDomain(const char* name);
};

/* ------------------------------------------------------------------ *
 * Predefined power domains for MSM8916 / PM8916
 * ------------------------------------------------------------------ */

enum APMsm8916Regulator {
    kAPRegulatorS1,    /* APC core      — 0.6V–1.066V */
    kAPRegulatorS2,    /*              — 0.6V–1.066V */
    kAPRegulatorS3,    /*              — 0.6V–1.066V */
    kAPRegulatorS4,    /*              — 0.6V–1.066V */
    kAPRegulatorL1,    /*  GP1         — 0.375V–1.525V */
    kAPRegulatorL2,    /*  GP2         — 0.375V–1.525V */
    kAPRegulatorL3,    /*  MX          — 0.375V–1.525V */
    kAPRegulatorL4,    /*  MX          — 0.375V–1.525V */
    kAPRegulatorL5,    /*  UFS         — 0.375V–1.525V */
    kAPRegulatorL6,    /*  SDC1        — 0.375V–1.525V */
    kAPRegulatorL7,    /*  SDC2        — 0.375V–1.525V */
    kAPRegulatorL8,    /*  USB         — 0.375V–1.525V */
    kAPRegulatorL9,    /*  S1          — 0.375V–1.525V */
    kAPRegulatorL10,   /*  S2          — 0.375V–1.525V */
    kAPRegulatorL11,   /*  S3          — 0.375V–1.525V */
    kAPRegulatorL12,   /*  S4          — 0.375V–1.525V */
    kAPRegulatorL13,   /*  MIPI        — 0.375V–1.525V */
    kAPRegulatorL14,   /*  LPI         — 0.375V–1.525V */
    kAPRegulatorL15,   /*  LPI         — 0.375V–1.525V */
    kAPRegulatorL16,   /*  LPI         — 0.375V–1.525V */
    kAPRegulatorL17,   /*  LPI         — 0.375V–1.525V */
    kAPRegulatorL18,   /*  LPI         — 0.375V–1.525V */
};

#endif /* _AP_CLOCK_POWER_H_ */
