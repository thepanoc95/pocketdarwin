#ifndef _AP_TYPES_H_
#define _AP_TYPES_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <libkern/OSTypes.h>

#define AP_EXPORT __attribute__((visibility("default")))
#define AP_INLINE static inline __attribute__((always_inline))
#define AP_UNUSED __attribute__((unused))
#define AP_PACKED __attribute__((packed))
#define AP_ALIGNED(x) __attribute__((aligned(x)))
#define AP_SECTION(x) __attribute__((section(x)))
#define AP_USED __attribute__((used))
#define AP_DEPRECATED __attribute__((deprecated))
#define AP_WARN_UNUSED __attribute__((warn_unused_result))

typedef uint64_t ap_addr_t;
typedef uint32_t ap_reg_t;
typedef uintptr_t ap_vaddr_t;
typedef uint32_t ap_size_t;
typedef int32_t ap_status_t;
typedef uint32_t ap_flags_t;
typedef uint8_t ap_bool_t;

enum {
    kAPStatusOK           = 0,
    kAPStatusError        = -1,
    kAPStatusTimeout      = -2,
    kAPStatusBusy         = -3,
    kAPStatusNoDevice     = -4,
    kAPStatusNoMemory     = -5,
    kAPStatusInvalidParam = -6,
    kAPStatusNotSupported = -7,
    kAPStatusResourceBusy = -8,
};

typedef uint32_t APRegister;
typedef uint32_t APRegisterSet;

struct APRange {
    ap_addr_t base;
    ap_size_t size;
};

struct APVersion {
    uint16_t major;
    uint16_t minor;
    uint16_t patch;
    uint16_t build;
};

AP_INLINE uint8_t ap_reg_read8(const volatile void* base)
{
    return *(const volatile uint8_t*)base;
}

AP_INLINE uint16_t ap_reg_read16(const volatile void* base)
{
    return *(const volatile uint16_t*)base;
}

AP_INLINE uint32_t ap_reg_read32(const volatile void* base)
{
    return *(const volatile uint32_t*)base;
}

AP_INLINE void ap_reg_write8(volatile void* base, uint8_t val)
{
    *(volatile uint8_t*)base = val;
}

AP_INLINE void ap_reg_write16(volatile void* base, uint16_t val)
{
    *(volatile uint16_t*)base = val;
}

AP_INLINE void ap_reg_write32(volatile void* base, uint32_t val)
{
    *(volatile uint32_t*)base = val;
}

AP_INLINE void ap_reg_set32(volatile void* base, uint32_t mask)
{
    *(volatile uint32_t*)base |= mask;
}

AP_INLINE void ap_reg_clear32(volatile void* base, uint32_t mask)
{
    *(volatile uint32_t*)base &= ~mask;
}

AP_INLINE void ap_reg_modify32(volatile void* base, uint32_t clr, uint32_t set)
{
    uint32_t v = *(volatile uint32_t*)base;
    v &= ~clr;
    v |= set;
    *(volatile uint32_t*)base = v;
}

AP_INLINE void ap_lower32(uint32_t* hi, uint32_t* lo, uint64_t val)
{
    *hi = (uint32_t)(val >> 32);
    *lo = (uint32_t)(val & 0xFFFFFFFF);
}

AP_INLINE uint64_t ap_combine64(uint32_t hi, uint32_t lo)
{
    return ((uint64_t)hi << 32) | lo;
}

AP_INLINE uint32_t ap_align_up(uint32_t val, uint32_t align)
{
    return (val + align - 1) & ~(align - 1);
}

AP_INLINE uint32_t ap_align_down(uint32_t val, uint32_t align)
{
    return val & ~(align - 1);
}

AP_INLINE uint32_t ap_min(uint32_t a, uint32_t b) { return a < b ? a : b; }
AP_INLINE uint32_t ap_max(uint32_t a, uint32_t b) { return a > b ? a : b; }
AP_INLINE uint32_t ap_clamp(uint32_t v, uint32_t lo, uint32_t hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}

AP_INLINE uint32_t ap_bit(uint32_t n) { return 1U << n; }
AP_INLINE uint32_t ap_mask(uint32_t n) { return (1U << n) - 1; }
AP_INLINE uint32_t ap_bits(uint32_t high, uint32_t low)
{
    return ap_mask(high - low + 1) << low;
}

AP_INLINE uint32_t ap_extract(uint32_t val, uint32_t high, uint32_t low)
{
    return (val >> low) & ap_mask(high - low + 1);
}

AP_INLINE uint32_t ap_insert(uint32_t val, uint32_t field, uint32_t high, uint32_t low)
{
    uint32_t mask = ap_bits(high, low);
    return (val & ~mask) | ((field << low) & mask);
}

#endif /* _AP_TYPES_H_ */
