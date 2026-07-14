# GNUKext Support in KotlinKit

Building kernel extensions (kexts) with Kotlin through the KotlinKit framework.

## Overview

KotlinKit provides Kotlin wrappers for all GNUKext classes, enabling you to write kext drivers in Kotlin that compile and run on Darwin, Linux, and BSD platforms.

## Architecture

The Kotlin-GNUKext bridge consists of:

```
Kotlin Code
    ↓ (JVM/JIGS bridge)
KotlinKit Classes (Objective-C)
    ↓
GNUKext Framework (Objective-C/C)
    ↓
Platform-specific kext APIs
```

## KotlinKext Class

Load and manage kext bundles from Kotlin:

```kotlin
import base.kotlinkit.*

// Create and load a kext
val kext = KotlinKext.kextWithPath("/path/to/MyKext.kext")
kext.load()

// Check status
if (kext.isLoaded()) {
    println("Kext loaded: ${kext.bundleIdentifier}")
    println("Version: ${kext.bundleVersion}")
}

// Get personalities and dependencies
val personalities = kext.personalities
val dependencies = kext.dependencies

// Unload
kext.unload()
```

## KotlinKextDriver Class

Subclass `KotlinKextDriver` to create your kext driver:

```kotlin
import base.kotlinkit.*

class MyKextDriver : KotlinKextDriver() {
    
    override fun personalityKeys(): List<String> {
        return listOf("com.example.mykext")
    }
    
    override fun doesMatchProvider(provider: Any?, personality: Any?): Boolean {
        // Custom matching logic
        return super.doesMatchProvider(provider, personality)
    }
    
    override fun start(provider: Any?): Boolean {
        println("Driver starting on provider: $provider")
        return super.start(provider)
    }
    
    override fun stop(provider: Any?) {
        println("Driver stopping")
        super.stop(provider)
    }
}
```

## KotlinKextPersonality Class

Work with IOKit personalities from Kotlin:

```kotlin
val personality = KotlinKextPersonality.personalityWithName(
    "MyDriver",
    mapOf(
        "IOProviderClass" to "IOProviderClass",
        "IOClass" to "MyDriver",
        "IOProbeScore" to 100
    )
)

val providerClass = personality.providerClass()
val ioClass = personality.ioClass()
val probeScore = personality.probeScore()
```

## Info.plist Integration

Create kext Info.plist with Kotlin personalities:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<plist version="1.0">
<dict>
    <key>CFBundleIdentifier</key>
    <string>com.example.mykext</string>
    
    <key>CFBundleName</key>
    <string>MyKext</string>
    
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
</dict>
</plist>
```

## Building Kexts with Kotlin

### GNUmakefile Configuration

```makefile
# Enable Kotlin kext support
ENABLE_KOTLIN_KEXT = yes

# Include GNUKext rules
include $(GNUSTEP_MAKEFILES)/Makefiles/kext.make

# Kext configuration
KEXT_BUNDLE_ID = com.example.kotlin.kext
KEXT_BUNDLE_NAME = MyKext
KEXT_BUNDLE_VERSION = 1.0

# Source files (can be mixed Objective-C and Kotlin)
KEXT_OBJC_FILES = MyKextWrapper.m
KEXT_KOTLIN_FILES = driver.kt

# Frameworks
KEXT_LDFLAGS = -framework GNUKext -framework KotlinKit
```

### Compiling Kotlin Sources

Set KOTLIN_HOME for Kotlin compilation:

```bash
export KOTLIN_HOME=/path/to/kotlinc
make
```

## Lifecycle Management

The kext lifecycle from Kotlin:

```kotlin
class MyKextApp {
    fun installAndLoad() {
        val kext = KotlinKext.kextWithPath("/path/to/MyKext.kext")
        
        // Register driver
        // (requires native Objective-C registration)
        
        // Load kext
        if (!kext.load()) {
            throw RuntimeException("Failed to load kext")
        }
        
        println("Kext installed and loaded")
    }
    
    fun unload() {
        KotlinKext.unloadKextWithIdentifier("com.example.mykext")
    }
}
```

## Registry Matching

Match against the kernel registry:

```kotlin
// Through Objective-C bridge, match providers
// The GNUKext framework handles the matching automatically
// when you register your KotlinKextDriver subclass
```

## Cross-Platform Considerations

### Darwin (macOS)

Uses native OSKext APIs:

```kotlin
// On Darwin, uses OSKext directly
val kext = KotlinKext.kextWithPath("/path/to/MyKext.kext")
```

### Linux

Uses kernel module system:

```kotlin
// On Linux, uses kernel module loading
val kext = KotlinKext.kextWithPath("/lib/modules/$(uname -r)/extra/mykext.ko")
```

### BSD

Uses BSD module system:

```kotlin
// On BSD, uses module loading
val kext = KotlinKext.kextWithPath("/boot/modules/mykext.ko")
```

## Error Handling

```kotlin
when (val error = kext.load()) {
    false -> {
        // Check error domain
        val errorDomain = KotlinKextErrorDomain
        // KotlinKextErrorLoadFailed, etc.
    }
    true -> {
        // Success
    }
}
```

## See Also

- [GNUKext API Reference](API.md)
- [GNUKext Introduction](../GNUKext/Documentation/Introduction.md)
- [Building GNUKext Kexts](../GNUKext/Documentation/Linux_Kexts.md)