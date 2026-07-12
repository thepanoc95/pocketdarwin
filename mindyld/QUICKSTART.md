# mindyld Quick Start Guide

Get mindyld running in 5 minutes!

## Installation

### 1. Clone/Navigate to mindyld

```bash
cd pocketdarwin/mindyld
```

### 2. Build

```bash
# Default build (auto-detects platform)
make all

# Or with debug symbols
make DEBUG=1 all

# Or optimized release
make release
```

### 3. Install (Optional)

```bash
sudo make install
```

## First Program

Create `example.c`:

```c
#include <mindyld.h>
#include <stdio.h>

int main(void) {
    mindyld_init();
    printf("mindyld loaded!\n");
    return 0;
}
```

Compile:

```bash
gcc -o example example.c -L. -lmindyld -Wl,-rpath,.
```

Run:

```bash
./example
```

## Load a Library

Create `load_lib.c`:

```c
#include <mindyld.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    mindyld_init();
    
    // Load the math library
    void *libm = mindyld_load("libm.so");
    if (!libm) {
        fprintf(stderr, "Failed to load libm\n");
        return 1;
    }
    
    printf("Successfully loaded libm!\n");
    
    // Try to resolve the sin function
    typedef double (*sin_func_t)(double);
    sin_func_t sin_func = (sin_func_t)mindyld_resolve_symbol("sin");
    
    if (sin_func) {
        double result = sin_func(3.14159 / 2.0);
        printf("sin(pi/2) = %f\n", result);
    } else {
        printf("sin() symbol not found\n");
    }
    
    // Cleanup
    mindyld_unload(libm);
    return 0;
}
```

Compile and run:

```bash
gcc -o load_lib load_lib.c -L. -lmindyld -Wl,-rpath,.
./load_lib
```

## Run Tests

```bash
cd tests
make test
```

## Common Tasks

### Load a Custom Library

```c
void *handle = mindyld_load("./libmycustom.so");
```

### Load from Standard Path

```c
void *handle = mindyld_load("libssl.so");
```

### Resolve a Symbol

```c
typedef int (*my_func_t)(void);
my_func_t my_func = (my_func_t)mindyld_resolve_symbol("my_function");

if (my_func) {
    int result = my_func();
}
```

### Unload Library

```c
mindyld_unload(handle);
```

### Debug Output

Build with debug:

```bash
make DEBUG=1 all
```

Then use in code:

```c
mindyld_debug_print("Loading: %s\n", path);
```

## Platform-Specific Notes

### Linux
- Uses ELF format
- Search paths: `/lib64`, `/usr/lib64`, `/lib`, `/usr/lib`

### macOS
- Uses Mach-O format
- Search paths: `/usr/local/lib`, `/usr/lib`

### BSD
- Uses ELF format
- Search paths: `/lib`, `/usr/lib`, `/usr/local/lib`

## Troubleshooting

### Build fails

```bash
make clean
make info          # See build configuration
make DEBUG=1 all   # Verbose build
```

### Library not found

```bash
# Check if library exists
ls -la /usr/lib/libm.so*

# Use absolute path
mindyld_load("/usr/lib/libm.so.6")
```

### Symbol not found

```bash
# List symbols in library
nm -D /usr/lib/libm.so

# Check symbol name is correct (case-sensitive)
```

### Memory errors

- Ensure library is valid ELF/Mach-O
- Check file permissions
- Verify no corruption

## Next Steps

1. **Integration Guide** - Read `INTEGRATION.md` for detailed examples
2. **API Reference** - Check `mindyld.h` for all functions
3. **Tests** - Study `tests/test_*.c` for more examples
4. **Documentation** - See `README.md` for full feature list

## Build Commands Reference

```bash
# Build everything
make all

# Debug build
make DEBUG=1 all

# Release build
make release

# Clean
make clean

# Install
make install

# Show info
make info

# Run tests
cd tests && make test

# Help
make help
```

## Performance Tips

1. Keep libraries in standard paths for faster search
2. Use absolute paths to avoid search overhead
3. Load libraries once, cache the handles
4. Resolve symbols once, cache the function pointers

## API Cheat Sheet

```c
#include <mindyld.h>

// Initialize
mindyld_init();

// Load library
void *h = mindyld_load("lib.so");

// Resolve symbol
void *addr = mindyld_resolve_symbol("func");

// Use symbol
typedef int (*func_t)(void);
func_t f = (func_t)addr;
int result = f();

// Unload
mindyld_unload(h);

// Debug
mindyld_debug_print("Message: %s\n", text);
```

## Examples Directory

See the `tests/` directory for:
- `test_basic.c` - Basic usage examples
- `test_advanced.c` - Advanced features
- `test_helper_lib.c` - Sample library

## Support

For issues or questions:
1. Check the documentation files
2. Review test code for examples
3. Enable debug mode for diagnostics
4. Examine source comments

## Happy Linking! 🔗

You're now ready to use mindyld. Happy dynamic linking!
