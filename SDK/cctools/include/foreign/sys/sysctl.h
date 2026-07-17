#if defined(__linux__) || defined(__CYGWIN__)
#ifndef __SYSCTL_H__
#define __SYSCTL_H__

#include <errno.h>
#include <stdio.h>

typedef unsigned int u_int;

#define CTL_KERN 1
#define KERN_OSRELEASE 2

static inline
int sysctl(const int *name, u_int namelen, void *oldp, size_t *oldlenp,
           const void *newp, size_t newlen)
{
    errno = EINVAL;
    return -1;
}
#endif /* __SYSCTL_H__ */
#else
#ifndef __SYSCTL_H__
#define __SYSCTL_H__

#include_next <sys/sysctl.h>
#endif /* __SYSCTL_H__ */
#endif
