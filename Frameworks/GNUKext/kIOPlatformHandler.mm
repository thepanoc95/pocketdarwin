/*
   Project: GNUKext

   Copyright (C) 2026 Free Software Foundation

   Author: milo

   Created: 2026-07-13 20:12:30 -0300 by milo

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
#import "GNUKext.h"
#import "LinuxModule.h"
#import "BSDModule.h"
#import "DarwinModule.h"
#import "kIOPlatformHandler.h"

#if defined(__APPLE__)
  #define kIOPlatform_is_native 1
#endif

#if !defined(kIOPlatform_is_native)
  #define kIOPlatform_is_native 0
#endif

#include <sys/utsname.h>

@implementation kIOPlatformHandler

+ (BOOL)isNativePlatform
{
    return kIOPlatform_is_native;
}

+ (NSString *)platformName
{
#if defined(__APPLE__)
    return @"Darwin";
#elif defined(__linux__)
    return @"Linux";
#elif defined(__FreeBSD__)
    return @"FreeBSD";
#elif defined(__NetBSD__)
    return @"NetBSD";
#elif defined(__OpenBSD__)
    return @"OpenBSD";
#else
    return @"Unknown";
#endif
}

+ (NSString *)kernelVersionString
{
    struct utsname u;
    if (uname(&u) == 0) {
        return [NSString stringWithFormat:@"%s %s", u.sysname, u.release];
    }
    return @"Unknown";
}

+ (NSString *)machineArchitecture
{
    struct utsname u;
    if (uname(&u) == 0) {
        return [NSString stringWithUTF8String:u.machine];
    }
    return @"Unknown";
}

+ (NSString *)kextInstallPathForIdentifier:(NSString *)identifier
{
#if defined(__APPLE__)
    if (identifier != nil) {
        return [NSString stringWithFormat:
            @"/System/Library/Extensions/%@.kext", identifier];
    }
#elif defined(__linux__)
    NSString *version = [LinuxKernelModule kernelVersion];
    if (version != nil && identifier != nil) {
        return [NSString stringWithFormat:
            @"/lib/modules/%@/extra/%@.kmod", version, identifier];
    }
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
    if (identifier != nil) {
        return [NSString stringWithFormat:
            @"/boot/modules/%@.ko", identifier];
    }
#endif
    return nil;
}

+ (BOOL)isModuleLoaded:(NSString *)moduleName
{
    if (moduleName == nil) return NO;

#if defined(__APPLE__)
    return [DarwinKext isKextLoaded:moduleName];
#elif defined(__linux__)
    return [[LinuxKernelModule loadedModules] containsObject:moduleName];
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
    return [[BSDKernelModule loadedModules] containsObject:moduleName];
#else
    return NO;
#endif
}

+ (BOOL)loadModuleAtPath:(NSString *)path
{
    if (path == nil) return NO;

#if defined(__APPLE__)
    return [DarwinKext loadKextAtPath:path];
#elif defined(__linux__)
    return [LinuxKernelModule loadModuleAtPath:path];
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
    return [BSDKernelModule loadModuleAtPath:path];
#else
    NSLog(@"kIOPlatformHandler: load not supported on this platform");
    return NO;
#endif
}

+ (BOOL)unloadModuleNamed:(NSString *)name
{
    if (name == nil) return NO;

#if defined(__APPLE__)
    return [DarwinKext unloadKextWithIdentifier:name];
#elif defined(__linux__)
    return [LinuxKernelModule unloadModuleNamed:name];
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
    return [BSDKernelModule unloadModuleNamed:name];
#else
    NSLog(@"kIOPlatformHandler: unload not supported on this platform");
    return NO;
#endif
}

+ (NSArray *)loadedModules
{
#if defined(__APPLE__)
    return [DarwinKext allLoadedIdentifiers];
#elif defined(__linux__)
    return [LinuxKernelModule loadedModules];
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
    return [BSDKernelModule loadedModules];
#else
    return [NSArray array];
#endif
}

+ (BOOL)installKext:(GNUKext *)kext error:(NSError **)error
{
    if (kext == nil) return NO;

#if defined(__APPLE__)
    return [DarwinKext installKext:[kext bundlePath] error:error];
#elif defined(__linux__)
    {
        NSString *modulesPath = [libkernDriver defaultModulesPath];
        return [LinuxKernelModule installKext:[kext bundlePath]
                                 toModulesPath:modulesPath
                                        error:error];
    }
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
    {
        NSString *modulesPath = [BSDModule defaultModulesPath];
        if (modulesPath == nil) {
            modulesPath = @"/boot/modules";
        }
        return [BSDKernelModule installKext:[kext bundlePath]
                               toModulesPath:modulesPath
                                      error:error];
    }
#else
    if (error != NULL) {
        *error = [NSError errorWithDomain:GNUKextErrorDomain
                                     code:GNUKextErrorLoadFailed
                                 userInfo:@{NSLocalizedDescriptionKey:
            @"Kext installation is not supported on this platform"}];
    }
    return NO;
#endif
}

+ (BOOL)uninstallKextWithIdentifier:(NSString *)identifier error:(NSError **)error
{
    if (identifier == nil) return NO;

#if defined(__APPLE__)
    return [DarwinKext uninstallKext:identifier error:error];
#elif defined(__linux__)
    {
        NSString *modulesPath = [libkernDriver defaultModulesPath];
        return [LinuxKernelModule uninstallKext:identifier
                                    modulesPath:modulesPath
                                          error:error];
    }
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
    {
        NSString *modulesPath = [BSDModule defaultModulesPath];
        if (modulesPath == nil) {
            modulesPath = @"/boot/modules";
        }
        return [BSDKernelModule uninstallKext:identifier
                                  modulesPath:modulesPath
                                        error:error];
    }
#else
    if (error != NULL) {
        *error = [NSError errorWithDomain:GNUKextErrorDomain
                                     code:GNUKextErrorUnloadFailed
                                 userInfo:@{NSLocalizedDescriptionKey:
            @"Kext uninstallation is not supported on this platform"}];
    }
    return NO;
#endif
}

@end

#ifdef __cplusplus
extern "C" {
#endif

static BOOL _platformInitialized = NO;

void kIOPlatformInitialize(void)
{
    if (_platformInitialized) return;
    _platformInitialized = YES;

    NSLog(@"kIOPlatformHandler: platform=%@  arch=%@  kernel=%@",
          [kIOPlatformHandler platformName],
          [kIOPlatformHandler machineArchitecture],
          [kIOPlatformHandler kernelVersionString]);
}

#ifdef __cplusplus
}
#endif
