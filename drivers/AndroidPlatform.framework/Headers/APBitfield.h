#ifndef _AP_BITFIELD_H_
#define _AP_BITFIELD_H_

#include <AndroidPlatform/APTypes.h>

/* ------------------------------------------------------------------ *
 * APBitfield: Compile-time bitfield descriptor + runtime helper
 *
 * Usage:
 *   static const APBitfield STATUS_READY  = AP_BITS(0);
 *   static const APBitfield STATUS_ERROR  = AP_BITS(1);
 *   static const APBitfield DATA_WIDTH    = AP_FLDS(7, 4);
 *
 *   uint32_t val = AP_READ32(base, offset);
 *   if (ap_bf_test(val, STATUS_READY)) { ... }
 *   uint32_t w = ap_bf_get(val, DATA_WIDTH);
 *   val = ap_bf_set(val, DATA_WIDTH, 0xA);
 *   AP_WRITE32(base, offset, val);
 * ------------------------------------------------------------------ */

typedef struct {
    uint32_t mask;
    uint8_t  shift;
    uint8_t  width;
} APBitfield;

#define AP_BITS(n)        { 1U << (n), (n), 1 }
#define AP_FLDS(hi, lo)   { (((1U << ((hi) - (lo) + 1)) - 1) << (lo)), (lo), ((hi) - (lo) + 1) }
#define AP_BIT(n)         AP_BITS(n)
#define AP_FLD(hi, lo)    AP_FLDS(hi, lo)
#define AP_RESERVED       { 0, 0, 0 }

AP_INLINE uint32_t ap_bf_get(uint32_t regVal, APBitfield bf)
{
    return (regVal >> bf.shift) & (bf.mask >> bf.shift);
}

AP_INLINE uint32_t ap_bf_set(uint32_t regVal, APBitfield bf, uint32_t val)
{
    return (regVal & ~bf.mask) | ((val << bf.shift) & bf.mask);
}

AP_INLINE bool ap_bf_test(uint32_t regVal, APBitfield bf)
{
    return (regVal & bf.mask) != 0;
}

AP_INLINE uint32_t ap_bf_test_all(uint32_t regVal, APBitfield bf)
{
    return (regVal & bf.mask) == bf.mask;
}

AP_INLINE uint32_t ap_bf_set_bit(uint32_t regVal, APBitfield bf)
{
    return regVal | bf.mask;
}

AP_INLINE uint32_t ap_bf_clear_bit(uint32_t regVal, APBitfield bf)
{
    return regVal & ~bf.mask;
}

AP_INLINE uint32_t ap_bf_toggle(uint32_t regVal, APBitfield bf)
{
    return regVal ^ bf.mask;
}

/* ------------------------------------------------------------------ *
 * APRegisterSet — read/modify/write a group of bitfields atomically
 *
 * Usage:
 *   APRegisterSet rs(base, offset);
 *   rs.read();
 *   rs.set(READY);
 *   rs.clear(ERROR);
 *   rs.write();
 * ------------------------------------------------------------------ */

struct APRegisterSet {
    volatile uint32_t* reg;
    uint32_t shadow;

    AP_INLINE void init(volatile void* base, uint32_t offset)
    {
        reg = (volatile uint32_t*)((uintptr_t)base + offset);
    }

    AP_INLINE void read(void)
    {
        shadow = *reg;
    }

    AP_INLINE void write(void)
    {
        *reg = shadow;
    }

    AP_INLINE void modify(uint32_t clr, uint32_t set)
    {
        shadow = (shadow & ~clr) | set;
    }

    AP_INLINE void setBit(APBitfield bf)
    {
        shadow |= bf.mask;
    }

    AP_INLINE void clearBit(APBitfield bf)
    {
        shadow &= ~bf.mask;
    }

    AP_INLINE bool test(APBitfield bf)
    {
        return (shadow & bf.mask) != 0;
    }

    AP_INLINE uint32_t get(APBitfield bf)
    {
        return (shadow >> bf.shift) & (bf.mask >> bf.shift);
    }

    AP_INLINE void put(APBitfield bf, uint32_t val)
    {
        shadow = (shadow & ~bf.mask) | ((val << bf.shift) & bf.mask);
    }
};

/* ------------------------------------------------------------------ *
 * APRegisterDump — debug helper to print a block of registers
 * ------------------------------------------------------------------ */

void ap_register_dump(volatile void* base,
                      const struct APRegField* fields,
                      uint32_t numFields,
                      const char* label);

#endif /* _AP_BITFIELD_H_ */
