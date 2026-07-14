/*
   Project: GNUKext

   Copyright (C) 2026 Free Software Foundation

   Author: milo

   Created: 2026-07-13 20:11:26 -0300 by milo

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

#import "LinuxModule.h"

#if defined(__linux__)

#import <dlfcn.h>
#import <sys/syscall.h>
#import <sys/utsname.h>
#import <unistd.h>
#import <errno.h>
#import <libgen.h>

NSString * const LinuxKernelModuleErrorDomain = @"LinuxKernelModuleErrorDomain";

static NSString *_cachedKernelVersion = nil;

static int _initModuleFromFile(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL) return -1;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    void *image = malloc(size);
    if (image == NULL) {
        fclose(f);
        return -1;
    }

    fread(image, 1, size, f);
    fclose(f);

    int result = syscall(__NR_finit_module, AT_FDCWD, path, "", 0);
    if (result != 0) {
        result = syscall(__NR_init_module, image, size, "", 0);
    }

    free(image);
    return result;
}

static int _deleteModuleByName(const char *name)
{
    return syscall(__NR_delete_module, name, O_NONBLOCK);
}

@implementation LinuxKernelModule

+ (NSString *)kernelVersion
{
    if (_cachedKernelVersion != nil) return _cachedKernelVersion;

    char buf[256] = {0};
    if (uname((struct utsname *)buf) == 0) {
        struct utsname *u = (struct utsname *)buf;
        _cachedKernelVersion = [[NSString stringWithUTF8String:u->release] copy];
    }
    return _cachedKernelVersion;
}

+ (BOOL)loadModuleAtPath:(NSString *)path
{
    if (path == nil) return NO;
    if (![[NSFileManager defaultManager] fileExistsAtPath:path]) return NO;

    int result = _initModuleFromFile([path fileSystemRepresentation]);
    if (result != 0) {
        NSLog(@"LinuxKernelModule: failed to load %@: %s",
              path, strerror(errno));
        return NO;
    }

    NSLog(@"LinuxKernelModule: loaded %@", path);
    return YES;
}

+ (BOOL)unloadModuleAtPath:(NSString *)path
{
    if (path == nil) return NO;

    NSString *name = [[[path lastPathComponent]
        stringByDeletingPathExtension]
        stringByDeletingPathExtension];

    return [self unloadModuleNamed:name];
}

+ (BOOL)loadModuleNamed:(NSString *)name
{
    if (name == nil) return NO;

    NSString *version = [self kernelVersion];
    NSString *modulePath = [NSString stringWithFormat:
        @"/lib/modules/%@/extra/%@.ko", version, name];

    if (![[NSFileManager defaultManager] fileExistsAtPath:modulePath]) {
        modulePath = [NSString stringWithFormat:
            @"/lib/modules/%@/kernel/%@.ko", version, name];
    }

    if (![[NSFileManager defaultManager] fileExistsAtPath:modulePath]) {
        NSLog(@"LinuxKernelModule: module not found: %@", name);
        return NO;
    }

    return [self loadModuleAtPath:modulePath];
}

+ (BOOL)unloadModuleNamed:(NSString *)name
{
    if (name == nil) return NO;

    int result = _deleteModuleByName([name UTF8String]);
    if (result != 0) {
        NSLog(@"LinuxKernelModule: failed to unload %@: %s",
              name, strerror(errno));
        return NO;
    }

    NSLog(@"LinuxKernelModule: unloaded %@", name);
    return YES;
}

+ (NSArray *)loadedModules
{
    NSMutableArray *modules = [NSMutableArray array];
    NSString *procModules = [NSString stringWithContentsOfFile:
        @"/proc/modules" encoding:NSUTF8StringEncoding error:nil];
    if (procModules == nil) return modules;

    for (NSString *line in [procModules componentsSeparatedByString:@"\n"]) {
        NSString *trimmed = [line stringByTrimmingCharactersInSet:
            [NSCharacterSet whitespaceCharacterSet]];
        if ([trimmed length] == 0) continue;

        NSString *modName = [trimmed componentsSeparatedByString:@" "][0];
        [modules addObject:modName];
    }

    return modules;
}

+ (BOOL)installKext:(NSString *)kextPath toModulesPath:(NSString *)modulesPath
              error:(NSError **)error
{
    if (kextPath == nil || modulesPath == nil) {
        if (error != NULL) {
            *error = [NSError errorWithDomain:LinuxKernelModuleErrorDomain
                                         code:LinuxKernelModuleErrorNotFound
                                     userInfo:@{NSLocalizedDescriptionKey:
                @"kextPath and modulesPath must not be nil"}];
        }
        return NO;
    }

    if (![[NSFileManager defaultManager] fileExistsAtPath:kextPath]) {
        if (error != NULL) {
            *error = [NSError errorWithDomain:LinuxKernelModuleErrorDomain
                                         code:LinuxKernelModuleErrorNotFound
                                     userInfo:@{NSLocalizedDescriptionKey:
                [NSString stringWithFormat:@"Kext not found at %@", kextPath]}];
        }
        return NO;
    }

    NSDictionary *plist = nil;
    NSString *plistPath = [kextPath stringByAppendingPathComponent:@"Info.plist"];
    plist = [NSDictionary dictionaryWithContentsOfFile:plistPath];
    if (plist == nil) {
        plistPath = [kextPath stringByAppendingPathComponent:@"Contents/Info.plist"];
        plist = [NSDictionary dictionaryWithContentsOfFile:plistPath];
    }

    NSString *bundleID = [plist objectForKey:@"CFBundleIdentifier"];
    if (bundleID == nil) {
        bundleID = [[kextPath lastPathComponent]
            stringByDeletingPathExtension];
    }

    NSString *version = [self kernelVersion];
    NSString *targetDir = [NSString stringWithFormat:
        @"/lib/modules/%@/extra", version];

    [[NSFileManager defaultManager] createDirectoryAtPath:targetDir
        withIntermediateDirectories:YES attributes:nil error:nil];

    NSString *linkName = [targetDir stringByAppendingPathComponent:
        [bundleID stringByAppendingString:@".kmod"]];

    [[NSFileManager defaultManager] removeItemAtPath:linkName error:nil];

    BOOL ok = [[NSFileManager defaultManager]
        createSymbolicLinkAtPath:linkName
        withDestinationPath:kextPath error:error];

    if (!ok) {
        if (error != NULL) {
            NSDictionary *info = @{
                NSLocalizedDescriptionKey:
                    [NSString stringWithFormat:@"Failed to create symlink %@ -> %@",
                        linkName, kextPath],
                NSUnderlyingErrorKey: *error
            };
            *error = [NSError errorWithDomain:LinuxKernelModuleErrorDomain
                                         code:LinuxKernelModuleErrorSymlinkFailed
                                     userInfo:info];
        }
        return NO;
    }

    NSLog(@"LinuxKernelModule: installed kext %@ -> %@", linkName, kextPath);
    return YES;
}

+ (BOOL)uninstallKext:(NSString *)kextName
          modulesPath:(NSString *)modulesPath
                error:(NSError **)error
{
    if (kextName == nil) return NO;

    NSString *version = [self kernelVersion];
    NSString *linkPath = [NSString stringWithFormat:
        @"/lib/modules/%@/extra/%@.kmod", version, kextName];

    if (![[NSFileManager defaultManager] fileExistsAtPath:linkPath]) {
        if (error != NULL) {
            *error = [NSError errorWithDomain:LinuxKernelModuleErrorDomain
                                         code:LinuxKernelModuleErrorNotFound
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
        _loaded = NO;
        _handle = NULL;
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

    if (![LinuxKernelModule loadModuleAtPath:_path]) return NO;

    _loaded = YES;
    return YES;
}

- (BOOL)unload
{
    if (!_loaded) return YES;

    if (![LinuxKernelModule unloadModuleAtPath:_path]) return NO;

    _loaded = NO;
    return YES;
}

- (BOOL)isLoaded
{
    return _loaded;
}

@end

#endif /* __linux__ */
