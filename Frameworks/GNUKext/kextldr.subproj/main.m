/*
   Project: kextldr

   Copyright (C) 2026 Free Software Foundation

   Author: milo

   Created: 2026-07-13 19:37:14 -0300 by milo

   kextldr - Cross-platform Kernel Extension Loader
   Loads and manages kexts on Linux (insmod), BSD (kldload),
   and Darwin/XNU (OSKext injection).
*/

#import <Foundation/Foundation.h>
#import "GNUKext.h"
#import "LinuxModule.h"
#import "BSDModule.h"
#import "DarwinModule.h"
#import "kIOPlatformHandler.h"

static void printUsage(const char *progName)
{
    fprintf(stderr,
        "Usage: %s <command> [arguments]\n"
        "\n"
        "Commands:\n"
        "  load <path>          Load a kernel module / kext from path\n"
        "  unload <name>        Unload a kernel module / kext by name\n"
        "  install <kext-path>  Install a kext (platform-specific)\n"
        "  uninstall <name>     Uninstall a kext by name\n"
        "  list                 List loaded modules / kexts\n"
        "  info <kext-path>     Show kext Info.plist information\n"
        "  version              Show kextldr version\n"
        "\n"
        "Platforms:\n"
        "  Linux    - loads .ko via init_module syscall\n"
        "  BSD      - loads .ko via kldload\n"
        "  Darwin   - injects .kext via OSKextLoad\n"
        "\n"
        "Examples:\n"
        "  %s install /path/to/IOStorageFamily.kext\n"
        "  %s load /path/to/module.ko\n"
        "  %s unload mymodule\n"
        "  %s list\n",
        progName, progName, progName, progName, progName);
}

static int cmdLoad(NSString *path)
{
    if (path == nil) {
        fprintf(stderr, "error: missing module path\n");
        return 1;
    }

    BOOL ok = [kIOPlatformHandler loadModuleAtPath:path];
    if (!ok) {
        fprintf(stderr, "error: failed to load module from %s\n",
                [path UTF8String]);
        return 1;
    }

    printf("loaded: %s\n", [path UTF8String]);
    return 0;
}

static int cmdUnload(NSString *name)
{
    if (name == nil) {
        fprintf(stderr, "error: missing module name\n");
        return 1;
    }

    BOOL ok = [kIOPlatformHandler unloadModuleNamed:name];
    if (!ok) {
        fprintf(stderr, "error: failed to unload module %s\n",
                [name UTF8String]);
        return 1;
    }

    printf("unloaded: %s\n", [name UTF8String]);
    return 0;
}

static int cmdInstall(NSString *kextPath)
{
    if (kextPath == nil) {
        fprintf(stderr, "error: missing kext path\n");
        return 1;
    }

    if (![[NSFileManager defaultManager] fileExistsAtPath:kextPath]) {
        fprintf(stderr, "error: kext not found at %s\n", [kextPath UTF8String]);
        return 1;
    }

    GNUKext *kext = [GNUKext kextWithPath:kextPath];
    if (kext == nil) {
        fprintf(stderr, "error: could not read kext at %s\n", [kextPath UTF8String]);
        return 1;
    }

    NSError *error = nil;
    BOOL ok = [kIOPlatformHandler installKext:kext error:&error];

    if (!ok) {
        fprintf(stderr, "error: %s\n",
                [[error localizedDescription] UTF8String]);
        return 1;
    }

    printf("installed: %s\n", [kextPath UTF8String]);
    return 0;
}

static int cmdUninstall(NSString *name)
{
    if (name == nil) {
        fprintf(stderr, "error: missing kext name\n");
        return 1;
    }

    NSError *error = nil;
    BOOL ok = [kIOPlatformHandler uninstallKextWithIdentifier:name error:&error];

    if (!ok) {
        fprintf(stderr, "error: %s\n",
                [[error localizedDescription] UTF8String]);
        return 1;
    }

    printf("uninstalled: %s\n", [name UTF8String]);
    return 0;
}

static int cmdList(void)
{
    NSArray *modules = [kIOPlatformHandler loadedModules];
    printf("Loaded modules/kexts:\n");
    for (NSString *mod in modules) {
        printf("  %s\n", [mod UTF8String]);
    }
    printf("%lu loaded\n", (unsigned long)[modules count]);
    return 0;
}

static int cmdInfo(NSString *kextPath)
{
    if (kextPath == nil) {
        fprintf(stderr, "error: missing kext path\n");
        return 1;
    }

    NSDictionary *plist = [libkernDriver driverPlistForKext:kextPath];
    if (plist == nil) {
        fprintf(stderr, "error: could not read Info.plist from %s\n",
                [kextPath UTF8String]);
        return 1;
    }

    printf("Kext Info: %s\n", [kextPath UTF8String]);
    printf("  BundleIdentifier:  %s\n",
           [[plist objectForKey:@"CFBundleIdentifier"] UTF8String] ?: "(none)");
    printf("  BundleName:        %s\n",
           [[plist objectForKey:@"CFBundleName"] UTF8String] ?: "(none)");
    printf("  BundleVersion:     %s\n",
           [[plist objectForKey:@"CFBundleVersion"] UTF8String] ?: "(none)");
    printf("  PackageType:       %s\n",
           [[plist objectForKey:@"CFBundlePackageType"] UTF8String] ?: "(none)");

    NSDictionary *libs = [plist objectForKey:@"OSBundleLibraries"];
    if (libs != nil && [libs count] > 0) {
        printf("  Dependencies:\n");
        for (NSString *key in libs) {
            printf("    %s = %s\n", [key UTF8String],
                   [[libs objectForKey:key] UTF8String]);
        }
    }

    NSDictionary *personalities = [plist objectForKey:@"IOKitPersonalities"];
    if (personalities != nil && [personalities count] > 0) {
        printf("  Personalities: %lu\n",
               (unsigned long)[[personalities allKeys] count]);
    }

    return 0;
}

static int cmdVersion(void)
{
    printf("kextldr %s (GNUKext %s)\n",
           [@"0.1" UTF8String],
           [[GNUKext libkernVersion] UTF8String]);
    printf("Platform: %s\n",
           [[kIOPlatformHandler platformName] UTF8String]);
    printf("Kernel: %s\n",
           [[kIOPlatformHandler kernelVersionString] UTF8String]);
    printf("Arch: %s\n",
           [[kIOPlatformHandler machineArchitecture] UTF8String]);
    return 0;
}

int main(int argc, const char *argv[])
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    kIOPlatformInitialize();

    if (argc < 2) {
        printUsage(argv[0]);
        [pool release];
        return 1;
    }

    NSString *command = [NSString stringWithUTF8String:argv[1]];
    NSString *arg = nil;
    if (argc >= 3) {
        arg = [NSString stringWithUTF8String:argv[2]];
    }

    int result = 0;

    if ([command isEqualToString:@"load"]) {
        result = cmdLoad(arg);
    } else if ([command isEqualToString:@"unload"]) {
        result = cmdUnload(arg);
    } else if ([command isEqualToString:@"install"]) {
        result = cmdInstall(arg);
    } else if ([command isEqualToString:@"uninstall"]) {
        result = cmdUninstall(arg);
    } else if ([command isEqualToString:@"list"]) {
        result = cmdList();
    } else if ([command isEqualToString:@"info"]) {
        result = cmdInfo(arg);
    } else if ([command isEqualToString:@"version"]) {
        result = cmdVersion();
    } else if ([command isEqualToString:@"--help"] ||
               [command isEqualToString:@"-h"]) {
        printUsage(argv[0]);
    } else {
        fprintf(stderr, "error: unknown command '%s'\n", [command UTF8String]);
        printUsage(argv[0]);
        result = 1;
    }

    [pool release];
    return result;
}
