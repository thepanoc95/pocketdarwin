/*
 * mindyld - ELF Binary Loader Implementation
 * Supports loading ELF binaries on Linux and BSD
 */

#include "mindyld_elf.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * ELF Validation
 * ============================================================================ */

static int elf_validate_header(elf64_header_t *header)
{
    /* Check magic number */
    if (header->e_ident[0] != ELFMAG0 ||
        header->e_ident[1] != ELFMAG1 ||
        header->e_ident[2] != ELFMAG2 ||
        header->e_ident[3] != ELFMAG3) {
        return MINDYLD_ERR_INVALID;
    }
    
    /* Check class (32-bit or 64-bit) */
    if (header->e_ident[4] != ELFCLASS32 && header->e_ident[4] != ELFCLASS64) {
        return MINDYLD_ERR_INVALID;
    }
    
    /* Check data encoding (endianness) */
    if (header->e_ident[5] != ELFDATA2LSB && header->e_ident[5] != ELFDATA2MSB) {
        return MINDYLD_ERR_INVALID;
    }
    
    /* Check version */
    if (header->e_ident[6] != 1) {
        return MINDYLD_ERR_INVALID;
    }
    
    /* Check file type */
    if (header->e_type != ET_DYN && header->e_type != ET_EXEC) {
        return MINDYLD_ERR_INVALID;
    }
    
    return MINDYLD_OK;
}

/* ============================================================================
 * ELF Memory Mapping
 * ============================================================================ */

static void *elf_map_segments(fd_t fd, elf64_header_t *header)
{
    void *base_address = NULL;
    uint64_t min_addr = (uint64_t)-1;
    uint64_t max_addr = 0;
    
    /* Read program headers to calculate required memory */
    elf64_phdr_t *phdrs = (elf64_phdr_t *)malloc(header->e_phentsize * header->e_phnum);
    if (!phdrs)
        return NULL;
    
    /* Seek to program header offset */
    if (lseek(fd, header->e_phoff, SEEK_SET) < 0) {
        free(phdrs);
        return NULL;
    }
    
    /* Read all program headers */
    size_t phdrs_size = header->e_phentsize * header->e_phnum;
    if (read(fd, phdrs, phdrs_size) != (ssize_t)phdrs_size) {
        free(phdrs);
        return NULL;
    }
    
    /* Calculate virtual address space needed */
    for (uint32_t i = 0; i < header->e_phnum; i++) {
        if (phdrs[i].p_type == PT_LOAD) {
            if (phdrs[i].p_vaddr < min_addr)
                min_addr = phdrs[i].p_vaddr;
            if (phdrs[i].p_vaddr + phdrs[i].p_memsz > max_addr)
                max_addr = phdrs[i].p_vaddr + phdrs[i].p_memsz;
        }
    }
    
    uint64_t total_size = max_addr - min_addr;
    
    /* Allocate memory for all segments */
    int mmap_flags = MAP_PRIVATE | MAP_ANONYMOUS;
    if (header->e_type == ET_EXEC)
        mmap_flags |= MAP_FIXED;
    
    base_address = mmap(NULL, total_size, PROT_WRITE, mmap_flags, -1, 0);
    if (!base_address) {
        free(phdrs);
        return NULL;
    }
    
    /* Load each PT_LOAD segment */
    for (uint32_t i = 0; i < header->e_phnum; i++) {
        elf64_phdr_t *phdr = &phdrs[i];
        
        if (phdr->p_type != PT_LOAD)
            continue;
        
        /* Calculate load address */
        void *segment_addr = (char *)base_address + (phdr->p_vaddr - min_addr);
        
        /* Seek to segment data */
        if (lseek(fd, phdr->p_offset, SEEK_SET) < 0) {
            munmap(base_address, total_size);
            free(phdrs);
            return NULL;
        }
        
        /* Read segment data from file */
        ssize_t read_bytes = read(fd, segment_addr, phdr->p_filesz);
        if (read_bytes != (ssize_t)phdr->p_filesz) {
            munmap(base_address, total_size);
            free(phdrs);
            return NULL;
        }
        
        /* Zero out BSS section if filesz < memsz */
        if (phdr->p_filesz < phdr->p_memsz) {
            char *bss_start = (char *)segment_addr + phdr->p_filesz;
            size_t bss_size = phdr->p_memsz - phdr->p_filesz;
            memset(bss_start, 0, bss_size);
        }
        
        /* Set appropriate page protections */
        int prot = PROT_NONE;
        if (phdr->p_flags & PF_R) prot |= PROT_READ;
        if (phdr->p_flags & PF_W) prot |= PROT_WRITE;
        if (phdr->p_flags & PF_X) prot |= PROT_EXEC;
        
        if (mprotect(segment_addr, phdr->p_memsz, prot) < 0) {
            munmap(base_address, total_size);
            free(phdrs);
            return NULL;
        }
    }
    
    free(phdrs);
    return base_address;
}

/* ============================================================================
 * ELF Symbol Resolution
 * ============================================================================ */

static mindyld_symbol_t *elf_find_symbol_in_symtab(
    mindyld_handle_t *handle,
    const char *name,
    elf64_sym_t *symtab,
    uint32_t symtab_size,
    char *strtab)
{
    uint32_t num_symbols = symtab_size / sizeof(elf64_sym_t);
    
    for (uint32_t i = 0; i < num_symbols; i++) {
        elf64_sym_t *sym = &symtab[i];
        
        /* Get symbol name from string table */
        char *sym_name = strtab + sym->st_name;
        
        /* Check if name matches */
        if (strcmp(sym_name, name) == 0) {
            mindyld_symbol_t *result = (mindyld_symbol_t *)malloc(sizeof(mindyld_symbol_t));
            if (result) {
                result->name = sym_name;
                result->address = (char *)handle->base_address + sym->st_value;
                result->size = sym->st_size;
                result->binding = sym->st_info >> 4;
                result->type = sym->st_info & 0x0F;
            }
            return result;
        }
    }
    
    return NULL;
}

/* ============================================================================
 * ELF Relocation Processing
 * ============================================================================ */

static int elf_process_rela(
    mindyld_handle_t *handle,
    elf64_rela_t *rela,
    uint32_t rela_count,
    elf64_sym_t *symtab,
    char *strtab)
{
    for (uint32_t i = 0; i < rela_count; i++) {
        elf64_rela_t *rel = &rela[i];
        uint32_t sym_index = rel->r_info >> 32;
        uint32_t rel_type = rel->r_info & 0xFFFFFFFF;
        
        /* Get symbol if needed */
        uint64_t sym_value = 0;
        if (sym_index != 0) {
            elf64_sym_t *sym = &symtab[sym_index];
            sym_value = (uint64_t)handle->base_address + sym->st_value;
        }
        
        /* Get relocation target address */
        uint64_t *rel_addr = (uint64_t *)((char *)handle->base_address + rel->r_offset);
        
        /* Apply relocation based on type */
        switch (rel_type) {
            case R_ARM_RELATIVE:
            case R_AARCH64_RELATIVE:
            case R_X86_64_RELATIVE:
            case R_386_RELATIVE:
                /* Relative relocation: value += base */
                *rel_addr += (uint64_t)handle->base_address;
                break;
                
            case R_ARM_ABS32:
            case R_AARCH64_ABS64:
            case R_X86_64_64:
            case R_386_32:
                /* Absolute relocation: value = symbol + addend */
                *rel_addr = sym_value + rel->r_addend;
                break;
                
            case R_ARM_GLOB_DAT:
            case R_AARCH64_GLOB_DAT:
            case R_X86_64_GLOB_DAT:
            case R_386_GLOB_DAT:
                /* Global data relocation */
                *rel_addr = sym_value;
                break;
                
            case R_ARM_JUMP_SLOT:
            case R_AARCH64_JUMP_SLOT:
            case R_X86_64_JUMP_SLOT:
            case R_386_JMP_SLOT:
                /* PLT relocation */
                *rel_addr = sym_value;
                break;
        }
    }
    
    return MINDYLD_OK;
}

/* ============================================================================
 * Main ELF Loading Function
 * ============================================================================ */

void *elf_load(const char *path)
{
    fd_t fd = open(path, O_RDONLY, 0);
    if (fd < 0)
        return NULL;
    
    /* Read and validate ELF header */
    elf64_header_t header;
    memset(&header, 0, sizeof(header));
    
    if (read(fd, &header, sizeof(header)) != sizeof(header)) {
        close(fd);
        return NULL;
    }
    
    if (elf_validate_header(&header) != MINDYLD_OK) {
        close(fd);
        return NULL;
    }
    
    /* Map segments into memory */
    void *base_addr = elf_map_segments(fd, &header);
    if (!base_addr) {
        close(fd);
        return NULL;
    }
    
    /* Create handle */
    mindyld_handle_t *handle = (mindyld_handle_t *)malloc(sizeof(mindyld_handle_t));
    if (!handle) {
        close(fd);
        return NULL;
    }
    
    handle->base_address = base_addr;
    handle->is_executable = (header.e_type == ET_EXEC) ? 1 : 0;
    handle->size = 0;  /* TODO: calculate total size */
    handle->next = NULL;
    
    close(fd);
    return handle;
}

mindyld_symbol_t *elf_find_symbol(mindyld_handle_t *handle, const char *name)
{
    /* This would need to read and parse the dynamic symbol table
       from the loaded binary. For now, return NULL. */
    (void)handle;
    (void)name;
    return NULL;
}

int elf_apply_relocations(mindyld_handle_t *handle)
{
    /* This would process all relocations in the binary.
       For now, return OK. */
    (void)handle;
    return MINDYLD_OK;
}
