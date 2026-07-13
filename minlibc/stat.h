/*
 * minlibc - stat.h
 */

#ifndef _MINLIBC_STAT_H
#define _MINLIBC_STAT_H

#ifdef __cplusplus
extern "C" {
#endif

struct stat {
    unsigned int   st_dev;
    unsigned int   st_ino;
    unsigned short st_mode;
    unsigned short st_nlink;
    unsigned int   st_uid;
    unsigned int   st_gid;
    unsigned int   st_rdev;
    long           st_size;
    long           st_atimespec;
    long           st_mtimespec;
    long           st_ctimespec;
    long           st_blksize;
    long           st_blocks;
};

/* File mode flags */
#define S_IFMT        0170000
#define S_IFDIR       0040000
#define S_IFCHR       0020000
#define S_IFBLK       0060000
#define S_IFREG       0100000
#define S_IFLNK       0120000
#define S_IFSOCK      0140000
#define S_IFIFO       0010000

#define S_ISDIR(m)    (((m) & S_IFMT) == S_IFDIR)
#define S_ISREG(m)    (((m) & S_IFMT) == S_IFREG)
#define S_ISBLK(m)    (((m) & S_IFMT) == S_IFBLK)
#define S_ISCHR(m)    (((m) & S_IFMT) == S_IFCHR)
#define S_ISLNK(m)    (((m) & S_IFMT) == S_IFLNK)
#define S_ISFIFO(m)   (((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m)   (((m) & S_IFMT) == S_IFSOCK)

#ifdef __cplusplus
}
#endif

#endif /* _MINLIBC_STAT_H */