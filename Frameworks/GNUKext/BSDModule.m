/*
   Project: GNUKext

   Copyright (C) 2026 Free Software Foundation

   Author: milo

   Created: 2026-07-13 20:11:36 -0300 by milo

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

#import "BSDModule.h"

#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)

#import <sys/param.h>
#import <sys/module.h>
#import <sys/linker.h>
#import <dlfcn.h>
#import <unistd.h>

NSString * const BSDKernelModuleErrorDomain = @"BSDKernelModuleErrorDomain";

static int _kldLoad(const char *path)
{
    struct kld_file_stat stat;
    memset(&stat, 0, sizeof(stat));
    stat.version = sizeof(stat);

    int id = kldload(path);
    if (id >= 0) {
        if (kldstat(id, &stat) == 0) {
            NSLog(@"BSDKernelModule: loaded %s (id=%d)", stat.name, id);
        }
    }
    return id;
}

static int _kldUnload(int id)
{
    return kldunload(id);
}

@implementation BSDKernelModule

+ (NSString *)kernelVersion
{
#if defined(__FreeBSD__)
    char version[256];
    size_t len = sizeof(version);
    if (sysctlbyname("kern.osrelease", version, &len, NULL, 0) == 0) {
        return [NSString stringWithUTF8String:version];
    }
#endif
    return @"unknown";
}

+ (BOOL)loadModuleAtPath:(NSString *)path
{
    if (path == nil) return NO;
    if (![[NSFileManager defaultManager] fileExistsAtPath:path]) return NO;

    int id = _kldLoad([path fileSystemRepresentation]);
    return (id >= 0);
}

+ (BOOL)unloadModuleAtPath:(NSString *)path
{
    if (path == nil) return NO;

    struct kld_file_stat stat;
    memset(&stat, 0, sizeof(stat));
    stat.version = sizeof(stat);

    int id = kldfind([path fileSystemRepresentation]);
    if (id < 0) return NO;

    return (_kldUnload(id) == 0);
}

+ (BOOL)loadModuleNamed:(NSString *)name
{
    if (name == nil) return NO;

    NSString *modulePath = [NSString stringWithFormat:
        @"/boot/kernel/%@.ko", name];

    if (![[NSFileManager defaultManager] fileExistsAtPath:modulePath]) {
        modulePath = [NSString stringWithFormat:
            @"/boot/modules/%@.ko", name];
    }

    if (![[NSFileManager defaultManager] fileExistsAtPath:modulePath]) {
        NSLog(@"BSDKernelModule: module not found: %@", name);
        return NO;
    }

    return [self loadModuleAtPath:modulePath];
}

+ (BOOL)unloadModuleNamed:(NSString *)name
{
    if (name == nil) return NO;

    void *handle = dlopen(NULL, RTLD_LAZY);
    if (handle == NULL) return NO;

    int id = kldfind([name UTF8String]);
    dlclose(handle);

    if (id < 0) return NO;
    return (_kldUnload(id) == 0);
}

+ (NSArray *)loadedModules
{
    NSMutableArray *modules = [NSMutableArray array];
    int id = 0;

    while ((id = kldnext(id)) != 0) {
        struct kld_file_stat stat;
        memset(&stat, 0, sizeof(stat));
        stat.version = sizeof(stat);

        if (kldstat(id, &stat) == 0) {
            [modules addObject:[NSString stringWithUTF8String:stat.name]];
        }
    }

    return modules;
}

+ (NSString *)defaultModulesPath
{
    return @"/boot/modules";
}

+ (BOOL)installKext:(NSString *)bundlePath toModulesPath:(NSString *)modulesPath
              error:(NSError **)error
{
    if (bundlePath == nil || modulesPath == nil) {
        if (error != NULL) {
            *error = [NSError errorWithDomain:BSDKernelModuleErrorDomain
                                         code:BSDKernelModuleErrorNotFound
                                     userInfo:@{NSLocalizedDescriptionKey:
                @"bundlePath and modulesPath must not be nil"}];
        }
        return NO;
    }

    if (![[NSFileManager defaultManager] fileExistsAtPath:bundlePath]) {
        if (error != NULL) {
            *error = [NSError errorWithDomain:BSDKernelModuleErrorDomain
                                         code:BSDKernelModuleErrorNotFound
                                     userInfo:@{NSLocalizedDescriptionKey:
                [NSString stringWithFormat:@"Kext not found at %@", bundlePath]}];
        }
        return NO;
    }

    NSString *bundleID = nil;
    NSString *plistPath = [bundlePath stringByAppendingPathComponent:@"Info.plist"];
    NSDictionary *plist = [NSDictionary dictionaryWithContentsOfFile:plistPath];
    if (plist == nil) {
        plistPath = [bundlePath stringByAppendingPathComponent:@"Contents/Info.plist"];
        plist = [NSDictionary dictionaryWithContentsOfFile:plistPath];
    }
    bundleID = [plist objectForKey:@"CFBundleIdentifier"];
    if (bundleID == nil) {
        bundleID = [[bundlePath lastPathComponent] stringByDeletingPathExtension];
    }

    NSString *execName = [plist objectForKey:@"CFBundleExecutable"];
    NSString *koPath = nil;
    if (execName != nil) {
        koPath = [bundlePath stringByAppendingPathComponent:execName];
    } else {
        koPath = bundlePath;
    }

    [[NSFileManager defaultManager] createDirectoryAtPath:modulesPath
        withIntermediateDirectories:YES attributes:nil error:nil];

    NSString *linkName = [modulesPath stringByAppendingPathComponent:
        [bundleID stringByAppendingString:@".ko"]];

    [[NSFileManager defaultManager] removeItemAtPath:linkName error:nil];

    BOOL ok = [[NSFileManager defaultManager]
        createSymbolicLinkAtPath:linkName
        withDestinationPath:koPath error:error];

    if (!ok) {
        if (error != NULL) {
            NSDictionary *info = @{
                NSLocalizedDescriptionKey:
                    [NSString stringWithFormat:@"Failed to create symlink %@ -> %@",
                        linkName, koPath],
                NSUnderlyingErrorKey: *error
            };
            *error = [NSError errorWithDomain:BSDKernelModuleErrorDomain
                                         code:BSDKernelModuleErrorLoadFailed
                                     userInfo:info];
        }
        return NO;
    }

    NSLog(@"BSDKernelModule: installed kext %@ -> %@", linkName, koPath);
    return YES;
}

+ (BOOL)uninstallKext:(NSString *)kextName
          modulesPath:(NSString *)modulesPath
                error:(NSError **)error
{
    if (kextName == nil || modulesPath == nil) return NO;

    NSString *linkPath = [modulesPath stringByAppendingPathComponent:
        [kextName stringByAppendingString:@".ko"]];

    if (![[NSFileManager defaultManager] fileExistsAtPath:linkPath]) {
        if (error != NULL) {
            *error = [NSError errorWithDomain:BSDKernelModuleErrorDomain
                                         code:BSDKernelModuleErrorNotFound
                                     userInfo:@{NSLocalizedDescriptionKey:
                [NSString stringWithFormat:@"Kext %@ is not installed", kextName]}];
        }
        return NO;
    }

    return [[NSFileManager defaultManager] removeItemAtPath:linkPath error:error];
}

- (instancetype)initWithPath:(NSString *)path
{
    self = [super init];
    if (self) {
        _path = [path copy];
        _name = [[[path lastPathComponent]
            stringByDeletingPathExtension] copy];
        _kldId = -1;
        _loaded = NO;
    }
    return self;
}

- (void)dealloc
{
    [_name release];
    [_path release];
    [super dealloc];
}

- (BOOL)load
{
    if (_loaded) return YES;

    _kldId = _kldLoad([_path fileSystemRepresentation]);
    if (_kldId < 0) return NO;

    _loaded = YES;
    return YES;
}

- (BOOL)unload
{
    if (!_loaded) return YES;
    if (_kldId < 0) return NO;

    if (_kldUnload(_kldId) != 0) return NO;

    _kldId = -1;
    _loaded = NO;
    return YES;
}

- (BOOL)isLoaded
{
    return _loaded;
}

@end

#endif /* __FreeBSD__ || __NetBSD__ || __OpenBSD__ */
