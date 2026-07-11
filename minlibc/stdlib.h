/*
 * minlibc - stdlib.h
 */

#ifndef _MINLIBC_STDLIB_H
#define _MINLIBC_STDLIB_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Termination */
void exit(int status) __attribute__((noreturn));
void _exit(int status) __attribute__((noreturn));

/* Memory allocation */
void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);

#ifdef __cplusplus
}
#endif

#endif /* _MINLIBC_STDLIB_H */