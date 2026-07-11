/*
 * minlibc - stddef.h
 */

#ifndef _MINLIBC_STDDEF_H
#define _MINLIBC_STDDEF_H

#ifdef __cplusplus
extern "C" {
#endif

typedef __PTRDIFF_TYPE__ ptrdiff_t;
typedef __SIZE_TYPE__    size_t;
typedef __WCHAR_TYPE__   wchar_t;

#ifndef NULL
  #define NULL ((void *)0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _MINLIBC_STDDEF_H */