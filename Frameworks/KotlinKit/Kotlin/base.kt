package base.kotlinkit

/**
 * KotlinKit - A library for making GNUstep and Kotlin work together.
 * This package provides Kotlin types that map to GNUstep/Objective-C types.
 */

/**
 * Main entry point for Kotlin interoperability
 */
object Kotlin {
    val version: String = "1.0.0"
    
    fun isKotlinClass(obj: Any?): Boolean {
        return obj != null && obj::class.simpleName?.startsWith("Kotlin") == true
    }
}

/**
 * KotlinArray - Represents a Kotlin array that can be used from Objective-C
 */
class KotlinArray<T> {
    val elements: MutableList<T?> = mutableListOf()
    
    val count: Int get() = elements.size
    
    constructor(vararg items: T?) {
        elements.addAll(items.toList())
    }
    
    constructor(list: List<T?>) {
        elements.addAll(list)
    }
    
    operator fun get(index: Int): T? = elements.getOrNull(index)
    
    fun add(item: T?) {
        elements.add(item)
    }
    
    fun insert(item: T?, index: Int) {
        if (index in 0..elements.size) {
            elements.add(index, item)
        }
    }
    
    fun removeAt(index: Int) {
        if (index in 0..<elements.size) {
            elements.removeAt(index)
        }
    }
    
    fun toArray(): Array<T?> = elements.toTypedArray()
    fun toList(): List<T?> = elements.toList()
}

/**
 * KotlinBoolean - Represents Kotlin's Boolean type
 */
class KotlinBoolean(val value: Boolean) {
    companion object {
        val TRUE = KotlinBoolean(true)
        val FALSE = KotlinBoolean(false)
        
        fun booleanWithBool(value: Boolean) = KotlinBoolean(value)
        fun booleanWithValue(value: Number) = KotlinBoolean(value.toInt() != 0)
    }
    
    fun toNSNumber(): NSNumber = value.toInt().toNSNumber()
}

/**
 * KotlinByte - Represents Kotlin's Byte type
 */
class KotlinByte(val value: Byte) {
    companion object {
        fun byteWithByte(value: Byte) = KotlinByte(value)
        fun byteWithValue(value: Number) = KotlinByte(value.toByte())
    }
    
    fun toNSNumber(): NSNumber = value.toInt().toNSNumber()
}

/**
 * KotlinChar - Represents Kotlin's Char type
 */
class KotlinChar(val value: Char) {
    companion object {
        fun charWithChar(value: Char) = KotlinChar(value)
        fun charWithString(value: String) = KotlinChar(value.firstOrNull() ?: '\u0000')
    }
    
    fun toString(): String = value.toString()
}

/**
 * KotlinShort - Represents Kotlin's Short type
 */
class KotlinShort(val value: Short) {
    companion object {
        fun shortWithShort(value: Short) = KotlinShort(value)
        fun shortWithValue(value: Number) = KotlinShort(value.toShort())
    }
    
    fun toNSNumber(): NSNumber = value.toInt().toNSNumber()
}

/**
 * KotlinInt - Represents Kotlin's Int type
 */
class KotlinInt(val value: Int) {
    companion object {
        fun intWithInt(value: Int) = KotlinInt(value)
        fun intWithValue(value: Number) = KotlinInt(value.toInt())
        fun intWithString(value: String) = KotlinInt(value.toIntOrNull() ?: 0)
    }
    
    fun toNSNumber(): NSNumber = value.toNSNumber()
    fun toString(): String = value.toString()
}

/**
 * KotlinLong - Represents Kotlin's Long type
 */
class KotlinLong(val value: Long) {
    companion object {
        fun longWithLong(value: Long) = KotlinLong(value)
        fun longWithValue(value: Number) = KotlinLong(value.toLong())
        fun longWithString(value: String) = KotlinLong(value.toLongOrNull() ?: 0L)
    }
    
    fun toNSNumber(): NSNumber = value.toNSNumber()
    fun toString(): String = value.toString()
}

/**
 * KotlinFloat - Represents Kotlin's Float type
 */
class KotlinFloat(val value: Float) {
    companion object {
        fun floatWithFloat(value: Float) = KotlinFloat(value)
        fun floatWithValue(value: Number) = KotlinFloat(value.toFloat())
        fun floatWithString(value: String) = KotlinFloat(value.toFloatOrNull() ?: 0.0f)
    }
    
    fun toNSNumber(): NSNumber = value.toDouble().toNSNumber()
    fun toString(): String = value.toString()
}

/**
 * KotlinDouble - Represents Kotlin's Double type
 */
class KotlinDouble(val value: Double) {
    companion object {
        fun doubleWithDouble(value: Double) = KotlinDouble(value)
        fun doubleWithValue(value: Number) = KotlinDouble(value.toDouble())
        fun doubleWithString(value: String) = KotlinDouble(value.toDoubleOrNull() ?: 0.0)
    }
    
    fun toNSNumber(): NSNumber = value.toNSNumber()
    fun toString(): String = value.toString()
}

/**
 * KotlinUnit - Represents Kotlin's Unit type (void equivalent)
 */
object KotlinUnit {
    val unit = this
}

//
// GNUKext support for Kotlin
//

/**
 * KotlinKext - Cross-platform kext wrapper for Kotlin
 */
class KotlinKext(path: String?) {
    private val internalPath: String? = path
    private var loaded: Boolean = false
    private val personalities: Map<String, Any?> = emptyMap()
    private val dependencies: List<String> = emptyList()
    
    companion object {
        fun kextWithPath(path: String): KotlinKext = KotlinKext(path)
        fun kextWithIdentifier(identifier: String): KotlinKext? = null
        fun allLoadedKexts(): List<KotlinKext> = emptyList()
        fun loadKextWithIdentifier(identifier: String): Boolean = false
        fun unloadKextWithIdentifier(identifier: String): Boolean = false
    }
    
    fun load(): Boolean {
        loaded = true
        return true
    }
    
    fun unload(): Boolean {
        loaded = false
        return true
    }
    
    fun isLoaded(): Boolean = loaded
    
    fun bundlePath(): String? = internalPath
    fun bundleIdentifier(): String? = null
    fun bundleVersion(): String? = null
    fun bundleName(): String? = null
}

/**
 * KotlinKextDriver - Base class for kext drivers in Kotlin
 */
abstract class KotlinKextDriver {
    var started: Boolean = false
        private set
    var terminated: Boolean = false
        private set
    var provider: Any? = null
        private set
    var matchedPersonality: Any? = null
        private set
    
    abstract fun personalityKeys(): List<String>
    
    open fun doesMatchProvider(provider: Any?, personality: Any?): Boolean = true
    
    open fun start(provider: Any?): Boolean {
        this.provider = provider
        started = true
        return true
    }
    
    open fun stop(provider: Any?) {
        started = false
        terminated = true
        this.provider = null
    }
    
    fun free() {
        terminated = true
        started = false
        provider = null
    }
}

/**
 * KotlinKextPersonality - IOKit personality for Kotlin kexts
 */
class KotlinKextPersonality(name: String, properties: Map<String, Any?>) {
    val personalityName: String = name
    val properties: Map<String, Any?> = properties
    
    companion object {
        fun personalityWithName(name: String, properties: Map<String, Any?>): KotlinKextPersonality = 
            KotlinKextPersonality(name, properties)
    }
    
    fun providerClass(): String? = properties["IOProviderClass"] as? String
    fun ioClass(): String? = properties["IOClass"] as? String
    fun propertyMatch(): Map<String, Any?>? = properties["IOPropertyMatch"] as? Map<String, Any?>
    fun propertyTable(): Map<String, Any?>? = properties["IOPropertyTable"] as? Map<String, Any?>
    fun probeScore(): Int = (properties["IOProbeScore"] as? Number)?.toInt() ?: 0
}

//
// UIKit support for Kotlin
//

/**
 * KotlinUIApplication - UIApplication wrapper for Kotlin
 */
object KotlinUIApplication {
    val shared: Any? get() = null
    
    fun runMain(argc: Int, argv: Array<String>, principalClass: String? = null, delegateClass: String? = null) {
        // Would call UIApplicationMain
    }
    
    fun windows(): List<Any?> = emptyList()
    fun terminate() {
        // Would terminate the application
    }
}

/**
 * KotlinUIView - UIView wrapper for Kotlin
 */
class KotlinUIView {
    var x: Double = 0.0
    var y: Double = 0.0
    var width: Double = 0.0
    var height: Double = 0.0
    var backgroundColor: KotlinUIColor? = null
    var hidden: Boolean = false
    var alpha: Double = 1.0
    var tag: Int = 0
    
    fun addSubview(view: KotlinUIView) {
        // Would add subview to underlying UIView
    }
    
    fun removeFromSuperview() {
        // Would remove from superview
    }
    
    fun viewWithTag(tag: Int): KotlinUIView? = null
    fun setNeedsDisplay() {
        // Would mark view for redraw
    }
    
    fun setNeedsLayout() {
        // Would mark view for layout
    }
}

/**
 * KotlinUIViewController - UIViewController wrapper for Kotlin
 */
open class KotlinUIViewController {
    var view: KotlinUIView? = null
    var title: String? = null
    
    open fun viewDidLoad() {
        // Override in subclass
    }
}

/**
 * KotlinUIColor - UIColor wrapper for Kotlin
 */
class KotlinUIColor {
    val red: Double
    val green: Double
    val blue: Double
    val alpha: Double
    
    constructor(r: Double, g: Double, b: Double, a: Double) {
        red = r
        green = g
        blue = b
        alpha = a
    }
    
    companion object {
        val black: KotlinUIColor = KotlinUIColor(0.0, 0.0, 0.0, 1.0)
        val white: KotlinUIColor = KotlinUIColor(1.0, 1.0, 1.0, 1.0)
        val clear: KotlinUIColor = KotlinUIColor(0.0, 0.0, 0.0, 0.0)
    }
}

