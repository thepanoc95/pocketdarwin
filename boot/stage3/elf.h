/* ELF format definitions for ARMv7 and ARM64 */
#ifndef ELF_H
#define ELF_H

#include "uefi.h"

/* ELF identification */
#define EI_MAG0    0
#define EI_MAG1    1
#define EI_MAG2    2
#define EI_MAG3    3
#define EI_CLASS   4
#define EI_DATA    5
#define EI_VERSION 6
#define EI_OSABI   7

#define ELFMAG0    0x7f
#define ELFMAG1    'E'
#define ELFMAG2    'L'
#define ELFMAG3    'F'

#define ELFCLASS32 1
#define ELFCLASS64 2

#define ELFDATA2LSB 1

/* ELF machine types */
#define EM_386     3
#define EM_ARM     40
#define EM_AARCH64 183

/* Program header types */
#define PT_NULL    0
#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_NOTE    4

/* Program header flags */
#define PF_X 0x1
#define PF_W 0x2
#define PF_R 0x4

/* ------------------------------------------------------------------ */
/* ELF32                                                               */
/* ------------------------------------------------------------------ */
typedef struct {
    UINT8  e_ident[16];
    UINT16 e_type;
    UINT16 e_machine;
    UINT32 e_version;
    UINT32 e_entry;
    UINT32 e_phoff;
    UINT32 e_shoff;
    UINT32 e_flags;
    UINT16 e_ehsize;
    UINT16 e_phentsize;
    UINT16 e_phnum;
    UINT16 e_shentsize;
    UINT16 e_shnum;
    UINT16 e_shstrndx;
} Elf32_Ehdr;

typedef struct {
    UINT32 p_type;
    UINT32 p_offset;
    UINT32 p_vaddr;
    UINT32 p_paddr;
    UINT32 p_filesz;
    UINT32 p_memsz;
    UINT32 p_flags;
    UINT32 p_align;
} Elf32_Phdr;

/* ------------------------------------------------------------------ */
/* ELF64                                                               */
/* ------------------------------------------------------------------ */
typedef struct {
    UINT8  e_ident[16];
    UINT16 e_type;
    UINT16 e_machine;
    UINT32 e_version;
    UINT64 e_entry;
    UINT64 e_phoff;
    UINT64 e_shoff;
    UINT32 e_flags;
    UINT16 e_ehsize;
    UINT16 e_phentsize;
    UINT16 e_phnum;
    UINT16 e_shentsize;
    UINT16 e_shnum;
    UINT16 e_shstrndx;
} Elf64_Ehdr;

typedef struct {
    UINT32 p_type;
    UINT32 p_flags;
    UINT64 p_offset;
    UINT64 p_vaddr;
    UINT64 p_paddr;
    UINT64 p_filesz;
    UINT64 p_memsz;
    UINT64 p_align;
} Elf64_Phdr;

/* ------------------------------------------------------------------ */
/* ELF validation helpers                                              */
/* ------------------------------------------------------------------ */
static inline int elf_is_valid(const void *data, UINTN size) {
    const UINT8 *p = (const UINT8*)data;
    if (size < 4) return 0;
    if (p[EI_MAG0] != ELFMAG0 || p[EI_MAG1] != ELFMAG1 ||
        p[EI_MAG2] != ELFMAG2 || p[EI_MAG3] != ELFMAG3)
        return 0;
    if (p[EI_CLASS] != ELFCLASS32 && p[EI_CLASS] != ELFCLASS64)
        return 0;
    if (p[EI_DATA] != ELFDATA2LSB)
        return 0;
    return 1;
}

static inline int elf_is_64bit(const void *data) {
    return ((const UINT8*)data)[EI_CLASS] == ELFCLASS64;
}

#endif /* ELF_H */
