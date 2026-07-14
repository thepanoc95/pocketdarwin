# KotlinKit

A library for making GNUstep and Kotlin work together, allowing stuff to be built with Kotlin.

## Overview

KotlinKit provides Objective-C wrappers for Kotlin types and enables interoperability between Kotlin and GNUstep/Objective-C. This framework allows developers to:

- Use Kotlin types from Objective-C code
- Bridge Kotlin arrays to NSArray
- Convert between Kotlin primitive types and NSNumber
- Call Kotlin methods from Objective-C through JIGS (Java Interface Generator for GNUstep)
- Build GNUKext-based kernel extensions with Kotlin
- Build UIKit applications with Kotlin

## Features

### Kotlin Types

The framework provides wrappers for all Kotlin primitive types:

- `KotlinBoolean` - Kotlin's Boolean type
- `KotlinByte` - Kotlin's Byte type (8-bit signed integer)
- `KotlinChar` - Kotlin's Char type (Unicode character)
- `KotlinShort` - Kotlin's Short type (16-bit signed integer)
- `KotlinInt` - Kotlin's Int type (32-bit signed integer)
- `KotlinLong` - Kotlin's Long type (64-bit signed integer)
- `KotlinFloat` - Kotlin's Float type (32-bit floating point)
- `KotlinDouble` - Kotlin's Double type (64-bit floating point)
- `KotlinUnit` - Kotlin's Unit type (void equivalent)
- `KotlinArray` - Kotlin's Array type

### GNUKext Support

Build kernel extensions (kexts) with Kotlin using the GNUKext framework:

- `KotlinKext` - Load, unload, and manage kexts
- `KotlinKextDriver` - Base class for kext drivers in Kotlin
- `KotlinKextPersonality` - IOKit personality matching for Kotlin kexts

### UIKit Support

Build cross-platform UI applications with Kotlin:

- `KotlinUIApplication` - UIApplication wrapper
- `KotlinUIView` - UIView wrapper
- `KotlinUIViewController` - UIViewController wrapper
- `KotlinUIColor` - UIColor wrapper

## API Reference

### Kotlin Types

#### KotlinBoolean

```objc
KotlinBoolean *bool = [KotlinBoolean booleanWithBOOL:YES];
NSNumber *number = [bool toNSNumber];
BOOL value = [bool booleanValue];
```

#### KotlinInt

```objc
KotlinInt *int = [KotlinInt intWithInt32:42];
NSNumber *number = [int toNSNumber];
int32_t value = [int intValue];
```

#### KotlinArray

```objc
KotlinArray *array = [KotlinArray arrayWithObjects:@"one", @"two", nil];
NSUInteger count = [array count];
id item = [array objectAtIndex:0];
```

### GNUKext Classes

#### KotlinKext

```objc
KotlinKext *kext = [KotlinKext kextWithPath:@"/path/to/Kext.kext"];
if ([kext load]) {
    NSLog(@"Loaded: %@", kext.bundleIdentifier);
}

// Get kext info
NSDictionary *info = kext.mergedInfoDictionary;
NSArray *personalities = kext.personalities;
```

#### KotlinKextDriver

Subclass `KotlinKextDriver` to create a kext driver:

```objc
@interface MyDriver : KotlinKextDriver
@end

@implementation MyDriver

+ (NSArray *)personalityKeys {
    return @[@"com.example.mykext"];
}

+ (BOOL)doesMatchProvider:(id)provider withPersonality:(id)personality {
    return YES;
}

- (BOOL)start:(id)provider {
    NSLog(@"Driver started");
    return [super start:provider];
}

@end
```

#### KotlinKextPersonality

```objc
KotlinKextPersonality *personality = [KotlinKextPersonality 
    personalityWithName:@"MyDriver"
    properties:@{@"IOProviderClass": @"IOProviderClass"}];

NSString *providerClass = personality.ioProviderClass;
NSInteger score = personality.probeScore;
```

### UIKit Classes

#### KotlinUIApplication

```objc
[application runMain:argc argv:argv 
         principalClass:NSStringFromClass([MyApp class])
         delegateClass:NSStringFromClass([AppDelegate class])];
```

#### KotlinUIView

```objc
KotlinUIView *view = [[KotlinUIView alloc] initWithFrame:CGRectMake(0,0,100,100)];
view.backgroundColor = [KotlinUIColor whiteColor];
view.tag = 42;
[view setNeedsDisplay];
```

#### KotlinUIViewController

```objc
@interface MyViewController : KotlinUIViewController
@end

@implementation MyViewController
- (void)viewDidLoad {
    [super viewDidLoad];
    self.view = [[KotlinUIView alloc] init];
    self.title = @"My View";
}
@end
```

#### KotlinUIColor

```objc
KotlinUIColor *white = [KotlinUIColor whiteColor];
KotlinUIColor *color = [KotlinUIColor colorWithRed:0.5 green:0.5 blue:0.5 alpha:1.0];
```

## GNUKext with Kotlin

### Building a Kext

To build a GNUKext-based kext with Kotlin support:

```makefile
ENABLE_KOTLIN_KEXT = yes
include $(GNUSTEP_MAKEFILES)/Makefiles/kext.make

KEXT_OBJC_FILES = MyKextDriver.m
KEXT_BUNDLE_ID = com.example.mykext
```

### Kotlin Kext Example

```kotlin
import base.kotlinkit.*

class MyKextDriver : KotlinKextDriver() {
    override fun personalityKeys(): List<String> = listOf("com.example.mykext")
    
    override fun start(provider: Any?): Boolean {
        println("Kext driver started on ${provider}")
        return super.start(provider)
    }
    
    override fun stop(provider: Any?) {
        println("Kext driver stopped")
        super.stop(provider)
    }
}

// Load and manage a kext
val kext = KotlinKext.kextWithPath("/path/to/MyKext.kext")
kext.load()
println("Kext loaded: ${kext.isLoaded()}")
```

## UIKit with Kotlin

### Kotlin UI Example

```kotlin
import base.kotlinkit.*

class MyViewController : KotlinUIViewController() {
    override fun viewDidLoad() {
        super.viewDidLoad()
        view = KotlinUIView().apply {
            x = 0.0
            y = 0.0
            width = 320.0
            height = 200.0
            backgroundColor = KotlinUIColor.white
            tag = 42
        }
    }
}

// Run the application
KotlinUIApplication.runMain(0, arrayOf())
```

## Full Objective-C Example

```objc
#import <KotlinKit/KotlinKit.h>

// Using Kotlin types
KotlinInt *kotlinInt = [KotlinInt intWithInt32:42];
NSNumber *number = [kotlinInt toNSNumber];

// Using GNUKext
KotlinKext *kext = [KotlinKext kextWithPath:@"/path/to/Kext.kext"];
[kext load];

// Using UIKit
KotlinUIView *view = [[KotlinUIView alloc] initWithFrame:CGRectMake(0,0,320,200)];
view.backgroundColor = [KotlinUIColor whiteColor];
```

## Building

### Prerequisites

- GNUstep make
- clang compiler
- Foundation framework
- GNUKext framework (optional, for kext support)
- UIKit framework (optional, for UI support)

### Build Commands

```bash
# Build the framework
make

# Install the framework
make install

# Build with GNUKext support
make ENABLE_KOTLIN_KEXT=yes

# Build with UIKit support  
make ENABLE_KOTLIN_UIKIT=yes

# Build documentation
make doc=yes
```

## Architecture

KotlinKit uses JIGS (Java Interface Generator for GNUstep) to bridge between Objective-C and Kotlin. The framework provides:

1. **Type Wrappers** - Objective-C classes that wrap Kotlin primitive types
2. **Array Support** - KotlinArray class for working with Kotlin arrays
3. **Function Bridge** - KotlinFuncs for calling Kotlin methods dynamically
4. **GNUKext Bridge** - KotlinKext classes for kext development
5. **UIKit Bridge** - KotlinUIKit classes for UI development

## File Structure

```
KotlinKit/
├── GNUmakefile                    - Main build file
├── GNUmakefile.preamble           - Build configuration
├── GNUmakefile.postamble          - Install rules
├── Headers/
│   ├── KotlinKit.h              - Main header
│   └── KotlinKitTypes.h         - Type definitions
├── Kotlin/
│   └── base.kt                  - Kotlin source
├── KotlinKext.h/mm                - GNUKext wrappers
├── KotlinUIKit.h/mm               - UIKit wrappers
├── Examples/
│   ├── GNUmakefile              - Examples master
│   ├── KextExample/             - GNUKext kext example
│   └── UIExample/               - UIKit application example
└── Source/                       - Implementation files
    ├── Kotlin.m
    ├── KotlinArray.m
    ├── KotlinBoolean.m
    ├── KotlinInt.m
    └── ...
```

## License

This library is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.