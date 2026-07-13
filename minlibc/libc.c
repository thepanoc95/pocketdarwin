/*
 * minlibc - Minimal C Runtime for XNU (macOS 10.7)
 * ARM EABI (eabihf) support
 * 
 * libc.c: Core libc stubs and syscall wrappers
 * EXPANDED: Shell-ready with fork, execve, waitpid, dup2, pipe, getcwd, chdir
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
#define SYS_dup2        63
#define SYS_pipe        42
#define SYS_fork_new    442  /* New fork on newer XNU */
#define SYS_execve      59
#define SYS_getpid      20
#define SYS_getppid     39
#define SYS_getcwd      326
#define SYS_chdir       12
#define SYS_stat        188
#define SYS_lstat       190
#define SYS_fstat       189
#define SYS_isatty      54
#define SYS_ioctl       54

/* ============================================================================
 * ARM EABI Syscall Interface
 * ============================================================================ */

static inline long __syscall(long num, long a1, long a2, long a3, 
                              long a4, long a5, long a6)
{
    register long r7 asm("r7") = num;
    register long r0 asm("r0") = a1;
    register long r1 asm("r1") = a2;
    register long r2 asm("r2") = a3;
    register long r3 asm("r3") = a4;
    
    asm volatile("svc 0" 
                 : "+r"(r0)
                 : "r"(r7), "r"(r1), "r"(r2), "r"(r3)
                 : "memory");
    
    return r0;
}

/* ============================================================================
 * Type Definitions
 * ============================================================================ */

typedef int fd_t;
typedef int pid_t;
typedef unsigned int uid_t;
typedef unsigned int gid_t;
typedef long ssize_t;
typedef unsigned long size_t;
typedef long off_t;

/* ============================================================================
 * Process Control
 * ============================================================================ */

void exit(int status)
{
    __syscall(SYS_exit, status, 0, 0, 0, 0, 0);
    while (1);
}

void _exit(int status)
{
    exit(status);
}

pid_t fork(void)
{
    return (pid_t)__syscall(SYS_fork, 0, 0, 0, 0, 0, 0);
}

pid_t getpid(void)
{
    return (pid_t)__syscall(SYS_getpid, 0, 0, 0, 0, 0, 0);
}

pid_t getppid(void)
{
    return (pid_t)__syscall(SYS_getppid, 0, 0, 0, 0, 0, 0);
}

/* waitpid - simplified wrapper */
typedef struct {
    int status;
} waitpid_status_t;

pid_t waitpid(pid_t pid, int *status, int options)
{
    long ret = __syscall(SYS_wait4, pid, (long)status, options, 0, 0, 0);
    return (pid_t)ret;
}

/* wait - wait for any child */
pid_t wait(int *status)
{
    return waitpid(-1, status, 0);
}

/* execve - execute a program */
int execve(const char *filename, char *const argv[], char *const envp[])
{
    return (int)__syscall(SYS_execve, (long)filename, (long)argv, (long)envp, 0, 0, 0);
}

/* ============================================================================
 * File I/O & Descriptors
 * ============================================================================ */

fd_t open(const char *path, int flags, int mode)
{
    return (fd_t)__syscall(SYS_open, (long)path, flags, mode, 0, 0, 0);
}

int close(fd_t fd)
{
    return (int)__syscall(SYS_close, fd, 0, 0, 0, 0, 0);
}

ssize_t read(fd_t fd, void *buf, size_t count)
{
    return (ssize_t)__syscall(SYS_read, fd, (long)buf, count, 0, 0, 0);
}

ssize_t write(fd_t fd, const void *buf, size_t count)
{
    return (ssize_t)__syscall(SYS_write, fd, (long)buf, count, 0, 0, 0);
}

/* dup2 - duplicate file descriptor to specific fd */
int dup2(fd_t oldfd, fd_t newfd)
{
    return (int)__syscall(SYS_dup2, oldfd, newfd, 0, 0, 0, 0);
}

/* pipe - create bidirectional pipe */
int pipe(fd_t pipefd[2])
{
    return (int)__syscall(SYS_pipe, (long)pipefd, 0, 0, 0, 0, 0);
}

/* ============================================================================
 * Directory Operations
 * ============================================================================ */

int chdir(const char *path)
{
    return (int)__syscall(SYS_chdir, (long)path, 0, 0, 0, 0, 0);
}

char *getcwd(char *buf, size_t size)
{
    long ret = __syscall(SYS_getcwd, (long)buf, size, 0, 0, 0, 0);
    return (ret >= 0) ? buf : NULL;
}

/* ============================================================================
 * File Status
 * ============================================================================ */

struct stat {
    unsigned int   st_dev;
    unsigned int   st_ino;
    unsigned short st_mode;
    unsigned short st_nlink;
    unsigned int   st_uid;
    unsigned int   st_gid;
    unsigned int   st_rdev;
    long           st_size;
    long           st_atimespec;  /* simplified */
    long           st_mtimespec;
    long           st_ctimespec;
    long           st_blksize;
    long           st_blocks;
};

int stat(const char *pathname, struct stat *statbuf)
{
    return (int)__syscall(SYS_stat, (long)pathname, (long)statbuf, 0, 0, 0, 0);
}

int fstat(fd_t fd, struct stat *statbuf)
{
    return (int)__syscall(SYS_fstat, fd, (long)statbuf, 0, 0, 0, 0);
}

/* isatty - check if fd is a terminal */
int isatty(fd_t fd)
{
    return (int)__syscall(SYS_isatty, fd, 0, 0, 0, 0, 0);
}

/* ============================================================================
 * Memory Management
 * ============================================================================ */

void *malloc(size_t size)
{
    extern char _end;
    static char *heap_ptr = &_end;
    
    char *result = heap_ptr;
    heap_ptr += size;
    
    return (void *)result;
}

void free(void *ptr)
{
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

char *strrchr(const char *s, int c)
{
    const char *last = NULL;
    while (*s) {
        if (*s == (char)c)
            last = s;
        s++;
    }
    if (c == 0)
        return (char *)s;
    return (char *)last;
}

/* strdup - allocate and copy string */
char *strdup(const char *s)
{
    size_t len = strlen(s) + 1;
    char *dup = (char *)malloc(len);
    if (dup)
        strcpy(dup, s);
    return dup;
}

/* strtok - tokenize string (simple version) */
char *strtok(char *str, const char *delim)
{
    static char *last = NULL;
    
    if (!str) {
        str = last;
        if (!str)
            return NULL;
    }
    
    /* Skip leading delimiters */
    while (*str && strchr(delim, *str))
        str++;
    
    if (!*str) {
        last = NULL;
        return NULL;
    }
    
    char *start = str;
    
    /* Find end of token */
    while (*str && !strchr(delim, *str))
        str++;
    
    if (*str) {
        *str++ = 0;
        last = str;
    } else {
        last = NULL;
    }
    
    return start;
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
 * Character Classification
 * ============================================================================ */

int isspace(int c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

int isalpha(int c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int isdigit(int c)
{
    return c >= '0' && c <= '9';
}

int isalnum(int c)
{
    return isalpha(c) || isdigit(c);
}

int tolower(int c)
{
    return (c >= 'A' && c <= 'Z') ? c - 'A' + 'a' : c;
}

int toupper(int c)
{
    return (c >= 'a' && c <= 'z') ? c - 'a' + 'A' : c;
}

/* ============================================================================
 * Environment Variables (minimal support)
 * ============================================================================ */

extern char **environ;

char *getenv(const char *name)
{
    if (!environ)
        return NULL;
    
    size_t namelen = strlen(name);
    
    for (int i = 0; environ[i]; i++) {
        if (strncmp(environ[i], name, namelen) == 0 && environ[i][namelen] == '=') {
            return environ[i] + namelen + 1;
        }
    }
    
    return NULL;
}

/* ============================================================================
 * C++ / Runtime Support
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