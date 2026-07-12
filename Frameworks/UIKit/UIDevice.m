#import <UIKit/UIDevice.h>

NSString *const UIDeviceBatteryStateDidChangeNotification = @"UIDeviceBatteryStateDidChangeNotification";
NSString *const UIDeviceBatteryLevelDidChangeNotification = @"UIDeviceBatteryLevelDidChangeNotification";
NSString *const UIDeviceProximityStateDidChangeNotification = @"UIDeviceProximityStateDidChangeNotification";
NSString *const UIDeviceOrientationDidChangeNotification = @"UIDeviceOrientationDidChangeNotification";

static UIDevice *currentDevice = nil;

@implementation UIDevice

+ (UIDevice *)currentDevice {
    if (!currentDevice)
        currentDevice = [[self alloc] init];
    return currentDevice;
}

- (instancetype)init {
    self = [super init];
    if (self) {
        _name = @"iPhone Simulator";
        _model = @"iPhone";
        _localizedModel = @"iPhone";
        _systemName = @"iPhone OS";
        _systemVersion = @"14.0";
        _batteryState = UIDeviceBatteryStateUnknown;
        _batteryLevel = -1;
        _batteryMonitoringEnabled = NO;
        _proximityMonitoringEnabled = NO;
        _proximityState = NO;
        _multitaskingSupported = YES;
        _orientation = UIDeviceOrientationPortrait;
        _userInterfaceIdiom = UIUserInterfaceIdiomPhone;
        _identifierForVendor = [[NSUUID alloc] initWithUUIDString:@"00000000-0000-0000-0000-000000000000"];
    }
    return self;
}

- (void)dealloc {
    [_name release];
    [_model release];
    [_localizedModel release];
    [_systemName release];
    [_systemVersion release];
    [_identifierForVendor release];
    [super dealloc];
}

- (NSString *)name {
    return _name;
}

- (NSString *)model {
    return _model;
}

- (NSString *)localizedModel {
    return _localizedModel;
}

- (NSString *)systemName {
    return _systemName;
}

- (NSString *)systemVersion {
    return _systemVersion;
}

- (UIDeviceBatteryState)batteryState {
    return _batteryState;
}

- (CGFloat)batteryLevel {
    return _batteryLevel;
}

- (BOOL)isBatteryMonitoringEnabled {
    return _batteryMonitoringEnabled;
}

- (void)setBatteryMonitoringEnabled:(BOOL)enabled {
    _batteryMonitoringEnabled = enabled;
}

- (BOOL)isProximityMonitoringEnabled {
    return _proximityMonitoringEnabled;
}

- (void)setProximityMonitoringEnabled:(BOOL)enabled {
    _proximityMonitoringEnabled = enabled;
}

- (BOOL)proximityState {
    return _proximityState;
}

- (BOOL)isMultitaskingSupported {
    return _multitaskingSupported;
}

- (UIUserInterfaceIdiom)userInterfaceIdiom {
    return _userInterfaceIdiom;
}

- (NSUUID *)identifierForVendor {
    return _identifierForVendor;
}

- (void)beginGeneratingDeviceOrientationNotifications {
}

- (void)endGeneratingDeviceOrientationNotifications {
}

- (BOOL)generatesDeviceOrientationNotifications {
    return NO;
}

- (UIDeviceOrientation)orientation {
    return (UIDeviceOrientation)_orientation;
}

- (void)playInputClick {
}

- (float)systemVersion {
    return [_systemVersion floatValue];
}

@end
