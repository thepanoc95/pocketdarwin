#ifndef _AP_INTERRUPT_H_
#define _AP_INTERRUPT_H_

#include <AndroidPlatform/APTypes.h>
#include <IOKit/IOService.h>
#include <IOKit/IOInterrupts.h>

/* ------------------------------------------------------------------ *
 * APInterrupt — simplified interrupt management
 *
 * Wraps IOKit's interrupt registration API and adds:
 *   - Named interrupt sources (from device tree)
 *   - Automatic handler dispatch based on IRQ number
 *   - CPU affinity for SMP IRQ balancing on MSM8916 (4x A53)
 * ------------------------------------------------------------------ */

/* Max handlers per interrupt source */
#define AP_INTERRUPT_MAX_HANDLERS  4

enum APInterruptType {
    kAPInterruptEdgeRising  = 0,
    kAPInterruptEdgeFalling = 1,
    kAPInterruptLevelHigh   = 2,
    kAPInterruptLevelLow    = 3,
};

struct APInterruptSpec {
    const char*     name;
    int             irqNumber;
    APInterruptType type;
    int             cpuAffinity;
};

typedef IOReturn (*APInterruptHandler)(OSObject* target, void* refcon,
                                       int irq, uint32_t timestamp);

class APInterruptController {
public:
    APInterruptController(IOService* provider);
    ~APInterruptController();

    IOReturn registerInterrupt(int index,
                               APInterruptHandler handler,
                               void* target,
                               void* refcon);

    IOReturn registerNamedInterrupt(const char* name,
                                    APInterruptHandler handler,
                                    void* target,
                                    void* refcon);

    IOReturn enable(int index);
    IOReturn disable(int index);
    IOReturn enableAll(void);
    IOReturn disableAll(void);

    int findInterruptByName(const char* name);

private:
    IOService* fProvider;
    int        fNumSources;
};

#endif /* _AP_INTERRUPT_H_ */
