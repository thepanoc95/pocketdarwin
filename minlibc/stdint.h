/*
 * minlibc - stdint.h
 */

#ifndef _MINLIBC_STDINT_H
#define _MINLIBC_STDINT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Fixed-width integer types */
typedef signed char         int8_t;
typedef unsigned char       uint8_t;
typedef short               int16_t;
typedef unsigned short      uint16_t;
typedef int                 int32_t;
typedef unsigned int        uint32_t;
typedef long long           int64_t;
typedef unsigned long long  uint64_t;

/* Pointer-sized types */
typedef int                 intptr_t;
typedef unsigned int        uintptr_t;

#ifdef __cplusplus
}
#endif

#endif /* _MINLIBC_STDINT_H */