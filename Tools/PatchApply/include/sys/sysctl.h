/*
 * Minimal sys/sysctl.h stub for building cctools on Linux.
 */
#ifndef _SYS_SYSCTL_H_
#define _SYS_SYSCTL_H_

#include <sys/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

#define CTL_KERN          1
#define KERN_OSRELEASE    2
#define KERN_OSVERSION    65

static inline int sysctl(const int *name, unsigned int namelen,
                         void *oldp, size_t *oldlenp,
                         const void *newp, size_t newlen)
{
    (void)name;
    (void)namelen;
    (void)newp;
    (void)newlen;

    if (oldp && oldlenp) {
        const char *version = "10.10.0";
        size_t vlen = strlen(version) + 1;
        if (*oldlenp > vlen)
            *oldlenp = vlen;
        memcpy(oldp, version, *oldlenp);
        return 0;
    }
    errno = ENOENT;
    return -1;
}

#define sysctlbyname(name, oldp, oldlenp, newp, newlen) \
    ({ errno = ENOSYS; -1; })

#endif /* _SYS_SYSCTL_H_ */
