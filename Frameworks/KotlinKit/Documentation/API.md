# KotlinKit API Reference

Complete API documentation for the KotlinKit framework.

## Constants and Types

### Error Domains

```objc
extern NSString * const KotlinKitErrorDomain;
extern NSString * const KotlinKextErrorDomain;
```

### Error Codes

#### KotlinKitErrorCode

```objc
typedef NS_ENUM(NSInteger, KotlinKitErrorCode) {
    KotlinKitErrorNotAvailable = 4000,
    KotlinKitErrorInvalidArgument,
    KotlinKitErrorTypeMismatch,
    KotlinKitErrorMethodNotFound,
    KotlinKitErrorInvocationFailed
};
```

#### KotlinKextErrorCode

```objc
typedef NS_ENUM(NSInteger, KotlinKextErrorCode) {
    KotlinKextErrorNotAvailable = 4100,
    KotlinKextErrorInvalidArgument,
    KotlinKextErrorLoadFailed,
    KotlinKextErrorUnloadFailed
};
```

### Type Constants

```objc
extern NSString * const KotlinVersion;
extern NSString * const KotlinBooleanType;
extern NSString * const KotlinByteType;
extern NSString * const KotlinCharType;
extern NSString * const KotlinShortType;
extern NSString * const KotlinIntType;
extern NSString * const KotlinLongType;
extern NSString * const KotlinFloatType;
extern NSString * const KotlinDoubleType;
extern NSString * const KotlinArrayType;
extern NSString * const KotlinKitErrorDomain;
```

## Block Types

```objc
typedef void (^KotlinCallback)(id result, NSError *error);
typedef id (^KotlinFunction)(NSArray *arguments);
```

## Classes

### Kotlin

Main entry point for Kotlin interoperability.

```objc
@interface Kotlin : NSObject

@property (class, nonatomic, readonly) NSString *version;

+ (BOOL)isKotlinClass:(id)object;
+ (void)JIGSClassMethod:(NSString *)methodName;

@end
```

### KotlinArray

Represents a Kotlin array that can be used from Objective-C.

```objc
@interface KotlinArray : NSObject

@property (nonatomic, readonly, assign) NSUInteger count;
@property (nonatomic, readonly, strong) Class elementClass;

+ (instancetype)arrayWithObjects:(id)firstObj, ... NS_REQUIRES_NIL_TERMINATION;
+ (instancetype)arrayWithArray:(NSArray *)array;
+ (instancetype)arrayWithSize:(NSUInteger)size elementClass:(Class)cls;

- (instancetype)initWithObjects:(id)firstObj, ... NS_REQUIRES_NIL_TERMINATION;
- (instancetype)initWithArray:(NSArray *)array;
- (id)objectAtIndex:(NSUInteger)index;
- (void)addObject:(id)object;
- (void)insertObject:(id)object atIndex:(NSUInteger)index;
- (void)removeObjectAtIndex:(NSUInteger)index;
- (NSArray *)toArray;

@end
```

### KotlinBoolean

Represents Kotlin's Boolean type.

```objc
@interface KotlinBoolean : NSObject

@property (nonatomic, readonly, assign) BOOL booleanValue;

+ (instancetype)TRUE;
+ (instancetype)FALSE;
+ (instancetype)booleanWithBOOL:(BOOL)value;
+ (instancetype)booleanWithValue:(NSNumber *)value;

- (instancetype)initWithBOOL:(BOOL)value;
- (instancetype)initWithValue:(NSNumber *)value;
- (BOOL)booleanValue;
- (NSNumber *)toNSNumber;

@end
```

### KotlinByte

Represents Kotlin's Byte type (8-bit signed integer).

```objc
@interface KotlinByte : NSObject

@property (nonatomic, readonly, assign) int8_t byteValue;

+ (instancetype)byteWithInt8:(int8_t)value;
+ (instancetype)byteWithNumber:(NSNumber *)value;

- (instancetype)initWithInt8:(int8_t)value;
- (instancetype)initWithNumber:(NSNumber *)value;
- (int8_t)byteValue;
- (NSNumber *)toNSNumber;

@end
```

### KotlinChar

Represents Kotlin's Char type (Unicode character).

```objc
@interface KotlinChar : NSObject

@property (nonatomic, readonly, assign) unichar charValue;

+ (instancetype)charWithUnichar:(unichar)value;
+ (instancetype)charWithString:(NSString *)value;

- (instancetype)initWithUnichar:(unichar)value;
- (instancetype)initWithString:(NSString *)value;
- (unichar)charValue;
- (NSString *)toString;

@end
```

### KotlinShort

Represents Kotlin's Short type (16-bit signed integer).

```objc
@interface KotlinShort : NSObject

@property (nonatomic, readonly, assign) int16_t shortValue;

+ (instancetype)shortWithInt16:(int16_t)value;
+ (instancetype)shortWithNumber:(NSNumber *)value;

- (instancetype)initWithInt16:(int16_t)value;
- (instancetype)initWithNumber:(NSNumber *)value;
- (int16_t)shortValue;
- (NSNumber *)toNSNumber;

@end
```

### KotlinInt

Represents Kotlin's Int type (32-bit signed integer).

```objc
@interface KotlinInt : NSObject

@property (nonatomic, readonly, assign) int32_t intValue;

+ (instancetype)intWithInt32:(int32_t)value;
+ (instancetype)intWithNumber:(NSNumber *)value;
+ (instancetype)intWithString:(NSString *)value;

- (instancetype)initWithInt32:(int32_t)value;
- (instancetype)initWithNumber:(NSNumber *)value;
- (int32_t)intValue;
- (NSNumber *)toNSNumber;
- (NSString *)toString;

@end
```

### KotlinLong

Represents Kotlin's Long type (64-bit signed integer).

```objc
@interface KotlinLong : NSObject

@property (nonatomic, readonly, assign) int64_t longValue;

+ (instancetype)longWithInt64:(int64_t)value;
+ (instancetype)longWithNumber:(NSNumber *)value;
+ (instancetype)longWithString:(NSString *)value;

- (instancetype)initWithInt64:(int64_t)value;
- (instancetype)initWithNumber:(NSNumber *)value;
- (int64_t)longValue;
- (NSNumber *)toNSNumber;
- (NSString *)toString;

@end
```

### KotlinFloat

Represents Kotlin's Float type (32-bit floating point).

```objc
@interface KotlinFloat : NSObject

@property (nonatomic, readonly, assign) float floatValue;

+ (instancetype)floatWithFloat:(float)value;
+ (instancetype)floatWithNumber:(NSNumber *)value;
+ (instancetype)floatWithString:(NSString *)value;

- (instancetype)initWithFloat:(float)value;
- (instancetype)initWithNumber:(NSNumber *)value;
- (float)floatValue;
- (NSNumber *)toNSNumber;
- (NSString *)toString;

@end
```

### KotlinDouble

Represents Kotlin's Double type (64-bit floating point).

```objc
@interface KotlinDouble : NSObject

@property (nonatomic, readonly, assign) double doubleValue;

+ (instancetype)doubleWithDouble:(double)value;
+ (instancetype)doubleWithNumber:(NSNumber *)value;
+ (instancetype)doubleWithString:(NSString *)value;

- (instancetype)initWithDouble:(double)value;
- (instancetype)initWithNumber:(NSNumber *)value;
- (double)doubleValue;
- (NSNumber *)toNSNumber;
- (NSString *)toString;

@end
```

### KotlinUnit

Represents Kotlin's Unit type (void equivalent).

```objc
@interface KotlinUnit : NSObject

+ (instancetype)unit;

@end
```

### KotlinFuncs

Utility functions for Kotlin interoperability.

```objc
@interface KotlinFuncs : NSObject

+ (NSString *)kotlinVersion;
+ (BOOL)isKotlinAvailable;
+ (id)callKotlinStaticMethod:(NSString *)className 
                   methodName:(NSString *)methodName 
                        args:(NSArray *)arguments;
+ (id)callKotlinInstanceMethod:(id)instance 
                    methodName:(NSString *)methodName 
                         args:(NSArray *)arguments;

@end
```

## GNUKext Classes

### KotlinKext

Cross-platform kext wrapper for Kotlin.

```objc
@interface KotlinKext : NSObject

@property (nonatomic, readonly, copy) NSString *bundlePath;
@property (nonatomic, readonly, copy) NSString *bundleIdentifier;
@property (nonatomic, readonly, copy) NSString *bundleVersion;
@property (nonatomic, readonly, copy) NSString *bundleName;
@property (nonatomic, readonly, assign) BOOL loaded;
@property (nonatomic, readonly, copy) NSDictionary *personalities;
@property (nonatomic, readonly, copy) NSArray *dependencies;

+ (instancetype)kextWithPath:(NSString *)path;
+ (instancetype)kextWithIdentifier:(NSString *)identifier;
+ (NSArray *)allLoadedKexts;
+ (BOOL)loadKextWithIdentifier:(NSString *)identifier;
+ (BOOL)unloadKextWithIdentifier:(NSString *)identifier;

- (instancetype)initWithPath:(NSString *)path;
- (BOOL)load;
- (BOOL)unload;
- (BOOL)isLoaded;
- (NSDictionary *)mergedInfoDictionary;
- (NSArray *)driverInstances;

@end
```

### KotlinKextDriver

Cross-platform kext driver wrapper for Kotlin.

```objc
@interface KotlinKextDriver : NSObject

@property (nonatomic, readonly, assign) BOOL started;
@property (nonatomic, readonly, assign) BOOL terminated;
@property (nonatomic, readonly, strong) id provider;
@property (nonatomic, readonly, strong) id matchedPersonality;
@property (nonatomic, readonly, copy) NSDictionary *personalityInfo;

+ (NSArray *)personalityKeys;
+ (BOOL)doesMatchProvider:(id)provider withPersonality:(id)personality;

- (instancetype)initWithPersonality:(id)personality;
- (BOOL)start:(id)provider;
- (void)stop:(id)provider;
- (void)free;

@end
```

### KotlinKextPersonality

Cross-platform kext personality wrapper for Kotlin.

```objc
@interface KotlinKextPersonality : NSObject

@property (nonatomic, readonly, copy) NSString *personalityName;
@property (nonatomic, readonly, copy) NSDictionary *properties;

+ (instancetype)personalityWithName:(NSString *)name properties:(NSDictionary *)props;
- (instancetype)initWithName:(NSString *)name properties:(NSDictionary *)props;
- (NSString *)providerClass;
- (NSString *)ioClass;
- (NSDictionary *)propertyMatch;
- (NSDictionary *)propertyTable;
- (NSInteger)probeScore;

@end
```

## UIKit Classes

### KotlinUIApplication

UIApplication wrapper for Kotlin.

```objc
@interface KotlinUIApplication : NSObject

@property (nonatomic, readonly, strong) KotlinUIApplication *shared;
@property (nonatomic, strong) id delegate;

+ (KotlinUIApplication *)sharedInstance;
+ (void)runMain:(int)argc argv:(char **)argv principalClass:(NSString *)principalClassName delegateClass:(NSString *)delegateClassName;

- (NSArray *)windows;
- (void)addWindow:(id)window;
- (void)terminate;

@end
```

### KotlinUIView

UIView wrapper for Kotlin.

```objc
@interface KotlinUIView : NSObject

@property (nonatomic, assign) CGFloat x, y, width, height;
@property (nonatomic, strong) id backgroundColor;
@property (nonatomic, assign) BOOL hidden;
@property (nonatomic, assign) CGFloat alpha;
@property (nonatomic, assign) NSInteger tag;
@property (nonatomic, strong) NSArray *subviews;
@property (nonatomic, strong) KotlinUIView *superview;

- (instancetype)initWithFrame:(CGRect)frame;
- (void)addSubview:(KotlinUIView *)view;
- (void)removeFromSuperview;
- (KotlinUIView *)viewWithTag:(NSInteger)tag;
- (void)setNeedsDisplay;
- (void)setNeedsLayout;

@end
```

### KotlinUIViewController

UIViewController wrapper for Kotlin.

```objc
@interface KotlinUIViewController : NSObject

@property (nonatomic, strong) KotlinUIView *view;
@property (nonatomic, strong) NSString *title;

- (instancetype)init;
- (void)viewDidLoad;

@end
```

### KotlinUIColor

UIColor wrapper for Kotlin.

```objc
@interface KotlinUIColor : NSObject

@property (nonatomic, readonly) CGFloat red, green, blue, alpha;

+ (instancetype)colorWithRed:(CGFloat)r green:(CGFloat)g blue:(CGFloat)b alpha:(CGFloat)a;
+ (instancetype)blackColor;
+ (instancetype)whiteColor;
+ (instancetype)clearColor;

- (instancetype)initWithRed:(CGFloat)r green:(CGFloat)g blue:(CGFloat)b alpha:(CGFloat)a;

@end
```

## See Also

- [GNUKext Framework Documentation](../GNUKext/Documentation/Introduction.md)
- [Building GNUKext-based Kexts](../GNUKext/Documentation/Linux_Kexts.md)