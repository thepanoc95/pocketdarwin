#include "cpu.h"
#include <stdio.h>
#include <string.h>

void cpu_init(arm_cpu_t *cpu) {
    memset(cpu, 0, sizeof(*cpu));
    cpu->exception_base = 0;
}

void cpu_reset(arm_cpu_t *cpu, uint32_t entry_point) {
    memset(cpu->regs, 0, sizeof(cpu->regs));
    cpu->cpsr = CPSR_MODE_SVC | CPSR_I | CPSR_F;
    cpu->spsr = 0;
    cpu->regs[ARM_REG_PC] = entry_point - 8;
    cpu->halted = false;
    cpu->cycle_count = 0;
    memset(cpu->cp15, 0, sizeof(cpu->cp15));
    cpu->cp15[CP15_SCTLR] = 0x00C50078;
}

static inline uint32_t cpu_get_spsr(arm_cpu_t *cpu) {
    return cpu->spsr;
}

static inline void cpu_set_cpsr_modebits(arm_cpu_t *cpu, uint32_t mode) {
    cpu->cpsr = (cpu->cpsr & ~CPSR_MODE_MASK) | (mode & CPSR_MODE_MASK);
}

/* Barrel shifter */
static uint32_t barrel_shift(arm_cpu_t *cpu, uint32_t val, uint32_t type, uint32_t amount, bool *carry_out) {
    uint32_t result;
    uint32_t old_c = (cpu->cpsr >> 29) & 1;

    if (amount == 0 && type != 3) {
        result = val; *carry_out = old_c; return result;
    }

    switch (type) {
    case 0: /* LSL */
        if (amount == 0) { result = val; *carry_out = old_c; }
        else if (amount < 32) { result = val << amount; *carry_out = (val >> (32 - amount)) & 1; }
        else if (amount == 32) { result = 0; *carry_out = val & 1; }
        else { result = 0; *carry_out = 0; }
        break;
    case 1: /* LSR */
        if (amount == 0) { result = val; *carry_out = old_c; }
        else if (amount < 32) { result = val >> amount; *carry_out = (val >> (amount - 1)) & 1; }
        else if (amount == 32) { result = 0; *carry_out = (val >> 31) & 1; }
        else { result = 0; *carry_out = 0; }
        break;
    case 2: /* ASR */
        if (amount == 0) { result = val; *carry_out = old_c; }
        else if (amount < 32) { result = (int32_t)val >> amount; *carry_out = (val >> (amount - 1)) & 1; }
        else { result = (int32_t)val >> 31; *carry_out = (val >> 31) & 1; }
        break;
    case 3: /* ROR/RRX */
        if (amount == 0) {
            result = (old_c << 31) | (val >> 1);
            *carry_out = val & 1;
        } else {
            amount &= 31;
            if (amount == 0) { result = val; *carry_out = old_c; }
            else { result = (val >> amount) | (val << (32 - amount)); *carry_out = (result >> 31) & 1; }
        }
        break;
    default:
        result = val; *carry_out = old_c; break;
    }
    return result;
}

static inline void cpu_update_flags_nz(arm_cpu_t *cpu, uint32_t result) {
    cpu->cpsr = (cpu->cpsr & ~(CPSR_N | CPSR_Z)) |
                (result & 0x80000000 ? CPSR_N : 0) |
                (result == 0 ? CPSR_Z : 0);
}

static inline void cpu_set_carry(arm_cpu_t *cpu, bool carry) {
    cpu->cpsr = (cpu->cpsr & ~CPSR_C) | (carry ? CPSR_C : 0);
}

static inline void cpu_set_overflow(arm_cpu_t *cpu, bool overflow) {
    cpu->cpsr = (cpu->cpsr & ~CPSR_V) | (overflow ? CPSR_V : 0);
}

/* Memory helpers */
static inline uint32_t mem_read32(arm_cpu_t *cpu, uint32_t addr) {
    return cpu->mem_read32(addr, cpu->mem_userdata);
}

static inline uint16_t mem_read16(arm_cpu_t *cpu, uint32_t addr) {
    return cpu->mem_read16(addr, cpu->mem_userdata);
}

static inline uint8_t mem_read8(arm_cpu_t *cpu, uint32_t addr) {
    return cpu->mem_read8(addr, cpu->mem_userdata);
}

static inline void mem_write32(arm_cpu_t *cpu, uint32_t addr, uint32_t val) {
    cpu->mem_write32(addr, val, cpu->mem_userdata);
}

static inline void mem_write16(arm_cpu_t *cpu, uint32_t addr, uint16_t val) {
    cpu->mem_write16(addr, val, cpu->mem_userdata);
}

static inline void mem_write8(arm_cpu_t *cpu, uint32_t addr, uint8_t val) {
    cpu->mem_write8(addr, val, cpu->mem_userdata);
}

/* Banked register access */
static uint32_t cpu_get_reg_banked(arm_cpu_t *cpu, int reg) {
    uint32_t mode = cpu->cpsr & CPSR_MODE_MASK;
    if (reg >= 8 && reg <= 12 && mode == CPSR_MODE_FIQ) {
        switch (reg) {
        case 8:  return cpu->r8_fiq;
        case 9:  return cpu->r9_fiq;
        case 10: return cpu->r10_fiq;
        case 11: return cpu->r11_fiq;
        case 12: return cpu->r12_fiq;
        }
    }
    if (reg == ARM_REG_SP) {
        switch (mode) {
        case CPSR_MODE_IRQ: return cpu->sp_irq;
        case CPSR_MODE_SVC: return cpu->sp_svc;
        case CPSR_MODE_ABT: return cpu->sp_abt;
        case CPSR_MODE_UND: return cpu->sp_und;
        case CPSR_MODE_FIQ: return cpu->sp_fiq;
        default: return cpu->regs[reg];
        }
    }
    if (reg == ARM_REG_LR) {
        switch (mode) {
        case CPSR_MODE_IRQ: return cpu->lr_irq;
        case CPSR_MODE_SVC: return cpu->lr_svc;
        case CPSR_MODE_ABT: return cpu->lr_abt;
        case CPSR_MODE_UND: return cpu->lr_und;
        case CPSR_MODE_FIQ: return cpu->lr_fiq;
        default: return cpu->regs[reg];
        }
    }
    return cpu->regs[reg];
}

static void cpu_set_reg_banked(arm_cpu_t *cpu, int reg, uint32_t val) {
    uint32_t mode = cpu->cpsr & CPSR_MODE_MASK;
    if (reg >= 8 && reg <= 12 && mode == CPSR_MODE_FIQ) {
        switch (reg) {
        case 8:  cpu->r8_fiq = val; return;
        case 9:  cpu->r9_fiq = val; return;
        case 10: cpu->r10_fiq = val; return;
        case 11: cpu->r11_fiq = val; return;
        case 12: cpu->r12_fiq = val; return;
        }
    }
    if (reg == ARM_REG_SP) {
        switch (mode) {
        case CPSR_MODE_IRQ: cpu->sp_irq = val; return;
        case CPSR_MODE_SVC: cpu->sp_svc = val; return;
        case CPSR_MODE_ABT: cpu->sp_abt = val; return;
        case CPSR_MODE_UND: cpu->sp_und = val; return;
        case CPSR_MODE_FIQ: cpu->sp_fiq = val; return;
        default: cpu->regs[reg] = val; return;
        }
    }
    if (reg == ARM_REG_LR) {
        switch (mode) {
        case CPSR_MODE_IRQ: cpu->lr_irq = val; return;
        case CPSR_MODE_SVC: cpu->lr_svc = val; return;
        case CPSR_MODE_ABT: cpu->lr_abt = val; return;
        case CPSR_MODE_UND: cpu->lr_und = val; return;
        case CPSR_MODE_FIQ: cpu->lr_fiq = val; return;
        default: cpu->regs[reg] = val; return;
        }
    }
    cpu->regs[reg] = val;
}

uint32_t cpu_get_lr(arm_cpu_t *cpu) {
    return cpu_get_reg_banked(cpu, ARM_REG_LR);
}

void cpu_set_lr(arm_cpu_t *cpu, uint32_t val) {
    cpu_set_reg_banked(cpu, ARM_REG_LR, val);
}

/* Decode immediate operand (rotate right) */
static uint32_t decode_imm(uint32_t insn, bool *carry) {
    uint32_t imm8 = insn & 0xFF;
    uint32_t rot = ((insn >> 8) & 0xF) * 2;
    if (rot == 0) return imm8;
    uint32_t result = (imm8 >> rot) | (imm8 << (32 - rot));
    *carry = (result >> 31) & 1;
    return result;
}

/* Decode register-shifted operand */
static uint32_t decode_reg_shift(arm_cpu_t *cpu, uint32_t insn, bool *carry) {
    uint32_t rm = insn & 0xF;
    uint32_t shift_type = (insn >> 5) & 0x3;
    uint32_t rs = (insn >> 8) & 0xF;
    uint32_t amount = cpu->regs[rs] & 0xFF;

    if (amount == 0) { *carry = (cpu->cpsr >> 29) & 1; return cpu->regs[rm]; }
    if (shift_type == 0 && amount == 0) { *carry = (cpu->cpsr >> 29) & 1; return cpu->regs[rm]; }

    if (amount < 32) return barrel_shift(cpu, cpu->regs[rm], shift_type, amount, carry);
    if (amount == 32) {
        switch (shift_type) {
        case 0: *carry = cpu->regs[rm] & 1; return 0;
        case 1: *carry = (cpu->regs[rm] >> 31) & 1; return 0;
        case 2: *carry = (cpu->regs[rm] >> 31) & 1; return (int32_t)cpu->regs[rm] >> 31;
        default: return barrel_shift(cpu, cpu->regs[rm], shift_type, amount & 31, carry);
        }
    }
    *carry = 0; return 0;
}

/* Decode immediate-shifted operand */
static uint32_t decode_imm_shift(arm_cpu_t *cpu, uint32_t insn, bool *carry) {
    uint32_t rm = insn & 0xF;
    uint32_t shift_type = (insn >> 5) & 0x3;
    uint32_t shift_amount = (insn >> 7) & 0x1F;

    if (shift_amount == 0 && shift_type == 0) {
        *carry = (cpu->cpsr >> 29) & 1;
        return cpu->regs[rm];
    }
    return barrel_shift(cpu, cpu->regs[rm], shift_type, shift_amount, carry);
}

/* Execute one ARM instruction */
uint32_t cpu_step(arm_cpu_t *cpu) {
    if (cpu->halted) return 0;

    uint32_t pc = cpu->regs[ARM_REG_PC] + 8;
    uint32_t insn = mem_read32(cpu, pc);
    cpu->regs[ARM_REG_PC] += 4;
    cpu->cycle_count++;

    /* Condition check - 0xE and 0xF are "always execute" (0xF = ARMv7 unconditional) */
    uint32_t cond = (insn >> 28) & 0xF;
    if (cond < 0xE && !cpu_condition_met(cpu, cond)) return 1;

    uint32_t op0 = (insn >> 25) & 0x7;

    /* ================================================================ */
    /* Unconditional hint instructions (DSB, DMB, ISB, SEV, WFI, etc.)  */
    /* Encoding: cond=0xF, bits[27:24]=0101                             */
    /* ================================================================ */
    if (cond == 0xF && op0 == 2) {
        uint32_t bits23_20 = (insn >> 20) & 0xF;
        if (bits23_20 == 0x7) {
            uint32_t bits7_4 = (insn >> 4) & 0xF;
            switch (bits7_4) {
            case 0x1: /* NOP */
                return 1;
            case 0x2: /* WFI */
                /* Don't halt, just treat as NOP to keep executing */
                return 1;
            case 0x3: /* WFE */
                return 1;
            case 0x4: /* SEV */
                return 1;
            case 0x5: /* DMB */
                return 1;
            case 0x6: /* DSB */
                return 1;
            default: /* CLREX, ISB, etc. */
                return 1;
            }
        }
        /* CLREX: F57FF01F */
        if ((insn & 0xFFFFFFF0) == 0xF57FF010) return 1;
        /* SEVL */
        if ((insn & 0xFFFFFFF0) == 0xF57FF020) return 1;
    }

    /* ================================================================ */
    /* Data Processing / PSR Transfer / Multiply (op0 == 0 or 1)        */
    /* ================================================================ */
    if (op0 == 0 || op0 == 1) {
        uint32_t opcode = (insn >> 21) & 0xF;
        uint32_t s_flag = (insn >> 20) & 1;
        uint32_t rn_idx = (insn >> 16) & 0xF;
        uint32_t rd_idx = (insn >> 12) & 0xF;
        uint32_t i_flag = (insn >> 25) & 1;
        uint32_t bits7_4 = (insn >> 4) & 0xF;

        /* --- Multiply (op0==0, bits [7:4]=1001) --- */
        if (op0 == 0 && !i_flag && bits7_4 == 0x9 && (insn & 0x0FC00000) == 0x00000000) {
            uint32_t rd = (insn >> 16) & 0xF;
            uint32_t rn = (insn >> 12) & 0xF;
            uint32_t rs = (insn >> 8) & 0xF;
            uint32_t rm = insn & 0xF;
            uint32_t a = (insn >> 21) & 1;
            uint32_t s = (insn >> 20) & 1;
            uint32_t result = cpu->regs[rm] * cpu->regs[rs];
            if (a) result += cpu->regs[rn];
            cpu->regs[rd] = result;
            if (s) cpu_update_flags_nz(cpu, result);
            return 1;
        }

        /* --- Long multiply (op0==0, bits [7:4]=1001, bits [23:20] have U/A/S) --- */
        if (op0 == 0 && !i_flag && bits7_4 == 0x9 && (insn & 0x0F800000) == 0x00800000) {
            uint32_t rd_hi = (insn >> 16) & 0xF;
            uint32_t rd_lo = (insn >> 12) & 0xF;
            uint32_t rs = (insn >> 8) & 0xF;
            uint32_t rm = insn & 0xF;
            uint32_t u = (insn >> 22) & 1;
            uint32_t a = (insn >> 21) & 1;
            uint32_t s = (insn >> 20) & 1;
            if (u) {
                uint64_t res = (uint64_t)cpu->regs[rm] * (uint64_t)cpu->regs[rs];
                if (a) res += ((uint64_t)cpu->regs[rd_hi] << 32) | cpu->regs[rd_lo];
                cpu->regs[rd_lo] = (uint32_t)res;
                cpu->regs[rd_hi] = (uint32_t)(res >> 32);
            } else {
                int64_t res = (int64_t)(int32_t)cpu->regs[rm] * (int64_t)(int32_t)cpu->regs[rs];
                if (a) res += ((int64_t)cpu->regs[rd_hi] << 32) | cpu->regs[rd_lo];
                cpu->regs[rd_lo] = (uint32_t)res;
                cpu->regs[rd_hi] = (uint32_t)(res >> 32);
            }
            if (s) cpu_update_flags_nz(cpu, cpu->regs[rd_hi]);
            return 1;
        }

        /* --- Misc: MRS --- */
        if (!i_flag && (insn & 0xF1FF0000) == 0x010F0000) {
            uint32_t rd = rd_idx;
            bool spsr = (insn >> 22) & 1;
            cpu->regs[rd] = spsr ? cpu->spsr : cpu->cpsr;
            return 1;
        }

        /* --- Misc: MSR (register) --- */
        if (!i_flag && (insn & 0xF1FF0FF0) == 0x0129F000) {
            uint32_t field = (insn >> 16) & 0xFF;
            uint32_t val = cpu->regs[insn & 0xF];
            uint32_t mask = 0;
            if (field & 0x80) mask |= 0xFF000000;
            if (field & 0x40) mask |= 0x00FF0000;
            if (field & 0x20) mask |= 0x0000FF00;
            if (field & 0x10) mask |= 0x000000FF;
            bool spsr = (insn >> 22) & 1;
            if (spsr) cpu->spsr = (cpu->spsr & ~mask) | (val & mask);
            else cpu->cpsr = (cpu->cpsr & ~mask) | (val & mask);
            return 1;
        }

        /* --- Misc: MSR (immediate) --- */
        if (i_flag && (insn & 0xF1FF0FF0) == 0x0329F000) {
            uint32_t field = (insn >> 16) & 0xFF;
            bool dummy;
            uint32_t val = decode_imm(insn, &dummy);
            uint32_t mask = 0;
            if (field & 0x80) mask |= 0xFF000000;
            if (field & 0x40) mask |= 0x00FF0000;
            if (field & 0x20) mask |= 0x0000FF00;
            if (field & 0x10) mask |= 0x000000FF;
            bool spsr = (insn >> 22) & 1;
            if (spsr) cpu->spsr = (cpu->spsr & ~mask) | (val & mask);
            else cpu->cpsr = (cpu->cpsr & ~mask) | (val & mask);
            return 1;
        }

        /* --- Misc: BX --- */
        if (!i_flag && bits7_4 == 0x1 && rd_idx == 0xF) {
            uint32_t addr = cpu->regs[insn & 0xF];
            if (addr & 1) addr &= ~1;
            cpu->regs[ARM_REG_PC] = addr - 8;
            return 1;
        }

        /* --- Misc: BLX (register) --- */
        if (!i_flag && bits7_4 == 0x3 && rd_idx == 0xF) {
            uint32_t addr = cpu->regs[insn & 0xF];
            cpu_set_lr(cpu, pc + 4);
            if (addr & 1) addr &= ~1;
            cpu->regs[ARM_REG_PC] = addr - 8;
            return 1;
        }

        /* --- Misc: CLZ --- */
        if (!i_flag && bits7_4 == 0x1 && ((insn >> 16) & 0xF) == 0x1) {
            uint32_t rd = rd_idx;
            uint32_t val = cpu->regs[insn & 0xF];
            uint32_t count = 0;
            if (val == 0) count = 32;
            else while (!(val & 0x80000000)) { count++; val <<= 1; }
            cpu->regs[rd] = count;
            return 1;
        }

        /* --- Data Processing --- */
        uint32_t rn_val = cpu->regs[rn_idx];
        uint32_t op2;
        bool shifter_carry = (cpu->cpsr >> 29) & 1;

        if (i_flag) {
            op2 = decode_imm(insn, &shifter_carry);
        } else {
            if (bits7_4 & 0x1) op2 = decode_reg_shift(cpu, insn, &shifter_carry);
            else op2 = decode_imm_shift(cpu, insn, &shifter_carry);
        }

        uint32_t result;
        bool write_result = true;
        bool carry = (cpu->cpsr >> 29) & 1;
        bool overflow = (cpu->cpsr >> 28) & 1;

        switch (opcode) {
        case 0x0: result = rn_val & op2; break;
        case 0x1: result = rn_val ^ op2; break;
        case 0x2: result = rn_val - op2; carry = (uint64_t)rn_val >= (uint64_t)op2;
                  overflow = ((rn_val ^ op2) & (rn_val ^ result)) >> 31; break;
        case 0x3: result = op2 - rn_val; carry = (uint64_t)op2 >= (uint64_t)rn_val;
                  overflow = ((op2 ^ rn_val) & (op2 ^ result)) >> 31; break;
        case 0x4: result = rn_val + op2; carry = (uint64_t)rn_val + (uint64_t)op2 > 0xFFFFFFFF;
                  overflow = ~((rn_val ^ op2) & 0x80000000) & ((rn_val ^ result) & 0x80000000); break;
        case 0x5: { uint32_t oc = (cpu->cpsr >> 29) & 1;
                    uint64_t r64 = (uint64_t)rn_val + (uint64_t)op2 + oc;
                    result = (uint32_t)r64; carry = r64 > 0xFFFFFFFF;
                    overflow = ~((rn_val ^ op2) & 0x80000000) & ((rn_val ^ result) & 0x80000000); } break;
        case 0x6: { uint32_t oc = (cpu->cpsr >> 29) & 1;
                    uint64_t r64 = (uint64_t)rn_val - (uint64_t)op2 - (1 - oc);
                    result = (uint32_t)r64; carry = (uint64_t)rn_val >= (uint64_t)op2 + (1 - oc);
                    overflow = ((rn_val ^ op2) & (rn_val ^ result)) >> 31; } break;
        case 0x7: { uint32_t oc = (cpu->cpsr >> 29) & 1;
                    uint64_t r64 = (uint64_t)op2 - (uint64_t)rn_val - (1 - oc);
                    result = (uint32_t)r64; carry = (uint64_t)op2 >= (uint64_t)rn_val + (1 - oc);
                    overflow = ((op2 ^ rn_val) & (op2 ^ result)) >> 31; } break;
        case 0x8: result = rn_val & op2; cpu_update_flags_nz(cpu, result); cpu_set_carry(cpu, shifter_carry); write_result = false; return 1;
        case 0x9: result = rn_val ^ op2; cpu_update_flags_nz(cpu, result); cpu_set_carry(cpu, shifter_carry); write_result = false; return 1;
        case 0xA: result = rn_val - op2; carry = (uint64_t)rn_val >= (uint64_t)op2;
                  overflow = ((rn_val ^ result) & (op2 ^ result)) >> 31;
                  cpu_update_flags_nz(cpu, result); cpu_set_carry(cpu, carry); cpu_set_overflow(cpu, overflow); write_result = false; return 1;
        case 0xB: result = rn_val + op2; carry = (uint64_t)rn_val + (uint64_t)op2 > 0xFFFFFFFF;
                  overflow = ~((rn_val ^ op2) & 0x80000000) & ((rn_val ^ result) & 0x80000000);
                  cpu_update_flags_nz(cpu, result); cpu_set_carry(cpu, carry); cpu_set_overflow(cpu, overflow); write_result = false; return 1;
        case 0xC: result = rn_val | op2; break;
        case 0xD: result = op2; break;
        case 0xE: result = rn_val & ~op2; break;
        case 0xF: result = ~op2; break;
        default:  result = 0; break;
        }

        if (s_flag && rd_idx == 15) {
            cpu->cpsr = cpu->spsr;
        } else if (s_flag) {
            cpu_update_flags_nz(cpu, result);
            cpu_set_carry(cpu, shifter_carry);
        }

        if (write_result) {
            cpu->regs[rd_idx] = result;
            if (rd_idx == 15) cpu->regs[ARM_REG_PC] = result - 8;
        }

        return 1;
    }

    /* ================================================================ */
    /* Load/Store Word or Byte (op0 == 2 or 3)                          */
    /* ================================================================ */
    if (op0 == 2 || op0 == 3) {
        uint32_t p = (insn >> 24) & 1;
        uint32_t u = (insn >> 23) & 1;
        uint32_t b = (insn >> 22) & 1;
        uint32_t w = (insn >> 21) & 1;
        uint32_t l = (insn >> 20) & 1;
        uint32_t rn = (insn >> 16) & 0xF;
        uint32_t rd = (insn >> 12) & 0xF;
        uint32_t rn_val = cpu->regs[rn];

        uint32_t offset;
        if (op0 == 2) {
            /* Immediate offset */
            offset = insn & 0xFFF;
        } else {
            uint32_t bits7_4 = (insn >> 4) & 0xF;
            /* Half-word / signed load-store */
            if (bits7_4 == 0xB || bits7_4 == 0xD || bits7_4 == 0xF || bits7_4 == 0xE) {
                uint32_t imm4h = (insn >> 4) & 0xF0;
                uint32_t imm4l = insn & 0xF;
                offset = imm4h | imm4l;
                uint32_t addr;
                if (p) { addr = u ? rn_val + offset : rn_val - offset; if (w) cpu->regs[rn] = addr; }
                else { addr = rn_val; cpu->regs[rn] = u ? rn_val + offset : rn_val - offset; }

                if (l) {
                    switch (bits7_4) {
                    case 0xB: cpu->regs[rd] = mem_read16(cpu, addr); break;
                    case 0xD: cpu->regs[rd] = (uint32_t)(int32_t)(int16_t)mem_read16(cpu, addr); break;
                    case 0xF: cpu->regs[rd] = (uint32_t)(int32_t)(int8_t)mem_read8(cpu, addr); break;
                    case 0xE: cpu->regs[rd] = mem_read8(cpu, addr); break;
                    }
                } else {
                    uint32_t val = rd == 15 ? cpu->regs[rd] + 12 : cpu->regs[rd];
                    mem_write16(cpu, addr, (uint16_t)val);
                }
                return 1;
            }
            /* SWP */
            if (bits7_4 == 0x9) {
                uint32_t rm = insn & 0xF;
                if (b) {
                    uint8_t val = mem_read8(cpu, rn_val);
                    mem_write8(cpu, rn_val, cpu->regs[rm] & 0xFF);
                    cpu->regs[rd] = val;
                } else {
                    uint32_t val = mem_read32(cpu, rn_val);
                    mem_write32(cpu, rn_val, cpu->regs[rm]);
                    cpu->regs[rd] = val;
                }
                return 1;
            }
            /* Register offset */
            bool dummy;
            if (bits7_4 & 1) offset = decode_reg_shift(cpu, insn, &dummy);
            else offset = decode_imm_shift(cpu, insn, &dummy);
        }

        uint32_t addr;
        if (p) { addr = u ? rn_val + offset : rn_val - offset; if (w) cpu->regs[rn] = addr; }
        else { addr = rn_val; cpu->regs[rn] = u ? rn_val + offset : rn_val - offset; }

        if (l) {
            if (b) cpu->regs[rd] = mem_read8(cpu, addr);
            else { uint32_t v = mem_read32(cpu, addr & ~3u); uint32_t rot = (addr & 3) * 8;
                   cpu->regs[rd] = rot ? (v >> rot) | (v << (32 - rot)) : v; }
        } else {
            uint32_t val = rd == 15 ? cpu->regs[rd] + 12 : cpu->regs[rd];
            if (b) mem_write8(cpu, addr, (uint8_t)val);
            else mem_write32(cpu, addr & ~3u, val);
        }
        return 1;
    }

    /* ================================================================ */
    /* Load/Store Multiple (op0 == 4)                                   */
    /* ================================================================ */
    if (op0 == 4) {
        uint32_t p = (insn >> 24) & 1;
        uint32_t u = (insn >> 23) & 1;
        uint32_t w = (insn >> 21) & 1;
        uint32_t l = (insn >> 20) & 1;
        uint32_t rn = (insn >> 16) & 0xF;
        uint32_t reglist = insn & 0xFFFF;

        uint32_t count = __builtin_popcount(reglist);
        uint32_t base = u ? cpu->regs[rn] - count * 4 : cpu->regs[rn] + count * 4;
        if (p && u) base += 4;
        if (p && !u) base -= 4;

        uint32_t addr = base;
        if (l) {
            for (int i = 0; i < 16; i++) {
                if (reglist & (1u << i)) { cpu->regs[i] = mem_read32(cpu, addr); addr += 4; }
            }
            if (w) cpu->regs[rn] = u ? base + count * 4 : base - count * 4;
        } else {
            for (int i = 0; i < 16; i++) {
                if (reglist & (1u << i)) {
                    uint32_t val = i == 15 ? cpu->regs[i] + 12 : cpu->regs[i];
                    mem_write32(cpu, addr, val); addr += 4;
                }
            }
            if (w) cpu->regs[rn] = u ? base + count * 4 : base - count * 4;
        }
        return 1;
    }

    /* ================================================================ */
    /* Branch / Branch with Link (op0 == 5)                             */
    /* ================================================================ */
    if (op0 == 5) {
        uint32_t l = (insn >> 24) & 1;
        int32_t offset = (int32_t)(insn & 0x00FFFFFF) << 2;
        if (offset & 0x02000000) offset |= 0xFD000000;
        offset -= 8;
        if (l) cpu_set_lr(cpu, (cpu->regs[ARM_REG_PC] + 8) - 4);
        cpu->regs[ARM_REG_PC] = pc + offset - 8;
        return 1;
    }

    /* ================================================================ */
    /* Coprocessor (op0 == 6 or 7)                                      */
    /* ================================================================ */
    if (op0 == 6 || op0 == 7) {
        uint32_t l = (insn >> 20) & 1;
        uint32_t cp = (insn >> 8) & 0xF;
        uint32_t crn = (insn >> 16) & 0xF;
        uint32_t crm = insn & 0xF;
        uint32_t rd = (insn >> 12) & 0xF;

        if (cp == 0x15) {
            uint32_t reg_idx = (crn << 4) | crm;
            if (reg_idx < 32) {
                if (l) cpu->regs[rd] = cpu->cp15[reg_idx];
                else cpu->cp15[reg_idx] = cpu->regs[rd];
            }
        }
        return 1;
    }

    /* Undefined */
    fprintf(stderr, "[CPU] Undefined instruction at PC=0x%08X: 0x%08X\n", pc, insn);
    cpu->halted = true;
    return 1;
}

void cpu_dump_state(arm_cpu_t *cpu) {
    fprintf(stderr, "=== ARM CPU State ===\n");
    fprintf(stderr, "CPSR: 0x%08X [N=%c Z=%c C=%c V=%c I=%c F=%c T=%c Mode=%X]\n",
            cpu->cpsr,
            (cpu->cpsr & CPSR_N) ? '1' : '0',
            (cpu->cpsr & CPSR_Z) ? '1' : '0',
            (cpu->cpsr & CPSR_C) ? '1' : '0',
            (cpu->cpsr & CPSR_V) ? '1' : '0',
            (cpu->cpsr & CPSR_I) ? '1' : '0',
            (cpu->cpsr & CPSR_F) ? '1' : '0',
            (cpu->cpsr & CPSR_T) ? '1' : '0',
            cpu->cpsr & CPSR_MODE_MASK);
    fprintf(stderr, "SPSR: 0x%08X\n", cpu->spsr);
    for (int i = 0; i < 16; i++) {
        fprintf(stderr, "R%-2d: 0x%08X", i, cpu->regs[i]);
        if ((i + 1) % 4 == 0) fprintf(stderr, "\n");
        else fprintf(stderr, "  ");
    }
    fprintf(stderr, "Cycles: %lu\n", (unsigned long)cpu->cycle_count);
}

void cpu_dump_disasm(arm_cpu_t *cpu, uint32_t count) {
    static const char *conds[] = {"eq","ne","cs","cc","mi","pl","vs","vc",
                                  "hi","ls","ge","lt","gt","le","al","nv"};
    uint32_t start = cpu->regs[ARM_REG_PC] + 8;
    for (uint32_t i = 0; i < count; i++) {
        uint32_t addr = start + i * 4;
        uint32_t insn = mem_read32(cpu, addr);
        uint32_t c = (insn >> 28) & 0xF;
        uint32_t op0 = (insn >> 25) & 0x7;

        if (insn == 0) { fprintf(stderr, "0x%08X: nop\n", addr); continue; }

        if (op0 == 5) {
            uint32_t l = (insn >> 24) & 1;
            int32_t off = (int32_t)(insn & 0x00FFFFFF) << 2;
            if (off & 0x02000000) off |= 0xFD000000;
            fprintf(stderr, "0x%08X: %s%s 0x%08X\n", addr, l ? "bl" : "b", conds[c], addr + off + 8);
            continue;
        }

        fprintf(stderr, "0x%08X: 0x%08X\n", addr, insn);
    }
}
