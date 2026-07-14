# UIKit Support in KotlinKit

Building cross-platform UI applications with Kotlin through the KotlinKit framework.

## Overview

KotlinKit provides Kotlin wrappers for UIKit classes, enabling you to build cross-platform user interfaces that work on GNUstep/Linux and macOS. The API mirrors UIKit patterns while using AppKit as the underlying implementation on GNUstep.

## Architecture

The Kotlin-UIKit bridge consists of:

```
Kotlin Code
    ↓ (JIGS bridge)
KotlinKit UIKit Classes (Objective-C++)
    ↓
UIKit/AppKit Framework
    ↓
Native UI Rendering
```

## KotlinUIApplication

The main application class for Kotlin UI apps:

```kotlin
import base.kotlinkit.*

// Run the main application loop
KotlinUIApplication.runMain(0, arrayOf())

// Or with custom classes
KotlinUIApplication.runMain(
    0, 
    arrayOf(),
    principalClass = "MyApp",
    delegateClass = "AppDelegate"
)

// Get shared application instance
val app = KotlinUIApplication.shared
```

## KotlinUIView

UIView wrapper for building user interfaces:

```kotlin
// Create a view
val view = KotlinUIView().apply {
    x = 10.0
    y = 10.0
    width = 200.0
    height = 100.0
    backgroundColor = KotlinUIColor.white
    alpha = 1.0
    hidden = false
    tag = 42
}

// Add subviews
val subview = KotlinUIView()
view.addSubview(subview)

// Remove from superview
view.removeFromSuperview()

// Find by tag
val found = view.viewWithTag(42)
```

## KotlinUIViewController

UIViewController wrapper for view controller patterns:

```kotlin
open class MyViewController : KotlinUIViewController() {
    
    override fun viewDidLoad() {
        super.viewDidLoad()
        
        // Create and configure view
        view = KotlinUIView().apply {
            backgroundColor = KotlinUIColor.white
        }
        
        // Add UI elements
        val label = KotlinUIView().apply {
            // Configure label
        }
    }
    
    fun setTitle(title: String) {
        this.title = title
    }
}
```

## KotlinUIColor

UIColor wrapper for colors:

```kotlin
// Predefined colors
val black = KotlinUIColor.black
val white = KotlinUIColor.white
val clear = KotlinUIColor.clear

// Custom colors
val color = KotlinUIColor(0.5, 0.5, 1.0, 1.0)  // Purple-ish color

// Factory methods
val customColor = KotlinUIColor.colorWithRed(1.0, green = 0.0, blue = 0.0, alpha = 1.0)
```

## Building UI Applications with Kotlin

### GNUmakefile Configuration

```makefile
include $(GNUSTEP_MAKEFILES)/common.make

# Enable Kotlin UI support
ENABLE_KOTLIN_UIKIT = yes

PACKAGE_NAME = MyKotlinApp

# Source files
MyKotlinApp_OBJC_FILES = main.m

# Frameworks
MyKotlinApp_FRAMEWORKS_DEPEND_UPON = -framework UIKit -framework KotlinKit

include $(GNUSTEP_MAKEFILES)/tool.make
```

### Application Structure

```kotlin
// AppDelegate.kt
import base.kotlinkit.*

class AppDelegate : KotlinUIViewController() {
    
    override fun viewDidLoad() {
        super.viewDidLoad()
        
        // Create window and root view controller
        setupUI()
    }
    
    private fun setupUI() {
        // Build your UI
    }
}

// main.kt
fun main() {
    KotlinUIApplication.runMain(0, arrayOf())
}
```

## Layout and Geometry

Coordinate types (from UIKitTypes):

```kotlin
// Frame-based layout
val view = KotlinUIView().apply {
    x = 0.0
    y = 0.0
    width = 320.0
    height = 200.0
}
```

## Event Handling

UIKit event system through Kotlin:

```kotlin
// Through Objective-C bridge, add target-action
// This requires native Objective-C view manipulation
```

## Cross-Platform Compatibility

### GNUstep/Linux

Uses AppKit on GNUstep:

```kotlin
// Works with GNUstep's UIKit backend
val view = KotlinUIView()
view.backgroundColor = KotlinUIColor.white
```

### macOS

Uses native UIKit (or AppKit on GNUstep):

```kotlin
// On macOS with UIKit, uses native UIKit
// On GNUstep, uses AppKit backend
```

## View Hierarchy

Managing the view hierarchy:

```kotlin
// Create parent view
val parent = KotlinUIView().apply {
    backgroundColor = KotlinUIColor.clear
}

// Create child views
val child1 = KotlinUIView().apply {
    tag = 1
    backgroundColor = KotlinUIColor.black
}

val child2 = KotlinUIView().apply {
    tag = 2
    backgroundColor = KotlinUIColor.white
}

// Add to parent
parent.addSubview(child1)
parent.addSubview(child2)

// Find child
val found = parent.viewWithTag(1)  // Returns child1
```

## Animation and Redraw

Triggering view updates:

```kotlin
val view = KotlinUIView()

// Mark for redraw
view.setNeedsDisplay()

// Trigger layout update
view.setNeedsLayout()
```

## Example Application

Complete example:

```kotlin
import base.kotlinkit.*

class ViewController : KotlinUIViewController() {
    
    override fun viewDidLoad() {
        super.viewDidLoad()
        
        // Main view
        view = KotlinUIView().apply {
            backgroundColor = KotlinUIColor.white
        }
        
        // Add a colored view
        val coloredView = KotlinUIView().apply {
            x = 20.0
            y = 40.0
            width = 100.0
            height = 100.0
            backgroundColor = KotlinUIColor.black
        }
        
        view.addSubview(coloredView)
    }
}

class AppDelegate : KotlinUIViewController() {
    
    override fun viewDidLoad() {
        super.viewDidLoad()
        setupMainWindow()
    }
    
    private fun setupMainWindow() {
        // Create and show window
    }
}

fun main() {
    KotlinUIApplication.runMain(0, arrayOf())
}
```

## Limitations

Current limitations in Kotlin UIKit support:

- Some UI events require Objective-C bridge code
- Custom drawing requires native implementation
- Platform-specific features may need Objective-C wrappers
- Animation APIs are not yet implemented in Kotlin wrappers

## See Also

- [KotlinKit API Reference](API.md)
- [KotlinKit README](../KotlinKit/README.md)
- [GNUKext Documentation](GNUKext.md)