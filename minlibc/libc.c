#include <stddef.h>
#include <stdint.h>

#define SYS_exit    1
#define SYS_fork    2
#define SYS_read    3
#define SYS_write   4
#define SYS_open    5
#define SYS_close   6
#define SYS_wait4   114
#define SYS_mmap    197
#define SYS_munmap  73
#define SYS_mprotect    74
#define SYS_brk     45

static inline long __syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6)
{
    register long r7 asm("r7") = num;
    register long r0 asm("r0") = a1;
    register long r1 asm("r1") = a2;
    register long r2 asm("r2") = a3;
    register long r3 asm("r3") = a4;

    asm volatile(
        "svc 0"
        : "+r"(r0)
        : "r"(r7), "r"(r1), "r"(r2), "r"(r3)
        : "memory"
    );
    return r0;
}

void exit(int status)
{
    __syscall(SYS_exit, status, 0, 0, 0, 0, 0);
    while (1);
}

void _exit(int status)
{
    exit(status);
}

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

void *malloc(size_t size)
{
    extern char _end;
    static char *heap_ptr = &_end;

    char *result = heap_ptr;
    heap_ptr += size;

    return (void *)result;
}

void *free(size_t size)
{
  extern char 
}