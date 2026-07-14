# GNUKext Framework

Cross-platform Kernel Extension framework for Darwin, Linux, and BSD.

## Overview

GNUKext provides a unified API for building kernel extensions (kexts) that compile and run on:

- **Darwin/macOS** - Uses native OSKext APIs for kext loading/unloading
- **Linux** - Uses kernel module loading via filesystem operations  
- **BSD** - Uses BSD module system for kernel extensions

This allows developers to write kexts once and deploy them across different platforms.

## Architecture

```
GNUKext Framework
├── GNUKext.h              - Main kext management class
├── GNUKextDriver.h        - Base class for kext drivers
├── GNUKextPersonality.h   - IOKit personality matching
├── GNUKextRegistry.h      - Registry entry tree
├── libkern.h              - Cross-platform libkern types
├── DarwinModule.h         - Darwin-specific kext implementation
├── LinuxModule.h          - Linux kernel module implementation
├── BSDModule.h            - BSD module implementation
└── IOKit/                 - IOKit type definitions
    ├── IOReturn.h
    ├── IOKitLib.h
    ├── IOMessage.h
    ├── IOTypes.h
    └── IOKitServer.h
```

## Main Classes

### GNUKext

The main class for managing kext bundles:

```objc
#import <GNUKext/GNUKext.h>

// Load a kext from a bundle path
GNUKext *kext = [GNUKext kextWithPath:@"/path/to/MyDriver.kext"];
if ([kext load]) {
    NSLog(@"Kext loaded successfully");
}

// Unload a kext
[kext unload];

// Find kext by identifier
GNUKext *existing = [GNUKext kextWithIdentifier:@"com.example.mykext"];
```

### GNUKextDriver

Base class for kext drivers. Subclass this to create your driver:

```objc
#import <GNUKext/GNUKextDriver.h>

@interface MyDriver : GNUKextDriver
@end

@implementation MyDriver

+ (NSArray *)personalityKeys {
    return @[@"com.example.mykext"];
}

+ (BOOL)doesMatchProvider:(GNUKextRegistryEntry *)provider
           withPersonality:(GNUKextPersonality *)personality
{
    // Custom matching logic
    return YES;
}

- (BOOL)start:(GNUKextRegistryEntry *)provider {
    NSLog(@"Driver started on %@", [provider entryPath]);
    return [super start:provider];
}

- (void)stop:(GNUKextRegistryEntry *)provider {
    NSLog(@"Driver stopping");
    [super stop:provider];
}

@end
```

Register your driver class:

```objc
[GNUKext registerDriverClass:[MyDriver class]];
```

### GNUKextPersonality

Represents an IOKit personality dictionary:

```objc
GNUKextPersonality *personality = [GNUKextPersonality 
    personalityWithName:@"MyDriver"
    dictionary:@{@"IOClass": @"MyDriver", @"IOProviderClass": @"IOProviderClass"}];

if ([personality matchesProvider:registryEntry]) {
    // Provider matches this personality
}
```

### GNUKextRegistryEntry

Represents a node in the IOKit registry tree:

```objc
GNUKextRegistryEntry *root = [GNUKextRegistryEntry rootEntry];
NSArray *children = [root childEntries];

for (GNUKextRegistryEntry *child in children) {
    NSLog(@"Entry: %@, Path: %@", [child name], [child entryPath]);
    NSString *value = [child propertyForKey:@"key"];
}
```

## Building a Kext

### Using GNUmakefile

Create a GNUmakefile for your kext project:

```makefile
include $(GNUSTEP_MAKEFILES)/common.make

PACKAGE_NAME = MyKext
BUNDLE_EXTENSION = kext

# Your driver source files
BUNDLE_OBJC_FILES = MyDriver.m

# Link against GNUKext
ADDITIONAL_LDFLAGS += -framework GNUKext

include $(GNUSTEP_MAKEFILES)/bundle.make
```

### With Kotlin Support

To build kexts with Kotlin using KotlinKit:

```makefile
ENABLE_KOTLIN_KEXT = yes
include $(GNUSTEP_MAKEFILES)/Makefiles/kext.make

KEXT_OBJC_FILES = MyKextDriver.m
KEXT_BUNDLE_ID = com.example.kotlin.kext
KEXT_LDFLAGS = -framework GNUKext -framework KotlinKit
```

## Info.plist Structure

Kext bundles require an Info.plist with specific keys:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleIdentifier</key>
    <string>com.example.mykext</string>
    
    <key>CFBundleName</key>
    <string>MyKext</string>
    
    <key>CFBundleVersion</key>
    <string>1.0</string>
    
    <key>IOKitPersonalities</key>
    <dict>
        <key>MyDriver</key>
        <dict>
            <key>IOProviderClass</key>
            <string>IOProviderClass</string>
            <key>IOClass</key>
            <string>MyDriver</string>
            <key>IOProbeScore</key>
            <integer>100</integer>
        </dict>
    </dict>
    
    <key>OSBundleLibraries</key>
    <dict>
        <key>com.apple.kpi.libkern</key>
        <string>10.0.0</string>
    </dict>
</dict>
</plist>
```

## API Reference

### GNUKext Class Methods

| Method | Description |
|--------|-------------|
| `kextWithPath:` | Create kext from bundle path |
| `kextWithIdentifier:` | Find loaded kext by bundle identifier |
| `allLoadedKexts` | Returns array of all loaded kexts |
| `loadKextWithIdentifier:` | Load kext by identifier |
| `unloadKextWithIdentifier:` | Unload kext by identifier |
| `registerDriverClass:` | Register a driver subclass |
| `registerDriverClass:forPersonality:` | Register with specific personality |
| `registeredDriverClasses` | Get all registered driver classes |
| `matchAndStartDrivers` | Match all kexts against registry |

### GNUKext Instance Methods

| Method | Description |
|--------|-------------|
| `initWithPath:` | Initialize with bundle path |
| `load` | Load the kext bundle |
| `unload` | Unload the kext bundle |
| `isLoaded` | Check if kext is loaded |
| `mergedInfoDictionary` | Get combined Info.plist |
| `startMatchingWithProvider:` | Start driver matching |

### GNUKextDriver Methods

| Method | Description |
|--------|-------------|
| `personalityKeys` | Return array of personality names |
| `doesMatchProvider:withPersonality:` | Check if provider matches |
| `initWithPersonality:` | Initialize with personality |
| `start:` | Start the driver |
| `stop:` | Stop the driver |
| `free` | Free driver resources |

## Error Handling

```objc
typedef enum {
    GNUKextErrorNotLoaded = 3000,
    GNUKextErrorAlreadyLoaded,
    GNUKextErrorInvalidBundle,
    GNUKextErrorLoadFailed,
    GNUKextErrorUnloadFailed,
    GNUKextErrorDependencyMissing,
    GNUKextErrorNotFound
} GNUKextErrorCode;

extern NSString * const GNUKextErrorDomain;
```

## Cross-Platform Notes

- On **Darwin**: Uses OSKext APIs directly
- On **Linux**: Creates kernel modules compatible with the platform
- On **BSD**: Uses the BSD module loading infrastructure

The framework abstracts these differences through the libkern types defined in `libkern.h`.

## See Also

- [Linux Kernel Module Documentation](Linux_Kexts.md)
- [KotlinKit Documentation](../KotlinKit/README.md) - For building kexts with Kotlin