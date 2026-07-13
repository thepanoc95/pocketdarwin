#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (strong) NSWindow *window;
@end

@implementation AppDelegate

@synthesize window = _window;

// keep a popup for device selection
NSPopUpButton *devicePopup;

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
	NSRect frame = NSMakeRect(0, 0, 480, 320);
	NSUInteger style = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask;
	self.window = [[NSWindow alloc] initWithContentRect:frame styleMask:style backing:NSBackingStoreBuffered defer:NO];
	[self.window center];
	[self.window setTitle:@"Install Mac OS X"]; 

	NSView *content = [self.window contentView];

	// Use a non-Helvetica font family for the installer UI
	NSFont *titleFont = [NSFont fontWithName:@"Menlo-Bold" size:24];
	if (!titleFont) titleFont = [NSFont boldSystemFontOfSize:24];
	NSFont *controlFont = [NSFont fontWithName:@"Menlo-Regular" size:13];
	if (!controlFont) controlFont = [NSFont systemFontOfSize:13];

	NSTextField *titleLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(20, 260, 440, 32)];
	[titleLabel setStringValue:@"Install PocketDarwin"];
	[titleLabel setBezeled:NO];
	[titleLabel setDrawsBackground:NO];
	[titleLabel setEditable:NO];
	[titleLabel setSelectable:NO];
	[titleLabel setFont:titleFont];
	[content addSubview:titleLabel];

	NSArray *titles = @[@"Install", @"Restore System From Backup...", @"Customize...", @"Options...", @"Quit Installer"];
	for (NSInteger i = 0; i < [titles count]; i++) {
		NSButton *b = [[NSButton alloc] initWithFrame:NSMakeRect(20, 200 - i*44, 440, 36)];
		[b setTitle:titles[i]];
		[b setBezelStyle:NSRoundedBezelStyle];
		[b setFont:controlFont];
		[b setTarget:self];
		[b setAction:@selector(buttonPressed:)];
		[b setTag:i];
		[content addSubview:b];
	}

	// Device selection popup
	devicePopup = [[NSPopUpButton alloc] initWithFrame:NSMakeRect(260, 220, 200, 26) pullsDown:NO];
	[devicePopup setFont:controlFont];
	[devicePopup addItemWithTitle:@"Select Device..."];
	NSArray<NSString *> *devices = [self availableDeviceBundles];
	for (NSString *d in devices) [devicePopup addItemWithTitle:d];
	[content addSubview:devicePopup];

	[self.window makeKeyAndOrderFront:nil];
}

- (void)buttonPressed:(id)sender {
	NSInteger tag = [sender tag];
	switch (tag) {
		case 0: {
			// Before installing, check required host tools
			NSArray<NSString *> *missing = [self checkRequiredTools:@[@"adb", @"fastboot", @"dd"]];
			if ([missing count] > 0) {
				NSAlert *err = [[NSAlert alloc] init];
				[err setMessageText:@"Missing Required Tools"];
				NSString *info = [NSString stringWithFormat:@"The following required tools were not found in PATH: %@\nPlease install them and try again.", [missing componentsJoinedByString:@", "]];
				[err setInformativeText:info];
				[err addButtonWithTitle:@"OK"];
				[err runModal];
				break;
			}
			// Simulate install
			NSAlert *a = [[NSAlert alloc] init];
			[a setMessageText:@"Ready to Install"];
			[a setInformativeText:@"All host tools present. This will install PocketDarwin on the selected volume. Proceed?"];
			[a addButtonWithTitle:@"Install"];
			[a addButtonWithTitle:@"Cancel"];
			[a runModal];
		} break;
		case 1: {
			NSAlert *a = [[NSAlert alloc] init];
			[a setMessageText:@"Restore From Backup"];
			[a setInformativeText:@"Restore functionality is simulated in this clone."];
			[a runModal];
		} break;
		case 2: {
			NSAlert *a = [[NSAlert alloc] init];
			[a setMessageText:@"Customize Installation"];
			[a setInformativeText:@"No customizable packages are available in this simulated installer."];
			[a runModal];
		} break;
		case 3: {
			NSAlert *a = [[NSAlert alloc] init];
			[a setMessageText:@"Options"];
			[a setInformativeText:@"Installation options (simulated)."];
			[a runModal];
		} break;
		case 4: [NSApp terminate:self]; break;
		default: break;
	}
}

- (NSArray<NSString *> *)checkRequiredTools:(NSArray<NSString *> *)tools {
	NSMutableArray<NSString *> *missing = [NSMutableArray array];
	for (NSString *t in tools) {
		NSTask *which = [[NSTask alloc] init];
		which.launchPath = @"/usr/bin/which";
		which.arguments = @[t];
		NSPipe *out = [NSPipe pipe];
		which.standardOutput = out;
		which.standardError = [NSPipe pipe];
		@try {
			[which launch];
			[which waitUntilExit];
			if ([which terminationStatus] != 0) {
				[missing addObject:t];
			} else {
				// also ensure the output is non-empty
				NSData *d = [[out fileHandleForReading] readDataToEndOfFile];
				NSString *s = [[NSString alloc] initWithData:d encoding:NSUTF8StringEncoding];
				if (s == nil || [s length] == 0) [missing addObject:t];
			}
		} @catch (NSException *ex) {
			[missing addObject:t];
		}
	}
	return missing;
}

- (void)downloadURL:(NSString *)urlString toFilename:(NSString *)dest {
	NSURL *url = [NSURL URLWithString:urlString];
	NSURLSession *session = [NSURLSession sharedSession];
	NSURLSessionDownloadTask *task = [session downloadTaskWithURL:url completionHandler:^(NSURL *location, NSURLResponse *response, NSError *error) {
		if (error) {
			NSLog(@"Download failed: %@", error);
			return;
		}
		NSFileManager *fm = [NSFileManager defaultManager];
		NSURL *dst = [NSURL fileURLWithPath:dest];
		[fm removeItemAtURL:dst error:nil];
		NSError *mvErr = nil;
		[fm moveItemAtURL:location toURL:dst error:&mvErr];
		if (mvErr) NSLog(@"Move failed: %@", mvErr);
		else NSLog(@"Saved to %@", dest);
	}];
	[task resume];
}

- (void)loadKickstart {
	// Simulate loading kickstart_loader.elf; real implementation would interact with device
	NSLog(@"Loading kickstart_loader.elf (simulated)");
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender { return YES; }

@end

int main(int argc, char *argv[]) {
	[NSApplication sharedApplication];
	AppDelegate *del = [AppDelegate new];
	[NSApp setDelegate:del];
	return NSApplicationMain(argc, (const char **)argv);
}

