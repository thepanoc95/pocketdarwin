/*
 * mindyld - Test Program
 * Demonstrates basic dynamic loading and symbol resolution
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../mindyld.h"

/* ============================================================================
 * Simple Test Cases
 * ============================================================================ */

int test_basic_load(void)
{
    printf("Test: Basic Library Loading\n");
    printf("============================\n");
    
    /* Try to load the C math library */
    void *libm = mindyld_load("libm.so.6");
    if (!libm) {
        printf("FAIL: Could not load libm.so.6\n");
        return 1;
    }
    
    printf("PASS: Successfully loaded libm.so.6\n");
    
    /* Try to resolve the sin() function */
    typedef double (*sin_func_t)(double);
    sin_func_t sin_func = (sin_func_t)mindyld_resolve_symbol("sin");
    
    if (!sin_func) {
        printf("FAIL: Could not resolve 'sin' symbol\n");
        return 1;
    }
    
    printf("PASS: Successfully resolved 'sin' symbol\n");
    
    /* Unload the library */
    if (mindyld_unload(libm) != 0) {
        printf("FAIL: Could not unload libm.so.6\n");
        return 1;
    }
    
    printf("PASS: Successfully unloaded libm.so.6\n\n");
    return 0;
}

int test_symbol_resolution(void)
{
    printf("Test: Symbol Resolution\n");
    printf("=======================\n");
    
    /* Load libc */
    void *libc = mindyld_load("libc.so.6");
    if (!libc) {
        printf("FAIL: Could not load libc.so.6\n");
        return 1;
    }
    
    /* Try to resolve common functions */
    const char *symbols[] = {
        "printf",
        "malloc",
        "free",
        "strlen",
        NULL
    };
    
    int resolved_count = 0;
    for (int i = 0; symbols[i]; i++) {
        void *addr = mindyld_resolve_symbol(symbols[i]);
        if (addr) {
            printf("PASS: Resolved '%s' at %p\n", symbols[i], addr);
            resolved_count++;
        } else {
            printf("FAIL: Could not resolve '%s'\n", symbols[i]);
        }
    }
    
    mindyld_unload(libc);
    
    if (resolved_count == 4) {
        printf("PASS: All 4 symbols resolved\n\n");
        return 0;
    } else {
        printf("FAIL: Only %d/%d symbols resolved\n\n", resolved_count, 4);
        return 1;
    }
}

int test_multiple_libraries(void)
{
    printf("Test: Multiple Library Loading\n");
    printf("===============================\n");
    
    /* Load multiple libraries */
    void *libc = mindyld_load("libc.so.6");
    void *libm = mindyld_load("libm.so.6");
    
    if (!libc || !libm) {
        printf("FAIL: Could not load libraries\n");
        return 1;
    }
    
    printf("PASS: Loaded libc and libm\n");
    
    /* Resolve symbols from different libraries */
    void *malloc_addr = mindyld_resolve_symbol("malloc");
    void *sin_addr = mindyld_resolve_symbol("sin");
    
    if (!malloc_addr || !sin_addr) {
        printf("FAIL: Could not resolve symbols from loaded libraries\n");
        return 1;
    }
    
    printf("PASS: malloc = %p, sin = %p\n", malloc_addr, sin_addr);
    
    /* Unload libraries */
    mindyld_unload(libm);
    mindyld_unload(libc);
    
    printf("PASS: Successfully unloaded all libraries\n\n");
    return 0;
}

int test_invalid_path(void)
{
    printf("Test: Invalid Path Handling\n");
    printf("===========================\n");
    
    /* Try to load non-existent library */
    void *invalid = mindyld_load("/nonexistent/library/libnope.so");
    
    if (invalid) {
        printf("FAIL: Should not have loaded non-existent library\n");
        return 1;
    }
    
    printf("PASS: Correctly rejected non-existent library\n\n");
    return 0;
}

/* ============================================================================
 * Test Runner
 * ============================================================================ */

int main(int argc, char *argv[], char *envp[])
{
    (void)argc;
    (void)argv;
    (void)envp;
    
    printf("\nmindyld Test Suite\n");
    printf("==================\n\n");
    
    /* Initialize mindyld */
    mindyld_init();
    printf("Initialized mindyld\n\n");
    
    int failed = 0;
    
    /* Run tests */
    if (test_invalid_path())
        failed++;
    
    if (test_basic_load())
        failed++;
    
    if (test_symbol_resolution())
        failed++;
    
    if (test_multiple_libraries())
        failed++;
    
    /* Summary */
    printf("Test Summary\n");
    printf("============\n");
    if (failed == 0) {
        printf("All tests passed!\n");
        return 0;
    } else {
        printf("%d test(s) failed\n", failed);
        return 1;
    }
}
