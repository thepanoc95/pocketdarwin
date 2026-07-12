/*
 * mindyld - Core Dynamic Linker Implementation
 * Main loader logic and symbol resolution
 */

#include "mindyld_common.h"
#include "mindyld_elf.h"
#include "mindyld_macho.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

/* ============================================================================
 * Global Linker State
 * ============================================================================ */

mindyld_linker_map_t _mindyld_linker_map = {
    .loaded_objects = NULL,
    .dependencies = NULL,
    .num_dependencies = 0,
    .last_error = NULL
};

/* ============================================================================
 * Platform Detection & Format Selection
 * ============================================================================ */

static int detect_binary_format(const char *path)
{
    fd_t fd = open(path, O_RDONLY, 0);
    if (fd < 0)
        return MINDYLD_ERR_OPEN;
    
    /* Read first few bytes to detect format */
    unsigned char magic[4];
    if (read(fd, magic, 4) != 4) {
        close(fd);
        return MINDYLD_ERR_INVALID;
    }
    
    close(fd);
    
    /* Check for ELF magic */
    if (magic[0] == 0x7f && magic[1] == 'E' && magic[2] == 'L' && magic[3] == 'F') {
        #ifdef MINDYLD_ELF
            return 1;  /* ELF format */
        #else
            return MINDYLD_ERR_INVALID;
        #endif
    }
    
    /* Check for Mach-O magic (little-endian) */
    if (magic[0] == 0xfe && magic[1] == 0xed && magic[2] == 0xfa && magic[3] == 0xcf) {
        #ifdef MINDYLD_MACHO
            return 2;  /* Mach-O 64-bit format */
        #else
            return MINDYLD_ERR_INVALID;
        #endif
    }
    
    /* Check for Mach-O magic (big-endian, unlikely on modern systems) */
    if (magic[0] == 0xcf && magic[1] == 0xfa && magic[2] == 0xed && magic[3] == 0xfe) {
        #ifdef MINDYLD_MACHO
            return 2;  /* Mach-O 64-bit format */
        #else
            return MINDYLD_ERR_INVALID;
        #endif
    }
    
    return MINDYLD_ERR_INVALID;
}

/* ============================================================================
 * Library Search and Loading
 * ============================================================================ */

static const char *standard_search_paths[] = {
    #ifdef MINDYLD_LINUX
        "/lib",
        "/lib64",
        "/usr/lib",
        "/usr/lib64",
        "/usr/local/lib",
        "/usr/local/lib64",
        "/opt/lib",
        "/opt/lib64",
    #endif
    #ifdef MINDYLD_BSD
        "/lib",
        "/usr/lib",
        "/usr/local/lib",
        "/opt/lib",
    #endif
    #ifdef MINDYLD_APPLE
        "/usr/local/lib",
        "/usr/lib",
        "/opt/local/lib",
    #endif
    NULL
};

static char *find_library(const char *libname)
{
    static char resolved_path[4096];
    
    /* Try direct path first */
    if (access(libname, F_OK) == 0) {
        strcpy(resolved_path, libname);
        return resolved_path;
    }
    
    /* Search in standard paths */
    for (int i = 0; standard_search_paths[i]; i++) {
        char *search_path = (char *)standard_search_paths[i];
        
        /* Try lib + libname.so / .dylib */
        strcpy(resolved_path, search_path);
        strcat(resolved_path, "/");
        strcat(resolved_path, libname);
        
        #ifdef MINDYLD_ELF
            strcat(resolved_path, ".so");
        #endif
        #ifdef MINDYLD_MACHO
            strcat(resolved_path, ".dylib");
        #endif
        
        if (access(resolved_path, F_OK) == 0) {
            return resolved_path;
        }
        
        /* Reset for next try */
        strcpy(resolved_path, search_path);
        strcat(resolved_path, "/");
        strcat(resolved_path, libname);
        
        if (access(resolved_path, F_OK) == 0) {
            return resolved_path;
        }
    }
    
    return NULL;
}

/* ============================================================================
 * Object Tracking
 * ============================================================================ */

static void register_loaded_object(mindyld_handle_t *handle)
{
    if (!handle)
        return;
    
    /* Find end of list */
    mindyld_handle_t *current = _mindyld_linker_map.loaded_objects;
    if (!current) {
        _mindyld_linker_map.loaded_objects = handle;
        return;
    }
    
    while (current->next)
        current = current->next;
    
    /* Append to list */
    current->next = handle;
}

/* ============================================================================
 * Core Loading Functions
 * ============================================================================ */

void *mindyld_load(const char *path)
{
    if (!path)
        return NULL;
    
    /* Detect binary format */
    int format = detect_binary_format(path);
    
    mindyld_handle_t *handle = NULL;
    
    #ifdef MINDYLD_ELF
    if (format == 1) {
        handle = (mindyld_handle_t *)elf_load(path);
        if (handle) {
            elf_apply_relocations(handle);
            register_loaded_object(handle);
        }
    }
    #endif
    
    #ifdef MINDYLD_MACHO
    if (format == 2) {
        handle = (mindyld_handle_t *)macho_load(path);
        if (handle) {
            macho_apply_relocations(handle);
            register_loaded_object(handle);
        }
    }
    #endif
    
    return handle;
}

int mindyld_unload(void *handle_ptr)
{
    if (!handle_ptr)
        return MINDYLD_ERR_INVALID;
    
    mindyld_handle_t *handle = (mindyld_handle_t *)handle_ptr;
    
    /* Unmap memory */
    if (handle->base_address && handle->size > 0) {
        munmap(handle->base_address, handle->size);
    }
    
    /* Remove from linked list */
    mindyld_handle_t *current = _mindyld_linker_map.loaded_objects;
    mindyld_handle_t *prev = NULL;
    
    while (current) {
        if (current == handle) {
            if (prev) {
                prev->next = current->next;
            } else {
                _mindyld_linker_map.loaded_objects = current->next;
            }
            break;
        }
        prev = current;
        current = current->next;
    }
    
    /* Free handle */
    free(handle);
    
    return MINDYLD_OK;
}

void *mindyld_resolve_symbol(const char *name)
{
    if (!name)
        return NULL;
    
    /* Search through all loaded objects */
    mindyld_handle_t *handle = _mindyld_linker_map.loaded_objects;
    
    while (handle) {
        mindyld_symbol_t *symbol = NULL;
        
        #ifdef MINDYLD_ELF
            symbol = elf_find_symbol(handle, name);
        #endif
        
        #ifdef MINDYLD_MACHO
            symbol = macho_find_symbol(handle, name);
        #endif
        
        if (symbol) {
            void *addr = (void *)symbol->address;
            free(symbol);
            return addr;
        }
        
        handle = handle->next;
    }
    
    return NULL;
}

/* ============================================================================
 * Initialization
 * ============================================================================ */

void mindyld_init(void)
{
    /* Initialize linker map */
    _mindyld_linker_map.loaded_objects = NULL;
    _mindyld_linker_map.dependencies = NULL;
    _mindyld_linker_map.num_dependencies = 0;
    
    mindyld_debug_print("mindyld initialized\n");
}

/* ============================================================================
 * Debugging Support
 * ============================================================================ */

void mindyld_debug_print(const char *fmt, ...)
{
    /* Simple debug output to stderr */
    #ifdef MINDYLD_DEBUG
        va_list args;
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        va_end(args);
    #else
        (void)fmt;
    #endif
}

/* ============================================================================
 * Entry Point for mindyld as a dynamic linker
 * ============================================================================ */

/* This would be called as the interpreter when mindyld is set as the
   dynamic linker for a binary */
void _mindyld_start(void)
{
    mindyld_init();
    
    /* TODO: Parse command line, load main binary, load dependencies,
       perform relocations, and jump to entry point */
}
