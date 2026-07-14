# GNUKext API Reference

Complete API documentation for the GNUKext framework.

## Constants

### Error Domain

```objc
extern NSString * const GNUKextErrorDomain;
extern NSString * const GNUKextDriverErrorDomain;
```

## Error Codes

### GNUKextErrorCode

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
```

### GNUKextDriverErrorCode

```objc
typedef enum {
    GNUKextDriverErrorStartFailed = 5000,
    GNUKextDriverErrorStopFailed,
    GNUKextDriverErrorNotMatched,
    GNUKextDriverErrorProviderLost,
    GNUKextDriverErrorInitFailed
} GNUKextDriverErrorCode;
```

## Types

### KextInfo

Cross-platform kext information structure (libkern.h).

```c
typedef struct KextInfo {
    NSString           *bundleIdentifier;
    NSString           *version;
    NSString           *compatibleVersion;
    NSString           *path;
    NSString           *executablePath;
    NSString           *name;
    KextType            type;
    KextInfoState       state;
    KextLoadResult      lastLoadResult;
    NSDictionary       *infoPlist;
    NSDictionary       *personalities;
    NSArray            *dependencyIDs;
    BOOL                allowUserLoad;
    BOOL                isAuthenticated;
} KextInfo;

typedef struct KextInfo *KextInfoRef;
```

### KextType

```objc
typedef enum {
    kKextTypeKext = 0,
    kKextTypeLibrary,
    kKextTypeKernelModule
} KextType;
```

### KextInfoState

```objc
typedef enum {
    kKextInfoStateUnloaded = 0,
    kKextInfoStateLoading,
    kKextInfoStateLoaded,
    kKextInfoStateUnloading,
    kKextInfoStateFailed
} KextInfoState;
```

## Functions

### KextInfo Functions

```c
KextInfoRef  KextInfoCreate(void);
void         KextInfoFree(KextInfoRef info);

KextInfoRef  KextInfoCreateFromPath(NSString *path);
KextInfoRef  KextInfoCreateFromIdentifier(NSString *identifier);

KextLoadResult KextInfoLoad(KextInfoRef info);
KextLoadResult KextInfoUnload(KextInfoRef info);

BOOL         KextInfoAddPersonalities(KextInfoRef info);
BOOL         KextInfoRemovePersonalities(KextInfoRef info);

KextInfoRef  KextInfoFindWithIdentifier(NSString *identifier);
NSArray     *KextInfoGetAllLoaded(void);

NSString    *KextInfoGetVersion(void);
```

## Classes

### GNUKext

Main class for managing kext bundles.

```objc
@interface GNUKext : NSObject

+ (instancetype)kextWithPath:(NSString *)path;
+ (instancetype)kextWithIdentifier:(NSString *)identifier;
+ (NSArray *)allLoadedKexts;
+ (BOOL)loadKextWithIdentifier:(NSString *)identifier;
+ (BOOL)unloadKextWithIdentifier:(NSString *)identifier;
+ (NSString *)libkernVersion;

+ (void)registerDriverClass:(Class)driverClass;
+ (void)registerDriverClass:(Class)driverClass forPersonality:(NSString *)personalityName;
+ (NSArray *)registeredDriverClasses;
+ (void)matchAndStartDrivers;

- (instancetype)initWithPath:(NSString *)path;
- (BOOL)load;
- (BOOL)unload;
- (BOOL)isLoaded;
- (BOOL)addPersonalities;
- (BOOL)removePersonalities;
- (BOOL)startMatchingWithProvider:(GNUKextRegistryEntry *)provider;
- (NSDictionary *)mergedInfoDictionary;
- (NSArray *)driverInstances;

@property (nonatomic, readonly, copy) NSString *bundlePath;
@property (nonatomic, readonly, copy) NSString *bundleIdentifier;
@property (nonatomic, readonly, copy) NSString *bundleVersion;
@property (nonatomic, readonly, copy) NSString *bundleName;
@property (nonatomic, readonly, copy) NSDictionary *infoPlist;
@property (nonatomic, readonly, copy) NSDictionary *personalities;
@property (nonatomic, readonly, copy) NSArray *dependencies;
@property (nonatomic, readonly, assign) BOOL loaded;

@end
```

### libkernDriver

Utility class for kext operations.

```objc
@interface libkernDriver : NSObject

+ (NSDictionary *)driverPlistForKext:(NSString *)kextPath;
+ (NSDictionary *)driverPlistForIdentifier:(NSString *)identifier;
+ (BOOL)writeDriverPlist:(NSDictionary *)plist toPath:(NSString *)path;
+ (NSString *)defaultModulesPath;

@end
```

### Macros

```objc
#define GNUKextDriverDeclare(_class) \
    + (NSArray *)personalityKeys; \
    + (BOOL)doesMatchProvider:(GNUKextRegistryEntry *)provider \
              withPersonality:(GNUKextPersonality *)personality;

#define GNUKextDriverImplement(_class) \
    + (NSArray *)personalityKeys { return @[]; } \
    + (BOOL)doesMatchProvider:(GNUKextRegistryEntry *)provider \
              withPersonality:(GNUKextPersonality *)personality { \
        return YES; \
    }
```

## Protocols

### GNUKextDriverMatching

Optional protocol for driver matching.

```objc
@protocol GNUKextDriverMatching <NSObject>
@optional
- (BOOL)matchPersonality:(GNUKextPersonality *)personality
             withProvider:(GNUKextRegistryEntry *)provider;
@end
```

## Classes (continued)

### GNUKextDriver

Base class for kext drivers.

```objc
@interface GNUKextDriver : NSObject <GNUKextDriverMatching>

+ (NSArray *)personalityKeys;
+ (BOOL)doesMatchProvider:(GNUKextRegistryEntry *)provider
             withPersonality:(GNUKextPersonality *)personality;

- (instancetype)initWithPersonality:(GNUKextPersonality *)personality;
- (BOOL)start:(GNUKextRegistryEntry *)provider;
- (void)stop:(GNUKextRegistryEntry *)provider;
- (void)free;

- (BOOL)isStarted;
- (BOOL)isTerminated;

@property (nonatomic, readonly, retain) GNUKextRegistryEntry *provider;
@property (nonatomic, readonly, retain) GNUKextPersonality *matchedPersonality;
@property (nonatomic, readonly, copy) NSDictionary *personalityInfo;

@end
```

### GNUKextPersonality

Represents an IOKit personality dictionary.

```objc
@interface GNUKextPersonality : NSObject

+ (instancetype)personalityWithName:(NSString *)name dictionary:(NSDictionary *)dict;

- (instancetype)initWithName:(NSString *)name dictionary:(NSDictionary *)dict;

- (BOOL)matchesProvider:(GNUKextRegistryEntry *)provider;

- (NSString *)name;
- (NSDictionary *)dictionary;
- (id)objectForKey:(NSString *)key;
- (NSString *)providerClass;
- (NSString *)ioClass;
- (NSDictionary *)propertyMatch;
- (NSDictionary *)propertyTable;
- (NSInteger)probeScore;

@property (nonatomic, readonly, copy) NSString *name;
@property (nonatomic, readonly, copy) NSDictionary *dictionary;

@end
```

### GNUKextRegistryEntry

Represents a node in the IOKit registry tree.

```objc
@interface GNUKextRegistryEntry : NSObject

+ (instancetype)rootEntry;
+ (instancetype)entryWithName:(NSString *)name;
+ (instancetype)entryWithPath:(NSString *)path;

- (instancetype)initWithName:(NSString *)name;
- (instancetype)initWithName:(NSString *)name parent:(GNUKextRegistryEntry *)parent;

- (id)propertyForKey:(NSString *)key;
- (void)setProperty:(id)property forKey:(NSString *)key;
- (void)removePropertyForKey:(NSString *)key;
- (NSDictionary *)allProperties;

- (BOOL)hasProperty:(NSString *)key;
- (BOOL)hasProperty:(NSString *)key withValue:(id)value;
- (BOOL)hasProperty:(NSString *)key matchingPrefix:(NSString *)prefix;

- (void)addChild:(GNUKextRegistryEntry *)child;
- (void)removeChild:(GNUKextRegistryEntry *)child;
- (GNUKextRegistryEntry *)childWithName:(NSString *)name;
- (NSArray *)childEntries;

- (GNUKextRegistryEntry *)parentEntry;
- (GNUKextRegistryEntry *)rootEntry;
- (NSString *)entryPath;
- (NSString *)name;

- (GNUKextRegistryEntry *)entryWithPath:(NSString *)path;

@property (nonatomic, readonly, copy) NSString *name;
@property (nonatomic, readonly, copy) NSString *entryPath;

@end
```

## Darwin-Specific Classes

### DarwinKext

Darwin/XNU-specific kext implementation (only available on macOS).

```objc
@interface DarwinKext : NSObject

+ (BOOL)loadKextAtPath:(NSString *)bundlePath;
+ (BOOL)loadKextWithIdentifier:(NSString *)identifier;
+ (BOOL)unloadKextWithIdentifier:(NSString *)identifier;
+ (BOOL)isKextLoaded:(NSString *)identifier;
+ (NSArray *)allLoadedIdentifiers;
+ (NSString *)systemExtensionsPath;
+ (NSString *)localExtensionsPath;

+ (BOOL)installKext:(NSString *)bundlePath error:(NSError **)error;
+ (BOOL)uninstallKext:(NSString *)identifier error:(NSError **)error;

- (instancetype)initWithPath:(NSString *)bundlePath;
- (BOOL)load;
- (BOOL)unload;
- (BOOL)isLoaded;

@property (nonatomic, readonly, copy) NSString *identifier;
@property (nonatomic, readonly, copy) NSString *bundlePath;
@property (nonatomic, readonly, assign) BOOL loaded;

@end
```

## Linux-Specific Classes

### LinuxModule

Linux kernel module wrapper.

```objc
@interface LinuxModule : NSObject

+ (BOOL)loadModuleAtPath:(NSString *)path;
+ (BOOL)unloadModule:(NSString *)name;
+ (BOOL)isModuleLoaded:(NSString *)name;
+ (NSArray *)allLoadedModules;
+ (NSString *)kernelVersion;

- (instancetype)initWithPath:(NSString *)path;
- (BOOL)load;
- (BOOL)unload;

@property (nonatomic, readonly, copy) NSString *moduleName;
@property (nonatomic, readonly, copy) NSString *modulePath;

@end
```

## See Also

- [GNUKext Introduction](Introduction.md)
- [Linux Kernel Modules](Linux_Kexts.md)
- [KotlinKit Documentation](../KotlinKit/README.md)