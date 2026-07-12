/*
 * mindyld - Mach-O Binary Format Definitions
 * Supports 32-bit and 64-bit Mach-O binaries on macOS
 */

#ifndef _MINDYLD_MACHO_H
#define _MINDYLD_MACHO_H

#include "mindyld_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Mach-O Constants
 * ============================================================================ */

/* Mach magic numbers */
#define MH_MAGIC        0xfeedface
#define MH_MAGIC_64     0xfeedfacf
#define FAT_MAGIC       0xcafebabe
#define FAT_MAGIC_64    0xcafebabf

/* CPU types */
#define CPU_TYPE_I386       7
#define CPU_TYPE_X86_64     (0x01000007)
#define CPU_TYPE_ARM        12
#define CPU_TYPE_ARM64      0x0100000C

/* CPU subtypes */
#define CPU_SUBTYPE_ARM_V7  9
#define CPU_SUBTYPE_ARM64_ALL 0

/* Mach-O file types */
#define MH_OBJECT   0x1
#define MH_EXECUTE  0x2
#define MH_FVMLIB   0x3
#define MH_CORE     0x4
#define MH_PRELOAD  0x5
#define MH_DYLIB    0x6
#define MH_DYLINKER 0x7
#define MH_BUNDLE   0x8
#define MH_DYLIB_STUB 0x9

/* Load commands */
#define LC_SEGMENT          0x1
#define LC_SYMTAB           0x2
#define LC_SYMSEG           0x3
#define LC_THREAD           0x4
#define LC_UNIXTHREAD       0x5
#define LC_LOADFVMLIB       0x6
#define LC_IDFVMLIB         0x7
#define LC_IDENT            0x8
#define LC_FVMFILE          0x9
#define LC_PREPAGE          0xa
#define LC_DYSYMTAB         0xb
#define LC_LOAD_DYLIB       0xc
#define LC_ID_DYLIB         0xd
#define LC_LOAD_DYLINKER    0xe
#define LC_ID_DYLINKER      0xf
#define LC_PREBOUND_DYLIB   0x10
#define LC_ROUTINES         0x11
#define LC_SUB_FRAMEWORK    0x12
#define LC_SUB_UMBRELLA     0x13
#define LC_SUB_CLIENT       0x14
#define LC_SUB_LIBRARY      0x15
#define LC_TWOLEVEL_HINTS   0x16
#define LC_PREBIND_CKSUM    0x17
#define LC_LOAD_WEAK_DYLIB  0x18
#define LC_SEGMENT_64       0x19
#define LC_ROUTINES_64      0x1a
#define LC_UUID             0x1b
#define LC_RPATH            0x1c
#define LC_CODE_SIGNATURE   0x1d
#define LC_SEGMENT_SPLIT_INFO 0x1e
#define LC_REEXPORT_DYLIB   0x1f
#define LC_LAZY_LOAD_DYLIB  0x20
#define LC_ENCRYPTION_INFO  0x21
#define LC_DYLD_INFO        0x22
#define LC_DYLD_INFO_ONLY   0x22
#define LC_LOAD_UPWARD_DYLIB 0x23
#define LC_VERSION_MIN_MACOSX 0x24
#define LC_VERSION_MIN_IPHONEOS 0x25
#define LC_FUNCTION_STARTS  0x26
#define LC_DYLD_ENVIRONMENT 0x27
#define LC_MAIN             0x28
#define LC_DATA_IN_CODE     0x29
#define LC_SOURCE_VERSION   0x2A
#define LC_DYLIB_CODE_SIGN_DRS 0x2B
#define LC_ENCRYPTION_INFO_64 0x2C

/* Section flags */
#define SECTION_TYPE            0x000000FF
#define S_REGULAR               0x0
#define S_ZEROFILL              0x1
#define S_CSTRING_LITERALS      0x2
#define S_4BYTE_LITERALS        0x3
#define S_8BYTE_LITERALS        0x4
#define S_LITERAL_POINTERS      0x5
#define S_NON_LAZY_SYMBOL_POINTERS 0x6
#define S_LAZY_SYMBOL_POINTERS  0x7
#define S_SYMBOL_STUBS          0x8
#define S_MOD_INIT_FUNC_POINTERS 0x9
#define S_MOD_TERM_FUNC_POINTERS 0xA
#define S_COALESCED             0xB
#define S_GB_ZEROFILL           0xC
#define S_INTERPOSING           0xD
#define S_16BYTE_LITERALS       0xE
#define S_DTRACE_DOF            0xF
#define S_LAZY_DYLIB_SYMBOL_POINTERS 0x10
#define S_THREAD_LOCAL_REGULAR  0x11
#define S_THREAD_LOCAL_ZEROFILL 0x12
#define S_THREAD_LOCAL_VARIABLES 0x13
#define S_THREAD_LOCAL_VARIABLE_POINTERS 0x14
#define S_THREAD_LOCAL_INIT_FUNCTION_POINTERS 0x15

/* Relocation types for x86-64 */
#define X86_64_RELOC_UNSIGNED   0
#define X86_64_RELOC_SIGNED     1
#define X86_64_RELOC_BRANCH     2
#define X86_64_RELOC_GOT_LOAD   3
#define X86_64_RELOC_GOT        4
#define X86_64_RELOC_SUBTRACTOR 5
#define X86_64_RELOC_SIGNED_1   6
#define X86_64_RELOC_SIGNED_2   7
#define X86_64_RELOC_SIGNED_4   8
#define X86_64_RELOC_TLV        9

/* Relocation types for ARM64 */
#define ARM64_RELOC_UNSIGNED            0
#define ARM64_RELOC_SUBTRACTOR          1
#define ARM64_RELOC_BRANCH26            2
#define ARM64_RELOC_PAGE21              3
#define ARM64_RELOC_PAGEOFF12           4
#define ARM64_RELOC_GOT_LOAD_PAGE21     5
#define ARM64_RELOC_GOT_LOAD_PAGEOFF12  6
#define ARM64_RELOC_POINTER_TO_GOT      7
#define ARM64_RELOC_TLVP_LOAD_PAGE21    8
#define ARM64_RELOC_TLVP_LOAD_PAGEOFF12 9
#define ARM64_RELOC_ADDEND              10

/* Segment flags */
#define VM_PROT_NONE   0x00
#define VM_PROT_READ   0x01
#define VM_PROT_WRITE  0x02
#define VM_PROT_EXEC   0x04

/* ============================================================================
 * Mach-O Structure Definitions
 * ============================================================================ */

typedef struct {
    uint32_t magic;
    uint32_t cputype;
    uint32_t cpusubtype;
    uint32_t filetype;
    uint32_t ncmds;
    uint32_t sizeofcmds;
    uint32_t flags;
} mach_header_t;

typedef struct {
    uint32_t magic;
    uint32_t cputype;
    uint32_t cpusubtype;
    uint32_t filetype;
    uint32_t ncmds;
    uint32_t sizeofcmds;
    uint32_t flags;
    uint32_t reserved;
} mach_header_64_t;

/* Load command */
typedef struct {
    uint32_t cmd;
    uint32_t cmdsize;
} load_command_t;

/* Segment command (32-bit) */
typedef struct {
    uint32_t cmd;
    uint32_t cmdsize;
    char segname[16];
    uint32_t vmaddr;
    uint32_t vmsize;
    uint32_t fileoff;
    uint32_t filesize;
    uint32_t maxprot;
    uint32_t initprot;
    uint32_t nsects;
    uint32_t flags;
} segment_command_t;

/* Segment command (64-bit) */
typedef struct {
    uint32_t cmd;
    uint32_t cmdsize;
    char segname[16];
    uint64_t vmaddr;
    uint64_t vmsize;
    uint64_t fileoff;
    uint64_t filesize;
    uint32_t maxprot;
    uint32_t initprot;
    uint32_t nsects;
    uint32_t flags;
} segment_command_64_t;

/* Section (32-bit) */
typedef struct {
    char sectname[16];
    char segname[16];
    uint32_t addr;
    uint32_t size;
    uint32_t offset;
    uint32_t align;
    uint32_t reloff;
    uint32_t nreloc;
    uint32_t flags;
    uint32_t reserved1;
    uint32_t reserved2;
} section_t;

/* Section (64-bit) */
typedef struct {
    char sectname[16];
    char segname[16];
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
} section_64_t;

/* Symbol table command */
typedef struct {
    uint32_t cmd;
    uint32_t cmdsize;
    uint32_t symoff;
    uint32_t nsyms;
    uint32_t stroff;
    uint32_t strsize;
} symtab_command_t;

/* Dynamic symbol table command */
typedef struct {
    uint32_t cmd;
    uint32_t cmdsize;
    uint32_t ilocalsym;
    uint32_t nlocalsym;
    uint32_t iextdefsym;
    uint32_t nextdefsym;
    uint32_t iundefsym;
    uint32_t nundefsym;
    uint32_t tocoff;
    uint32_t ntoc;
    uint32_t modtaboff;
    uint32_t nmodtab;
    uint32_t extrefsymoff;
    uint32_t nextrefsyms;
    uint32_t indirectsymoff;
    uint32_t nindirectsyms;
    uint32_t extreloff;
    uint32_t nextrel;
    uint32_t locreloff;
    uint32_t nlocrel;
} dysymtab_command_t;

/* Nlist entry (32-bit) */
typedef struct {
    uint32_t n_strx;
    uint8_t n_type;
    uint8_t n_sect;
    int16_t n_desc;
    uint32_t n_value;
} nlist_t;

/* Nlist entry (64-bit) */
typedef struct {
    uint32_t n_strx;
    uint8_t n_type;
    uint8_t n_sect;
    uint16_t n_desc;
    uint64_t n_value;
} nlist_64_t;

/* Relocation entry */
typedef struct {
    int32_t r_address;
    uint32_t r_symbolnum : 24;
    uint32_t r_pcrel : 1;
    uint32_t r_length : 2;
    uint32_t r_extern : 1;
    uint32_t r_type : 4;
} relocation_info_t;

/* Dyld info command */
typedef struct {
    uint32_t cmd;
    uint32_t cmdsize;
    uint32_t rebase_off;
    uint32_t rebase_size;
    uint32_t bind_off;
    uint32_t bind_size;
    uint32_t weak_bind_off;
    uint32_t weak_bind_size;
    uint32_t lazy_bind_off;
    uint32_t lazy_bind_size;
    uint32_t export_off;
    uint32_t export_size;
} dyld_info_command_t;

/* ============================================================================
 * Mach-O Loader Functions
 * ============================================================================ */

/* Load Mach-O shared library or executable */
void *macho_load(const char *path);

/* Find symbol in Mach-O binary */
mindyld_symbol_t *macho_find_symbol(mindyld_handle_t *handle, const char *name);

/* Process Mach-O relocations */
int macho_apply_relocations(mindyld_handle_t *handle);

#ifdef __cplusplus
}
#endif

#endif /* _MINDYLD_MACHO_H */
