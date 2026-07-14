/*
   Project: GNUKext

   Copyright (C) 2026 Free Software Foundation

   Author: milo

   Darwin/XNU kernel extension injection via the native OSKext APIs.

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

#import "DarwinModule.h"

#if defined(__APPLE__)

#import <Foundation/Foundation.h>
#import <libkern/OSKext.h>
#import <libkern/OSKextLib.h>

NSString * const DarwinKextErrorDomain = @"DarwinKextErrorDomain";

@implementation DarwinKext

+ (NSString *)systemExtensionsPath
{
    return @"/System/Library/Extensions";
}

+ (NSString *)localExtensionsPath
{
    NSString *home = NSHomeDirectory();
    return [home stringByAppendingPathComponent:@"Library/Extensions"];
}

+ (BOOL)loadKextAtPath:(NSString *)bundlePath
{
    if (bundlePath == nil) return NO;

    NSURL *url = [NSURL fileURLWithPath:bundlePath];
    if (url == nil) return NO;

    OSKextRef kext = OSKextGetWithIDentifier(
        (__bridge CFStringRef)[self _identifierForPath:bundlePath]);

    if (kext == NULL) {
        CFErrorRef cfError = NULL;
        kext = OSKextLoadFromURL((__bridge CFURLRef)url, 0, &cfError);
        if (cfError != NULL) {
            NSLog(@"DarwinKext: load failed for %@: %@",
                  bundlePath, cfError);
            CFRelease(cfError);
            return NO;
        }
    }

    if (kext == NULL) {
        NSLog(@"DarwinKext: failed to load kext from %@", bundlePath);
        return NO;
    }

    NSLog(@"DarwinKext: loaded %@", bundlePath);
    return YES;
}

+ (BOOL)loadKextWithIdentifier:(NSString *)identifier
{
    if (identifier == nil) return NO;

    OSKextRef kext = OSKextGetWithIDentifier(
        (__bridge CFStringRef)identifier);

    if (kext == NULL) {
        NSLog(@"DarwinKext: kext not found: %@", identifier);
        return NO;
    }

    if (OSKextIsLoaded(kext)) {
        NSLog(@"DarwinKext: already loaded: %@", identifier);
        return YES;
    }

    OSKextLoadResult result = OSKextLoad(kext, 0, NULL);
    if (result != kOSKextReturnSuccess) {
        NSLog(@"DarwinKext: failed to load %@: %d", identifier, result);
        return NO;
    }

    NSLog(@"DarwinKext: loaded %@", identifier);
    return YES;
}

+ (BOOL)unloadKextWithIdentifier:(NSString *)identifier
{
    if (identifier == nil) return NO;

    OSKextRef kext = OSKextGetWithIDentifier(
        (__bridge CFStringRef)identifier);

    if (kext == NULL) {
        NSLog(@"DarwinKext: kext not found: %@", identifier);
        return NO;
    }

    if (!OSKextIsLoaded(kext)) {
        NSLog(@"DarwinKext: not loaded: %@", identifier);
        return NO;
    }

    OSKextLoadResult result = OSKextUnload(kext);
    if (result != kOSKextReturnSuccess) {
        NSLog(@"DarwinKext: failed to unload %@: %d", identifier, result);
        return NO;
    }

    NSLog(@"DarwinKext: unloaded %@", identifier);
    return YES;
}

+ (BOOL)isKextLoaded:(NSString *)identifier
{
    if (identifier == nil) return NO;

    OSKextRef kext = OSKextGetWithIDentifier(
        (__bridge CFStringRef)identifier);

    return (kext != NULL && OSKextIsLoaded(kext));
}

+ (NSArray *)allLoadedIdentifiers
{
    NSMutableArray *identifiers = [NSMutableArray array];
    CFArrayRef allKexts = OSKextGetAllKexts();
    if (allKexts == NULL) return identifiers;

    CFIndex count = CFArrayGetCount(allKexts);
    for (CFIndex i = 0; i < count; i++) {
        OSKextRef kext = (OSKextRef)CFArrayGetValueAtIndex(allKexts, i);
        if (OSKextIsLoaded(kext)) {
            CFStringRef idRef = OSKextGetIdentifier(kext);
            if (idRef != NULL) {
                [identifiers addObject:(__bridge NSString *)idRef];
            }
        }
    }

    return identifiers;
}

+ (BOOL)installKext:(NSString *)bundlePath error:(NSError **)error
{
    if (bundlePath == nil) {
        if (error != NULL) {
            *error = [NSError errorWithDomain:DarwinKextErrorDomain
                                         code:DarwinKextErrorInstallFailed
                                     userInfo:@{NSLocalizedDescriptionKey:
                @"bundlePath must not be nil"}];
        }
        return NO;
    }

    if (![[NSFileManager defaultManager] fileExistsAtPath:bundlePath]) {
        if (error != NULL) {
            *error = [NSError errorWithDomain:DarwinKextErrorDomain
                                         code:DarwinKextErrorNotFound
                                     userInfo:@{NSLocalizedDescriptionKey:
                [NSString stringWithFormat:@"Kext not found at %@", bundlePath]}];
        }
        return NO;
    }

    NSString *destDir = [self systemExtensionsPath];
    NSString *destPath = [destDir stringByAppendingPathComponent:
        [bundlePath lastPathComponent]];

    NSFileManager *fm = [NSFileManager defaultManager];
    [fm removeItemAtPath:destPath error:nil];

    BOOL ok = [fm copyItemAtPath:bundlePath toPath:destPath error:error];
    if (!ok) {
        NSLog(@"DarwinKext: install failed: %@", [(*error) localizedDescription]);
        return NO;
    }

    NSLog(@"DarwinKext: installed %@ -> %@", bundlePath, destPath);
    return YES;
}

+ (BOOL)uninstallKext:(NSString *)identifier error:(NSError **)error
{
    if (identifier == nil) {
        if (error != NULL) {
            *error = [NSError errorWithDomain:DarwinKextErrorDomain
                                         code:DarwinKextErrorUninstallFailed
                                     userInfo:@{NSLocalizedDescriptionKey:
                @"identifier must not be nil"}];
        }
        return NO;
    }

    NSString *destDir = [self systemExtensionsPath];
    NSString *destPath = [destDir stringByAppendingPathComponent:
        [identifier stringByAppendingString:@".kext"]];

    if (![[NSFileManager defaultManager] fileExistsAtPath:destPath]) {
        destPath = [[self localExtensionsPath]
            stringByAppendingPathComponent:[identifier stringByAppendingString:@".kext"]];
    }

    if (![[NSFileManager defaultManager] fileExistsAtPath:destPath]) {
        if (error != NULL) {
            *error = [NSError errorWithDomain:DarwinKextErrorDomain
                                         code:DarwinKextErrorNotFound
                                     userInfo:@{NSLocalizedDescriptionKey:
                [NSString stringWithFormat:@"Kext %@ is not installed", identifier]}];
        }
        return NO;
    }

    BOOL ok = [[NSFileManager defaultManager] removeItemAtPath:destPath error:error];
    if (!ok) {
        NSLog(@"DarwinKext: uninstall failed: %@", [(*error) localizedDescription]);
        return NO;
    }

    NSLog(@"DarwinKext: uninstalled %@", identifier);
    return YES;
}

+ (NSString *)_identifierForPath:(NSString *)path
{
    NSString *plistPath = [path stringByAppendingPathComponent:@"Info.plist"];
    NSDictionary *plist = [NSDictionary dictionaryWithContentsOfFile:plistPath];
    return [plist objectForKey:@"CFBundleIdentifier"];
}

- (instancetype)initWithPath:(NSString *)bundlePath
{
    self = [super init];
    if (self) {
        _bundlePath = [bundlePath copy];

        NSString *plistPath = [bundlePath stringByAppendingPathComponent:@"Info.plist"];
        NSDictionary *plist = [NSDictionary dictionaryWithContentsOfFile:plistPath];
        _identifier = [[plist objectForKey:@"CFBundleIdentifier"] copy];
        _loaded = NO;
    }
    return self;
}

- (void)dealloc
{
    [_identifier release];
    [_bundlePath release];
    [super dealloc];
}

- (BOOL)load
{
    if (_loaded) return YES;
    if (_bundlePath == nil) return NO;

    BOOL ok = [[self class] loadKextAtPath:_bundlePath];
    if (ok) _loaded = YES;
    return ok;
}

- (BOOL)unload
{
    if (!_loaded) return YES;
    if (_identifier == nil) return NO;

    BOOL ok = [[self class] unloadKextWithIdentifier:_identifier];
    if (ok) _loaded = NO;
    return ok;
}

- (BOOL)isLoaded
{
    return _loaded;
}

@end

#endif /* __APPLE__ */
