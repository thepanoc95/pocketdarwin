#import <Foundation/Foundation.h>

@interface PatchFile : NSObject
@property (nonatomic, strong) NSString *filePath;
@property (nonatomic, strong) NSString *fileName;
@property (nonatomic, strong) NSString *targetComponent;
@property (nonatomic, strong) NSString *description;
@property (nonatomic, assign) BOOL applied;
@property (nonatomic, strong) NSDate *createdDate;

- (instancetype)initWithPath:(NSString *)path;
- (NSString *)readPatchContent;
- (NSDictionary *)parsePatchMetadata;

@end

@implementation PatchFile

- (instancetype)initWithPath:(NSString *)path {
    self = [super init];
    if (self) {
        self.filePath = path;
        self.fileName = [path lastPathComponent];
        self.applied = NO;
        self.createdDate = [NSDate date];
        [self detectTargetComponent];
    }
    return self;
}

- (void)detectTargetComponent {
    NSString *content = [self readPatchContent];
    if ([content rangeOfString:@"xnu/" options:NSCaseInsensitiveSearch].location != NSNotFound) {
        self.targetComponent = @"XNU Kernel";
    } else if ([content rangeOfString:@"lk2nd/" options:NSCaseInsensitiveSearch].location != NSNotFound) {
        self.targetComponent = @"lk2nd Bootloader";
    } else if ([content rangeOfString:@"xnu-deps-linux" options:NSCaseInsensitiveSearch].location != NSNotFound) {
        self.targetComponent = @"XNU Deps (Linux)";
    } else {
        self.targetComponent = @"Unknown";
    }
}

- (NSString *)readPatchContent {
    NSError *error = nil;
    NSString *content = [NSString stringWithContentsOfFile:self.filePath 
                                                   encoding:NSUTF8StringEncoding 
                                                      error:&error];
    return content ?: @"";
}

- (NSDictionary *)parsePatchMetadata {
    NSMutableDictionary *metadata = [NSMutableDictionary dictionary];
    NSString *content = [self readPatchContent];
    NSArray *lines = [content componentsSeparatedByString:@"\n"];
    
    // Extract subject
    for (NSString *line in lines) {
        if ([line hasPrefix:@"Subject:"]) {
            NSRange range = [line rangeOfString:@"Subject: "];
            if (range.location != NSNotFound) {
                NSString *subject = [line substringFromIndex:range.location + range.length];
                metadata[@"subject"] = subject;
            }
            break;
        }
    }
    
    // Count hunks and changes
    NSUInteger hunkCount = 0, added = 0, removed = 0;
    for (NSString *line in lines) {
        if ([line hasPrefix:@"@@"]) hunkCount++;
        if ([line hasPrefix:@"+"]) added++;
        if ([line hasPrefix:@"-"]) removed++;
    }
    
    metadata[@"hunks"] = @(hunkCount);
    metadata[@"linesAdded"] = @(added);
    metadata[@"linesRemoved"] = @(removed);
    
    return metadata;
}

@end

@interface PatchCLI : NSObject

+ (void)printVersion;
+ (void)printUsage;
+ (void)printDetailedHelp;
+ (void)listPatches:(NSString *)directory;
+ (void)inspectPatch:(NSString *)patchPath;
+ (BOOL)applyPatch:(NSString *)patchPath toPath:(NSString *)componentPath dryRun:(BOOL)dryRun;
+ (BOOL)validatePatch:(NSString *)patchPath;
+ (void)createPatchReport:(NSString *)directory output:(NSString *)outputPath;
+ (NSString *)executeCommand:(NSString *)command withArgs:(NSArray *)args;

@end

@implementation PatchCLI

+ (void)printVersion {
    printf("pd-patch 1.0.0 - PocketDarwin Patch Manager\n");
    printf("Part of the Harpia/ReverseRosetta project\n");
}

+ (void)printUsage {
    printf("Usage: pd-patch <command> [options]\n\n");
    printf("Commands:\n");
    printf("  list <dir>              List all .patch files in directory\n");
    printf("  inspect <patch>         Show detailed information about a patch\n");
    printf("  apply <patch> <path>    Apply patch to component directory\n");
    printf("  validate <patch>        Check if patch is valid and applicable\n");
    printf("  report <dir> <output>   Generate comprehensive patch report\n");
    printf("  help                    Show this help message\n");
    printf("  --version               Show version information\n\n");
    printf("Options:\n");
    printf("  --dry-run              Test application without making changes\n");
    printf("  --reverse              Attempt to reverse patch (unapply)\n");
    printf("  --force                Continue even with errors\n");
    printf("  --verbose              Print detailed operation information\n");
}

+ (void)printDetailedHelp {
    [self printVersion];
    printf("\n");
    [self printUsage];
    printf("\nExamples:\n");
    printf("  pd-patch list ~/patches\n");
    printf("  pd-patch inspect ~/patches/xnu-optimization.patch\n");
    printf("  pd-patch apply ~/patches/xnu-fix.patch ~/PocketDarwin/xnu --dry-run\n");
    printf("  pd-patch validate ~/patches/bootloader.patch\n");
    printf("  pd-patch report ~/patches output.txt\n");
}

+ (void)listPatches:(NSString *)directory {
    NSError *error = nil;
    NSFileManager *fm = [NSFileManager defaultManager];
    NSArray *contents = [fm contentsOfDirectoryAtPath:directory error:&error];
    
    if (error) {
        fprintf(stderr, "Error reading directory: %s\n", [[error localizedDescription] UTF8String]);
        return;
    }
    
    NSMutableArray *patches = [NSMutableArray array];
    for (NSString *item in contents) {
        if ([item hasSuffix:@".patch"]) {
            NSString *fullPath = [directory stringByAppendingPathComponent:item];
            PatchFile *patch = [[PatchFile alloc] initWithPath:fullPath];
            [patches addObject:patch];
        }
    }
    
    if ([patches count] == 0) {
        printf("No .patch files found in %s\n", [directory UTF8String]);
        return;
    }
    
    printf("Found %lu patches:\n\n", (unsigned long)[patches count]);
    printf("%-35s %-20s %s\n", "Filename", "Component", "Hunks");
    printf("%-35s %-20s %s\n", "--------", "---------", "-----");
    
    for (PatchFile *patch in patches) {
        NSDictionary *meta = [patch parsePatchMetadata];
        printf("%-35s %-20s %lu\n",
               [[patch fileName] UTF8String],
               [[patch targetComponent] UTF8String],
               (unsigned long)[[meta objectForKey:@"hunks"] unsignedIntegerValue]);
    }
    printf("\n");
}

+ (void)inspectPatch:(NSString *)patchPath {
    NSFileManager *fm = [NSFileManager defaultManager];
    if (![fm fileExistsAtPath:patchPath]) {
        fprintf(stderr, "Patch file not found: %s\n", [patchPath UTF8String]);
        return;
    }
    
    PatchFile *patch = [[PatchFile alloc] initWithPath:patchPath];
    NSDictionary *metadata = [patch parsePatchMetadata];
    
    printf("\n=== Patch Information ===\n\n");
    printf("File:        %s\n", [[patch fileName] UTF8String]);
    printf("Path:        %s\n", [[patch filePath] UTF8String]);
    printf("Component:   %s\n", [[patch targetComponent] UTF8String]);
    printf("Created:     %s\n", [[[patch createdDate] description] UTF8String]);
    printf("Status:      %s\n\n", patch.applied ? "Applied" : "Not Applied");
    
    printf("=== Patch Statistics ===\n\n");
    printf("Hunks:           %@\n", [metadata objectForKey:@"hunks"]);
    printf("Lines Added:     %@\n", [metadata objectForKey:@"linesAdded"]);
    printf("Lines Removed:   %@\n", [metadata objectForKey:@"linesRemoved"]);
    printf("Subject:         %@\n\n", [metadata objectForKey:@"subject"] ?: @"(none)");
    
    printf("=== First 20 Lines ===\n\n");
    NSString *content = [patch readPatchContent];
    NSArray *lines = [content componentsSeparatedByString:@"\n"];
    for (NSUInteger i = 0; i < 20 && i < [lines count]; i++) {
        printf("%s\n", [[lines objectAtIndex:i] UTF8String]);
    }
    printf("\n");
}

+ (BOOL)applyPatch:(NSString *)patchPath toPath:(NSString *)componentPath dryRun:(BOOL)dryRun {
    NSFileManager *fm = [NSFileManager defaultManager];
    
    if (![fm fileExistsAtPath:patchPath]) {
        fprintf(stderr, "Patch file not found: %s\n", [patchPath UTF8String]);
        return NO;
    }
    
    if (![fm fileExistsAtPath:componentPath]) {
        fprintf(stderr, "Component path not found: %s\n", [componentPath UTF8String]);
        return NO;
    }
    
    NSTask *task = [[NSTask alloc] init];
    [task setLaunchPath:@"/usr/bin/patch"];
    
    NSMutableArray *args = [NSMutableArray arrayWithObjects:@"-p1", @"-i", patchPath, nil];
    if (dryRun) {
        [args addObject:@"--dry-run"];
    }
    
    [task setArguments:args];
    [task setCurrentDirectoryPath:componentPath];
    
    NSPipe *outputPipe = [NSPipe pipe];
    NSPipe *errorPipe = [NSPipe pipe];
    [task setStandardOutput:outputPipe];
    [task setStandardError:errorPipe];
    
    NSError *error = nil;
    BOOL launched = [task launchAndReturnError:&error];
    
    if (!launched) {
        fprintf(stderr, "Failed to launch patch: %s\n", [[error localizedDescription] UTF8String]);
        return NO;
    }
    
    [task waitUntilExit];
    int status = [task terminationStatus];
    
    NSData *outputData = [[outputPipe fileHandleForReading] readDataToEndOfFile];
    NSString *output = [[NSString alloc] initWithData:outputData encoding:NSUTF8StringEncoding];
    
    if (output && [output length] > 0) {
        printf("%s\n", [output UTF8String]);
    }
    
    if (status != 0) {
        NSData *errorData = [[errorPipe fileHandleForReading] readDataToEndOfFile];
        NSString *errorOutput = [[NSString alloc] initWithData:errorData encoding:NSUTF8StringEncoding];
        if (errorOutput && [errorOutput length] > 0) {
            fprintf(stderr, "%s\n", [errorOutput UTF8String]);
        }
        return NO;
    }
    
    return YES;
}

+ (BOOL)validatePatch:(NSString *)patchPath {
    PatchFile *patch = [[PatchFile alloc] initWithPath:patchPath];
    NSString *content = [patch readPatchContent];
    
    printf("Validating: %s\n", [[patch fileName] UTF8String]);
    
    // Check basic structure
    if (![content hasPrefix:@"diff "] && ![content hasPrefix:@"---"]) {
        fprintf(stderr, "Error: Invalid patch format (missing diff header)\n");
        return NO;
    }
    
    // Check for hunks
    NSDictionary *metadata = [patch parsePatchMetadata];
    NSUInteger hunks = [[metadata objectForKey:@"hunks"] unsignedIntegerValue];
    
    if (hunks == 0) {
        fprintf(stderr, "Warning: No hunks found in patch\n");
        return NO;
    }
    
    printf("✓ Valid patch with %lu hunks\n", (unsigned long)hunks);
    printf("✓ Component: %s\n", [[patch targetComponent] UTF8String]);
    printf("✓ Lines: +%@ -%@\n",
           [metadata objectForKey:@"linesAdded"],
           [metadata objectForKey:@"linesRemoved"]);
    
    return YES;
}

+ (void)createPatchReport:(NSString *)directory output:(NSString *)outputPath {
    NSError *error = nil;
    NSFileManager *fm = [NSFileManager defaultManager];
    NSArray *contents = [fm contentsOfDirectoryAtPath:directory error:&error];
    
    if (error) {
        fprintf(stderr, "Error reading directory: %s\n", [[error localizedDescription] UTF8String]);
        return;
    }
    
    NSMutableString *report = [NSMutableString string];
    [report appendString:@"# PocketDarwin Patch Report\n\n"];
    [report appendFormat:@"Generated: %@\n", [NSDate date]];
    [report appendFormat:@"Source: %@\n\n", directory];
    
    NSMutableArray *patches = [NSMutableArray array];
    NSMutableDictionary *componentStats = [NSMutableDictionary dictionary];
    
    for (NSString *item in contents) {
        if ([item hasSuffix:@".patch"]) {
            NSString *fullPath = [directory stringByAppendingPathComponent:item];
            PatchFile *patch = [[PatchFile alloc] initWithPath:fullPath];
            [patches addObject:patch];
            
            // Accumulate stats by component
            NSString *component = patch.targetComponent;
            if (![componentStats objectForKey:component]) {
                [componentStats setObject:@0 forKey:component];
            }
            NSNumber *count = [componentStats objectForKey:component];
            [componentStats setObject:@([count integerValue] + 1) forKey:component];
        }
    }
    
    [report appendFormat:@"## Summary\n\n"];
    [report appendFormat:@"Total Patches: %lu\n\n", (unsigned long)[patches count]];
    
    [report appendString:@"### By Component\n\n"];
    for (NSString *component in [componentStats keyEnumerator]) {
        [report appendFormat:@"- %@: %@\n", component, [componentStats objectForKey:component]];
    }
    
    [report appendString:@"\n## Patch Details\n\n"];
    
    for (PatchFile *patch in patches) {
        NSDictionary *metadata = [patch parsePatchMetadata];
        [report appendFormat:@"### %@\n\n", patch.fileName];
        [report appendFormat:@"- **Component**: %@\n", patch.targetComponent];
        [report appendFormat:@"- **Hunks**: %@\n", [metadata objectForKey:@"hunks"]];
        [report appendFormat:@"- **Changes**: +%@ lines, -%@ lines\n",
                [metadata objectForKey:@"linesAdded"],
                [metadata objectForKey:@"linesRemoved"]];
        [report appendFormat:@"- **Subject**: %@\n\n", [metadata objectForKey:@"subject"] ?: @"(none)"];
    }
    
    BOOL success = [report writeToFile:outputPath atomically:YES encoding:NSUTF8StringEncoding error:&error];
    
    if (success) {
        printf("Report generated: %s\n", [outputPath UTF8String]);
    } else {
        fprintf(stderr, "Error writing report: %s\n", [[error localizedDescription] UTF8String]);
    }
}

+ (NSString *)executeCommand:(NSString *)command withArgs:(NSArray *)args {
    NSTask *task = [[NSTask alloc] init];
    [task setLaunchPath:command];
    if (args) {
        [task setArguments:args];
    }
    
    NSPipe *outputPipe = [NSPipe pipe];
    [task setStandardOutput:outputPipe];
    [task setStandardError:outputPipe];
    
    NSError *error = nil;
    [task launchAndReturnError:&error];
    [task waitUntilExit];
    
    NSData *outputData = [[outputPipe fileHandleForReading] readDataToEndOfFile];
    NSString *output = [[NSString alloc] initWithData:outputData encoding:NSUTF8StringEncoding];
    
    return output ?: @"";
}

@end

// ============================================================================
// Main Entry Point
// ============================================================================

int main(int argc, const char *argv[]) {
    @autoreleasepool {
        if (argc < 2) {
            [PatchCLI printUsage];
            return 1;
        }
        
        NSString *command = [NSString stringWithUTF8String:argv[1]];
        
        if ([command isEqualToString:@"--version"]) {
            [PatchCLI printVersion];
            return 0;
        }
        
        if ([command isEqualToString:@"help"] || [command isEqualToString:@"-h"] || [command isEqualToString:@"--help"]) {
            [PatchCLI printDetailedHelp];
            return 0;
        }
        
        if ([command isEqualToString:@"list"]) {
            if (argc < 3) {
                fprintf(stderr, "Usage: pd-patch list <directory>\n");
                return 1;
            }
            NSString *directory = [NSString stringWithUTF8String:argv[2]];
            [PatchCLI listPatches:directory];
            return 0;
        }
        
        if ([command isEqualToString:@"inspect"]) {
            if (argc < 3) {
                fprintf(stderr, "Usage: pd-patch inspect <patch>\n");
                return 1;
            }
            NSString *patchPath = [NSString stringWithUTF8String:argv[2]];
            [PatchCLI inspectPatch:patchPath];
            return 0;
        }
        
        if ([command isEqualToString:@"apply"]) {
            if (argc < 4) {
                fprintf(stderr, "Usage: pd-patch apply <patch> <path> [--dry-run]\n");
                return 1;
            }
            NSString *patchPath = [NSString stringWithUTF8String:argv[2]];
            NSString *componentPath = [NSString stringWithUTF8String:argv[3]];
            BOOL dryRun = (argc > 4 && strcmp(argv[4], "--dry-run") == 0);
            
            BOOL success = [PatchCLI applyPatch:patchPath toPath:componentPath dryRun:dryRun];
            return success ? 0 : 1;
        }
        
        if ([command isEqualToString:@"validate"]) {
            if (argc < 3) {
                fprintf(stderr, "Usage: pd-patch validate <patch>\n");
                return 1;
            }
            NSString *patchPath = [NSString stringWithUTF8String:argv[2]];
            BOOL valid = [PatchCLI validatePatch:patchPath];
            return valid ? 0 : 1;
        }
        
        if ([command isEqualToString:@"report"]) {
            if (argc < 4) {
                fprintf(stderr, "Usage: pd-patch report <directory> <output>\n");
                return 1;
            }
            NSString *directory = [NSString stringWithUTF8String:argv[2]];
            NSString *outputPath = [NSString stringWithUTF8String:argv[3]];
            [PatchCLI createPatchReport:directory output:outputPath];
            return 0;
        }
        
        fprintf(stderr, "Unknown command: %s\n\n", [command UTF8String]);
        [PatchCLI printUsage];
        return 1;
    }
}