#import <Foundation/Foundation.h>
#include <sys/utsname.h>
#include <sys/sysctl.h>
#include <stdlib.h>
#include <stdio.h>

static NSString *stringForSysctl(const char *name)
{
    size_t size = 0;
    if (sysctlbyname(name, NULL, &size, NULL, 0) != 0 || size == 0) {
        return nil;
    }

    char *buffer = (char *)malloc(size);
    if (!buffer) {
        return nil;
    }

    if (sysctlbyname(name, buffer, &size, NULL, 0) != 0) {
        free(buffer);
        return nil;
    }

    NSString *value = [NSString stringWithUTF8String:buffer];
    free(buffer);
    return value;
}

static long long longForSysctl(const char *name)
{
    long long value = 0;
    size_t size = sizeof(value);
    if (sysctlbyname(name, &value, &size, NULL, 0) != 0) {
        return 0;
    }
    return value;
}

int main(int argc, const char * argv[])
{
    @autoreleasepool {
        struct utsname systemInfo;
        uname(&systemInfo);

        NSString *deviceModel = stringForSysctl("hw.model") ?: @"Unknown";
        NSString *machine = stringForSysctl("hw.machine") ?: @"Unknown";
        NSString *kernelRelease = stringForSysctl("kern.osrelease") ?: @"Unknown";
        NSString *kernelType = stringForSysctl("kern.ostype") ?: @"Unknown";
        NSString *architecture = stringForSysctl("hw.machine") ?: @"Unknown";
        long long cpuCount = longForSysctl("hw.ncpu");
        long long memSize = longForSysctl("hw.memsize");

        NSString *formattedMem = @"Unknown";
        if (memSize > 0) {
            double gigabytes = (double)memSize / (1024.0 * 1024.0 * 1024.0);
            formattedMem = [NSString stringWithFormat:@"%.2f GB", gigabytes];
        }

        NSLog("About This Phone\n");
        NSLog("______________________________\n");
        NSLog("Device Model: %s\n", [deviceModel UTF8String]);
        NSLog("Machine: %s\n", [machine UTF8String]);
        NSLog("Architecture: %s\n", [architecture UTF8String]);
        NSLog("Kernel: %s %s\n", [kernelType UTF8String], [kernelRelease UTF8String]);
        NSLog("Node Name: %s\n", systemInfo.nodename);
        NSLog("Release: %s\n", systemInfo.release);
        NSLog("Version: %s\n", systemInfo.version);
        NSLog("CPU Count: %lld\n", cpuCount > 0 ? cpuCount : 0);
        NSLog("Memory: %s\n", [formattedMem UTF8String]);
    }

    return 0;
}
