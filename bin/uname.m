#import <Foundation/Foundation.h>
#import <unistd.h>

static NSDictionary *loadVersionInfo(void)
{
    NSString *path = @"/System/PocketDarwin.System/Resources/Version.json";
    NSData *data = [NSData dataWithContentsOfFile:path];
    if (!data) {
        return nil;
    }

    NSError *error = nil;
    id json = [NSJSONSerialization JSONObjectWithData:data options:0 error:&error];
    if (![json isKindOfClass:[NSDictionary class]]) {
        return nil;
    }

    return json;
}

static NSString *stringForKeys(NSDictionary *info, NSArray<NSString *> *keys, NSString *fallback)
{
    for (NSString *key in keys) {
        id value = info[key];
        if ([value isKindOfClass:[NSString class]] && [(NSString *)value length] > 0) {
            return value;
        }
    }
    return fallback ?: @"";
}

int main(int argc, const char *argv[])
{
    @autoreleasepool {
        NSDictionary *versionInfo = loadVersionInfo();

        NSString *sysname = stringForKeys(versionInfo, (@[@"ProductName", @"KernelName", @"SystemName"]), @"PocketDarwin");
        NSString *release = stringForKeys(versionInfo, (@[@"ProductVersion", @"ProductUserVisibleVersion", @"KernelRelease"]), @"0.0");
        NSString *version = stringForKeys(versionInfo, (@[@"ProductBuildVersion", @"BuildVersion", @"KernelVersion"]), @"");
        NSString *machine = stringForKeys(versionInfo, (@[@"Machine", @"Architecture", @"CPUType"]), @"arm64");
        NSString *operatingSystem = @"PocketDarwin";

        char hostnameBuffer[256] = {0};
        gethostname(hostnameBuffer, sizeof(hostnameBuffer));
        NSString *nodename = [NSString stringWithUTF8String:hostnameBuffer] ?: @"";

        BOOL doAll = NO;
        NSMutableArray<NSString *> *output = [NSMutableArray array];

        for (int i = 1; i < argc; i++) {
            const char *arg = argv[i];
            if (strcmp(arg, "-a") == 0 || strcmp(arg, "--all") == 0) {
                doAll = YES;
                break;
            }
        }

        if (argc == 1) {
            printf("%s\n", [sysname UTF8String]);
            return 0;
        }

        if (doAll) {
            [output addObject:sysname];
            if ([nodename length] > 0) {
                [output addObject:nodename];
            }
            if ([release length] > 0) {
                [output addObject:release];
            }
            if ([version length] > 0) {
                [output addObject:version];
            }
            if ([machine length] > 0) {
                [output addObject:machine];
            }
            [output addObject:operatingSystem];
            printf("%s\n", [[output componentsJoinedByString:@" "] UTF8String]);
            return 0;
        }

        for (int i = 1; i < argc; i++) {
            const char *arg = argv[i];
            if (strcmp(arg, "-s") == 0 || strcmp(arg, "--kernel-name") == 0) {
                [output addObject:sysname];
            } else if (strcmp(arg, "-n") == 0 || strcmp(arg, "--nodename") == 0) {
                [output addObject:nodename];
            } else if (strcmp(arg, "-r") == 0 || strcmp(arg, "--kernel-release") == 0) {
                [output addObject:release];
            } else if (strcmp(arg, "-v") == 0 || strcmp(arg, "--kernel-version") == 0) {
                [output addObject:version];
            } else if (strcmp(arg, "-m") == 0 || strcmp(arg, "--machine") == 0) {
                [output addObject:machine];
            } else if (strcmp(arg, "-o") == 0 || strcmp(arg, "--operating-system") == 0) {
                [output addObject:operatingSystem];
            } else {
                fprintf(stderr, "uname: invalid option -- %s\n", arg);
                return 1;
            }
        }

        if ([output count] == 0) {
            printf("%s\n", [sysname UTF8String]);
        } else {
            printf("%s\n", [[output componentsJoinedByString:@" "] UTF8String]);
        }
    }

    return 0;
}
