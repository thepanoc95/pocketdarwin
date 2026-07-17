/*
 * Copyright © 2009 Apple Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1.  Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer. 
 * 2.  Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution. 
 * 3.  Neither the name of Apple Inc. ("Apple") nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission. 
 * 
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @APPLE_LICENSE_HEADER_END@
 */
#ifndef _OTOOL_H
#define _OTOOL_H

#include <stuff/bool.h>
#include "stuff/llvm.h"
#include "llvm-c/Disassembler.h"
#include <stdio.h>
#include <stuff/bytesex.h>
#include <mach-o/loader.h>

typedef unsigned long long bfd_vma;
typedef unsigned char bfd_byte;
typedef enum bool bfd_boolean;
typedef int (*fprintf_ftype) (void *, const char*, ...);
typedef enum bfd_endian {
    BFD_ENDIAN_BIG,
    BFD_ENDIAN_LITTLE,
    BFD_ENDIAN_UNKNOWN
} enum_bfd_endian;

typedef struct bfd_section {
    char *name;
} *bfd_section_ptr;

#define INSN_HAS_RELOC	0x80000000

struct section;

/* Name of this program for error messages (argv[0]) */
extern char *progname;

/*
 * The flags to indicate the actions to perform.
 */
extern enum bool fflag; /* print the fat headers */
extern enum bool aflag; /* print the archive header */
extern enum bool hflag; /* print the exec or mach header */
extern enum bool lflag; /* print the load commands */
extern enum bool Lflag; /* print the shared library names */
extern enum bool Dflag; /* print the shared library id name */
extern enum bool tflag; /* print the text */
extern enum bool dflag; /* print the data */
extern enum bool oflag; /* print the objctive-C info */
extern enum bool Oflag; /* print the objctive-C selector strings only */
extern enum bool rflag; /* print the relocation entries */
extern enum bool Sflag; /* print the contents of the __.SYMDEF file */
extern enum bool vflag; /* print verbosely (symbolicly) when possible */
extern enum bool Vflag; /* print dissassembled operands verbosely */
extern enum bool cflag; /* print the argument and environ strings of a core file */
extern enum bool iflag; /* print the shared library initialization table */
extern enum bool Wflag; /* print the mod time of an archive as an decimal number */
extern enum bool Xflag; /* don't print leading address in disassembly */
extern enum bool Zflag; /* don't use simplified ppc mnemonics in disassembly */
extern enum bool Bflag; /* force Thumb disassembly (ARM objects only) */
extern enum bool Qflag; /* use the HACKED llvm-mc disassembler */
extern enum bool qflag; /* use 'C' Public llvm-mc disassembler */
extern enum bool gflag; /* group the disassembly */
extern enum bool jflag; /* print opcode bytes */
extern char *pflag; 	/* procedure name to start disassembling from */
extern char *segname,
     *sectname;	    /* name of the section to print the contents of */
extern char *mcpu; 	/* the arg of the -mcpu=arg flag */
/* Print function offsets when disassembling when TRUE. */
extern enum bool function_offsets;

extern enum bool no_show_raw_insn;

uint32_t m68k_usrstack(void);
uint32_t m88k_usrstack(void);
uint32_t i386_usrstack(void);
uint32_t hppa_usrstack(void);
uint32_t sparc_usrstack(void);

#ifndef STRUCT_INST
#define STRUCT_INST

struct inst {
    uint64_t address;
    char *label;
    enum bool needs_tmp_label;
    char *tmp_label;
    enum bool print;
    enum bool has_raw_target_address;
    uint64_t raw_target_address;
};

#endif

struct disassemble_info {
    void *application_data;
    fprintf_ftype fprintf_func;
    void *stream;
    void (*print_address_func)(bfd_vma pc, bfd_vma addr,
                               struct disassemble_info *info);
    int (*read_memory_func)(bfd_vma memaddr, bfd_byte *myaddr,
                            unsigned int length,
                            struct disassemble_info *info);
    void (*memory_error_func)(int status, bfd_vma memaddr,
                              struct disassemble_info *info);
    int (*symbol_at_address_func)(bfd_vma addr,
                                  struct disassemble_info *info);
    enum bool verbose;
    unsigned int flags;
    bfd_vma sec_base_address;
    struct relocation_info *relocs;
    uint32_t nrelocs;
    struct nlist *symbols;
    struct nlist_64 *symbols64;
    uint32_t nsymbols;
    struct symbol *sorted_symbols;
    uint32_t nsorted_symbols;
    struct relocation_info *sorted_relocs;
    uint32_t nsorted_relocs;
    char *strings;
    uint32_t strings_size;
    enum byte_sex object_byte_sex;
    uint32_t *indirect_symbols;
    uint32_t nindirect_symbols;
    char *sect;
    uint32_t left;
    uint32_t addr;
    uint32_t sect_addr;
    uint32_t ncmds;
    uint32_t sizeofcmds;
    struct load_command *load_commands;
    char *object_addr;
    uint32_t object_size;
    struct relocation_info *ext_relocs;
    uint32_t next_relocs;
    struct relocation_info *loc_relocs;
    uint32_t nloc_relocs;
    struct dyld_bind_info *dbi;
    uint64_t ndbi;
    uint32_t cpu_type;
    uint32_t cputype;
    uint64_t adrp_addr;
    uint32_t adrp_inst;
    struct inst *inst;
    struct inst *insts;
    uint32_t ninsts;
    char *demangled_name;
    enum bool swapped;
    char *method;
    const char *class_name;
    const char *selector_name;
    uint64_t seg_addr;
    struct data_in_code_entry *dices;
    uint32_t ndices;
    LLVMDisasmContextRef arm_dc;
    LLVMDisasmContextRef thumb_dc;
    enum bool in_thumb;
    char *seg;
    char *start;
    bfd_vma (*symbol_at_address)(bfd_vma addr, struct disassemble_info *info);

    /* ARM-specific fields (from binutils dis-asm.h) */
    uint32_t mach;
    int bytes_per_line;
    int bytes_per_chunk;
    enum bfd_endian display_endian;
    enum bool (*print_immediate_func)(bfd_vma pc, unsigned int imm,
                                      struct disassemble_info *info,
                                      enum bool pool);
    uint32_t arch;
    struct inst *indirect;
    const char *name;
    bfd_vma r_address;
    uint32_t r_extern;
    uint32_t r_length;
    int r_scattered;
    bfd_vma value;
    struct section *section;
    char *symtab;
    uint32_t symtab_pos;
    uint64_t symtab_size;
    void *internal_elf_sym;
    int (*assembler)(void);
    char *disassembler_options;
    int all;
    union {
        uint32_t u32;
        uint64_t u64;
    } u;
};

extern struct disassemble_info dis_info;
extern enum bool in_thumb;

#endif /* _OTOOL_H */