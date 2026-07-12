/*
 * mindyld - Custom Dynamic Linker/Loader
 * Common header with cross-platform types and constants
 *
 * Supports: macOS (Mach-O), Linux (ELF), BSD (ELF)
 */

#ifndef _MINDYLD_COMMON_H
#define _MINDYLD_COMMON_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Platform Detection
 * ============================================================================ */

#if defined(__APPLE__)
  #define MINDYLD_MACHO 1
  #define MINDYLD_APPLE 1
#elif defined(__linux__)
  #define MINDYLD_ELF 1
  #define MINDYLD_LINUX 1
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
  #define MINDYLD_ELF 1
  #define MINDYLD_BSD 1
#else
  #error "Unsupported platform: mindyld only supports macOS, Linux, and BSD"
#endif

/* ============================================================================
 * Basic Types
 * ============================================================================ */

typedef int32_t               elf32_sword;
typedef uint32_t              elf32_word;
typedef int64_t               elf64_sxword;
typedef uint64_t              elf64_xword;
typedef uint8_t               uint8;
typedef uint16_t              uint16;
typedef uint32_t              uint32;
typedef uint64_t              uint64;
typedef int8_t                int8;
typedef int16_t               int16;
typedef int32_t               int32;
typedef int64_t               int64;

/* File descriptor type */
typedef int fd_t;

/* Handle to a loaded library/binary */
typedef struct mindyld_handle {
    void *base_address;
    size_t size;
    int is_executable;
    struct mindyld_handle *next;
} mindyld_handle_t;

/* Symbol information */
typedef struct {
    const char *name;
    uint64_t address;
    uint32_t size;
    int binding;  /* Local, Global, Weak */
    int type;     /* Function, Object, etc */
} mindyld_symbol_t;

/* Relocation information */
typedef struct {
    uint64_t offset;
    uint32_t type;
    uint32_t symbol_index;
    int64_t addend;
} mindyld_relocation_t;

/* Dynamic library dependencies */
typedef struct {
    const char *name;
    void *handle;
} mindyld_dependency_t;

/* ============================================================================
 * Linker Map - tracks all loaded objects
 * ============================================================================ */

typedef struct {
    mindyld_handle_t *loaded_objects;
    mindyld_dependency_t *dependencies;
    uint32_t num_dependencies;
    void *last_error;
} mindyld_linker_map_t;

/* Global linker state */
extern mindyld_linker_map_t _mindyld_linker_map;

/* ============================================================================
 * Error Handling
 * ============================================================================ */

#define MINDYLD_OK              0
#define MINDYLD_ERR_OPEN        -1
#define MINDYLD_ERR_READ        -2
#define MINDYLD_ERR_INVALID     -3
#define MINDYLD_ERR_NO_MEMORY   -4
#define MINDYLD_ERR_SYMBOL      -5
#define MINDYLD_ERR_RELOC       -6
#define MINDYLD_ERR_MMAP        -7
#define MINDYLD_ERR_PROT        -8

/* ============================================================================
 * Function Declarations
 * ============================================================================ */

/* Core loading */
void *mindyld_load(const char *path);
int mindyld_unload(void *handle);
void *mindyld_resolve_symbol(const char *name);

/* Platform-specific functions */
#ifdef MINDYLD_ELF
  void *mindyld_load_elf(const char *path);
#endif

#ifdef MINDYLD_MACHO
  void *mindyld_load_macho(const char *path);
#endif

/* Relocation processing */
int mindyld_apply_relocations(mindyld_handle_t *handle);

/* Symbol resolution */
mindyld_symbol_t *mindyld_find_symbol(const char *name);

/* Initialization */
void mindyld_init(void);

/* Debugging */
void mindyld_debug_print(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* _MINDYLD_COMMON_H */
