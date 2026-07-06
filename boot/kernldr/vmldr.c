#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "XNUKernelMachO.hpp"

#define MAX_KERNEL_SIZE 0x10000000

const char* VMLdrKernelBinName = "mach_kernel";

#pragma mark Boot argument structures

struct xnu_boot_args {
    uint64_t    revision;
    uint64_t    flags;
    uint64_t    phys_mem_base;
    uint64_t    phys_mem_size;
    uint64_t    dtb_ptr;
    uint64_t    dtb_length;
    uint64_t    kernel_phys_base;
    uint64_t    kernel_virt_base;
    uint64_t    kernel_entry;
    uint64_t    top_of_kernel_data;
    uint64_t    cpu_type;
    uint64_t    cpu_subtype;
    uint64_t    mem_size_actual;
    uint64_t    debug_flags;
    uint64_t    reserved[8];
};

#define XNU_BOOT_ARGS_REVISION 1
#define XNU_BOOT_FLAG_DTB_AT_LOCAL 0x1

#pragma mark Page table helpers (4KB granule, 3-level for early boot)

#define TT_L0_BLOCK     0x400000000000ULL
#define TT_L1_BLOCK     0x2000000000ULL
#define TT_L2_BLOCK     0x200000ULL
#define TT_L3_PAGE      0x1000ULL

#define TT_TYPE_TABLE   0x3
#define TT_TYPE_BLOCK   0x1
#define TT_TYPE_PAGE    0x3

#define TT_AF           0x400
#define TT_ISH          0x600
#define TT_NS           0x200
#define TT_AP_RW        0x000
#define TT_AP_RO        0x200

#define TT_ATTR_DEVICE  0x000ULL
#define TT_ATTR_NORMAL  0x0400ULL  /* AttrIndx 1 in MAIR */
#define TT_ATTR_NORMAL_WB  0x0400ULL

#define TT_MT_DEVICE    (TT_TYPE_BLOCK | TT_AF | TT_NS | TT_ISH)
#define TT_MT_NORMAL    (TT_TYPE_BLOCK | TT_AF | TT_NS | TT_ISH | TT_ATTR_NORMAL)
#define TT_MT_NORMAL_WB (TT_TYPE_BLOCK | TT_AF | TT_NS | TT_ISH | TT_ATTR_NORMAL_WB)

#pragma mark External symbols from boot.arm64.S
extern void xnu_kernel_entry(uint64_t entry, uint64_t x0, uint64_t x1, uint64_t x2);

#pragma mark Memory management state

static uint64_t tt_l0[512] __attribute__((aligned(4096)));
static uint64_t tt_l1[512] __attribute__((aligned(4096))) __attribute__((unused));
static uint64_t tt_l2[512] __attribute__((aligned(4096))) __attribute__((unused));
static uint64_t tt_l3[512] __attribute__((aligned(4096))) __attribute__((unused));

static volatile uint64_t page_table_pool[4096] __attribute__((aligned(4096)));
static volatile uint32_t page_table_pool_used = 0;

static inline void write_mair(uint64_t val) {
    __asm__ volatile("msr mair_el1, %0" : : "r"(val) : "memory");
}

static inline void write_ttbr0(uint64_t val) {
    __asm__ volatile("msr ttbr0_el1, %0" : : "r"(val) : "memory");
}

static inline void write_ttbr1(uint64_t val) {
    __asm__ volatile("msr ttbr1_el1, %0" : : "r"(val) : "memory");
}

static inline void write_tcr(uint64_t val) {
    __asm__ volatile("msr tcr_el1, %0" : : "r"(val) : "memory");
}

static inline void write_sctlr(uint64_t val) {
    __asm__ volatile("msr sctlr_el1, %0" : : "r"(val) : "memory");
}

static inline uint64_t read_sctlr(void) {
    uint64_t val;
    __asm__ volatile("mrs %0, sctlr_el1" : "=r"(val));
    return val;
}

static inline void isb(void) {
    __asm__ volatile("isb" : : : "memory");
}

static inline void dsb(void) {
    __asm__ volatile("dsb sy" : : : "memory");
}

static inline void tlbi_all(void) {
    __asm__ volatile("tlbi vmalle1is" : : : "memory");
    dsb();
    isb();
}

static uint64_t* alloc_page_table(void) {
    uint64_t* table = (uint64_t*)((uint64_t)&page_table_pool + page_table_pool_used * 4096);
    page_table_pool_used++;
    for (int i = 0; i < 512; i++)
        table[i] = 0;
    return table;
}

static void map_range(uint64_t virt, uint64_t phys, uint64_t size, uint64_t attr) {
    while (size > 0) {
        uint64_t l0_idx = (virt >> 39) & 0x1ff;
        uint64_t l1_idx = (virt >> 30) & 0x1ff;
        uint64_t l2_idx = (virt >> 21) & 0x1ff;
        uint64_t l3_idx = (virt >> 12) & 0x1ff;

        if ((virt & (TT_L3_PAGE - 1)) || (phys & (TT_L3_PAGE - 1)) || size < TT_L3_PAGE) {
            return;
        }

        if ((virt & (TT_L2_BLOCK - 1)) == 0 && (phys & (TT_L2_BLOCK - 1)) == 0 && size >= TT_L2_BLOCK) {
            uint64_t* l1 = (uint64_t*)(tt_l0[l0_idx] & ~0xfffULL);
            if (!l1) {
                l1 = alloc_page_table();
                tt_l0[l0_idx] = (uint64_t)l1 | TT_TYPE_TABLE;
            }

            uint64_t* l2 = (uint64_t*)(l1[l1_idx] & ~0xfffULL);
            if (!l2) {
                l2 = alloc_page_table();
                l1[l1_idx] = (uint64_t)l2 | TT_TYPE_TABLE;
            }

            l2[l2_idx] = phys | attr;
            virt += TT_L2_BLOCK;
            phys += TT_L2_BLOCK;
            size -= TT_L2_BLOCK;
        } else {
            uint64_t* l1 = (uint64_t*)(tt_l0[l0_idx] & ~0xfffULL);
            if (!l1) {
                l1 = alloc_page_table();
                tt_l0[l0_idx] = (uint64_t)l1 | TT_TYPE_TABLE;
            }

            uint64_t* l2 = (uint64_t*)(l1[l1_idx] & ~0xfffULL);
            if (!l2) {
                l2 = alloc_page_table();
                l1[l1_idx] = (uint64_t)l2 | TT_TYPE_TABLE;
            }

            uint64_t* l3 = (uint64_t*)(l2[l2_idx] & ~0xfffULL);
            if (!l3) {
                l3 = alloc_page_table();
                l2[l2_idx] = (uint64_t)l3 | TT_TYPE_TABLE;
            }

            l3[l3_idx] = phys | TT_TYPE_PAGE | attr;
            virt += TT_L3_PAGE;
            phys += TT_L3_PAGE;
            size -= TT_L3_PAGE;
        }
    }
}

#pragma mark Initial page table setup

static void setup_boot_page_tables(uint64_t kernel_phys_base, uint64_t kernel_virt_base,
                                    uint64_t kernel_size __attribute__((unused)),
                                    uint64_t dtb_phys, uint64_t dtb_size)
{
    for (int i = 0; i < 512; i++)
        tt_l0[i] = 0;

    uint64_t mair = 0;
    mair |= (0x00ULL << 0);   /* Attr0: Device-nGnRnE */
    mair |= (0xffULL << 8);   /* Attr1: Normal, WB, RA, WA */
    mair |= (0x44ULL << 16);  /* Attr2: Normal, WT, RA, WA */
    mair |= (0x04ULL << 32);  /* Attr4: Device-nGnRE */
    write_mair(mair);

    uint64_t tcr = (0x00ULL << 37) |  /* IPS: 32-bit PA (0=32/4GB) */
                   (0x00ULL << 32) |  /* TG0: 4KB */
                   (0x00 << 30) |  /* SH0: Non-shareable */
                   (0x03 << 28) |  /* ORGN0: Normal memory, Outer Write-Back */
                   (0x03 << 26) |  /* IRGN0: Normal memory, Inner Write-Back */
                   (0x03 << 14) |  /* TG1: 4KB */
                   (0x03 << 12) |  /* SH1: Outer shareable */
                   (0x03 << 10) |  /* ORGN1 */
                   (0x03 << 8)  |  /* IRGN1 */
                   (0x10 << 0)  |  /* T0SZ: 2^48 VA space */
                   (0x10 << 16);   /* T1SZ: 2^48 VA space */
    write_tcr(tcr);

    map_range(kernel_virt_base & ~0x3fffffffULL, kernel_phys_base & ~0x3fffffffULL,
              0x40000000, TT_MT_NORMAL);

    if (dtb_phys && dtb_size)
        map_range(dtb_phys, dtb_phys, (dtb_size + 0xfff) & ~0xfffULL, TT_MT_NORMAL);

    map_range(0x00000000, 0x00000000, 0x10000000, TT_MT_DEVICE);
    map_range(0x10000000, 0x10000000, 0x10000000, TT_MT_DEVICE);

    write_ttbr0((uint64_t)tt_l0);
    write_ttbr1((uint64_t)tt_l0);
    isb();
}

static void enable_mmu(void) {
    uint64_t sctlr = read_sctlr();
    sctlr |= (1 << 0);   /* M: MMU enable */
    sctlr |= (1 << 2);   /* C: Cache enable */
    sctlr |= (1 << 12);  /* I: I-cache enable */
    sctlr |= (1 << 3);   /* SA: Stack Alignment Check */
    sctlr |= (1 << 11);  /* WXN: Write implies eXecute Never (for kernel) */
    write_sctlr(sctlr);
    isb();
}

static void __attribute__((unused)) disable_mmu(void) {
    uint64_t sctlr = read_sctlr();
    sctlr &= ~(1 << 0);
    write_sctlr(sctlr);
    isb();
}

#pragma mark Segmentation loading

static int load_segments(const uint8_t* kernel_base, struct XNUKernelInfo* info,
                         uint64_t load_phys_base)
{
    for (int i = 0; i < info->num_segments; i++) {
        struct XNUSegment* seg = &info->segments[i];

        if (strncmp(seg->segname, SEG_PAGEZERO, 16) == 0)
            continue;

        uint64_t seg_phys = load_phys_base + (seg->vmaddr - info->virt_base);
        uint64_t seg_virt = seg->vmaddr;
        uint64_t seg_size = seg->vmsize;

        printf("  Loading segment %s: phys=0x%llx virt=0x%llx size=0x%llx\n",
               seg->segname,
               (unsigned long long)seg_phys,
               (unsigned long long)seg_virt,
               (unsigned long long)seg_size);

        if (seg->filesize > 0 && seg->fileoff > 0 && kernel_base) {
            uint64_t copy_size = seg->filesize < seg_size ? seg->filesize : seg_size;
            const uint8_t* src = kernel_base + seg->fileoff;
            volatile uint8_t* dst = (volatile uint8_t*)(uintptr_t)seg_phys;

            for (uint64_t j = 0; j < copy_size; j++)
                dst[j] = src[j];

            uint64_t zero_size = seg_size - copy_size;
            if (zero_size > 0) {
                for (uint64_t j = 0; j < zero_size; j++)
                    dst[copy_size + j] = 0;
            }
        }

        map_range(seg_virt, seg_phys, seg_size,
                  (seg->initprot & 1) ? TT_MT_NORMAL : TT_MT_NORMAL);
    }

    return 0;
}

#pragma mark Main entry point

void vml_load_and_boot(const uint8_t* kernel_data, size_t kernel_size,
                        uint64_t dtb_phys, uint64_t dtb_size,
                        uint64_t phys_mem_base, uint64_t phys_mem_size)
{
    struct XNUKernelInfo info;

    printf("VMLdr: parsing XNU kernel binary...\n");
    if (xnu_kernel_parse(kernel_data, kernel_size, &info) != 0) {
        printf("VMLdr: failed to parse kernel\n");
        return;
    }

    xnu_kernel_dump_info(&info);

    uint64_t kernel_total_size = 0;
    uint64_t kernel_virt_base = info.virt_base;
    for (int i = 0; i < info.num_segments; i++) {
        struct XNUSegment* seg = &info.segments[i];
        if (strncmp(seg->segname, SEG_PAGEZERO, 16) == 0)
            continue;
        uint64_t seg_end = seg->vmaddr + seg->vmsize;
        kernel_total_size = (seg_end > kernel_total_size) ? seg_end : kernel_total_size;
    }
    kernel_total_size -= kernel_virt_base;
    kernel_total_size = (kernel_total_size + 0x3fffff) & ~0x3fffffULL;

    uint64_t kernel_phys_base = phys_mem_base + 0x100000;
    uint64_t kernel_load_base = kernel_phys_base;

    printf("VMLdr: kernel_virt_base=0x%llx kernel_phys_base=0x%llx total_size=0x%llx\n",
           (unsigned long long)kernel_virt_base,
           (unsigned long long)kernel_phys_base,
           (unsigned long long)kernel_total_size);

    if (kernel_total_size > MAX_KERNEL_SIZE) {
        printf("VMLdr: kernel too large (0x%llx > 0x%x)\n",
               (unsigned long long)kernel_total_size, MAX_KERNEL_SIZE);
        return;
    }

    printf("VMLdr: setting up boot page tables...\n");
    setup_boot_page_tables(kernel_phys_base, kernel_virt_base,
                            kernel_total_size, dtb_phys, dtb_size);

    printf("VMLdr: loading segments...\n");
    load_segments(kernel_data, &info, kernel_load_base);

    dsb();
    tlbi_all();

    struct xnu_boot_args boot_args;
    memset(&boot_args, 0, sizeof(boot_args));
    boot_args.revision = XNU_BOOT_ARGS_REVISION;
    boot_args.flags = dtb_phys ? XNU_BOOT_FLAG_DTB_AT_LOCAL : 0;
    boot_args.phys_mem_base = phys_mem_base;
    boot_args.phys_mem_size = phys_mem_size;
    boot_args.dtb_ptr = dtb_phys;
    boot_args.dtb_length = dtb_size;
    boot_args.kernel_phys_base = kernel_phys_base;
    boot_args.kernel_virt_base = kernel_virt_base;
    boot_args.kernel_entry = info.entry_point;
    boot_args.top_of_kernel_data = info.virt_base + kernel_total_size;
    boot_args.cpu_type = info.cputype;
    boot_args.cpu_subtype = info.cpusubtype;

    uint64_t boot_args_phys = phys_mem_base + 0x100;
    volatile uint32_t* boot_args_dst = (volatile uint32_t*)(uintptr_t)boot_args_phys;
    uint32_t* boot_args_src = (uint32_t*)&boot_args;
    for (size_t i = 0; i < sizeof(boot_args) / 4; i++)
        boot_args_dst[i] = boot_args_src[i];

    dsb();

    printf("VMLdr: enabling MMU...\n");
    enable_mmu();

    printf("VMLdr: jumping to kernel entry at 0x%llx...\n",
           (unsigned long long)info.entry_point);

    xnu_kernel_entry(info.entry_point,
                     boot_args_phys,
                     dtb_phys,
                     0);
}
