#include "XNUKernelMachO.hpp"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MH_MAGIC     0xfeedface
#define MH_CIGAM     0xcefaedfe
#define MH_MAGIC_64  0xfeedfacf
#define MH_CIGAM_64  0xcffaedfe

#define MH_EXECUTE   0x2
#define MH_PRELOAD   0x5

#define LC_SEGMENT        0x1
#define LC_SYMTAB         0x2
#define LC_THREAD         0x4
#define LC_UNIXTHREAD     0x5
#define LC_SEGMENT_64     0x19
#define LC_UUID           0x1b
#define LC_CODE_SIGNATURE 0x1d
#define LC_SOURCE_VERSION 0x2A

#define CPU_TYPE_ARM64    0x0100000c
#define CPU_TYPE_ARM      12

#define SEG_TEXT        "__TEXT"
#define SEG_DATA        "__DATA"
#define SEG_LINKEDIT    "__LINKEDIT"
#define SEG_PRELINK     "__PRELINK"
#define SEG_PPLTEXT     "__PPLTEXT"
#define SEG_PPLDATA     "__PPLDATA"
#define SEG_LAST        "__LAST"
#define SEG_HIB         "__HIB"
#define SEG_KLD         "__KLD"
#define SEG_CTF         "__CTF"

#define ARM_THREAD_STATE64     4
#define ARM_THREAD_STATE64_COUNT 70

#pragma pack(push, 1)
struct mach_header {
    uint32_t magic;
    uint32_t cputype;
    uint32_t cpusubtype;
    uint32_t filetype;
    uint32_t ncmds;
    uint32_t sizeofcmds;
    uint32_t flags;
};

struct mach_header_64 {
    uint32_t magic;
    uint32_t cputype;
    uint32_t cpusubtype;
    uint32_t filetype;
    uint32_t ncmds;
    uint32_t sizeofcmds;
    uint32_t flags;
    uint32_t reserved;
};

struct load_command {
    uint32_t cmd;
    uint32_t cmdsize;
};

struct segment_command_64 {
    uint32_t cmd;
    uint32_t cmdsize;
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

struct segment_command {
    uint32_t cmd;
    uint32_t cmdsize;
    char     segname[16];
    uint32_t vmaddr;
    uint32_t vmsize;
    uint32_t fileoff;
    uint32_t filesize;
    uint32_t maxprot;
    uint32_t initprot;
    uint32_t nsects;
    uint32_t flags;
};

struct section_64 {
    char     sectname[16];
    char     segname[16];
    uint64_t addr;
    uint64_t size;
    uint32_t offset;
    uint32_t align;
    uint32_t reloff;
    uint32_t nreloc;
    uint32_t flags;
    uint32_t reserved1;
    uint32_t reserved2;
    uint32_t reserved3;
};

struct section {
    char     sectname[16];
    char     segname[16];
    uint32_t addr;
    uint32_t size;
    uint32_t offset;
    uint32_t align;
    uint32_t reloff;
    uint32_t nreloc;
    uint32_t flags;
    uint32_t reserved1;
    uint32_t reserved2;
};

struct thread_command {
    uint32_t cmd;
    uint32_t cmdsize;
};

struct arm_thread_state64_t {
    uint64_t x[29];
    uint64_t fp;
    uint64_t lr;
    uint64_t sp;
    uint64_t pc;
    uint32_t cpsr;
};

struct symtab_command {
    uint32_t cmd;
    uint32_t cmdsize;
    uint32_t symoff;
    uint32_t nsyms;
    uint32_t stroff;
    uint32_t strsize;
};
#pragma pack(pop)

static uint32_t read_be32(const uint8_t* p) {
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8)  | ((uint32_t)p[3]);
}

static int is_byte_swapped(uint32_t magic) {
    return magic == MH_CIGAM || magic == MH_CIGAM_64;
}

int xnu_kernel_parse(const uint8_t* base, size_t size, struct XNUKernelInfo* info) {
    if (!base || !info || size < sizeof(struct mach_header))
        return -1;

    memset(info, 0, sizeof(*info));
    info->base = (uint8_t*)base;
    info->size = size;

    struct mach_header* hdr = (struct mach_header*)base;
    int swap = is_byte_swapped(hdr->magic);
    int is64 = (hdr->magic == MH_MAGIC_64 || hdr->magic == MH_CIGAM_64);

#   define SWAP32(x) (swap ? __builtin_bswap32(x) : (x))
#   define SWAP64(x) (swap ? __builtin_bswap64(x) : (x))

    uint32_t ncmds = SWAP32(hdr->ncmds);
    uint32_t sizeofcmds = SWAP32(hdr->sizeofcmds);
    uint32_t filetype = SWAP32(hdr->filetype);
    uint32_t cputype = SWAP32(hdr->cputype);
    uint32_t cpusubtype = SWAP32(hdr->cpusubtype);

    info->ncmds = ncmds;
    info->cputype = cputype;
    info->cpusubtype = cpusubtype;

    if (filetype != MH_EXECUTE && filetype != MH_PRELOAD) {
        printf("XNUKernelMachO: unexpected filetype 0x%x (expected EXECUTE or PRELOAD)\n", filetype);
        return -1;
    }

    if (is64 && cputype != CPU_TYPE_ARM64) {
        printf("XNUKernelMachO: not ARM64 (cputype=0x%x)\n", cputype);
        return -1;
    }
    if (!is64 && cputype != CPU_TYPE_ARM) {
        printf("XNUKernelMachO: not ARM (cputype=0x%x)\n", cputype);
        return -1;
    }

    size_t hdr_size = is64 ? sizeof(struct mach_header_64) : sizeof(struct mach_header);
    const uint8_t* cmds_ptr = base + hdr_size;
    const uint8_t* cmds_end = cmds_ptr + sizeofcmds;

    for (uint32_t i = 0; i < ncmds; i++) {
        if (cmds_ptr + sizeof(struct load_command) > cmds_end)
            break;

        struct load_command* lc = (struct load_command*)cmds_ptr;
        uint32_t cmd = SWAP32(lc->cmd);
        uint32_t cmdsize = SWAP32(lc->cmdsize);

        if (cmdsize < sizeof(struct load_command) || cmds_ptr + cmdsize > cmds_end)
            break;

        switch (cmd) {
        case LC_SEGMENT_64: {
            struct segment_command_64* seg = (struct segment_command_64*)cmds_ptr;
            if (info->num_segments >= MAX_KERNEL_SEGMENTS)
                break;

            struct XNUSegment* xs = &info->segments[info->num_segments++];
            memcpy(xs->segname, seg->segname, 16);
            xs->vmaddr   = SWAP64(seg->vmaddr);
            xs->vmsize   = SWAP64(seg->vmsize);
            xs->fileoff  = SWAP64(seg->fileoff);
            xs->filesize = SWAP64(seg->filesize);
            xs->maxprot  = SWAP32(seg->maxprot);
            xs->initprot = SWAP32(seg->initprot);
            xs->nsects   = SWAP32(seg->nsects);
            xs->flags    = SWAP32(seg->flags);

            if (strncmp(xs->segname, SEG_TEXT, 16) == 0)
                info->text_base = xs->vmaddr;
            else if (strncmp(xs->segname, SEG_DATA, 16) == 0)
                info->data_base = xs->vmaddr;
            else if (strncmp(xs->segname, SEG_PRELINK, 16) == 0)
                info->prelink_base = xs->vmaddr;
            else if (strncmp(xs->segname, SEG_LINKEDIT, 16) == 0)
                info->linkedit_base = xs->vmaddr;

            struct section_64* sec = (struct section_64*)(cmds_ptr + sizeof(struct segment_command_64));
            for (uint32_t j = 0; j < xs->nsects && info->num_sections < MAX_KERNEL_SECTIONS; j++) {
                struct XNUSection* xs_sec = &info->sections[info->num_sections++];
                memcpy(xs_sec->sectname, sec->sectname, 16);
                memcpy(xs_sec->segname, sec->segname, 16);
                xs_sec->addr   = SWAP64(sec->addr);
                xs_sec->size   = SWAP64(sec->size);
                xs_sec->offset = SWAP32(sec->offset);
                xs_sec->align  = SWAP32(sec->align);
                xs_sec->flags  = SWAP32(sec->flags);
                sec++;
            }
            break;
        }

        case LC_SEGMENT: {
            struct segment_command* seg = (struct segment_command*)cmds_ptr;
            if (info->num_segments >= MAX_KERNEL_SEGMENTS)
                break;

            struct XNUSegment* xs = &info->segments[info->num_segments++];
            memcpy(xs->segname, seg->segname, 16);
            xs->vmaddr   = SWAP32(seg->vmaddr);
            xs->vmsize   = SWAP32(seg->vmsize);
            xs->fileoff  = SWAP32(seg->fileoff);
            xs->filesize = SWAP32(seg->filesize);
            xs->maxprot  = SWAP32(seg->maxprot);
            xs->initprot = SWAP32(seg->initprot);
            xs->nsects   = SWAP32(seg->nsects);
            xs->flags    = SWAP32(seg->flags);

            if (strncmp(xs->segname, SEG_TEXT, 16) == 0)
                info->text_base = xs->vmaddr;
            else if (strncmp(xs->segname, SEG_DATA, 16) == 0)
                info->data_base = xs->vmaddr;

            struct section* sec = (struct section*)(cmds_ptr + sizeof(struct segment_command));
            for (uint32_t j = 0; j < xs->nsects && info->num_sections < MAX_KERNEL_SECTIONS; j++) {
                struct XNUSection* xs_sec = &info->sections[info->num_sections++];
                memcpy(xs_sec->sectname, sec->sectname, 16);
                memcpy(xs_sec->segname, sec->segname, 16);
                xs_sec->addr   = SWAP32(sec->addr);
                xs_sec->size   = SWAP32(sec->size);
                xs_sec->offset = SWAP32(sec->offset);
                xs_sec->align  = SWAP32(sec->align);
                xs_sec->flags  = SWAP32(sec->flags);
                sec++;
            }
            break;
        }

        case LC_UNIXTHREAD: {
            struct thread_command* tc = (struct thread_command*)cmds_ptr;
            uint8_t* thread_data = cmds_ptr + sizeof(struct thread_command);
            uint32_t flavor = SWAP32(*(uint32_t*)thread_data);
            uint32_t count  = SWAP32(*(uint32_t*)(thread_data + 4));

            if (is64 && flavor == ARM_THREAD_STATE64) {
                struct arm_thread_state64_t* state =
                    (struct arm_thread_state64_t*)(thread_data + 8);
                info->entry_point = SWAP64(state->pc);
            }
            break;
        }

        case LC_SYMTAB: {
            break;
        }

        case LC_UUID:
        case LC_SOURCE_VERSION:
        case LC_CODE_SIGNATURE:
            break;

        default:
            break;
        }

        cmds_ptr += cmdsize;
    }

    if (info->num_segments == 0) {
        printf("XNUKernelMachO: no segments found\n");
        return -1;
    }

    if (info->entry_point == 0) {
        printf("XNUKernelMachO: no entry point (no LC_UNIXTHREAD)\n");
        return -1;
    }

    info->virt_base = info->text_base;
    for (int i = 0; i < info->num_segments; i++) {
        struct XNUSegment* seg = &info->segments[i];
        if (seg->vmaddr < info->virt_base)
            info->virt_base = seg->vmaddr;
    }

#   undef SWAP32
#   undef SWAP64

    return 0;
}

int xnu_kernel_find_segment(struct XNUKernelInfo* info, const char* name, struct XNUSegment** seg) {
    for (int i = 0; i < info->num_segments; i++) {
        if (strncmp(info->segments[i].segname, name, 16) == 0) {
            *seg = &info->segments[i];
            return 0;
        }
    }
    return -1;
}

int xnu_kernel_find_section(struct XNUKernelInfo* info, const char* segname, const char* sectname, struct XNUSection** sec) {
    for (int i = 0; i < info->num_sections; i++) {
        struct XNUSection* s = &info->sections[i];
        if (strncmp(s->segname, segname, 16) == 0 && strncmp(s->sectname, sectname, 16) == 0) {
            *sec = s;
            return 0;
        }
    }
    return -1;
}

void xnu_kernel_dump_info(struct XNUKernelInfo* info) {
    printf("XNU Kernel Info:\n");
    printf("  Entry point: 0x%llx\n", (unsigned long long)info->entry_point);
    printf("  CPU type: 0x%x, subtype: 0x%x\n", info->cputype, info->cpusubtype);
    printf("  Text base: 0x%llx\n", (unsigned long long)info->text_base);
    printf("  Data base: 0x%llx\n", (unsigned long long)info->data_base);
    printf("  Prelink base: 0x%llx\n", (unsigned long long)info->prelink_base);
    printf("  Linkedit base: 0x%llx\n", (unsigned long long)info->linkedit_base);
    printf("  Virt base: 0x%llx\n", (unsigned long long)info->virt_base);
    printf("  Num segments: %d\n", info->num_segments);
    printf("  Num sections: %d\n", info->num_sections);

    for (int i = 0; i < info->num_segments; i++) {
        struct XNUSegment* seg = &info->segments[i];
        printf("  Segment[%d]: %-16s vmaddr=0x%llx vmsize=0x%llx "
               "fileoff=0x%llx filesize=0x%llx prot=%c%c%c nsects=%d\n",
               i, seg->segname,
               (unsigned long long)seg->vmaddr,
               (unsigned long long)seg->vmsize,
               (unsigned long long)seg->fileoff,
               (unsigned long long)seg->filesize,
               (seg->initprot & 4) ? 'r' : '-',
               (seg->initprot & 2) ? 'w' : '-',
               (seg->initprot & 1) ? 'x' : '-',
               seg->nsects);
    }
}
