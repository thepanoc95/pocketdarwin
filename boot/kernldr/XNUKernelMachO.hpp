#ifndef _XNU_KERNEL_MACH_O_H_
#define _XNU_KERNEL_MACH_O_H_

#include <stdint.h>
#include <stddef.h>

#define MAX_KERNEL_SEGMENTS 16
#define MAX_KERNEL_SECTIONS 64

struct XNUSegment {
    char     segname[16];
    uint64_t vmaddr;
    uint64_t vmsize;
    uint64_t fileoff;
    uint64_t filesize;
    uint32_t maxprot;
    uint32_t initprot;
    uint32_t nsects;
    uint32_t flags;
};

struct XNUSection {
    char     sectname[16];
    char     segname[16];
    uint64_t addr;
    uint64_t size;
    uint32_t offset;
    uint32_t align;
    uint32_t flags;
};

struct XNUKernelInfo {
    uint8_t*    base;
    size_t      size;
    uint64_t    entry_point;
    uint32_t    ncmds;

    int         num_segments;
    XNUSegment  segments[MAX_KERNEL_SEGMENTS];

    int         num_sections;
    XNUSection  sections[MAX_KERNEL_SECTIONS];

    uint64_t    text_base;
    uint64_t    data_base;
    uint64_t    prelink_base;
    uint64_t    linkedit_base;

    uint64_t    phys_base;
    uint64_t    virt_base;

    uint32_t    cputype;
    uint32_t    cpusubtype;
};

int  xnu_kernel_parse(const uint8_t* base, size_t size, struct XNUKernelInfo* info);
int  xnu_kernel_find_segment(struct XNUKernelInfo* info, const char* name, struct XNUSegment** seg);
int  xnu_kernel_find_section(struct XNUKernelInfo* info, const char* segname, const char* sectname, struct XNUSection** sec);
void xnu_kernel_dump_info(struct XNUKernelInfo* info);

#endif
