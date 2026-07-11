/*
 * minlibc - unistd.h
 */

#ifndef _MINLIBC_UNISTD_H
#define _MINLIBC_UNISTD_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int ssize_t;
typedef int fd_t;

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

/* File I/O */
fd_t open(const char *path, int flags, int mode);
int close(fd_t fd);
ssize_t read(fd_t fd, void *buf, size_t count);
ssize_t write(fd_t fd, const void *buf, size_t count);

/* Process control */
void exit(int status) __attribute__((noreturn));
void _exit(int status) __attribute__((noreturn));

#ifdef __cplusplus
}
#endif

#endif /* _MINLIBC_UNISTD_H */