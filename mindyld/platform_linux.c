/*
 * mindyld - Linux Platform Abstraction
 * Platform-specific functionality for Linux systems
 */

#include "mindyld_common.h"

#ifdef MINDYLD_LINUX

#include <unistd.h>

/* ============================================================================
 * Linux Specific Initialization
 * ============================================================================ */

void mindyld_platform_init(void)
{
    /* Linux-specific initialization code */
    /* This could include:
       - Reading /proc/self/maps for memory map information
       - Getting system page size
       - Checking for SELinux restrictions
     */
}

/* ============================================================================
 * Library Search Paths (Linux)
 * ============================================================================ */

const char *linux_library_paths[] = {
    "/lib64",
    "/lib",
    "/usr/lib64",
    "/usr/lib",
    "/usr/local/lib64",
    "/usr/local/lib",
    "/opt/lib64",
    "/opt/lib",
    NULL
};

/* ============================================================================
 * ASLR Support (Address Space Layout Randomization)
 * ============================================================================ */

int mindyld_query_aslr_status(void)
{
    /* Check /proc/sys/kernel/randomize_va_space
       0 = disabled
       1 = enabled (conservative)
       2 = enabled (full)
     */
    return -1;  /* TODO: Implement */
}

/* ============================================================================
 * Stack Canary Support
 * ============================================================================ */

unsigned long __stack_chk_guard = 0;

void __stack_chk_fail(void)
{
    /* Stack smashing detected */
    _exit(1);
}

#endif /* MINDYLD_LINUX */
