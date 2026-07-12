/*
 * mindyld - Mach-O Binary Loader Implementation
 * Supports loading Mach-O binaries on macOS
 */

#include "mindyld_macho.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * Mach-O Validation
 * ============================================================================ */

static int macho_validate_header(mach_header_64_t *header)
{
    /* Check magic number */
    if (header->magic != MH_MAGIC_64 && header->magic != MH_MAGIC) {
        return MINDYLD_ERR_INVALID;
    }
    
    /* Check file type - should be dylib or executable */
    if (header->filetype != MH_DYLIB && 
        header->filetype != MH_EXECUTE && 
        header->filetype != MH_DYLINKER &&
        header->filetype != MH_BUNDLE) {
        return MINDYLD_ERR_INVALID;
    }
    
    /* Verify sizeofcmds makes sense */
    if (header->sizeofcmds > 10 * 1024 * 1024) {  /* Sanity check: 10MB max */
        return MINDYLD_ERR_INVALID;
    }
    
    return MINDYLD_OK;
}

/* ============================================================================
 * Mach-O Memory Mapping
 * ============================================================================ */

static void *macho_map_segments(fd_t fd, mach_header_64_t *header)
{
    void *base_address = NULL;
    uint64_t min_addr = (uint64_t)-1;
    uint64_t max_addr = 0;
    
    /* Read load commands to find segments */
    char *load_commands = (char *)malloc(header->sizeofcmds);
    if (!load_commands)
        return NULL;
    
    /* Seek to load commands (right after header) */
    size_t header_size = (header->magic == MH_MAGIC_64) ? 
                         sizeof(mach_header_64_t) : 
                         sizeof(mach_header_t);
    
    if (lseek(fd, header_size, SEEK_SET) < 0) {
        free(load_commands);
        return NULL;
    }
    
    /* Read all load commands */
    if (read(fd, load_commands, header->sizeofcmds) != (ssize_t)header->sizeofcmds) {
        free(load_commands);
        return NULL;
    }
    
    /* First pass: find min/max addresses for all PT_SEGMENT_64 commands */
    load_command_t *cmd = (load_command_t *)load_commands;
    for (uint32_t i = 0; i < header->ncmds; i++) {
        if (cmd->cmd == LC_SEGMENT_64) {
            segment_command_64_t *seg = (segment_command_64_t *)cmd;
            if (seg->vmaddr < min_addr)
                min_addr = seg->vmaddr;
            if (seg->vmaddr + seg->vmsize > max_addr)
                max_addr = seg->vmaddr + seg->vmsize;
        }
        
        /* Move to next command */
        cmd = (load_command_t *)((char *)cmd + cmd->cmdsize);
    }
    
    /* Handle case where no segments found */
    if (min_addr == (uint64_t)-1) {
        free(load_commands);
        return NULL;
    }
    
    uint64_t total_size = max_addr - min_addr;
    
    /* Allocate memory for all segments */
    int mmap_flags = MAP_PRIVATE | MAP_ANONYMOUS;
    
    base_address = mmap(NULL, total_size, PROT_WRITE, mmap_flags, -1, 0);
    if (!base_address) {
        free(load_commands);
        return NULL;
    }
    
    /* Second pass: load all segments */
    cmd = (load_command_t *)load_commands;
    for (uint32_t i = 0; i < header->ncmds; i++) {
        if (cmd->cmd == LC_SEGMENT_64) {
            segment_command_64_t *seg = (segment_command_64_t *)cmd;
            
            /* Calculate load address relative to base */
            void *segment_addr = (char *)base_address + (seg->vmaddr - min_addr);
            
            /* Seek to segment data in file */
            if (lseek(fd, seg->fileoff, SEEK_SET) < 0) {
                munmap(base_address, total_size);
                free(load_commands);
                return NULL;
            }
            
            /* Read segment data from file */
            ssize_t read_bytes = read(fd, segment_addr, seg->filesize);
            if (read_bytes != (ssize_t)seg->filesize) {
                munmap(base_address, total_size);
                free(load_commands);
                return NULL;
            }
            
            /* Zero out BSS section if filesize < vmsize */
            if (seg->filesize < seg->vmsize) {
                char *bss_start = (char *)segment_addr + seg->filesize;
                size_t bss_size = seg->vmsize - seg->filesize;
                memset(bss_start, 0, bss_size);
            }
            
            /* Set appropriate page protections */
            int prot = PROT_NONE;
            if (seg->initprot & VM_PROT_READ)  prot |= PROT_READ;
            if (seg->initprot & VM_PROT_WRITE) prot |= PROT_WRITE;
            if (seg->initprot & VM_PROT_EXEC)  prot |= PROT_EXEC;
            
            if (mprotect(segment_addr, seg->vmsize, prot) < 0) {
                munmap(base_address, total_size);
                free(load_commands);
                return NULL;
            }
        }
        
        /* Move to next command */
        cmd = (load_command_t *)((char *)cmd + cmd->cmdsize);
    }
    
    free(load_commands);
    return base_address;
}

/* ============================================================================
 * Mach-O Symbol Resolution
 * ============================================================================ */

static mindyld_symbol_t *macho_find_symbol_in_symtab(
    mindyld_handle_t *handle,
    const char *name,
    nlist_64_t *symtab,
    uint32_t symtab_size,
    char *strtab)
{
    uint32_t num_symbols = symtab_size / sizeof(nlist_64_t);
    
    for (uint32_t i = 0; i < num_symbols; i++) {
        nlist_64_t *sym = &symtab[i];
        
        /* Get symbol name from string table */
        char *sym_name = strtab + sym->n_strx;
        
        /* Skip leading underscore (Mach-O convention) */
        if (name[0] != '_' && sym_name[0] == '_')
            sym_name++;
        
        /* Check if name matches */
        if (strcmp(sym_name, name) == 0) {
            mindyld_symbol_t *result = (mindyld_symbol_t *)malloc(sizeof(mindyld_symbol_t));
            if (result) {
                result->name = sym_name;
                result->address = (char *)handle->base_address + sym->n_value;
                result->size = 0;  /* Mach-O doesn't store size in n_list */
                result->binding = (sym->n_desc >> 8) & 0xF;
                result->type = sym->n_type & 0x0E;
            }
            return result;
        }
    }
    
    return NULL;
}

/* ============================================================================
 * Mach-O Relocation Processing
 * ============================================================================ */

static int macho_process_relocations(
    mindyld_handle_t *handle,
    relocation_info_t *relocs,
    uint32_t reloc_count)
{
    for (uint32_t i = 0; i < reloc_count; i++) {
        relocation_info_t *rel = &relocs[i];
        
        /* Get relocation target address */
        uint64_t *rel_addr = (uint64_t *)((char *)handle->base_address + rel->r_address);
        
        /* Determine relocation type and CPU architecture */
        uint32_t rel_type = rel->r_type;
        
        /* Apply relocation based on type and architecture */
        switch (rel_type) {
            case X86_64_RELOC_UNSIGNED:
                /* Unsigned relocation - just copy value */
                break;
                
            case X86_64_RELOC_SUBTRACTOR:
                /* Subtractor relocation */
                break;
                
            case X86_64_RELOC_BRANCH:
                /* Branch relocation */
                break;
                
            case ARM64_RELOC_UNSIGNED:
                /* ARM64 unsigned */
                break;
                
            case ARM64_RELOC_SUBTRACTOR:
                /* ARM64 subtractor */
                break;
        }
    }
    
    return MINDYLD_OK;
}

/* ============================================================================
 * Main Mach-O Loading Function
 * ============================================================================ */

void *macho_load(const char *path)
{
    fd_t fd = open(path, O_RDONLY, 0);
    if (fd < 0)
        return NULL;
    
    /* Read and validate Mach-O header */
    mach_header_64_t header;
    memset(&header, 0, sizeof(header));
    
    if (read(fd, &header, sizeof(header)) != sizeof(header)) {
        close(fd);
        return NULL;
    }
    
    if (macho_validate_header(&header) != MINDYLD_OK) {
        close(fd);
        return NULL;
    }
    
    /* Map segments into memory */
    void *base_addr = macho_map_segments(fd, &header);
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
    handle->is_executable = (header.filetype == MH_EXECUTE) ? 1 : 0;
    handle->size = 0;  /* TODO: calculate total size */
    handle->next = NULL;
    
    close(fd);
    return handle;
}

mindyld_symbol_t *macho_find_symbol(mindyld_handle_t *handle, const char *name)
{
    /* This would need to read and parse the symbol table
       from the loaded binary. For now, return NULL. */
    (void)handle;
    (void)name;
    return NULL;
}

int macho_apply_relocations(mindyld_handle_t *handle)
{
    /* This would process all relocations in the binary.
       For now, return OK. */
    (void)handle;
    return MINDYLD_OK;
}
