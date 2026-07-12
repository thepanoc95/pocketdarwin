# mindyld - Custom Dynamic Linker/Loader

A cross-platform dynamic linker/loader (dyld replacement) written in C with minimal dependencies. Designed to work with the custom minlibc on Linux, FreeBSD, OpenBSD, NetBSD, and macOS.

## Features

- **Cross-Platform Support**
  - Linux (ELF binaries)
  - BSD variants: FreeBSD, OpenBSD, NetBSD (ELF binaries)
  - macOS (Mach-O binaries)

- **Multi-Architecture**
  - x86 (32-bit)
  - x86_64 (64-bit)
  - ARM v7 (32-bit)
  - ARM64 (64-bit AArch64)

- **Binary Format Support**
  - ELF (Executable and Linkable Format) - for Linux and BSD
  - Mach-O (Mach Object Format) - for macOS

- **Core Features**
  - Dynamic binary loading and unloading
  - Symbol resolution
  - Relocation processing
  - Memory protection and mapping
  - Dependency tracking

## Architecture

```
mindyld/
├── mindyld.h              # Public API header
├── mindyld_common.h       # Common types and constants
├── mindyld_elf.h          # ELF format definitions
├── mindyld_macho.h        # Mach-O format definitions
├── mindyld_core.c         # Core loader logic
├── elf_loader.c           # ELF binary loading implementation
├── macho_loader.c         # Mach-O binary loading implementation
├── Makefile               # Cross-platform build system
└── README.md              # This file
```

## Usage

### Basic Usage

```c
#include <mindyld.h>

int main(void) {
    // Initialize mindyld
    mindyld_init();
    
    // Load a library
    void *handle = mindyld_load("/path/to/libexample.so");
    
    if (!handle) {
        // Error loading library
        return 1;
    }
    
    // Resolve a symbol
    typedef int (*example_func_t)(void);
    example_func_t example_func = (example_func_t)mindyld_resolve_symbol("example_function");
    
    if (example_func) {
        int result = example_func();
    }
    
    // Unload library
    mindyld_unload(handle);
    
    return 0;
}
```

### Linking

For ELF systems (Linux, BSD):
```bash
gcc -o myapp myapp.c -L/path/to/mindyld -lmindyld -Wl,-rpath,/path/to/mindyld
```

For macOS:
```bash
clang -o myapp myapp.c -L/path/to/mindyld -lmindyld -Wl,-rpath,/path/to/mindyld
```

## Building

### Prerequisites

- C compiler (gcc or clang)
- make
- Standard POSIX development environment

### Build Targets

```bash
# Build both static and shared libraries (default)
make all

# Debug build with symbols
make debug

# Optimized release build
make release

# Run tests
make test

# Install to /usr/local
make install

# Clean build artifacts
make clean

# Show build configuration
make info
```

### Build Examples

Linux x86_64:
```bash
make clean
make all
```

macOS ARM64 (Apple Silicon):
```bash
make clean
make all CC=clang ARCH_FLAGS="-march=armv8-a"
```

Linux x86 (32-bit):
```bash
make clean
make all ARCH_FLAGS="-m32"
```

## Implementation Details

### ELF Loading Process

1. **Validation**: Check ELF magic number and header structure
2. **Segment Mapping**: Map all PT_LOAD segments into memory
3. **Symbol Resolution**: Load symbol tables from dynamic sections
4. **Relocations**: Process all relocation entries (REL and RELA)
5. **Protection**: Set correct memory permissions (RWX)
6. **Execution**: Return handle for symbol lookups

### Mach-O Loading Process

1. **Validation**: Check Mach-O magic number and file type
2. **Segment Mapping**: Map all LC_SEGMENT_64 segments
3. **Symbol Tables**: Load symbol tables from LC_SYMTAB
4. **Relocations**: Process relocation entries
5. **Memory Protection**: Set segment memory permissions
6. **Execution**: Return handle for use

### Symbol Resolution

Symbols are resolved using:
1. Dynamic symbol table (`.dynsym` in ELF, symbol table in Mach-O)
2. String table lookup for symbol names
3. Binding information (local, global, weak)
4. Scope tracking across loaded libraries

### Relocation Processing

Supports multiple relocation types:

**ELF Relocations:**
- R_ARM_RELATIVE, R_AARCH64_RELATIVE, R_X86_64_RELATIVE
- R_ARM_ABS32, R_AARCH64_ABS64, R_X86_64_64
- R_ARM_GLOB_DAT, R_AARCH64_GLOB_DAT, R_X86_64_GLOB_DAT
- R_ARM_JUMP_SLOT, R_AARCH64_JUMP_SLOT, R_X86_64_JUMP_SLOT

**Mach-O Relocations:**
- X86_64_RELOC_UNSIGNED, X86_64_RELOC_RELATIVE
- ARM64_RELOC_UNSIGNED, ARM64_RELOC_RELATIVE
- And more...

## Integration with minlibc

mindyld depends on minlibc for basic system services:

- Memory operations: `mmap()`, `munmap()`, `mprotect()`
- File I/O: `open()`, `read()`, `close()`, `lseek()`
- Process info: `getpid()`, `getuid()`, etc.
- Memory operations: `malloc()`, `free()`, `memcpy()`
- String operations: `strcmp()`, `strcpy()`, `strlen()`

All these functions are provided by the custom minlibc in `../minlibc/`.

## Limitations

### Current Implementation

- No lazy binding/PLT support yet
- Limited TLSLE (Thread Local Storage) support
- No versioning symbols support
- No GNU hash table optimization
- Basic symbol resolution (first match only)
- No circular dependency detection

### Future Improvements

1. **Lazy Binding**: Implement PLT stubs for lazy symbol resolution
2. **Thread Safety**: Add mutex protection for symbol table access
3. **TLS Support**: Full Thread Local Storage implementation
4. **ASLR**: Address Space Layout Randomization support
5. **Debugging**: Enhanced debug symbol support
6. **Performance**: Symbol caching and hash table optimization

## Platform-Specific Notes

### Linux

- Uses ELF format
- Supports glibc, musl, and other libc implementations
- Tested on x86, x86_64, ARM, ARM64

### BSD

- FreeBSD, OpenBSD, NetBSD all use ELF format
- Similar ABI to Linux but with BSD-specific system calls
- Requires BSD-compatible syscall numbers

### macOS

- Uses Mach-O format
- Supports both 32-bit and 64-bit binaries
- Weak imports and framework support planned

## Performance Characteristics

- **Load Time**: O(n) where n is the size of binary sections
- **Symbol Resolution**: O(m) where m is number of symbols (linear search)
- **Memory Usage**: Minimal - only necessary data structures

Future versions will optimize symbol resolution to O(log m) using hash tables.

## Testing

Test programs are in the `tests/` directory:

```bash
make test
```

## Contributing

Contributions are welcome! Areas for improvement:

1. Performance optimization
2. Additional platform support
3. Better error handling and reporting
4. Comprehensive test suite
5. Documentation improvements

## License

This project follows the licensing of the pocketdarwin project.
The Apple code references are used for compatibility and educational purposes.

## References

- [ELF Specification](http://www.sco.com/developers/gabi/latest/contents.html)
- [Mach-O File Format](https://opensource.apple.com/source/xnu/)
- [System V ABI](https://refspecs.linuxbase.org/elf/gabi4+/)
- [ARM ELF Supplement](https://github.com/ARM-software/abi-aa/releases)
- [Apple Mach-O Runtime Architecture](https://developer.apple.com/documentation/)

## Support

For issues, questions, or contributions, please refer to the main pocketdarwin project.
