#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>

@interface BuildController : NSObject <NSTextViewDelegate>
{
    NSWindow *window;
    NSTextView *outputView;
    NSButton *buildButton;
    NSButton *cleanButton;
    NSButton *stopButton;
    NSProgressIndicator *progressIndicator;
    NSTask *currentTask;
    NSString *sourceFolder;
    NSTextField *folderPathLabel;
    NSString *selectedDeviceBundle;
    NSTextField *deviceLabel;
    NSPopUpButton *boardConfigPopup;
    NSTimer *outputTimer;
    NSFileHandle *outputHandle;
}
- (void)build:(id)sender;
- (void)clean:(id)sender;
- (void)stop:(id)sender;
- (void)taskCompleted:(NSNotification *)notification;
- (NSString *)getMakeCommand;
- (void)openOutputFile:(NSString *)filename;
- (void)openLk2nd:(id)sender;
- (void)openKickstartLoader:(id)sender;
- (void)openMachKernel:(id)sender;
- (void)openUserland:(id)sender;
- (void)selectSourceFolder:(id)sender;
- (void)createSDCardImage:(id)sender;
- (void)flashLk2nd:(id)sender;
- (void)readOutput:(NSTimer *)timer;
@end

@implementation BuildController

- (id)init
{
    self = [super init];
    if (self) {
        currentTask = nil;
        sourceFolder = @"/workspaces/pocketdarwin";
        outputTimer = nil;
    }
    return self;
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
    NSRect windowRect = NSMakeRect(100, 100, 1000, 700);
    window = [[NSWindow alloc] initWithContentRect:windowRect
                                         styleMask:(NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask)
                                           backing:NSBackingStoreBuffered
                                             defer:NO];
    [window setTitle:@"PocketDarwin Build System"];
    
    NSView *contentView = [window contentView];
    
    // Title Label
    NSTextField *titleLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(10, 660, 980, 30)];
    [titleLabel setStringValue:@"PocketDarwin Operating System Builder"];
    [titleLabel setEditable:NO];
    [titleLabel setBezeled:NO];
    [titleLabel setDrawsBackground:NO];
    NSFont *titleFont = [NSFont systemFontOfSize:16];
    [titleLabel setFont:titleFont];
    [contentView addSubview:titleLabel];
    
    // Output TextView
    NSScrollView *scrollView = [[NSScrollView alloc] initWithFrame:NSMakeRect(10, 150, 980, 500)];
    [scrollView setHasVerticalScroller:YES];
    [scrollView setHasHorizontalScroller:NO];
    
    outputView = [[NSTextView alloc] initWithFrame:NSMakeRect(0, 0, 980, 500)];
    [outputView setDelegate:self];
    [outputView setEditable:NO];
    [outputView setFont:[NSFont systemFontOfSize:11]];
    [scrollView setDocumentView:outputView];
    [contentView addSubview:scrollView];
    
    // Progress Indicator
    progressIndicator = [[NSProgressIndicator alloc] initWithFrame:NSMakeRect(10, 120, 300, 20)];
    [progressIndicator setStyle:NSProgressIndicatorBarStyle];
    [progressIndicator setIndeterminate:YES];
    [contentView addSubview:progressIndicator];
    
    // Build Button
    buildButton = [[NSButton alloc] initWithFrame:NSMakeRect(10, 80, 100, 32)];
    [buildButton setTitle:@"Build"];
    [buildButton setTarget:self];
    [buildButton setAction:@selector(build:)];
    [contentView addSubview:buildButton];
    
    // Clean Button
    cleanButton = [[NSButton alloc] initWithFrame:NSMakeRect(120, 80, 100, 32)];
    [cleanButton setTitle:@"Clean"];
    [cleanButton setTarget:self];
    [cleanButton setAction:@selector(clean:)];
    [contentView addSubview:cleanButton];
    
    // Stop Button
    stopButton = [[NSButton alloc] initWithFrame:NSMakeRect(230, 80, 100, 32)];
    [stopButton setTitle:@"Stop"];
    [stopButton setTarget:self];
    [stopButton setAction:@selector(stop:)];
    [stopButton setEnabled:NO];
    [contentView addSubview:stopButton];
    
    // Source Folder Selection
    NSTextField *folderLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(10, 55, 100, 20)];
    [folderLabel setStringValue:@"Source Folder:"];
    [folderLabel setEditable:NO];
    [folderLabel setBezeled:NO];
    [folderLabel setDrawsBackground:NO];
    [contentView addSubview:folderLabel];
    
    folderPathLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(110, 55, 400, 20)];
    [folderPathLabel setStringValue:sourceFolder];
    [folderPathLabel setEditable:NO];
    [folderPathLabel setBezeled:YES];
    [contentView addSubview:folderPathLabel];
    
    NSButton *selectFolderButton = [[NSButton alloc] initWithFrame:NSMakeRect(520, 50, 120, 32)];
    [selectFolderButton setTitle:@"Browse..."];
    [selectFolderButton setTarget:self];
    [selectFolderButton setAction:@selector(selectSourceFolder:)];
    [contentView addSubview:selectFolderButton];
    
    // Device Bundle Selection
    NSTextField *deviceBundleLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(10, 30, 100, 20)];
    [deviceBundleLabel setStringValue:@"Device Bundle:"];
    [deviceBundleLabel setEditable:NO];
    [deviceBundleLabel setBezeled:NO];
    [deviceBundleLabel setDrawsBackground:NO];
    [contentView addSubview:deviceBundleLabel];
    
    deviceLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(110, 30, 400, 20)];
    [deviceLabel setStringValue:@"(None selected)"];
    [deviceLabel setEditable:NO];
    [deviceLabel setBezeled:YES];
    [contentView addSubview:deviceLabel];
    
    NSButton *selectDeviceButton = [[NSButton alloc] initWithFrame:NSMakeRect(520, 25, 120, 32)];
    [selectDeviceButton setTitle:@"Select..."];
    [selectDeviceButton setTarget:self];
    [selectDeviceButton setAction:@selector(selectDeviceBundle:)];
    [contentView addSubview:selectDeviceButton];
    
    // Board Configuration Selection
    NSTextField *boardLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(650, 30, 100, 20)];
    [boardLabel setStringValue:@"Board Config:"];
    [boardLabel setEditable:NO];
    [boardLabel setBezeled:NO];
    [boardLabel setDrawsBackground:NO];
    [contentView addSubview:boardLabel];
    
    boardConfigPopup = [[NSPopUpButton alloc] initWithFrame:NSMakeRect(750, 25, 220, 32)];
    [boardConfigPopup addItemWithTitle:@"Select a board..."];
    [boardConfigPopup addItemWithTitle:@"Motorola Harpia (MSM8916)"];
    [boardConfigPopup addItemWithTitle:@"QEMU ARM64"];
    [boardConfigPopup setTarget:self];
    [boardConfigPopup setAction:@selector(boardConfigChanged:)];
    [contentView addSubview:boardConfigPopup];
    
    [window makeKeyAndOrderFront:nil];
}

- (NSString *)getMakeCommand
{
    NSTask *task = [[NSTask alloc] init];
    [task setLaunchPath:@"/bin/sh"];
    [task setArguments:@[@"-c", @"which gmake || which make"]];
    
    NSPipe *pipe = [NSPipe pipe];
    [task setStandardOutput:pipe];
    [task launch];
    [task waitUntilExit];
    
    NSData *data = [[pipe fileHandleForReading] readDataToEndOfFile];
    NSString *result = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    result = [result stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
    
    return [result length] > 0 ? result : @"make";
}

- (void)readOutput:(NSTimer *)timer
{
    if (!outputHandle || !currentTask) {
        return;
    }
    
    NSData *data;
    while ((data = [outputHandle availableData]) && [data length]) {
        NSString *output = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        NSTextStorage *textStorage = [outputView textStorage];
        [textStorage appendAttributedString:[[NSAttributedString alloc] initWithString:output]];
        [outputView scrollRangeToVisible:NSMakeRange([textStorage length], 0)];
    }
    
    if ([currentTask isRunning] == NO) {
        [outputTimer invalidate];
        outputTimer = nil;
    }
}

- (void)build:(id)sender
{
    if (currentTask) {
        return;
    }
    
    NSString *makeCmd = [self getMakeCommand];
    
    currentTask = [[NSTask alloc] init];
    [currentTask setLaunchPath:@"/bin/sh"];
    [currentTask setArguments:@[@"-c", [NSString stringWithFormat:@"cd %@ && %@", sourceFolder, makeCmd]]];
    
    NSPipe *pipe = [NSPipe pipe];
    [currentTask setStandardOutput:pipe];
    [currentTask setStandardError:pipe];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(taskCompleted:)
                                                 name:NSTaskDidTerminateNotification
                                               object:currentTask];
    
    [buildButton setEnabled:NO];
    [cleanButton setEnabled:NO];
    [stopButton setEnabled:YES];
    [progressIndicator startAnimation:self];
    
    outputHandle = [pipe fileHandleForReading];
    outputTimer = [NSTimer scheduledTimerWithTimeInterval:0.1 target:self selector:@selector(readOutput:) userInfo:nil repeats:YES];
    
    [currentTask launch];
}

- (void)clean:(id)sender
{
    if (currentTask) {
        return;
    }
    
    NSString *makeCmd = [self getMakeCommand];
    
    currentTask = [[NSTask alloc] init];
    [currentTask setLaunchPath:@"/bin/sh"];
    [currentTask setArguments:@[@"-c", [NSString stringWithFormat:@"cd %@ && %@ clean", sourceFolder, makeCmd]]];
    
    NSPipe *pipe = [NSPipe pipe];
    [currentTask setStandardOutput:pipe];
    [currentTask setStandardError:pipe];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(taskCompleted:)
                                                 name:NSTaskDidTerminateNotification
                                               object:currentTask];
    
    [buildButton setEnabled:NO];
    [cleanButton setEnabled:NO];
    [stopButton setEnabled:YES];
    [progressIndicator startAnimation:self];
    
    outputHandle = [pipe fileHandleForReading];
    outputTimer = [NSTimer scheduledTimerWithTimeInterval:0.1 target:self selector:@selector(readOutput:) userInfo:nil repeats:YES];
    
    [currentTask launch];
}

- (void)stop:(id)sender
{
    if (currentTask) {
        [currentTask terminate];
    }
}

- (void)taskCompleted:(NSNotification *)notification
{
    [[NSNotificationCenter defaultCenter] removeObserver:self
                                                     name:NSTaskDidTerminateNotification
                                                   object:currentTask];
    
    int status = [currentTask terminationStatus];
    NSString *statusMsg = (status == 0) ? @"\n✓ Build completed successfully\n" : [NSString stringWithFormat:@"\n✗ Build failed with status %d\n", status];
    
    NSTextStorage *textStorage = [outputView textStorage];
    [textStorage appendAttributedString:[[NSAttributedString alloc] initWithString:statusMsg]];
    [outputView scrollRangeToVisible:NSMakeRange([textStorage length], 0)];
    
    [buildButton setEnabled:YES];
    [cleanButton setEnabled:YES];
    [stopButton setEnabled:NO];
    [progressIndicator stopAnimation:self];
    
    currentTask = nil;
    outputHandle = nil;
}

- (void)selectSourceFolder:(id)sender
{
    NSOpenPanel *openPanel = [NSOpenPanel openPanel];
    [openPanel setCanChooseDirectories:YES];
    [openPanel setCanChooseFiles:NO];
    [openPanel setAllowsMultipleSelection:NO];
    [openPanel setTitle:@"Select PocketDarwin Source Folder"];
    [openPanel setMessage:@"Choose the root folder of your PocketDarwin source code"];
    [openPanel setDirectoryURL:[NSURL fileURLWithPath:sourceFolder]];
    
    [openPanel beginSheetForDirectory:sourceFolder 
                                file:nil 
                               types:nil 
                  modalForWindow:window 
                   modalDelegate:self 
                  didEndSelector:@selector(sourceFolderPanelDidEnd:returnCode:contextInfo:) 
                     contextInfo:nil];
}

- (void)sourceFolderPanelDidEnd:(NSOpenPanel *)panel returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
    if (returnCode == NSOKButton) {
        sourceFolder = [[panel filename] retain];
        [folderPathLabel setStringValue:sourceFolder];
    }
}

- (void)selectDeviceBundle:(id)sender
{
    NSOpenPanel *openPanel = [NSOpenPanel openPanel];
    [openPanel setCanChooseDirectories:YES];
    [openPanel setCanChooseFiles:NO];
    [openPanel setAllowsMultipleSelection:NO];
    [openPanel setTitle:@"Select Device Bundle"];
    [openPanel setMessage:@"Choose a .device NSBundle for device configuration"];
    
    [openPanel beginSheetForDirectory:NSHomeDirectory()
                                file:nil
                               types:nil
                  modalForWindow:window
                   modalDelegate:self
                  didEndSelector:@selector(deviceBundlePanelDidEnd:returnCode:contextInfo:)
                     contextInfo:nil];
}

- (void)deviceBundlePanelDidEnd:(NSOpenPanel *)panel returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
    if (returnCode == NSOKButton) {
        selectedDeviceBundle = [[[panel filename] lastPathComponent] retain];
        [deviceLabel setStringValue:selectedDeviceBundle];
    }
}

- (void)boardConfigChanged:(id)sender
{
    NSInteger selectedIndex = [boardConfigPopup indexOfSelectedItem];
    NSString *boardConfig = nil;
    
    if (selectedIndex == 1) {
        boardConfig = @"motorola_harpia_msm8916";
    } else if (selectedIndex == 2) {
        boardConfig = @"qemu_arm64";
    }
    
    if (boardConfig) {
        // Store board config for build
    }
}

@end

int main(int argc, char *argv[])
{
    [NSApplication sharedApplication];
    
    BuildController *controller = [[BuildController alloc] init];
    [NSApp setDelegate:controller];
    
    return NSApplicationMain(argc, argv);
}