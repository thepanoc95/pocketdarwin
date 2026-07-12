# mindyld Integration Guide

This guide explains how to integrate mindyld with your custom minlibc and use it to load dynamic binaries across different platforms.

## Quick Start

### 1. Build mindyld

```bash
cd pocketdarwin/mindyld
make all
```

### 2. Link with minlibc

mindyld depends on minlibc functions. Ensure your project is linked against minlibc:

```bash
gcc -o myapp myapp.c \
    -L../minlibc -lSystem \
    -L. -lmindyld \
    -Wl,-rpath,.:../minlibc
```

### 3. Use in Your Code

```c
#include <mindyld.h>

int main(void) {
    mindyld_init();
    
    void *handle = mindyld_load("libexample.so");
    if (!handle) return 1;
    
    typedef void (*func_t)(void);
    func_t fn = (func_t)mindyld_resolve_symbol("my_function");
    if (fn) fn();
    
    mindyld_unload(handle);
    return 0;
}
```

## Platform Support

### Linux

**ELF Binary Format**
- Automatic detection and loading of ELF shared objects
- Support for standard library paths
- Symbol resolution from dynamic symbol tables

Build:
```bash
make all  # Automatically detects Linux
```

### BSD (FreeBSD, OpenBSD, NetBSD)

**ELF Binary Format**
- Similar to Linux but with BSD-specific syscalls
- Compatible with standard BSD library structure

Build:
```bash
make all  # Automatically detects BSD
```

### macOS

**Mach-O Binary Format**
- Support for 64-bit Mach-O dylib format
- Framework loading (coming soon)
- Code signing verification (planned)

Build:
```bash
make all  # Automatically detects macOS
```

## Architecture Support

mindyld automatically detects your architecture:

- **x86_64** (Intel 64-bit) - Most common on modern systems
- **x86** (Intel 32-bit) - Requires `-m32` flag
- **ARM64** (Apple Silicon, ARM v8) - macOS and modern Linux on ARM
- **ARM v7** (32-bit ARM) - Older ARM devices

Force architecture:
```bash
make ARCH_FLAGS="-m32" all  # 32-bit x86
make ARCH_FLAGS="-march=armv8-a" all  # ARM64
```

## API Reference

### Core Functions

#### `void *mindyld_load(const char *path)`

Load a shared library or executable binary.

**Parameters:**
- `path`: File path to the binary (absolute or relative)

**Returns:**
- Handle to loaded binary on success
- NULL on error

**Example:**
```c
void *handle = mindyld_load("/usr/lib/libm.so.6");
if (!handle) {
    perror("Failed to load libm");
}
```

#### `int mindyld_unload(void *handle)`

Unload a previously loaded binary.

**Parameters:**
- `handle`: Handle returned by `mindyld_load()`

**Returns:**
- 0 on success
- Negative error code on failure

**Example:**
```c
if (mindyld_unload(handle) != 0) {
    perror("Failed to unload");
}
```

#### `void *mindyld_resolve_symbol(const char *name)`

Resolve a symbol name to its runtime address.

**Parameters:**
- `name`: Name of the symbol to resolve

**Returns:**
- Address of the symbol on success
- NULL if symbol not found

**Example:**
```c
typedef int (*strlen_t)(const char *);
strlen_t my_strlen = (strlen_t)mindyld_resolve_symbol("strlen");

if (my_strlen) {
    int len = my_strlen("hello");  // Returns 5
}
```

#### `void mindyld_init(void)`

Initialize the dynamic linker. Called automatically on first use.

**Example:**
```c
mindyld_init();  // Explicit initialization
```

#### `void mindyld_debug_print(const char *fmt, ...)`

Debug output (only available if compiled with `DEBUG=1`).

**Example:**
```bash
make DEBUG=1 all
```

Then in code:
```c
mindyld_debug_print("Loading library from: %s\n", path);
```

## Error Handling

mindyld functions return standard error codes:

```c
#define MINDYLD_OK              0   /* Success */
#define MINDYLD_ERR_OPEN        -1  /* Cannot open file */
#define MINDYLD_ERR_READ        -2  /* Cannot read file */
#define MINDYLD_ERR_INVALID     -3  /* Invalid binary format */
#define MINDYLD_ERR_NO_MEMORY   -4  /* Out of memory */
#define MINDYLD_ERR_SYMBOL      -5  /* Symbol not found */
#define MINDYLD_ERR_RELOC       -6  /* Relocation failed */
#define MINDYLD_ERR_MMAP        -7  /* Memory mapping failed */
#define MINDYLD_ERR_PROT        -8  /* Memory protection failed */
```

Example error handling:
```c
void *handle = mindyld_load(path);
if (!handle) {
    switch ((intptr_t)handle) {
        case MINDYLD_ERR_OPEN:
            fprintf(stderr, "Cannot open %s\n", path);
            break;
        case MINDYLD_ERR_INVALID:
            fprintf(stderr, "%s is not a valid binary\n", path);
            break;
        default:
            fprintf(stderr, "Error loading %s\n", path);
    }
}
```

## Compilation Examples

### Simple Program

```c
// example.c
#include <mindyld.h>
#include <stdio.h>

int main(void) {
    mindyld_init();
    
    void *libm = mindyld_load("libm.so");
    if (libm) {
        typedef double (*sin_t)(double);
        sin_t sin_fn = (sin_t)mindyld_resolve_symbol("sin");
        
        if (sin_fn) {
            double result = sin_fn(3.14159 / 2);
            printf("sin(pi/2) = %f\n", result);  // Should be ~1.0
        }
        
        mindyld_unload(libm);
    }
    
    return 0;
}
```

Compile on Linux:
```bash
gcc -o example example.c \
    -I/path/to/mindyld \
    -L/path/to/mindyld -lmindyld \
    -Wl,-rpath,/path/to/mindyld
```

### Using with Build Systems

**CMake:**
```cmake
find_library(MINDYLD NAMES mindyld PATHS /path/to/mindyld)
target_link_libraries(myapp PRIVATE ${MINDYLD})
```

**Make:**
```makefile
LDFLAGS := -L/path/to/mindyld -lmindyld -Wl,-rpath,/path/to/mindyld
```

## Library Search Paths

mindyld automatically searches standard library locations:

**Linux:**
- `/lib64`, `/lib`
- `/usr/lib64`, `/usr/lib`
- `/usr/local/lib64`, `/usr/local/lib`
- `/opt/lib64`, `/opt/lib`

**BSD:**
- `/lib`, `/usr/lib`, `/usr/local/lib`, `/opt/lib`

**macOS:**
- `/usr/local/lib`, `/usr/lib`
- `/opt/local/lib`, `/opt/homebrew/lib`

You can specify the full path or use a relative path:
```c
mindyld_load("libexample.so");        // Searches standard paths
mindyld_load("./libexample.so");      // Current directory
mindyld_load("/usr/lib/libexample.so"); // Absolute path
```

## Advanced Usage

### Custom Function Pointers

```c
typedef struct {
    int (*init)(void);
    void (*cleanup)(void);
    int (*process)(int);
} plugin_api_t;

void load_plugin(const char *path) {
    void *handle = mindyld_load(path);
    
    plugin_api_t api = {
        .init = (int (*)(void))mindyld_resolve_symbol("plugin_init"),
        .cleanup = (void (*)(void))mindyld_resolve_symbol("plugin_cleanup"),
        .process = (int (*)(int))mindyld_resolve_symbol("plugin_process"),
    };
    
    if (api.init && api.cleanup && api.process) {
        api.init();
        api.process(42);
        api.cleanup();
    }
    
    mindyld_unload(handle);
}
```

### Runtime Library Detection

```c
void *load_library_optional(const char *name) {
    /* Try multiple library names/paths */
    const char *candidates[] = {
        name,
        "./lib",
        name,
        ".so",
        "/usr/lib/",
        name,
        ".so",
        NULL
    };
    
    for (int i = 0; candidates[i]; i += 3) {
        char path[256];
        snprintf(path, sizeof(path), "%s%s%s", 
                 candidates[i], candidates[i+1], candidates[i+2]);
        
        void *handle = mindyld_load(path);
        if (handle) return handle;
    }
    
    return NULL;
}
```

## Testing

Run the test suite:

```bash
cd mindyld/tests
make test
```

Individual tests:
```bash
make test-basic      # Basic functionality tests
make test-advanced   # Advanced feature tests
```

## Troubleshooting

### "Cannot open file"

- Verify the file path is correct
- Check file permissions (readable)
- Verify the file is not corrupted

### "Invalid binary format"

- Binary format doesn't match platform (e.g., ELF on macOS)
- File is corrupted or truncated
- Use `file` command to verify: `file /path/to/lib.so`

### "Symbol not found"

- Symbol may not be exported from the library
- Check symbol names are correct (case-sensitive)
- Use `nm` or `readelf` to list symbols: `nm libexample.so`

### "Memory mapping failed"

- System is out of memory
- Virtual address space exhausted
- Try reducing binary size or unloading unused libraries

## Performance Considerations

1. **Symbol Resolution**: Currently O(n) - each symbol lookup scans all loaded libraries
   - Future versions will use hash tables for O(1) lookups

2. **Library Loading**: O(n) where n is the binary size
   - Time proportional to amount of data that needs to be mapped

3. **Relocations**: O(m) where m is number of relocations
   - All relocations must be processed before library is usable

## Future Enhancements

- [ ] Lazy binding and PLT support
- [ ] Symbol caching and hash tables
- [ ] Thread safety with mutexes
- [ ] Full TLS (Thread Local Storage) support
- [ ] Framework loading on macOS
- [ ] Code signature verification
- [ ] ASLR support
- [ ] Circular dependency detection
- [ ] Version symbol support
- [ ] GNU extension symbols

## Contributing

Contributions are welcome! Areas for improvement:

1. Performance optimization
2. Additional platform support (Solaris, Haiku, etc.)
3. More comprehensive tests
4. Better error messages
5. Documentation improvements

## License

This project is part of pocketdarwin and follows its licensing terms.

## References

- [ELF Specification](http://www.sco.com/developers/gabi/latest/contents.html)
- [System V ABI](https://refspecs.linuxbase.org/elf/gabi4+/)
- [Mach-O Runtime Architecture](https://developer.apple.com/documentation/)
- [ARM ELF Supplement](https://github.com/ARM-software/abi-aa/)
