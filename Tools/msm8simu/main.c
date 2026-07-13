#include "cpu.h"
#include "platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_usage(const char *prog) {
    fprintf(stderr, "MSM8916 Simulator - KickStart Bootloader Debug Tool\n\n");
    fprintf(stderr, "Usage: %s [options] <elf-file>\n\n", prog);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -t          Enable instruction tracing\n");
    fprintf(stderr, "  -n <count>  Number of instructions to execute (default: unlimited)\n");
    fprintf(stderr, "  -v          Verbose output\n");
    fprintf(stderr, "  -d          Dump CPU state on exit\n");
    fprintf(stderr, "  -g          Wait for GDB connection on port 1234 (TODO)\n");
    fprintf(stderr, "  -h          Show this help\n");
}

static uint32_t trace_start = 0x80000000;
static uint32_t trace_end   = 0x80100000;

int main(int argc, char **argv) {
    const char *elf_path = NULL;
    bool trace = false;
    bool dump_state = false;
    uint64_t max_insns = 0;
    bool verbose = false;

    int i;
    for (i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            elf_path = argv[i];
            break;
        }
        if (strcmp(argv[i], "-t") == 0) {
            trace = true;
        } else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            max_insns = strtoull(argv[++i], NULL, 0);
        } else if (strcmp(argv[i], "-v") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "-d") == 0) {
            dump_state = true;
        } else if (strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }

    if (!elf_path) {
        print_usage(argv[0]);
        return 1;
    }

    /* Initialize CPU */
    arm_cpu_t cpu;
    cpu_init(&cpu);
    cpu.trace_enabled = trace;
    cpu.trace_addr_start = trace_start;
    cpu.trace_addr_end = trace_end;

    /* Initialize platform */
    msm8916_platform_t plat;
    platform_init(&plat, &cpu);

    /* Load ELF */
    uint32_t entry;
    if (platform_load_elf(&plat, elf_path, &entry) != 0) {
        fprintf(stderr, "[MAIN] Failed to load ELF\n");
        platform_destroy(&plat);
        return 1;
    }

    fprintf(stderr, "[MAIN] Resetting CPU to entry point 0x%08X\n", entry);
    cpu_reset(&cpu, entry);

    /* Also set SP to a reasonable value in case the bootloader's
     * initial stack setup hasn't run yet */
    cpu.regs[ARM_REG_SP] = MSM8916_RAM_BASE + MSM8916_RAM_SIZE - 0x1000;

    fprintf(stderr, "[MAIN] Starting execution...\n");
    fprintf(stderr, "[MAIN] Press Ctrl+C to stop\n\n");

    /* Execute */
    uint64_t insn_count = 0;
    uint32_t last_pc = 0;
    uint32_t same_pc_count = 0;

    while (!cpu.halted) {
        uint32_t pc_before = cpu.regs[ARM_REG_PC] + 8;

        cpu_step(&cpu);
        insn_count++;

        /* Detect infinite loop (PC stuck) */
        if (cpu.regs[ARM_REG_PC] + 8 == last_pc) {
            same_pc_count++;
            if (same_pc_count > 100) {
                fprintf(stderr, "\n[MAIN] Detected infinite loop at PC=0x%08X, stopping.\n", pc_before);
                break;
            }
        } else {
            same_pc_count = 0;
        }
        last_pc = cpu.regs[ARM_REG_PC] + 8;

        /* Verbose trace */
        if (trace) {
            if (pc_before >= trace_start && pc_before < trace_end) {
                uint32_t insn_val = cpu.mem_read32(pc_before, cpu.mem_userdata);
                fprintf(stderr, "[TRACE] 0x%08X: 0x%08X  R0=0x%08X R1=0x%08X SP=0x%08X LR=0x%08X CPSR=0x%08X\n",
                        pc_before, insn_val,
                        cpu.regs[0], cpu.regs[1], cpu.regs[ARM_REG_SP],
                        cpu_get_lr(&cpu), cpu.cpsr);
            }
        }

        /* Progress indicator every 1M instructions */
        if ((insn_count % 1000000) == 0) {
            fprintf(stderr, "[MAIN] Executed %lu instructions, PC=0x%08X\n",
                    (unsigned long)insn_count, cpu.regs[ARM_REG_PC] + 8);
        }

        /* Instruction limit */
        if (max_insns > 0 && insn_count >= max_insns) {
            fprintf(stderr, "\n[MAIN] Reached instruction limit (%lu)\n", (unsigned long)max_insns);
            break;
        }
    }

    fprintf(stderr, "\n[MAIN] Execution stopped after %lu instructions\n", (unsigned long)insn_count);
    fprintf(stderr, "[MAIN] Final PC: 0x%08X\n", cpu.regs[ARM_REG_PC] + 8);

    if (dump_state) {
        cpu_dump_state(&cpu);
    }

    platform_destroy(&plat);
    return 0;
}
