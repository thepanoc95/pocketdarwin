/*
 * mindyld.h - Public API for Dynamic Linker/Loader
 * 
 * Usage:
 *   #include <mindyld.h>
 *   
 *   void *handle = mindyld_load("libexample.so");
 *   typedef int (*func_t)(void);
 *   func_t fn = (func_t)mindyld_resolve_symbol("example_function");
 *   int result = fn();
 *   mindyld_unload(handle);
 */

#ifndef _MINDYLD_H
#define _MINDYLD_H

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Function Types
 * ============================================================================ */

/* Load a shared library or executable
 * 
 * path: Path to the binary file to load
 * 
 * Returns: Handle to loaded binary, or NULL on error
 */
void *mindyld_load(const char *path);

/* Unload a previously loaded binary
 * 
 * handle: Handle returned by mindyld_load()
 * 
 * Returns: 0 on success, negative on error
 */
int mindyld_unload(void *handle);

/* Resolve a symbol name to its address
 * 
 * name: Name of the symbol to resolve
 * 
 * Returns: Address of the symbol, or NULL if not found
 */
void *mindyld_resolve_symbol(const char *name);

/* Initialize the dynamic linker
 * 
 * Called automatically on first use, but can be called explicitly
 */
void mindyld_init(void);

/* Debug printing (only available if compiled with DEBUG=1)
 * 
 * fmt: Printf-style format string
 */
void mindyld_debug_print(const char *fmt, ...);

/* ============================================================================
 * Error Codes
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

#ifdef __cplusplus
}
#endif

#endif /* _MINDYLD_H */
