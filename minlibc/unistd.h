/*
 * minlibc - unistd.h
 * POSIX Unistd definitions - Enhanced for dynamic linking
 */

#ifndef _MINLIBC_UNISTD_H
#define _MINLIBC_UNISTD_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef long ssize_t;
typedef int fd_t;
typedef int pid_t;
typedef unsigned int uid_t;
typedef unsigned int gid_t;
typedef long off_t;

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

/* File I/O */
fd_t open(const char *path, int flags, int mode);
int close(fd_t fd);
ssize_t read(fd_t fd, void *buf, size_t count);
ssize_t write(fd_t fd, const void *buf, size_t count);
off_t lseek(int fd, off_t offset, int whence);
int access(const char *pathname, int mode);

/* File flags for open() */
#define O_RDONLY   0x0000
#define O_WRONLY   0x0001
#define O_RDWR     0x0002
#define O_CREAT    0x0040
#define O_TRUNC    0x0200
#define O_APPEND   0x0400

/* Seek whence */
#define SEEK_SET   0
#define SEEK_CUR   1
#define SEEK_END   2

/* Access modes */
#define F_OK   0
#define X_OK   1
#define W_OK   2
#define R_OK   4

/* Process control */
void exit(int status) __attribute__((noreturn));
void _exit(int status) __attribute__((noreturn));
pid_t fork(void);
pid_t getpid(void);
pid_t getppid(void);
pid_t waitpid(pid_t pid, int *status, int options);
int execve(const char *filename, char *const argv[], char *const envp[]);

/* User/group control */
uid_t getuid(void);
uid_t geteuid(void);
gid_t getgid(void);
gid_t getegid(void);
int setuid(uid_t uid);
int setgid(gid_t gid);

/* Directory operations */
int chdir(const char *path);
char *getcwd(char *buf, size_t size);

/* File information */
int stat(const char *pathname, void *statbuf);
int fstat(fd_t fd, void *statbuf);
int isatty(fd_t fd);

/* Memory mapping */
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int munmap(void *addr, size_t length);
int mprotect(void *addr, size_t length, int prot);

/* Memory protection flags */
#define PROT_NONE   0x00
#define PROT_READ   0x01
#define PROT_WRITE  0x02
#define PROT_EXEC   0x04

/* Mmap flags */
#define MAP_SHARED     0x0001
#define MAP_PRIVATE    0x0002
#define MAP_FIXED      0x0010
#define MAP_ANONYMOUS  0x0020
#define MAP_ANON       0x0020

#ifdef __cplusplus
}
#endif

#endif /* _MINLIBC_UNISTD_H */