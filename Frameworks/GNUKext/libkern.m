/*
   Project: GNUKext

   Copyright (C) 2026 Free Software Foundation

   Author: milo

   Created: 2026-07-13 19:39:55 -0300 by milo

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

#import "libkern.h"
#import "LinuxModule.h"
#import "BSDModule.h"
#import "DarwinModule.h"

#if defined(__APPLE__)
  #include <libkern/OSKext.h>
  #include <libkern/OSKextLib.h>
  #include <libkern/OSKextPrivate.h>
#endif

#if !defined(__APPLE__)

static NSMutableArray *_loadedKexts = nil;

static void _ensureLoadedKextsArray(void)
{
    if (_loadedKexts == nil) {
        _loadedKexts = [[NSMutableArray alloc] init];
    }
}

KextInfoRef KextInfoCreate(void)
{
    struct KextInfo *info = calloc(1, sizeof(struct KextInfo));
    if (info != NULL) {
        info->state = kKextInfoStateUnloaded;
        info->type = kKextTypeKext;
        info->lastLoadResult = kKextLoadResultSuccess;
        info->allowUserLoad = YES;
        info->isAuthenticated = NO;
    }
    return info;
}

void KextInfoFree(KextInfoRef info)
{
    if (info == NULL) return;
    [info->bundleIdentifier release];
    [info->version release];
    [info->compatibleVersion release];
    [info->path release];
    [info->executablePath release];
    [info->name release];
    [info->infoPlist release];
    [info->personalities release];
    [info->dependencyIDs release];
    free(info);
}

KextInfoRef KextInfoCreateFromPath(NSString *path)
{
    if (path == nil) return NULL;

    NSString *plistPath = [path stringByAppendingPathComponent:@"Info.plist"];
    NSDictionary *plist = [NSDictionary dictionaryWithContentsOfFile:plistPath];
    if (plist == nil) {
        plistPath = [path stringByAppendingPathComponent:@"Contents/Info.plist"];
        plist = [NSDictionary dictionaryWithContentsOfFile:plistPath];
    }
    if (plist == nil) return NULL;

    KextInfoRef info = KextInfoCreate();
    if (info == NULL) return NULL;

    info->path = [path copy];
    info->infoPlist = [plist copy];
    info->bundleIdentifier = [[plist objectForKey:@"CFBundleIdentifier"] copy];
    info->version = [[plist objectForKey:@"CFBundleVersion"] copy];
    info->compatibleVersion = [[plist objectForKey:@"OSBundleCompatibleVersion"] copy];
    info->name = [[plist objectForKey:@"CFBundleName"] copy];

    NSString *execName = [plist objectForKey:@"CFBundleExecutable"];
    if (execName != nil) {
        info->executablePath = [[path stringByAppendingPathComponent:execName] copy];
    }

    info->personalities = [[plist objectForKey:@"IOKitPersonalities"] copy];
    info->dependencyIDs = [[[plist objectForKey:@"OSBundleLibraries"] allKeys] copy];
    info->allowUserLoad = [[plist objectForKey:@"OSBundleAllowUserLoad"] boolValue];

    NSString *required = [plist objectForKey:@"OSBundleRequired"];
    if ([required isEqualToString:@"Root"] || [required isEqualToString:@"Local-Root"]) {
        info->allowUserLoad = NO;
    }

    NSString *pkgType = [plist objectForKey:@"CFBundlePackageType"];
    if ([pkgType isEqualToString:@"KEXT"]) {
        info->type = kKextTypeKext;
    } else if ([pkgType isEqualToString:@"BNDL"]) {
        info->type = kKextTypeLibrary;
    } else {
        info->type = kKextTypeKernelModule;
    }

    return info;
}

KextInfoRef KextInfoCreateFromIdentifier(NSString *identifier)
{
    if (identifier == nil) return NULL;

    _ensureLoadedKextsArray();
    for (NSValue *val in _loadedKexts) {
        KextInfoRef info = [val pointerValue];
        if ([info->bundleIdentifier isEqualToString:identifier]) {
            return info;
        }
    }
    return NULL;
}

KextInfoRef KextInfoFindWithIdentifier(NSString *identifier)
{
    return KextInfoCreateFromIdentifier(identifier);
}

NSArray *KextInfoGetAllLoaded(void)
{
    _ensureLoadedKextsArray();
    return [[_loadedKexts copy] autorelease];
}

NSString *KextInfoGetVersion(void)
{
    return @"0.1";
}

static BOOL _checkDependencies(KextInfoRef info)
{
    if (info->dependencyIDs == nil || [info->dependencyIDs count] == 0) {
        return YES;
    }

    _ensureLoadedKextsArray();
    for (NSString *depID in info->dependencyIDs) {
        BOOL found = NO;
        for (NSValue *val in _loadedKexts) {
            KextInfoRef loaded = [val pointerValue];
            if ([loaded->bundleIdentifier isEqualToString:depID]) {
                found = YES;
                break;
            }
        }
        if (!found) {
            NSLog(@"GNUKext: missing dependency: %@", depID);
            return NO;
        }
    }
    return YES;
}

KextLoadResult KextInfoLoad(KextInfoRef info)
{
    if (info == NULL) return kKextLoadResultInvalidInfo;
    if (info->state == kKextInfoStateLoaded) return kKextLoadResultAlreadyLoaded;
    if (info->bundleIdentifier == nil) return kKextLoadResultInvalidInfo;

    if (!_checkDependencies(info)) {
        info->lastLoadResult = kKextLoadResultMissingDependencies;
        return kKextLoadResultMissingDependencies;
    }

    info->state = kKextInfoStateLoading;

#if defined(__APPLE__)
    if (info->path != nil) {
        if (![DarwinKext loadKextAtPath:info->path]) {
            info->state = kKextInfoStateFailed;
            info->lastLoadResult = kKextLoadResultFailedToLoad;
            return kKextLoadResultFailedToLoad;
        }
    }
#elif defined(__linux__)
    if (info->executablePath != nil &&
        [[NSFileManager defaultManager] fileExistsAtPath:info->executablePath]) {
        if (![LinuxKernelModule loadModuleAtPath:info->executablePath]) {
            info->state = kKextInfoStateFailed;
            info->lastLoadResult = kKextLoadResultFailedToLoad;
            return kKextLoadResultFailedToLoad;
        }
    }
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
    if (info->executablePath != nil &&
        [[NSFileManager defaultManager] fileExistsAtPath:info->executablePath]) {
        if (![BSDKernelModule loadModuleAtPath:info->executablePath]) {
            info->state = kKextInfoStateFailed;
            info->lastLoadResult = kKextLoadResultFailedToLoad;
            return kKextLoadResultFailedToLoad;
        }
    }
#endif

    info->state = kKextInfoStateLoaded;
    info->lastLoadResult = kKextLoadResultSuccess;

    _ensureLoadedKextsArray();
    [_loadedKexts addObject:[NSValue valueWithPointer:info]];

    return kKextLoadResultSuccess;
}

KextLoadResult KextInfoUnload(KextInfoRef info)
{
    if (info == NULL) return kKextLoadResultInvalidInfo;
    if (info->state != kKextInfoStateLoaded) return kKextLoadResultFailedToLoad;

    info->state = kKextInfoStateUnloading;

    KextInfoRemovePersonalities(info);

#if defined(__APPLE__)
    if (info->bundleIdentifier != nil) {
        [DarwinKext unloadKextWithIdentifier:info->bundleIdentifier];
    }
#elif defined(__linux__)
    if (info->executablePath != nil &&
        [[NSFileManager defaultManager] fileExistsAtPath:info->executablePath]) {
        [LinuxKernelModule unloadModuleAtPath:info->executablePath];
    }
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
    if (info->executablePath != nil &&
        [[NSFileManager defaultManager] fileExistsAtPath:info->executablePath]) {
        [BSDKernelModule unloadModuleAtPath:info->executablePath];
    }
#endif

    info->state = kKextInfoStateUnloaded;

    _ensureLoadedKextsArray();
    for (NSUInteger i = 0; i < [_loadedKexts count]; i++) {
        KextInfoRef loaded = [[_loadedKexts objectAtIndex:i] pointerValue];
        if (loaded == info) {
            [_loadedKexts removeObjectAtIndex:i];
            break;
        }
    }

    return kKextLoadResultSuccess;
}

BOOL KextInfoAddPersonalities(KextInfoRef info)
{
    if (info == NULL || info->personalities == nil) return NO;
    if (info->state != kKextInfoStateLoaded) return NO;

    NSLog(@"GNUKext: adding %lu personalities for %@",
          (unsigned long)[[info->personalities allKeys] count],
          info->bundleIdentifier);
    return YES;
}

BOOL KextInfoRemovePersonalities(KextInfoRef info)
{
    if (info == NULL || info->personalities == nil) return NO;

    NSLog(@"GNUKext: removing personalities for %@", info->bundleIdentifier);
    return YES;
}

#endif /* !__APPLE__ */
