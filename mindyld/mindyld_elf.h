/*
 * mindyld - ELF Binary Format Definitions
 * Supports 32-bit and 64-bit ELF binaries on Linux and BSD
 */

#ifndef _MINDYLD_ELF_H
#define _MINDYLD_ELF_H

#include "mindyld_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * ELF Header Constants
 * ============================================================================ */

/* ELF magic number */
#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

/* ELF class (32-bit vs 64-bit) */
#define ELFCLASS32  1
#define ELFCLASS64  2

/* ELF data encoding */
#define ELFDATA2LSB 1  /* Little-endian */
#define ELFDATA2MSB 2  /* Big-endian */

/* ELF OS/ABI */
#define ELFOSABI_NONE       0
#define ELFOSABI_LINUX      3
#define ELFOSABI_FREEBSD    9

/* ELF type */
#define ET_NONE   0
#define ET_REL    1  /* Relocatable file */
#define ET_EXEC   2  /* Executable file */
#define ET_DYN    3  /* Shared object file */
#define ET_CORE   4  /* Core file */

/* ELF machine type */
#define EM_386      3   /* Intel 80386 */
#define EM_ARM      40  /* ARM */
#define EM_X86_64   62  /* AMD x86-64 */
#define EM_AARCH64  183 /* ARM 64-bit */

/* Program header types */
#define PT_NULL    0
#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_NOTE    4
#define PT_SHLIB   5
#define PT_PHDR    6
#define PT_TLS     7

/* Program header flags */
#define PF_X 0x1
#define PF_W 0x2
#define PF_R 0x4

/* Section header types */
#define SHT_NULL          0
#define SHT_PROGBITS      1
#define SHT_SYMTAB        2
#define SHT_STRTAB        3
#define SHT_RELA          4
#define SHT_HASH          5
#define SHT_DYNAMIC       6
#define SHT_NOTE          7
#define SHT_NOBITS        8
#define SHT_REL           9
#define SHT_SHLIB        10
#define SHT_DYNSYM       11
#define SHT_LOPROC   0x70000000
#define SHT_HIPROC   0x7fffffff
#define SHT_LOUSER   0x80000000
#define SHT_HIUSER   0xffffffff

/* Dynamic section tags */
#define DT_NULL         0
#define DT_NEEDED       1
#define DT_PLTRELSZ     2
#define DT_PLTGOT       3
#define DT_HASH         4
#define DT_STRTAB       5
#define DT_SYMTAB       6
#define DT_RELA         7
#define DT_RELASZ       8
#define DT_RELAENT      9
#define DT_STRSZ        10
#define DT_SYMENT       11
#define DT_INIT         12
#define DT_FINI         13
#define DT_SONAME       14
#define DT_RPATH        15
#define DT_SYMBOLIC     16
#define DT_REL          17
#define DT_RELSZ        18
#define DT_RELENT       19
#define DT_PLTREL       20
#define DT_DEBUG        21
#define DT_TEXTREL      22
#define DT_JMPREL       23
#define DT_RUNPATH      29
#define DT_VERNEED      0x6ffffffe
#define DT_VERSYM       0x6ffffff0

/* Relocation types for ARM (32-bit) */
#define R_ARM_NONE              0
#define R_ARM_ABS32             2
#define R_ARM_REL32             3
#define R_ARM_ABS16             0
#define R_ARM_ABS8              1
#define R_ARM_JUMP_SLOT         22
#define R_ARM_GLOB_DAT          21
#define R_ARM_RELATIVE          23

/* Relocation types for ARM64 (64-bit) */
#define R_AARCH64_NONE          0
#define R_AARCH64_ABS64         257
#define R_AARCH64_ABS32         258
#define R_AARCH64_REL64         260
#define R_AARCH64_GLOB_DAT      1025
#define R_AARCH64_JUMP_SLOT     1026
#define R_AARCH64_RELATIVE      1027

/* Relocation types for x86-64 */
#define R_X86_64_NONE           0
#define R_X86_64_64             1
#define R_X86_64_PC32           2
#define R_X86_64_GOT32          3
#define R_X86_64_PLT32          4
#define R_X86_64_COPY           5
#define R_X86_64_GLOB_DAT       6
#define R_X86_64_JUMP_SLOT      7
#define R_X86_64_RELATIVE       8

/* Relocation types for x86 (32-bit) */
#define R_386_NONE              0
#define R_386_32                1
#define R_386_PC32              2
#define R_386_GOT32             3
#define R_386_PLT32             4
#define R_386_COPY              5
#define R_386_GLOB_DAT          6
#define R_386_JMP_SLOT          7
#define R_386_RELATIVE          8

/* Symbol binding */
#define STB_LOCAL   0
#define STB_GLOBAL  1
#define STB_WEAK    2

/* Symbol type */
#define STT_NOTYPE  0
#define STT_OBJECT  1
#define STT_FUNC    2
#define STT_SECTION 3
#define STT_FILE    4
#define STT_COMMON  5
#define STT_TLS     6

/* Symbol visibility */
#define STV_DEFAULT   0
#define STV_INTERNAL  1
#define STV_HIDDEN    2
#define STV_PROTECTED 3

/* ============================================================================
 * ELF Structure Definitions (64-bit)
 * ============================================================================ */

typedef struct {
    uint8_t e_ident[16];    /* ELF identification */
    uint16_t e_type;        /* Object file type */
    uint16_t e_machine;     /* Machine type */
    uint32_t e_version;     /* Object file version */
    uint64_t e_entry;       /* Entry point address */
    uint64_t e_phoff;       /* Program header offset */
    uint64_t e_shoff;       /* Section header offset */
    uint32_t e_flags;       /* Processor-specific flags */
    uint16_t e_ehsize;      /* ELF header size */
    uint16_t e_phentsize;   /* Program header entry size */
    uint16_t e_phnum;       /* Number of program headers */
    uint16_t e_shentsize;   /* Section header entry size */
    uint16_t e_shnum;       /* Number of section headers */
    uint16_t e_shstrndx;    /* Section header string index */
} elf64_header_t;

typedef struct {
    uint8_t e_ident[16];    /* ELF identification */
    uint16_t e_type;        /* Object file type */
    uint16_t e_machine;     /* Machine type */
    uint32_t e_version;     /* Object file version */
    uint32_t e_entry;       /* Entry point address */
    uint32_t e_phoff;       /* Program header offset */
    uint32_t e_shoff;       /* Section header offset */
    uint32_t e_flags;       /* Processor-specific flags */
    uint16_t e_ehsize;      /* ELF header size */
    uint16_t e_phentsize;   /* Program header entry size */
    uint16_t e_phnum;       /* Number of program headers */
    uint16_t e_shentsize;   /* Section header entry size */
    uint16_t e_shnum;       /* Number of section headers */
    uint16_t e_shstrndx;    /* Section header string index */
} elf32_header_t;

/* Program header (64-bit) */
typedef struct {
    uint32_t p_type;        /* Segment type */
    uint32_t p_flags;       /* Segment flags */
    uint64_t p_offset;      /* Segment offset in file */
    uint64_t p_vaddr;       /* Virtual address */
    uint64_t p_paddr;       /* Physical address */
    uint64_t p_filesz;      /* Size in file */
    uint64_t p_memsz;       /* Size in memory */
    uint64_t p_align;       /* Alignment */
} elf64_phdr_t;

/* Program header (32-bit) */
typedef struct {
    uint32_t p_type;        /* Segment type */
    uint32_t p_offset;      /* Segment offset in file */
    uint32_t p_vaddr;       /* Virtual address */
    uint32_t p_paddr;       /* Physical address */
    uint32_t p_filesz;      /* Size in file */
    uint32_t p_memsz;       /* Size in memory */
    uint32_t p_flags;       /* Segment flags */
    uint32_t p_align;       /* Alignment */
} elf32_phdr_t;

/* Section header (64-bit) */
typedef struct {
    uint32_t sh_name;       /* Section name index */
    uint32_t sh_type;       /* Section type */
    uint64_t sh_flags;      /* Section flags */
    uint64_t sh_addr;       /* Section address */
    uint64_t sh_offset;     /* Section offset */
    uint64_t sh_size;       /* Section size */
    uint32_t sh_link;       /* Section link */
    uint32_t sh_info;       /* Section info */
    uint64_t sh_addralign;  /* Alignment */
    uint64_t sh_entsize;    /* Entry size */
} elf64_shdr_t;

/* Symbol table entry (64-bit) */
typedef struct {
    uint32_t st_name;       /* Symbol name index */
    uint8_t st_info;        /* Symbol binding and type */
    uint8_t st_other;       /* Symbol visibility */
    uint16_t st_shndx;      /* Section index */
    uint64_t st_value;      /* Symbol value */
    uint64_t st_size;       /* Symbol size */
} elf64_sym_t;

/* Relocation entry (64-bit) */
typedef struct {
    uint64_t r_offset;      /* Relocation offset */
    uint64_t r_info;        /* Relocation type and symbol */
    int64_t r_addend;       /* Relocation addend */
} elf64_rela_t;

typedef struct {
    uint64_t r_offset;      /* Relocation offset */
    uint64_t r_info;        /* Relocation type and symbol */
} elf64_rel_t;

/* Dynamic section entry (64-bit) */
typedef struct {
    int64_t d_tag;
    union {
        uint64_t d_val;
        uint64_t d_ptr;
    } d_un;
} elf64_dyn_t;

/* ============================================================================
 * ELF Parser Functions
 * ============================================================================ */

/* Parse ELF header */
int elf_parse_header(fd_t fd, void *header_buf);

/* Load ELF shared library or executable */
void *elf_load(const char *path);

/* Find symbol in ELF binary */
mindyld_symbol_t *elf_find_symbol(mindyld_handle_t *handle, const char *name);

/* Process ELF relocations */
int elf_apply_relocations(mindyld_handle_t *handle);

#ifdef __cplusplus
}
#endif

#endif /* _MINDYLD_ELF_H */
