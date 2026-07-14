# Linux Kernel Modules with GNUKext

Building kernel extensions on Linux using the GNUKext framework.

## Overview

On Linux, GNUKext provides compatibility with the kernel module system. The framework allows you to:

- Write kernel modules that share code with Darwin kexts
- Use the same Info.plist-based configuration
- Load and unload modules programmatically
- Access kernel module information through the same API

## Linux-Specific Types

### LinuxModule

The Linux kernel module wrapper:

```objc
#import <GNUKext/LinuxModule.h>

LinuxModule *module = [LinuxModule moduleWithPath:@"/lib/modules/5.15.0/mydriver.ko"];
if ([module load]) {
    NSLog(@"Module loaded: %@", module.moduleName);
}
```

### KextInfo

Cross-platform kext information structure (defined in `libkern.h`):

```objc
KextInfoRef info = KextInfoCreateFromPath("/path/to/module");
KextInfoLoad(info);
KextInfoUnload(info);
KextInfoFree(info);
```

## Module Loading

Linux modules are loaded through the kernel module interface:

```objc
// Load a kernel module
if ([LinuxModule loadModuleAtPath:@"/lib/modules/$(uname -r)/extra/mydriver.ko"]) {
    NSLog(@"Module loaded successfully");
}

// Check module status
BOOL loaded = [LinuxModule isModuleLoaded:@"mydriver"];

// Get module information
NSString *version = [LinuxModule kernelVersion];
NSArray *loadedModules = [LinuxModule allLoadedModules];
```

## Building Linux Modules

### GNUmakefile Integration

```makefile
include $(GNUSTEP_MAKEFILES)/common.make

PACKAGE_NAME = mydriver
MODULE_EXTENSION = ko

# Linux kernel module sources
MODULE_C_FILES = driver.c

# Kernel headers and flags
ADDITIONAL_CFLAGS += -I/lib/modules/$(shell uname -r)/build/include
ADDITIONAL_LDFLAGS += -nostdlib

include $(GNUSTEP_MAKEFILES)/Makefiles/kext.make
```

### With Kotlin Support

Use the kext.make rules for Kotlin-based Linux modules:

```makefile
ENABLE_KOTLIN_KEXT = yes
KEXT_EXT = ko
include $(GNUSTEP_MAKEFILES)/Makefiles/kext.make
```

## Module Structure

Linux kernel modules follow the standard pattern:

```c
#include <linux/module.h>
#include <linux/kernel.h>

static int __init mydriver_init(void) {
    printk(KERN_INFO "mydriver: loaded\n");
    return 0;
}

static void __exit mydriver_exit(void) {
    printk(KERN_INFO "mydriver: unloaded\n");
}

module_init(mydriver_init);
module_exit(mydriver_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("My Kernel Driver");
```

## Info.plist for Linux Modules

Linux modules can use the same Info.plist format as Darwin kexts:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<plist version="1.0">
<dict>
    <key>CFBundleIdentifier</key>
    <string>com.example.mydriver</string>
    
    <key>CFBundleName</key>
    <string>MyDriver</string>
    
    <key>IOKitPersonalities</key>
    <dict>
        <key>MyDriver</key>
        <dict>
            <key>IOClass</key>
            <string>MyDriver</string>
            <key>IOProbeScore</key>
            <integer>100</integer>
        </dict>
    </dict>
</dict>
</plist>
```

## Personality Matching on Linux

On Linux, personality matching works against the sysfs device tree:

```objc
GNUKextRegistryEntry *root = [GNUKextRegistryEntry rootEntry];
NSArray *devices = [root childEntries];

for (GNUKextRegistryEntry *device in devices) {
    NSString *vendor = [device propertyForKey:@"vendor"];
    NSString *deviceId = [device propertyForKey:@"device"];
    
    GNUKextPersonality *personality = [GNUKextPersonality 
        personalityWithName:@"MyDriver"
        dictionary:@{@"IOProviderClass": @"pci"}];
    
    if ([personality matchesProvider:device]) {
        // Device matches - instantiate driver
    }
}
```

## Installation Paths

```objc
// Default module installation directory
NSString *modulesPath = [libkernDriver defaultModulesPath];

// Install module
[libkernDriver writeDriverPlist:plist toPath:modulesPath];
```

## Error Codes

Linux-specific error codes are defined in `libkern.h`:

```objc
typedef enum {
    kKextLoadResultSuccess = 0,
    kKextLoadResultNotFound,
    kKextLoadResultInvalidInfo,
    kKextLoadResultMissingDependencies,
    kKextLoadResultFailedToLoad,
    kKextLoadResultAlreadyLoaded
} KextLoadResult;
```

## See Also

- [GNUKext Introduction](Introduction.md)
- [KotlinKit Documentation](../KotlinKit/README.md) - For Kotlin-based kext development