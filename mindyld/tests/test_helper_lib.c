/*
 * mindyld - Test Helper Library
 * A simple library to test dynamic loading
 * 
 * Compile with:
 *   gcc -fPIC -shared -o test_helper.so test_helper_lib.c
 */

/* ============================================================================
 * Helper Functions
 * ============================================================================ */

/**
 * Add two integers
 * @param a First number
 * @param b Second number
 * @return a + b
 */
int helper_add(int a, int b)
{
    return a + b;
}

/**
 * Multiply two integers
 * @param a First number
 * @param b Second number
 * @return a * b
 */
int helper_multiply(int a, int b)
{
    return a * b;
}

/**
 * Get library version string
 * @return Version string
 */
const char *helper_get_version(void)
{
    return "Helper Library v1.0";
}

/**
 * Get library build info
 * @return Build info string
 */
const char *helper_get_buildinfo(void)
{
    return "Built with mindyld test suite";
}

/**
 * Simple string manipulation
 * @param str Input string
 * @return Length of string
 */
int helper_strlen(const char *str)
{
    int len = 0;
    if (str) {
        while (str[len] != '\0')
            len++;
    }
    return len;
}

/**
 * Get a constant value
 * @return Constant value for testing
 */
int helper_get_constant(void)
{
    return 42;
}

/* ============================================================================
 * Data Symbols (for testing data relocations)
 * ============================================================================ */

const char *helper_library_name = "mindyld-test-helper";
int helper_build_number = 1;
double helper_pi_approx = 3.14159265359;
