#ifndef _AP_TIMER_H_
#define _AP_TIMER_H_

#include <AndroidPlatform/APTypes.h>
#include <IOKit/IOTimerEventSource.h>
#include <IOKit/IOWorkLoop.h>

/* ------------------------------------------------------------------ *
 * APTimer — high-level one-shot and periodic timer abstraction
 *
 * Wraps IOTimerEventSource with a simpler callback interface.
 * ------------------------------------------------------------------ */

typedef void (*APTimerCallback)(OSObject* target, void* refcon);

class APTimer {
public:
    APTimer(IOWorkLoop* workLoop);
    ~APTimer();

    IOReturn scheduleRelative(uint64_t intervalNS);
    IOReturn scheduleAbsolute(uint64_t deadlineNS);
    IOReturn schedulePeriodic(uint64_t intervalNS);
    IOReturn cancel(void);
    bool     isPending(void);

    IOReturn setHandler(OSObject* target,
                       APTimerCallback callback,
                       void* refcon);

private:
    IOTimerEventSource* fTimerSource;
    IOWorkLoop*         fWorkLoop;
    bool                fActive;
};

/* ------------------------------------------------------------------ *
 * APTimeout — synchronous timeout helper (busy-wait with deadline)
 *
 * Usage:
 *   APTimeout to(1000000);   // 1 ms timeout
 *   while (!device_ready() && !to.expired()) {
 *       to.busySpin(1);      // spin 1 us
 *   }
 *   if (to.expired()) { ... handle timeout ... }
 * ------------------------------------------------------------------ */

class APTimeout {
public:
    APTimeout(uint64_t timeoutNS);
    ~APTimeout() = default;

    bool expired(void);
    void reset(void);
    uint64_t remaining(void);

private:
    uint64_t fDeadline;
    uint64_t fTimeoutNS;
};

#endif /* _AP_TIMER_H_ */
