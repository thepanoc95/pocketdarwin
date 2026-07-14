#import <Foundation/Foundation.h>
#import "KotlinKext.h"
#import <GNUKext/GNUKext.h>

NSString * const KotlinKextErrorDomain = @"KotlinKextErrorDomain";

@interface KotlinKext ()
@property (nonatomic, strong) GNUKext *internalKext;
@end

@implementation KotlinKext

+ (instancetype)kextWithPath:(NSString *)path {
    return [[self alloc] initWithPath:path];
}

+ (instancetype)kextWithIdentifier:(NSString *)identifier {
    GNUKext *kext = [GNUKext kextWithIdentifier:identifier];
    if (kext) {
        return [[self alloc] initWithKext:kext];
    }
    return nil;
}

+ (NSArray *)allLoadedKexts {
    NSArray *loadedKexts = [GNUKext allLoadedKexts];
    NSMutableArray *result = [NSMutableArray arrayWithCapacity:[loadedKexts count]];
    for (GNUKext *kext in loadedKexts) {
        [result addObject:[[self alloc] initWithKext:kext]];
    }
    return [result copy];
}

+ (BOOL)loadKextWithIdentifier:(NSString *)identifier {
    return [GNUKext loadKextWithIdentifier:identifier];
}

+ (BOOL)unloadKextWithIdentifier:(NSString *)identifier {
    return [GNUKext unloadKextWithIdentifier:identifier];
}

- (instancetype)initWithPath:(NSString *)path {
    GNUKext *kext = [GNUKext kextWithPath:path];
    return [self initWithKext:kext];
}

- (instancetype)initWithKext:(GNUKext *)kext {
    self = [super init];
    if (self) {
        _internalKext = [kext retain];
    }
    return self;
}

- (instancetype)init {
    return [self initWithPath:nil];
}

- (void)dealloc {
    [_internalKext release];
    [super dealloc];
}

- (BOOL)load {
    return [_internalKext load];
}

- (BOOL)unload {
    return [_internalKext unload];
}

- (BOOL)isLoaded {
    return [_internalKext isLoaded];
}

- (NSString *)bundlePath {
    return [[_internalKext bundlePath] copy];
}

- (NSString *)bundleIdentifier {
    return [[_internalKext bundleIdentifier] copy];
}

- (NSString *)bundleVersion {
    return [[_internalKext bundleVersion] copy];
}

- (NSString *)bundleName {
    return [[_internalKext bundleName] copy];
}

- (BOOL)loaded {
    return [[_internalKext loaded] boolValue];
}

- (NSDictionary *)personalities {
    return [[_internalKext personalities] copy];
}

- (NSArray *)dependencies {
    return [[_internalKext dependencies] copy];
}

- (NSDictionary *)mergedInfoDictionary {
    return [[_internalKext mergedInfoDictionary] copy];
}

- (NSArray *)driverInstances {
    return [[_internalKext driverInstances] copy];
}

@end

@implementation KotlinKextDriver

+ (NSArray *)personalityKeys {
    return @[];
}

+ (BOOL)doesMatchProvider:(id)provider withPersonality:(id)personality {
    return YES;
}

- (instancetype)initWithPersonality:(id)personality {
    self = [super init];
    if (self) {
        _provider = nil;
        _matchedPersonality = personality;
        _personalityInfo = @{};
        _started = NO;
        _terminated = NO;
    }
    return self;
}

- (BOOL)start:(id)provider {
    _started = YES;
    _provider = [provider retain];
    return YES;
}

- (void)stop:(id)provider {
    _started = NO;
    [_provider release];
    _provider = nil;
}

- (void)free {
    _terminated = YES;
    _started = NO;
    [_provider release];
    _provider = nil;
}

- (BOOL)isStarted {
    return _started;
}

- (BOOL)isTerminated {
    return _terminated;
}

@end

@implementation KotlinKextPersonality

+ (instancetype)personalityWithName:(NSString *)name properties:(NSDictionary *)props {
    return [[self alloc] initWithName:name properties:props];
}

- (instancetype)initWithName:(NSString *)name properties:(NSDictionary *)props {
    self = [super init];
    if (self) {
        _personalityName = [name copy];
        _properties = [props copy];
    }
    return self;
}

- (void)dealloc {
    [_personalityName release];
    [_properties release];
    [super dealloc];
}

- (NSString *)personalityName {
    return _personalityName;
}

- (NSDictionary *)properties {
    return _properties;
}

- (NSString *)providerClass {
    return [_properties objectForKey:@"IOProviderClass"];
}

- (NSString *)ioClass {
    return [_properties objectForKey:@"IOClass"];
}

- (NSDictionary *)propertyMatch {
    return [_properties objectForKey:@"IOPropertyMatch"];
}

- (NSDictionary *)propertyTable {
    return [_properties objectForKey:@"IOPropertyTable"];
}

- (NSInteger)probeScore {
    id score = [_properties objectForKey:@"IOProbeScore"];
    if (score != nil && [score respondsToSelector:@selector(integerValue)]) {
        return [score integerValue];
    }
    return 0;
}

@end