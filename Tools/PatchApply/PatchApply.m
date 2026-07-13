#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>

@interface PatchFile : NSObject
@property (nonatomic, strong) NSString *filePath;
@property (nonatomic, strong) NSString *fileName;
@property (nonatomic, strong) NSString *targetComponent;  // XNU, lk2nd, xnu-deps-linux
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
    
    // Extract subject line (first line after metadata)
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
    
    // Count hunks (sections starting with @@)
    NSUInteger hunkCount = 0;
    for (NSString *line in lines) {
        if ([line hasPrefix:@"@@"]) {
            hunkCount++;
        }
    }
    metadata[@"hunks"] = @(hunkCount);
    
    // Count added/removed lines
    NSUInteger added = 0, removed = 0;
    for (NSString *line in lines) {
        if ([line hasPrefix:@"+"]) added++;
        if ([line hasPrefix:@"-"]) removed++;
    }
    metadata[@"linesAdded"] = @(added);
    metadata[@"linesRemoved"] = @(removed);
    
    return metadata;
}

@end

@interface PatchRepository : NSObject
@property (nonatomic, strong) NSMutableArray *patches;
@property (nonatomic, strong) NSString *repositoryPath;

- (instancetype)initWithPath:(NSString *)path;
- (void)scanForPatches;
- (BOOL)applyPatch:(PatchFile *)patch toComponent:(NSString *)componentPath;
- (NSString *)executeCommand:(NSString *)command withArguments:(NSArray *)args;

@end

@implementation PatchRepository

- (instancetype)initWithPath:(NSString *)path {
    self = [super init];
    if (self) {
        self.repositoryPath = path;
        self.patches = [NSMutableArray array];
    }
    return self;
}

- (void)scanForPatches {
    NSError *error = nil;
    NSFileManager *fm = [NSFileManager defaultManager];
    NSArray *contents = [fm contentsOfDirectoryAtPath:self.repositoryPath error:&error];
    
    [self.patches removeAllObjects];
    
    for (NSString *item in contents) {
        if ([item hasSuffix:@".patch"]) {
            NSString *fullPath = [self.repositoryPath stringByAppendingPathComponent:item];
            PatchFile *patch = [[PatchFile alloc] initWithPath:fullPath];
            [self.patches addObject:patch];
        }
    }
}

- (BOOL)applyPatch:(PatchFile *)patch toComponent:(NSString *)componentPath {
    NSTask *task = [[NSTask alloc] init];
    [task setLaunchPath:@"/usr/bin/patch"];
    [task setArguments:@[@"-p1", @"-i", patch.filePath]];
    [task setCurrentDirectoryPath:componentPath];
    
    NSPipe *errorPipe = [NSPipe pipe];
    [task setStandardError:errorPipe];
    
    NSError *error = nil;
    BOOL success = [task launchAndReturnError:&error];
    
    if (success) {
        [task waitUntilExit];
        success = [task terminationStatus] == 0;
    }
    
    patch.applied = success;
    return success;
}

- (NSString *)executeCommand:(NSString *)command withArguments:(NSArray *)args {
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

@interface PatchManagerWindow : NSWindow <NSTableViewDataSource, NSTableViewDelegate>
@property (nonatomic, strong) PatchRepository *repository;
@property (nonatomic, strong) NSTableView *patchTable;
@property (nonatomic, strong) NSTextView *detailsView;
@property (nonatomic, strong) NSTextField *statusField;
@property (nonatomic, strong) NSSegmentedControl *componentSelector;

- (void)setupUI;
- (void)loadPatches;
- (void)applySelectedPatch:(id)sender;
- (void)displayPatchDetails:(PatchFile *)patch;

@end

@implementation PatchManagerWindow

- (instancetype)initWithContentRect:(NSRect)contentRect 
                          styleMask:(NSWindowStyleMask)styleMask 
                            backing:(NSBackingStoreType)backingType 
                              defer:(BOOL)flag {
    self = [super initWithContentRect:contentRect styleMask:styleMask backing:backingType defer:flag];
    if (self) {
        [self setTitle:@"PocketDarwin Patch Manager"];
        [self setReleasedWhenClosed:NO];
        self.repository = [[PatchRepository alloc] initWithPath:NSHomeDirectory()];
        [self setupUI];
        [self loadPatches];
    }
    return self;
}

- (void)setupUI {
    NSView *contentView = [self contentView];
    [contentView setWantsLayer:YES];
    contentView.layer.backgroundColor = [NSColor colorWithCalibratedWhite:0.95 alpha:1.0].CGColor;
    
    // Title
    NSTextField *titleField = [[NSTextField alloc] initWithFrame:NSMakeRect(20, 450, 560, 40)];
    [titleField setStringValue:@"PocketDarwin Patch Manager"];
    titleField.font = [NSFont boldSystemFontOfSize:24];
    titleField.backgroundColor = [NSColor clearColor];
    titleField.bordered = NO;
    titleField.editable = NO;
    [contentView addSubview:titleField];
    
    // Component selector
    NSBox *componentBox = [[NSBox alloc] initWithFrame:NSMakeRect(20, 390, 560, 50)];
    componentBox.title = @"Target Component";
    [contentView addSubview:componentBox];
    
    self.componentSelector = [[NSSegmentedControl alloc] initWithFrame:NSMakeRect(20, 395, 560, 28)];
    [self.componentSelector setSegmentCount:3];
    [self.componentSelector setLabel:@"XNU Kernel" forSegment:0];
    [self.componentSelector setLabel:@"lk2nd" forSegment:1];
    [self.componentSelector setLabel:@"XNU Deps" forSegment:2];
    [self.componentSelector setSelectedSegment:0];
    [contentView addSubview:self.componentSelector];
    
    // Patch table
    NSBox *tableBox = [[NSBox alloc] initWithFrame:NSMakeRect(20, 200, 560, 180)];
    tableBox.title = @"Available Patches";
    [contentView addSubview:tableBox];
    
    NSScrollView *scrollView = [[NSScrollView alloc] initWithFrame:NSMakeRect(30, 210, 540, 160)];
    [scrollView setHasVerticalScroller:YES];
    [scrollView setHasHorizontalScroller:NO];
    [contentView addSubview:scrollView];
    
    self.patchTable = [[NSTableView alloc] initWithFrame:NSMakeRect(0, 0, 540, 160)];
    self.patchTable.dataSource = self;
    self.patchTable.delegate = self;
    [scrollView setDocumentView:self.patchTable];
    
    // Table columns
    NSTableColumn *nameCol = [[NSTableColumn alloc] initWithIdentifier:@"fileName"];
    nameCol.headerCell.stringValue = @"File";
    nameCol.width = 250;
    [self.patchTable addTableColumn:nameCol];
    
    NSTableColumn *componentCol = [[NSTableColumn alloc] initWithIdentifier:@"targetComponent"];
    componentCol.headerCell.stringValue = @"Component";
    componentCol.width = 140;
    [self.patchTable addTableColumn:componentCol];
    
    NSTableColumn *statusCol = [[NSTableColumn alloc] initWithIdentifier:@"applied"];
    statusCol.headerCell.stringValue = @"Applied";
    statusCol.width = 80;
    [self.patchTable addTableColumn:statusCol];
    
    // Details view
    NSBox *detailsBox = [[NSBox alloc] initWithFrame:NSMakeRect(20, 60, 560, 130)];
    detailsBox.title = @"Patch Details";
    [contentView addSubview:detailsBox];
    
    NSScrollView *detailsScroll = [[NSScrollView alloc] initWithFrame:NSMakeRect(30, 70, 540, 110)];
    [detailsScroll setHasVerticalScroller:YES];
    [contentView addSubview:detailsScroll];
    
    self.detailsView = [[NSTextView alloc] initWithFrame:NSMakeRect(0, 0, 540, 110)];
    self.detailsView.editable = NO;
    self.detailsView.font = [NSFont systemFontOfSize:11];
    [detailsScroll setDocumentView:self.detailsView];
    
    // Status bar and buttons
    self.statusField = [[NSTextField alloc] initWithFrame:NSMakeRect(20, 30, 400, 20)];
    self.statusField.stringValue = @"Ready";
    self.statusField.bordered = NO;
    self.statusField.editable = NO;
    [contentView addSubview:self.statusField];
    
    NSButton *applyButton = [[NSButton alloc] initWithFrame:NSMakeRect(440, 30, 140, 32)];
    [applyButton setTitle:@"Apply Selected"];
    [applyButton setBezelStyle:NSBezelStyleRounded];
    [applyButton setTarget:self];
    [applyButton setAction:@selector(applySelectedPatch:)];
    [contentView addSubview:applyButton];
}

- (void)loadPatches {
    [self.repository scanForPatches];
    [self.patchTable reloadData];
    self.statusField.stringValue = [NSString stringWithFormat:@"Loaded %lu patches", 
                                    (unsigned long)self.repository.patches.count];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView {
    return [self.repository.patches count];
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
    PatchFile *patch = self.repository.patches[row];
    NSString *identifier = tableColumn.identifier;
    
    if ([identifier isEqualToString:@"fileName"]) {
        return patch.fileName;
    } else if ([identifier isEqualToString:@"targetComponent"]) {
        return patch.targetComponent;
    } else if ([identifier isEqualToString:@"applied"]) {
        return patch.applied ? @"✓" : @"–";
    }
    return @"";
}

- (void)tableViewSelectionDidChange:(NSNotification *)notification {
    NSInteger selectedRow = [self.patchTable selectedRow];
    if (selectedRow >= 0 && selectedRow < (NSInteger)self.repository.patches.count) {
        PatchFile *patch = self.repository.patches[selectedRow];
        [self displayPatchDetails:patch];
    }
}

- (void)displayPatchDetails:(PatchFile *)patch {
    NSDictionary *metadata = [patch parsePatchMetadata];
    
    NSString *details = [NSString stringWithFormat:
        @"File: %@\n"
        @"Component: %@\n"
        @"Created: %@\n"
        @"Status: %@\n\n"
        @"Statistics:\n"
        @"  Hunks: %@\n"
        @"  Lines Added: %@\n"
        @"  Lines Removed: %@\n\n"
        @"Subject: %@",
        patch.fileName,
        patch.targetComponent,
        patch.createdDate,
        patch.applied ? @"Applied" : @"Not Applied",
        metadata[@"hunks"],
        metadata[@"linesAdded"],
        metadata[@"linesRemoved"],
        metadata[@"subject"] ?: @"(none)"
    ];
    
    self.detailsView.string = details;
}

- (void)applySelectedPatch:(id)sender {
    NSInteger selectedRow = [self.patchTable selectedRow];
    if (selectedRow < 0) {
        self.statusField.stringValue = @"No patch selected";
        return;
    }
    
    PatchFile *patch = self.repository.patches[selectedRow];
    NSString *componentPath = [self getComponentPath];
    
    self.statusField.stringValue = [NSString stringWithFormat:@"Applying %@...", patch.fileName];
    
    BOOL success = [self.repository applyPatch:patch toComponent:componentPath];
    
    if (success) {
        self.statusField.stringValue = [NSString stringWithFormat:@"✓ Applied %@", patch.fileName];
        [self.patchTable reloadDataForRowIndexes:[NSIndexSet indexSetWithIndex:selectedRow] 
                                   columnIndexes:[NSIndexSet indexSetWithIndex:2]];
    } else {
        self.statusField.stringValue = [NSString stringWithFormat:@"✗ Failed to apply %@", patch.fileName];
    }
}

- (NSString *)getComponentPath {
    NSInteger segment = [self.componentSelector selectedSegment];
    NSString *homeDir = NSHomeDirectory();
    
    switch (segment) {
        case 0: return [homeDir stringByAppendingPathComponent:@"PocketDarwin/Kernel"];
        case 1: return [homeDir stringByAppendingPathComponent:@"PocketDarwin/boot/MegaBoot"];
        case 2: return [homeDir stringByAppendingPathComponent:@"PocketDarwin/BuildHost/xnu-deps-linux"];
        default: return homeDir;
    }
}

@end

@interface PatchManagerDelegate : NSObject <NSApplicationDelegate>
@end

@implementation PatchManagerDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
    NSRect frame = NSMakeRect(100, 100, 600, 550);
    PatchManagerWindow *window = [[PatchManagerWindow alloc] initWithContentRect:frame
                                                                        styleMask:(NSWindowStyleMaskTitled |
                                                                                   NSWindowStyleMaskClosable |
                                                                                   NSWindowStyleMaskMiniaturizable |
                                                                                   NSWindowStyleMaskResizable)
                                                                          backing:NSBackingStoreBuffered
                                                                            defer:NO];
    [window makeKeyAndOrderFront:nil];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return YES;
}

@end

int main(int argc, char *argv[]) {
    @autoreleasepool {
        NSApplication *app = [NSApplication sharedApplication];
        PatchManagerDelegate *delegate = [[PatchManagerDelegate alloc] init];
        [app setDelegate:delegate];
        [app run];
    }
    return 0;
}