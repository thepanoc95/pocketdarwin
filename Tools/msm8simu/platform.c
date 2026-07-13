#include "platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>

static uint8_t platform_read8(uint32_t addr, void *ud) {
    msm8916_platform_t *p = (msm8916_platform_t *)ud;

    if (addr >= p->ram_base && addr < p->ram_base + p->ram_size) {
        return p->ram[addr - p->ram_base];
    }

    /* UART BLSP1_UART2 */
    if (addr >= MSM8916_BLSP1_UART2 && addr < MSM8916_BLSP1_UART2 + MSM8916_BLSP1_UART2_SIZE) {
        uint32_t off = addr - MSM8916_BLSP1_UART2;
        switch (off) {
        case 0x00: /* UART data register */
            if (p->uart_fifo_head != p->uart_fifo_tail) {
                uint8_t ch = p->uart_fifo[p->uart_fifo_tail];
                p->uart_fifo_tail = (p->uart_fifo_tail + 1) % 64;
                return ch;
            }
            return 0;
        case 0x08: /* UART status */
            /* Bit 4 = TX FIFO not full (always ready for us) */
            return 0x10;
        case 0x04: /* UART ISR (interrupt status) */
            return 0;
        default:
            return 0;
        }
    }

    /* GIC Distributor */
    if (addr >= MSM8916_GIC_DIST && addr < MSM8916_GIC_DIST + 0x1000) {
        return p->gic_dist[addr - MSM8916_GIC_DIST];
    }

    /* GIC CPU Interface */
    if (addr >= MSM8916_GIC_CPU && addr < MSM8916_GIC_CPU + 0x1000) {
        return p->gic_cpu[addr - MSM8916_GIC_CPU];
    }

    /* Clock controller */
    if (addr >= MSM8916_CLK_CTL && addr < MSM8916_CLK_CTL + MSM8916_CLK_CTL_SIZE) {
        return p->clk_ctl[(addr - MSM8916_CLK_CTL) % 0x1000];
    }

    /* GPIO */
    if (addr >= MSM8916_GPIO && addr < MSM8916_GPIO + MSM8916_GPIO_SIZE) {
        return p->gpio[addr - MSM8916_GPIO];
    }

    /* Watchdog - return idle state */
    if (addr >= MSM8916_WDT_BASE && addr < MSM8916_WDT_BASE + 0x1000) {
        uint32_t off = addr - MSM8916_WDT_BASE;
        /* WDT_STATUS at offset 0 */
        if (off == 0x08) return 0; /* not expired */
        return 0;
    }

    /* DLOAD mode - all zeros */
    if (addr >= MSM8916_DLOAD_MSG && addr < MSM8916_DLOAD_MSG + MSM8916_DLOAD_MSG_SIZE) {
        return 0;
    }

    /* Timer */
    if (addr >= MSM8916_TIMER_BASE && addr < MSM8916_TIMER_BASE + 0x1000) {
        return 0;
    }

    /* SPMI */
    if (addr >= MSM8916_SPMI_BASE && addr < MSM8916_SPMI_BASE + 0x1000) {
        return 0;
    }

    /* Unmapped - return 0, don't crash */
    return 0;
}

static uint16_t platform_read16(uint32_t addr, void *ud) {
    uint8_t lo = platform_read8(addr, ud);
    uint8_t hi = platform_read8(addr + 1, ud);
    return lo | (hi << 8);
}

static uint32_t platform_read32(uint32_t addr, void *ud) {
    uint8_t b0 = platform_read8(addr, ud);
    uint8_t b1 = platform_read8(addr + 1, ud);
    uint8_t b2 = platform_read8(addr + 2, ud);
    uint8_t b3 = platform_read8(addr + 3, ud);
    return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
}

static void platform_write8(uint32_t addr, uint8_t val, void *ud) {
    msm8916_platform_t *p = (msm8916_platform_t *)ud;

    if (addr >= p->ram_base && addr < p->ram_base + p->ram_size) {
        p->ram[addr - p->ram_base] = val;
        return;
    }

    /* UART BLSP1_UART2 */
    if (addr >= MSM8916_BLSP1_UART2 && addr < MSM8916_BLSP1_UART2 + MSM8916_BLSP1_UART2_SIZE) {
        uint32_t off = addr - MSM8916_BLSP1_UART2;
        switch (off) {
        case 0x00: /* UART data register - output */
            if (val == '\r') return; /* skip CR */
            fputc(val, stdout);
            fflush(stdout);
            return;
        case 0x04: /* UART_IMR */
        case 0x08: /* UART_SR (read-only, ignore writes) */
        case 0x0C: /* UART_CR */
            return;
        default:
            return;
        }
    }

    /* GIC Distributor */
    if (addr >= MSM8916_GIC_DIST && addr < MSM8916_GIC_DIST + 0x1000) {
        p->gic_dist[addr - MSM8916_GIC_DIST] = val;
        return;
    }

    /* GIC CPU Interface */
    if (addr >= MSM8916_GIC_CPU && addr < MSM8916_GIC_CPU + 0x1000) {
        p->gic_cpu[addr - MSM8916_GIC_CPU] = val;
        return;
    }

    /* Clock controller */
    if (addr >= MSM8916_CLK_CTL && addr < MSM8916_CLK_CTL + MSM8916_CLK_CTL_SIZE) {
        uint32_t off = (addr - MSM8916_CLK_CTL) % 0x1000;
        p->clk_ctl[off] = val;
        return;
    }

    /* GPIO */
    if (addr >= MSM8916_GPIO && addr < MSM8916_GPIO + MSM8916_GPIO_SIZE) {
        p->gpio[addr - MSM8916_GPIO] = val;
        return;
    }

    /* Watchdog */
    if (addr >= MSM8916_WDT_BASE && addr < MSM8916_WDT_BASE + 0x1000) {
        return; /* Ignore watchdog writes */
    }

    /* SPMI */
    if (addr >= MSM8916_SPMI_BASE && addr < MSM8916_SPMI_BASE + 0x1000) {
        return;
    }

    /* Unmapped - ignore */
}

static void platform_write16(uint32_t addr, uint16_t val, void *ud) {
    platform_write8(addr, val & 0xFF, ud);
    platform_write8(addr + 1, (val >> 8) & 0xFF, ud);
}

static void platform_write32(uint32_t addr, uint32_t val, void *ud) {
    platform_write8(addr, val & 0xFF, ud);
    platform_write8(addr + 1, (val >> 8) & 0xFF, ud);
    platform_write8(addr + 2, (val >> 16) & 0xFF, ud);
    platform_write8(addr + 3, (val >> 24) & 0xFF, ud);
}

void platform_init(msm8916_platform_t *plat, arm_cpu_t *cpu) {
    memset(plat, 0, sizeof(*plat));

    /* Allocate RAM at MSM8916 base */
    plat->ram_base = MSM8916_RAM_BASE;
    plat->ram_size = MSM8916_RAM_SIZE;
    plat->ram = (uint8_t *)calloc(1, plat->ram_size);
    if (!plat->ram) {
        fprintf(stderr, "[PLATFORM] Failed to allocate %u bytes of RAM\n", plat->ram_size);
        exit(1);
    }

    plat->cpu = cpu;

    /* Wire up memory callbacks */
    cpu->mem_userdata = plat;
    cpu->mem_read8    = platform_read8;
    cpu->mem_write8   = platform_write8;
    cpu->mem_read16   = platform_read16;
    cpu->mem_write16  = platform_write16;
    cpu->mem_read32   = platform_read32;
    cpu->mem_write32  = platform_write32;

    fprintf(stderr, "[PLATFORM] MSM8916 initialized\n");
    fprintf(stderr, "[PLATFORM] RAM: 0x%08X - 0x%08X (%u MB)\n",
            plat->ram_base,
            plat->ram_base + plat->ram_size,
            plat->ram_size / (1024 * 1024));
    fprintf(stderr, "[PLATFORM] UART: BLSP1_UART2 @ 0x%08X\n", MSM8916_BLSP1_UART2);
}

void platform_destroy(msm8916_platform_t *plat) {
    if (plat->ram) {
        free(plat->ram);
        plat->ram = NULL;
    }
}

int platform_load_elf(msm8916_platform_t *plat, const char *path, uint32_t *entry_out) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "[PLATFORM] Cannot open ELF: %s\n", path);
        return -1;
    }

    /* Read ELF header */
    Elf32_Ehdr ehdr;
    if (fread(&ehdr, sizeof(ehdr), 1, f) != 1) {
        fprintf(stderr, "[PLATFORM] Failed to read ELF header\n");
        fclose(f);
        return -1;
    }

    /* Validate ELF magic */
    if (ehdr.e_ident[EI_MAG0] != ELFMAG0 || ehdr.e_ident[EI_MAG1] != ELFMAG1 ||
        ehdr.e_ident[EI_MAG2] != ELFMAG2 || ehdr.e_ident[EI_MAG3] != ELFMAG3) {
        fprintf(stderr, "[PLATFORM] Not a valid ELF file\n");
        fclose(f);
        return -1;
    }

    if (ehdr.e_ident[EI_CLASS] != ELFCLASS32) {
        fprintf(stderr, "[PLATFORM] Not a 32-bit ELF\n");
        fclose(f);
        return -1;
    }

    if (ehdr.e_machine != EM_ARM) {
        fprintf(stderr, "[PLATFORM] Not an ARM ELF (machine=%d)\n", ehdr.e_machine);
        fclose(f);
        return -1;
    }

    fprintf(stderr, "[PLATFORM] ELF entry point: 0x%08X\n", ehdr.e_entry);
    *entry_out = ehdr.e_entry;

    /* Read program headers */
    for (int i = 0; i < ehdr.e_phnum; i++) {
        Elf32_Phdr phdr;
        fseek(f, ehdr.e_phoff + i * ehdr.e_phentsize, SEEK_SET);
        if (fread(&phdr, sizeof(phdr), 1, f) != 1) {
            fprintf(stderr, "[PLATFORM] Failed to read program header %d\n", i);
            continue;
        }

        if (phdr.p_type != PT_LOAD) continue;

        fprintf(stderr, "[PLATFORM] LOAD segment %d: vaddr=0x%08X paddr=0x%08X filesz=0x%08X memsz=0x%08X flags=",
                i, phdr.p_vaddr, phdr.p_paddr, phdr.p_filesz, phdr.p_memsz);
        if (phdr.p_flags & PF_R) fprintf(stderr, "R");
        if (phdr.p_flags & PF_W) fprintf(stderr, "W");
        if (phdr.p_flags & PF_X) fprintf(stderr, "X");
        fprintf(stderr, "\n");

        uint32_t load_addr = phdr.p_paddr ? phdr.p_paddr : phdr.p_vaddr;

        /* Check if segment fits in RAM */
        if (load_addr < plat->ram_base || load_addr + phdr.p_memsz > plat->ram_base + plat->ram_size) {
            fprintf(stderr, "[PLATFORM] WARNING: Segment at 0x%08X (size 0x%08X) outside RAM region\n",
                    load_addr, phdr.p_memsz);

            /* If the address is within the physical address range but above our RAM, we need to handle it */
            /* For now, allocate dynamically if needed */
            fprintf(stderr, "[PLATFORM] Attempting direct memory write for segment at 0x%08X\n", load_addr);

            /* Check if it's within range we can address in our RAM */
            if (load_addr >= plat->ram_base && load_addr + phdr.p_memsz <= plat->ram_base + plat->ram_size) {
                uint32_t offset = load_addr - plat->ram_base;
                /* Read file data */
                if (phdr.p_filesz > 0) {
                    fseek(f, phdr.p_offset, SEEK_SET);
                    fread(plat->ram + offset, phdr.p_filesz, 1, f);
                }
                /* Zero BSS */
                if (phdr.p_memsz > phdr.p_filesz) {
                    memset(plat->ram + offset + phdr.p_filesz, 0, phdr.p_memsz - phdr.p_filesz);
                }
            }
            continue;
        }

        uint32_t offset = load_addr - plat->ram_base;

        /* Load segment data */
        if (phdr.p_filesz > 0) {
            fseek(f, phdr.p_offset, SEEK_SET);
            fread(plat->ram + offset, phdr.p_filesz, 1, f);
        }

        /* Zero BSS (memsz > filesz) */
        if (phdr.p_memsz > phdr.p_filesz) {
            memset(plat->ram + offset + phdr.p_filesz, 0, phdr.p_memsz - phdr.p_filesz);
        }
    }

    fclose(f);
    return 0;
}
