# NextStd - Modern Foundation Framework

A modern, easy-to-use Foundation framework for Objective-C/C++ that reimplements GNUstep Foundation with contemporary conveniences.

## Features

- **Modern API**: Clean, intuitive headers with modern conventions
- **C++ Interoperability**: Native C++ support with operator overloads and STL compatibility
- **Cross-platform**: Works on POSIX systems (uses GNUstep makefiles)
- **Lightweight**: Focused on essential Foundation classes

## Classes

| Class | Description |
|-------|-------------|
| `NSObject` | Base class with modern memory management API |
| `NSString` | Unicode string with C++ string conversion |
| `NSArray` | Object array with initializer_list support |
| `NSDictionary` | Key-value storage |
| `NSData` | Byte data container |
| `NSValue` / `NSNumber` | Value wrappers |
| `NSEnumerator` | Fast enumeration support |

## Usage

### Objective-C

```objc
#import <NextStd/NextStd.h>

NSString *str = [NSString stringWithUTF8String:"Hello, World!"];
NSArray *arr = [NSArray arrayWithObjects:@"one", @"two", @"three", nil];

NSLog(@"String: %@", str);
```

### C++

```cpp
#import <NextStd/CXXSupport.h>

auto str = @"Hello, NextStd!"_ns;
auto arr = NSArray::arrayWithObjects([@"one" _ns], [@"two" _ns], nil);
```

## Build

```bash
cd NextStd
make
```

Requires `gnustep-make`
