#ifndef _AP_RUNTIME_H_
#define _AP_RUNTIME_H_

#include <AndroidPlatform/APTypes.h>
#include <kern/debug.h>
#include <kern/clock.h>
#include <mach/mach_host.h>

/* ------------------------------------------------------------------ *
 * APRuntime — kernel runtime helpers (logging, delays, asserts)
 * ------------------------------------------------------------------ */

/* ---- Logging ---- */

#define AP_LOG_LEVEL_NONE   0
#define AP_LOG_LEVEL_ERROR  1
#define AP_LOG_LEVEL_WARN   2
#define AP_LOG_LEVEL_INFO   3
#define AP_LOG_LEVEL_DEBUG  4
#define AP_LOG_LEVEL_VERB   5

#ifndef AP_LOG_LEVEL
#define AP_LOG_LEVEL AP_LOG_LEVEL_INFO
#endif

#define AP_PRINT(fmt, ...) \
    kprintf("[AndroidPlatform] " fmt "\n", ##__VA_ARGS__)

#if AP_LOG_LEVEL >= AP_LOG_LEVEL_ERROR
#define AP_ERR(fmt, ...) \
    kprintf("[AP:ERR] %s: " fmt "\n", __FUNCTION__, ##__VA_ARGS__)
#else
#define AP_ERR(fmt, ...) ((void)0)
#endif

#if AP_LOG_LEVEL >= AP_LOG_LEVEL_WARN
#define AP_WARN(fmt, ...) \
    kprintf("[AP:WARN] %s: " fmt "\n", __FUNCTION__, ##__VA_ARGS__)
#else
#define AP_WARN(fmt, ...) ((void)0)
#endif

#if AP_LOG_LEVEL >= AP_LOG_LEVEL_INFO
#define AP_LOG(fmt, ...) \
    kprintf("[AP:INFO] %s: " fmt "\n", __FUNCTION__, ##__VA_ARGS__)
#else
#define AP_LOG(fmt, ...) ((void)0)
#endif

#if AP_LOG_LEVEL >= AP_LOG_LEVEL_DEBUG
#define AP_DBG(fmt, ...) \
    kprintf("[AP:DBG] %s: " fmt "\n", __FUNCTION__, ##__VA_ARGS__)
#else
#define AP_DBG(fmt, ...) ((void)0)
#endif

#if AP_LOG_LEVEL >= AP_LOG_LEVEL_VERB
#define AP_VERB(fmt, ...) \
    kprintf("[AP:VRB] %s: " fmt "\n", __FUNCTION__, ##__VA_ARGS__)
#else
#define AP_VERB(fmt, ...) ((void)0)
#endif

/* ---- Assertions ---- */

#if DEBUG
#define AP_ASSERT(cond)                                            \
    do {                                                           \
        if (!(cond)) {                                             \
            kprintf("[AP:ASSERT] %s:%d: %s\n",                     \
                    __FILE__, __LINE__, #cond);                    \
            panic("AndroidPlatform assertion failed: %s", #cond);  \
        }                                                          \
    } while (0)
#else
#define AP_ASSERT(cond) ((void)0)
#endif

#define AP_ASSERT_EQ(a, b)  AP_ASSERT((a) == (b))
#define AP_ASSERT_NE(a, b)  AP_ASSERT((a) != (b))
#define AP_ASSERT_NOT_NULL(p) AP_ASSERT((p) != nullptr)

/* ---- Memory barriers ---- */

AP_INLINE void ap_dmb(void)
{
    __sync_synchronize();
}

AP_INLINE void ap_wmb(void)
{
    __sync_synchronize();
}

AP_INLINE void ap_rmb(void)
{
    __sync_synchronize();
}

/* ---- Spin delay (busy-wait) ---- */

AP_EXPORT
void ap_udelay(uint32_t usecs);

AP_EXPORT
void ap_mdelay(uint32_t msecs);

/* ---- Read system tick counter (cycle counter on ARM) ---- */

AP_INLINE uint64_t ap_ticks(void)
{
    uint64_t t;
    __asm__ __volatile__("mrs %0, cntvct_el0" : "=r"(t));
    return t;
}

AP_INLINE uint64_t ap_ticks_per_sec(void)
{
    uint64_t f;
    __asm__ __volatile__("mrs %0, cntfrq_el0" : "=r"(f));
    return f;
}

AP_INLINE uint64_t ap_ticks_to_ns(uint64_t ticks)
{
    return ticks * 1000000000ULL / ap_ticks_per_sec();
}

AP_INLINE uint64_t ap_ticks_to_us(uint64_t ticks)
{
    return ticks * 1000000ULL / ap_ticks_per_sec();
}

#endif /* _AP_RUNTIME_H_ */
