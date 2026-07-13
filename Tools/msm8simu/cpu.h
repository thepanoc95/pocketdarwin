#pragma once
#include <stdint.h>
#include <stdbool.h>

#define ARM_REG_COUNT 16
#define ARM_REG_SP 13
#define ARM_REG_LR 14
#define ARM_REG_PC 15

/* CPSR bits */
#define CPSR_N (1u << 31)
#define CPSR_Z (1u << 30)
#define CPSR_C (1u << 29)
#define CPSR_V (1u << 28)
#define CPSR_I (1u << 7)
#define CPSR_F (1u << 6)
#define CPSR_T (1u << 5)
#define CPSR_MODE_MASK 0x1F
#define CPSR_MODE_USR 0x10
#define CPSR_MODE_FIQ 0x11
#define CPSR_MODE_IRQ 0x12
#define CPSR_MODE_SVC 0x13
#define CPSR_MODE_ABT 0x17
#define CPSR_MODE_UND 0x1B
#define CPSR_MODE_SYS 0x1F

/* CP15 registers we emulate */
#define CP15_SCTLR   0
#define CP15_ACTLR   1
#define CP15_TTBR0   2
#define CP15_TTBR1   3
#define CP15_TTBCR   4
#define CP15_DACR    5
#define CP15_DFSR    6
#define CP15_DFAR    7
#define CP15_IFSR    8
#define CP15_IFAR    9
#define CP15_TLBTR   10
#define CP15_CIDR    11
#define CP15_TLR     12
#define CP15_HTTLBR  13
#define CP15_CONTEXTIDR 14
#define CP15_FCSEIDR 15
#define CP15_TTBR0_2 16
#define CP15_TTBR1_2 17
#define CP15_TTBCR_2 18
#define CP15_CBAR    19
#define CP15_MAIR0   20
#define CP15_MAIR1   21
#define CP15_AIRCR   22

/* Memory read/write callbacks */
typedef uint8_t  (*mem_read8_fn)  (uint32_t addr, void *userdata);
typedef void     (*mem_write8_fn) (uint32_t addr, uint8_t val, void *userdata);
typedef uint16_t (*mem_read16_fn)(uint32_t addr, void *userdata);
typedef void     (*mem_write16_fn)(uint32_t addr, uint16_t val, void *userdata);
typedef uint32_t (*mem_read32_fn)(uint32_t addr, void *userdata);
typedef void     (*mem_write32_fn)(uint32_t addr, uint32_t val, void *userdata);

typedef struct {
    /* General purpose registers */
    uint32_t regs[ARM_REG_COUNT];

    /* Current/Saved program status registers */
    uint32_t cpsr;
    uint32_t spsr;

    /* Banked registers */
    uint32_t sp_svc, lr_svc;
    uint32_t sp_irq, lr_irq;
    uint32_t sp_abt, lr_abt;
    uint32_t sp_und, lr_und;
    uint32_t sp_fiq, lr_fiq;
    uint32_t r8_fiq, r9_fiq, r10_fiq, r11_fiq, r12_fiq;

    /* CP15 coprocessor registers */
    uint32_t cp15[32];

    /* Memory access callbacks */
    void *mem_userdata;
    mem_read8_fn   mem_read8;
    mem_write8_fn  mem_write8;
    mem_read16_fn  mem_read16;
    mem_write16_fn mem_write16;
    mem_read32_fn  mem_read32;
    mem_write32_fn mem_write32;

    /* Execution state */
    bool halted;
    uint64_t cycle_count;
    uint32_t exception_base;

    /* Trace */
    bool trace_enabled;
    uint32_t trace_addr_start;
    uint32_t trace_addr_end;
} arm_cpu_t;

void cpu_init(arm_cpu_t *cpu);
void cpu_reset(arm_cpu_t *cpu, uint32_t entry_point);
uint32_t cpu_step(arm_cpu_t *cpu);
uint32_t cpu_get_lr(arm_cpu_t *cpu);
void cpu_set_lr(arm_cpu_t *cpu, uint32_t val);
void cpu_dump_state(arm_cpu_t *cpu);
void cpu_dump_disasm(arm_cpu_t *cpu, uint32_t count);

/* Register access */
static inline uint32_t cpu_get_reg(arm_cpu_t *cpu, int reg) {
    if (reg == ARM_REG_PC) return cpu->regs[reg] + 8;
    return cpu->regs[reg];
}

static inline void cpu_set_reg(arm_cpu_t *cpu, int reg, uint32_t val) {
    if (reg == ARM_REG_PC) {
        cpu->regs[reg] = val - 8; /* compensate for pipeline */
    } else {
        cpu->regs[reg] = val;
    }
}

static inline bool cpu_condition_met(arm_cpu_t *cpu, uint32_t cond) {
    uint32_t n = (cpu->cpsr >> 31) & 1;
    uint32_t z = (cpu->cpsr >> 30) & 1;
    uint32_t c = (cpu->cpsr >> 29) & 1;
    uint32_t v = (cpu->cpsr >> 28) & 1;

    switch (cond) {
    case 0x0: return z == 1;               /* EQ */
    case 0x1: return z == 0;               /* NE */
    case 0x2: return c == 1;               /* CS/HS */
    case 0x3: return c == 0;               /* CC/LO */
    case 0x4: return n == 1;               /* MI */
    case 0x5: return n == 0;               /* PL */
    case 0x6: return v == 1;               /* VS */
    case 0x7: return v == 0;               /* VC */
    case 0x8: return c == 1 && z == 0;     /* HI */
    case 0x9: return c == 0 || z == 1;     /* LS */
    case 0xA: return n == v;               /* GE */
    case 0xB: return n != v;               /* LT */
    case 0xC: return z == 0 && n == v;     /* GT */
    case 0xD: return z == 1 || n != v;     /* LE */
    case 0xE: return 1;                    /* AL */
    case 0xF: return 0;                    /* NV (unpredictable in ARMv7) */
    default:  return 0;
    }
}
