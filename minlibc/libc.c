/*
 * minlibc - Minimal C Runtime for XNU (macOS 10.7)
 * ARM EABI (eabihf) support
 * 
 * libc.c: Core libc stubs and syscall wrappers
 */

#include <stddef.h>
#include <stdint.h>

/* ============================================================================
 * Syscall Numbers for ARM EABI on XNU (macOS 10.7)
 * ============================================================================ */

#define SYS_exit        1
#define SYS_fork        2
#define SYS_read        3
#define SYS_write       4
#define SYS_open        5
#define SYS_close       6
#define SYS_wait4       114
#define SYS_mmap        197
#define SYS_munmap      73
#define SYS_mprotect    74
#define SYS_brk         45

/* ============================================================================
 * ARM EABI Syscall Interface
 * ============================================================================
 * 
 * Registers:
 *   r0-r3: arguments
 *   r7: syscall number
 *   r0: return value
 * 
 * Call: svc 0x80 (or just svc 0 on newer versions)
 */

/* Low-level syscall macro */
static inline long __syscall(long num, long a1, long a2, long a3, 
                              long a4, long a5, long a6)
{
    register long r7 asm("r7") = num;
    register long r0 asm("r0") = a1;
    register long r1 asm("r1") = a2;
    register long r2 asm("r2") = a3;
    register long r3 asm("r3") = a4;
    
    /* Note: r4 and r5 for a5/a6 if needed, but we keep it simple */
    
    asm volatile("svc 0" 
                 : "+r"(r0)
                 : "r"(r7), "r"(r1), "r"(r2), "r"(r3)
                 : "memory");
    
    return r0;
}

/* ============================================================================
 * Process Control
 * ============================================================================ */

void exit(int status)
{
    __syscall(SYS_exit, status, 0, 0, 0, 0, 0);
    
    /* Infinite loop (should never reach) */
    while (1);
}

void _exit(int status)
{
    exit(status);
}

/* ============================================================================
 * File I/O
 * ============================================================================ */

typedef int fd_t;

fd_t open(const char *path, int flags, int mode)
{
    return (fd_t)__syscall(SYS_open, (long)path, flags, mode, 0, 0, 0);
}

int close(fd_t fd)
{
    return (int)__syscall(SYS_close, fd, 0, 0, 0, 0, 0);
}

typedef int ssize_t;
typedef unsigned long size_t;

ssize_t read(fd_t fd, void *buf, size_t count)
{
    return (ssize_t)__syscall(SYS_read, fd, (long)buf, count, 0, 0, 0);
}

ssize_t write(fd_t fd, const void *buf, size_t count)
{
    return (ssize_t)__syscall(SYS_write, fd, (long)buf, count, 0, 0, 0);
}

/* ============================================================================
 * Memory Management
 * ============================================================================ */

void *malloc(size_t size)
{
    /* Simplified: just allocate from the heap end */
    extern char _end;
    static char *heap_ptr = &_end;
    
    char *result = heap_ptr;
    heap_ptr += size;
    
    return (void *)result;
}

void free(void *ptr)
{
    /* Minimal implementation: do nothing */
    (void)ptr;
}

void *calloc(size_t nmemb, size_t size)
{
    size_t total = nmemb * size;
    void *ptr = malloc(total);
    
    if (ptr) {
        char *p = (char *)ptr;
        for (size_t i = 0; i < total; i++) {
            p[i] = 0;
        }
    }
    
    return ptr;
}

void *realloc(void *ptr, size_t size)
{
    /* Simplified: allocate new, copy old */
    (void)ptr;
    return malloc(size);
}

/* ============================================================================
 * String Functions
 * ============================================================================ */

size_t strlen(const char *s)
{
    size_t len = 0;
    while (s[len])
        len++;
    return len;
}

char *strcpy(char *dest, const char *src)
{
    char *d = dest;
    while ((*d++ = *src++))
        ;
    return dest;
}

char *strncpy(char *dest, const char *src, size_t n)
{
    size_t i;
    for (i = 0; i < n && src[i]; i++)
        dest[i] = src[i];
    while (i < n)
        dest[i++] = 0;
    return dest;
}

int strcmp(const char *s1, const char *s2)
{
    while (*s1 && *s1 == *s2)
        s1++, s2++;
    return (unsigned char)*s1 - (unsigned char)*s2;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        if (s1[i] != s2[i])
            return (unsigned char)s1[i] - (unsigned char)s2[i];
        if (!s1[i])
            return 0;
    }
    return 0;
}

char *strchr(const char *s, int c)
{
    while (*s) {
        if (*s == (char)c)
            return (char *)s;
        s++;
    }
    if (c == 0)
        return (char *)s;
    return NULL;
}

/* ============================================================================
 * Memory Functions
 * ============================================================================ */

void *memset(void *s, int c, size_t n)
{
    unsigned char *p = (unsigned char *)s;
    for (size_t i = 0; i < n; i++)
        p[i] = (unsigned char)c;
    return s;
}

void *memcpy(void *dest, const void *src, size_t n)
{
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    for (size_t i = 0; i < n; i++)
        d[i] = s[i];
    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *a = (const unsigned char *)s1;
    const unsigned char *b = (const unsigned char *)s2;
    for (size_t i = 0; i < n; i++) {
        if (a[i] != b[i])
            return (int)a[i] - (int)b[i];
    }
    return 0;
}

void *memmove(void *dest, const void *src, size_t n)
{
    /* Simple but safe: always go byte-by-byte */
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    
    if (d <= s) {
        for (size_t i = 0; i < n; i++)
            d[i] = s[i];
    } else {
        for (size_t i = n; i > 0; i--)
            d[i-1] = s[i-1];
    }
    
    return dest;
}

/* ============================================================================
 * Null Definitions (for compatibility)
 * ============================================================================ */

void *__dso_handle = NULL;

int __cxa_atexit(void (*func)(void *), void *arg, void *dso)
{
    (void)func;
    (void)arg;
    (void)dso;
    return 0;
}

void __cxa_finalize(void *dso)
{
    (void)dso;
}

/* ============================================================================
 * Weak main() stub (user code should override)
 * ============================================================================ */

int main(int argc, char *argv[], char *envp[])
{
    (void)argc;
    (void)argv;
    (void)envp;
    return 0;
}
