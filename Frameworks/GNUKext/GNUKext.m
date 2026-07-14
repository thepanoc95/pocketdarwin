/*
   Project: GNUKext

   Copyright (C) 2026 Free Software Foundation

   Author: milo

   Created: 2026-07-13 19:36:24 -0300 by milo

   Cross-platform Kernel Extension framework implementation.

   This application is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This application is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 31 Milk Street #960789 Boston, MA 02196 USA.
*/

#import "GNUKext.h"
#import "LinuxModule.h"
#import "BSDModule.h"
#import "libkern.h"

NSString * const GNUKextErrorDomain = @"GNUKextErrorDomain";

static NSMutableArray *_globalLoadedKexts = nil;
static NSMutableDictionary *_registeredDriverClasses = nil;

@implementation GNUKext

+ (void)initialize
{
    if (self == [GNUKext class]) {
        _globalLoadedKexts = [[NSMutableArray alloc] init];
        _registeredDriverClasses = [[NSMutableDictionary alloc] init];
    }
}

+ (instancetype)kextWithPath:(NSString *)path
{
    return [[[self alloc] initWithPath:path] autorelease];
}

+ (instancetype)kextWithIdentifier:(NSString *)identifier
{
    if (identifier == nil) return nil;

    for (GNUKext *kext in _globalLoadedKexts) {
        if ([kext.bundleIdentifier isEqualToString:identifier]) {
            return kext;
        }
    }
    return nil;
}

+ (NSArray *)allLoadedKexts
{
    return [[_globalLoadedKexts copy] autorelease];
}

+ (BOOL)loadKextWithIdentifier:(NSString *)identifier
{
    GNUKext *kext = [self kextWithIdentifier:identifier];
    if (kext == nil) return NO;
    return [kext load];
}

+ (BOOL)unloadKextWithIdentifier:(NSString *)identifier
{
    GNUKext *kext = [self kextWithIdentifier:identifier];
    if (kext == nil) return NO;
    return [kext unload];
}

+ (NSString *)libkernVersion
{
    return KextInfoGetVersion();
}

+ (void)registerDriverClass:(Class)driverClass
{
    if (driverClass == nil) return;

    NSString *className = NSStringFromClass(driverClass);
    NSArray *keys = nil;

    if ([driverClass respondsToSelector:@selector(personalityKeys)]) {
        keys = [driverClass performSelector:@selector(personalityKeys)];
    }

    if (keys != nil && [keys count] > 0) {
        for (NSString *key in keys) {
            [_registeredDriverClasses setObject:driverClass forKey:key];
        }
    } else {
        [_registeredDriverClasses setObject:driverClass forKey:className];
    }

    NSLog(@"GNUKext: registered driver class: %@", className);
}

+ (void)registerDriverClass:(Class)driverClass forPersonality:(NSString *)personalityName
{
    if (driverClass == nil || personalityName == nil) return;

    [_registeredDriverClasses setObject:driverClass forKey:personalityName];
    NSLog(@"GNUKext: registered driver class %@ for personality %@",
          NSStringFromClass(driverClass), personalityName);
}

+ (NSArray *)registeredDriverClasses
{
    return [_registeredDriverClasses allValues];
}

+ (void)matchAndStartDrivers
{
    for (GNUKext *kext in _globalLoadedKexts) {
        GNUKextRegistryEntry *root = [GNUKextRegistryEntry rootEntry];
        for (NSString *personalityName in [kext personalities]) {
            NSDictionary *personalityDict = [[kext personalities] objectForKey:personalityName];
            GNUKextPersonality *personality = [GNUKextPersonality personalityWithName:personalityName
                                                                         dictionary:personalityDict];

            NSArray *childEntries = [root childEntries];
            for (GNUKextRegistryEntry *provider in childEntries) {
                if ([personality matchesProvider:provider]) {
                    [kext startMatchingWithProvider:provider];
                }
            }
        }
    }
}

- (instancetype)initWithPath:(NSString *)path
{
    self = [super init];
    if (self) {
        _bundlePath = [path copy];
        _driverInstances = [[NSMutableArray alloc] init];

        NSString *plistPath = [path stringByAppendingPathComponent:@"Info.plist"];
        _infoPlist = [[NSDictionary dictionaryWithContentsOfFile:plistPath] copy];

        if (_infoPlist == nil) {
            plistPath = [path stringByAppendingPathComponent:@"Contents/Info.plist"];
            _infoPlist = [[NSDictionary dictionaryWithContentsOfFile:plistPath] copy];
        }

        _bundleIdentifier = [[_infoPlist objectForKey:@"CFBundleIdentifier"] copy];
        _bundleVersion = [[_infoPlist objectForKey:@"CFBundleVersion"] copy];
        _bundleName = [[_infoPlist objectForKey:@"CFBundleName"] copy];
        _personalities = [[_infoPlist objectForKey:@"IOKitPersonalities"] copy];
        _dependencies = [[[_infoPlist objectForKey:@"OSBundleLibraries"] allKeys] copy];
        _loaded = NO;
        _kextInfo = NULL;

        if (_bundleIdentifier != nil) {
            _kextInfo = KextInfoCreateFromPath(path);
        }
    }
    return self;
}

- (void)dealloc
{
    for (GNUKextDriver *driver in _driverInstances) {
        [driver free];
    }

    [_bundlePath release];
    [_bundleIdentifier release];
    [_bundleVersion release];
    [_bundleName release];
    [_infoPlist release];
    [_personalities release];
    [_dependencies release];
    [_driverInstances release];

    if (_kextInfo != NULL) {
        KextInfoFree(_kextInfo);
        _kextInfo = NULL;
    }

    [super dealloc];
}

- (BOOL)load
{
    if (_loaded) return YES;
    if (_bundleIdentifier == nil) return NO;

    if (_dependencies != nil) {
        for (NSString *depID in _dependencies) {
            BOOL found = NO;
            for (GNUKext *kext in _globalLoadedKexts) {
                if ([kext.bundleIdentifier isEqualToString:depID]) {
                    found = YES;
                    break;
                }
            }
            if (!found) {
                NSLog(@"GNUKext: dependency not satisfied: %@", depID);
                return NO;
            }
        }
    }

    if (_kextInfo != NULL) {
        KextLoadResult result = KextInfoLoad(_kextInfo);
        if (result != kKextLoadResultSuccess) {
            NSLog(@"GNUKext: failed to load %@: %d", _bundleIdentifier, result);
            return NO;
        }
    }

    _loaded = YES;
    [_globalLoadedKexts addObject:self];

    NSLog(@"GNUKext: loaded %@ (%@)", _bundleName ?: _bundleIdentifier, _bundleVersion);
    return YES;
}

- (BOOL)unload
{
    if (!_loaded) return YES;
    if (_bundleIdentifier == nil) return NO;

    for (GNUKextDriver *driver in _driverInstances) {
        if ([driver isStarted]) {
            [driver stop:[driver provider]];
        }
    }

    if (_kextInfo != NULL) {
        KextLoadResult result = KextInfoUnload(_kextInfo);
        if (result != kKextLoadResultSuccess) {
            NSLog(@"GNUKext: failed to unload %@: %d", _bundleIdentifier, result);
            return NO;
        }
    }

    [_globalLoadedKexts removeObject:self];
    _loaded = NO;

    NSLog(@"GNUKext: unloaded %@", _bundleIdentifier);
    return YES;
}

- (BOOL)isLoaded
{
    return _loaded;
}

- (BOOL)addPersonalities
{
    if (!_loaded) return NO;
    if (_personalities == nil) return YES;

    NSLog(@"GNUKext: adding %lu personalities for %@",
          (unsigned long)[[_personalities allKeys] count], _bundleIdentifier);
    return YES;
}

- (BOOL)removePersonalities
{
    if (_personalities == nil) return YES;

    NSLog(@"GNUKext: removing personalities for %@", _bundleIdentifier);
    return YES;
}

- (BOOL)startMatchingWithProvider:(GNUKextRegistryEntry *)provider
{
    if (provider == nil) return NO;
    if (_personalities == nil) return YES;

    for (NSString *personalityName in _personalities) {
        NSDictionary *personalityDict = [_personalities objectForKey:personalityName];
        GNUKextPersonality *personality = [GNUKextPersonality personalityWithName:personalityName
                                                                     dictionary:personalityDict];

        if (![personality matchesProvider:provider]) continue;

        NSString *ioClass = [personality ioClass];
        Class driverClass = nil;

        if (ioClass != nil) {
            driverClass = [_registeredDriverClasses objectForKey:ioClass];
        }

        if (driverClass == nil) {
            for (NSString *key in _registeredDriverClasses) {
                driverClass = [_registeredDriverClasses objectForKey:key];
                break;
            }
        }

        if (driverClass == nil) {
            NSLog(@"GNUKext: no driver class found for personality %@ (IOClass=%@)",
                  personalityName, ioClass ?: @"(any)");
            continue;
        }

        if (![driverClass isSubclassOfClass:[GNUKextDriver class]]) {
            NSLog(@"GNUKext: driver class %@ is not a GNUKextDriver subclass",
                  NSStringFromClass(driverClass));
            continue;
        }

        GNUKextDriver *driver = [(GNUKextDriver *)[driverClass alloc]
            initWithPersonality:personality];

        if ([driver start:provider]) {
            [_driverInstances addObject:driver];
            NSLog(@"GNUKext: started driver %@ on %@ for personality %@",
                  NSStringFromClass(driverClass),
                  [provider entryPath],
                  personalityName);
        } else {
            NSLog(@"GNUKext: driver %@ failed to start on %@",
                  NSStringFromClass(driverClass),
                  [provider entryPath]);
        }

        [driver release];
    }

    return YES;
}

- (NSDictionary *)mergedInfoDictionary
{
    return _infoPlist;
}

- (NSArray *)driverInstances
{
    return [[_driverInstances copy] autorelease];
}

@end

@implementation libkernDriver

+ (NSDictionary *)driverPlistForKext:(NSString *)kextPath
{
    if (kextPath == nil) return nil;

    NSString *plistPath = [kextPath stringByAppendingPathComponent:@"Info.plist"];
    NSDictionary *plist = [NSDictionary dictionaryWithContentsOfFile:plistPath];
    if (plist == nil) {
        plistPath = [kextPath stringByAppendingPathComponent:@"Contents/Info.plist"];
        plist = [NSDictionary dictionaryWithContentsOfFile:plistPath];
    }

    return plist;
}

+ (NSDictionary *)driverPlistForIdentifier:(NSString *)identifier
{
    if (identifier == nil) return nil;

    GNUKext *kext = [GNUKext kextWithIdentifier:identifier];
    return [kext infoPlist];
}

+ (BOOL)writeDriverPlist:(NSDictionary *)plist toPath:(NSString *)path
{
    if (plist == nil || path == nil) return NO;
    return [plist writeToFile:path atomically:YES];
}

+ (NSString *)defaultModulesPath
{
#if defined(__linux__)
    NSString *version = [LinuxKernelModule kernelVersion];
    if (version != nil) {
        return [NSString stringWithFormat:@"/lib/modules/%@", version];
    }
    return @"/lib/modules";
#else
    return @"/boot/modules";
#endif
}

@end
