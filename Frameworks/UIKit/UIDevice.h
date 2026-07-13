#import <Foundation/Foundation.h>
#import <UIKit/UIKitExport.h>

typedef NSInteger UIDeviceOrientation;

enum {
    UIDeviceOrientationUnknown,
    UIDeviceOrientationPortrait,
    UIDeviceOrientationPortraitUpsideDown,
    UIDeviceOrientationLandscapeLeft,
    UIDeviceOrientationLandscapeRight,
    UIDeviceOrientationFaceUp,
    UIDeviceOrientationFaceDown,
};

typedef NSInteger UIDeviceBatteryState;

enum {
    UIDeviceBatteryStateUnknown,
    UIDeviceBatteryStateUnplugged,
    UIDeviceBatteryStateCharging,
    UIDeviceBatteryStateFull,
};

typedef NSInteger UIUserInterfaceIdiom;

enum {
    UIUserInterfaceIdiomPhone,
    UIUserInterfaceIdiomPad,
    UIUserInterfaceIdiomTV,
    UIUserInterfaceIdiomCarPlay,
    UIUserInterfaceIdiomMac,
};

@interface UIDevice : NSObject {
    NSString *_name;
    NSString *_model;
    NSString *_localizedModel;
    NSString *_systemName;
    NSString *_systemVersion;
    UIDeviceBatteryState _batteryState;
    CGFloat _batteryLevel;
    BOOL _batteryMonitoringEnabled;
    BOOL _proximityMonitoringEnabled;
    BOOL _proximityState;
    BOOL _multitaskingSupported;
    UIDeviceOrientation _orientation;
    UIUserInterfaceIdiom _userInterfaceIdiom;
    NSUUID *_identifierForVendor;
}

+ (UIDevice *)currentDevice;

@property (nonatomic, readonly, retain) NSString *name;
@property (nonatomic, readonly, retain) NSString *model;
@property (nonatomic, readonly, retain) NSString *localizedModel;
@property (nonatomic, readonly, retain) NSString *systemName;
@property (nonatomic, readonly, retain) NSString *systemVersion;

@property (nonatomic, readonly) UIDeviceBatteryState batteryState;
@property (nonatomic, readonly) CGFloat batteryLevel;
@property (nonatomic, getter=isBatteryMonitoringEnabled) BOOL batteryMonitoringEnabled;

@property (nonatomic, getter=isProximityMonitoringEnabled) BOOL proximityMonitoringEnabled;
@property (nonatomic, readonly) BOOL proximityState;

@property (nonatomic, readonly, getter=isMultitaskingSupported) BOOL multitaskingSupported;

@property (nonatomic, readonly) UIUserInterfaceIdiom userInterfaceIdiom;

@property (nonatomic, readonly, retain) NSUUID *identifierForVendor;

- (void)beginGeneratingDeviceOrientationNotifications;
- (void)endGeneratingDeviceOrientationNotifications;
@property (nonatomic, readonly) BOOL generatesDeviceOrientationNotifications;
@property (nonatomic, readonly) UIDeviceOrientation orientation;

- (void)playInputClick;

@end

UIKIT_EXPORT NSString *const UIDeviceBatteryStateDidChangeNotification;
UIKIT_EXPORT NSString *const UIDeviceBatteryLevelDidChangeNotification;
UIKIT_EXPORT NSString *const UIDeviceProximityStateDidChangeNotification;
UIKIT_EXPORT NSString *const UIDeviceOrientationDidChangeNotification;
