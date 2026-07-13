#import <UIKit/UIResponder.h>
#import <UIKit/UIKitExport.h>
#import <UIKit/UIEvent.h>

@class UIWindow;
@class UIViewController;
@class UIRemoteNotification;

@protocol UIApplicationDelegate;

typedef NSInteger UIApplicationState;

enum {
    UIApplicationStateActive,
    UIApplicationStateInactive,
    UIApplicationStateBackground,
};

typedef NSInteger UIBackgroundRefreshStatus;

enum {
    UIBackgroundRefreshStatusRestricted,
    UIBackgroundRefreshStatusDenied,
    UIBackgroundRefreshStatusAvailable,
};

typedef NSInteger UIRemoteNotificationType;

enum {
    UIRemoteNotificationTypeNone    = 0,
    UIRemoteNotificationTypeBadge   = 1 << 0,
    UIRemoteNotificationTypeSound   = 1 << 1,
    UIRemoteNotificationTypeAlert   = 1 << 2,
    UIRemoteNotificationTypeNewsstandContentAvailability = 1 << 3,
};

typedef NSInteger UIStatusBarStyle;

enum {
    UIStatusBarStyleDefault,
    UIStatusBarStyleLightContent,
    UIStatusBarStyleDarkContent,
};

typedef NSInteger UIStatusBarAnimation;

enum {
    UIStatusBarAnimationNone,
    UIStatusBarAnimationFade,
    UIStatusBarAnimationSlide,
};

typedef NSInteger UIInterfaceOrientation;

enum {
    UIInterfaceOrientationUnknown            = 0,
    UIInterfaceOrientationPortrait           = 1,
    UIInterfaceOrientationPortraitUpsideDown = 2,
    UIInterfaceOrientationLandscapeLeft      = 4,
    UIInterfaceOrientationLandscapeRight     = 3,
};

typedef NSInteger UIInterfaceOrientationMask;

enum {
    UIInterfaceOrientationMaskPortrait       = (1 << UIInterfaceOrientationPortrait),
    UIInterfaceOrientationMaskLandscapeLeft  = (1 << UIInterfaceOrientationLandscapeLeft),
    UIInterfaceOrientationMaskLandscapeRight = (1 << UIInterfaceOrientationLandscapeRight),
    UIInterfaceOrientationMaskPortraitUpsideDown = (1 << UIInterfaceOrientationPortraitUpsideDown),
    UIInterfaceOrientationMaskLandscape      = (UIInterfaceOrientationMaskLandscapeLeft | UIInterfaceOrientationMaskLandscapeRight),
    UIInterfaceOrientationMaskAll            = (UIInterfaceOrientationMaskPortrait | UIInterfaceOrientationMaskLandscapeLeft | UIInterfaceOrientationMaskLandscapeRight | UIInterfaceOrientationMaskPortraitUpsideDown),
    UIInterfaceOrientationMaskAllButUpsideDown = (UIInterfaceOrientationMaskPortrait | UIInterfaceOrientationMaskLandscapeLeft | UIInterfaceOrientationMaskLandscapeRight),
};

typedef NSInteger UIBackgroundTaskIdentifier;
typedef NSInteger UIBackgroundFetchResult;

enum {
    UIBackgroundFetchResultNewData,
    UIBackgroundFetchResultNoData,
    UIBackgroundFetchResultFailed,
};

typedef NSUInteger UIUserNotificationType;

enum {
    UIUserNotificationTypeNone    = 0,
    UIUserNotificationTypeBadge   = 1 << 0,
    UIUserNotificationTypeSound   = 1 << 1,
    UIUserNotificationTypeAlert   = 1 << 2,
};

@class UIUserNotificationSettings;
@class UILocalNotification;

@interface UIApplication : UIResponder {
    UIWindow *_keyWindow;
    NSMutableArray *_windows;
    id<UIApplicationDelegate> _delegate;
    UIApplicationState _applicationState;
    BOOL _idleTimerDisabled;
    BOOL _networkActivityIndicatorVisible;
    BOOL _statusBarHidden;
    UIStatusBarStyle _statusBarStyle;
    NSTimeInterval _backgroundTimeRemaining;
    UIBackgroundRefreshStatus _backgroundRefreshStatus;
    BOOL _applicationSupportsShakeToEdit;
    BOOL _isHandlingRemoteControlEvents;
}

+ (UIApplication *)sharedApplication;

@property (nonatomic, assign) id<UIApplicationDelegate> delegate;

- (void)sendEvent:(UIEvent *)event;

@property (nonatomic, readonly) UIWindow *keyWindow;
@property (nonatomic, readonly) NSArray *windows;

- (BOOL)sendAction:(SEL)action to:(id)target from:(id)sender forEvent:(UIEvent *)event;

@property (nonatomic, getter=isNetworkActivityIndicatorVisible) BOOL networkActivityIndicatorVisible;
@property (nonatomic) BOOL applicationSupportsShakeToEdit;

@property (nonatomic, readonly) UIApplicationState applicationState;

- (void)beginIgnoringInteractionEvents;
- (void)endIgnoringInteractionEvents;
- (BOOL)isIgnoringInteractionEvents;

@property (nonatomic, getter=isIdleTimerDisabled) BOOL idleTimerDisabled;

- (BOOL)openURL:(NSURL *)url;
- (BOOL)openURL:(NSURL *)url options:(NSDictionary *)options completionHandler:(void (^)(BOOL success))completion;
- (BOOL)canOpenURL:(NSURL *)url;

- (void)registerForRemoteNotificationTypes:(UIRemoteNotificationType)types;
- (UIRemoteNotificationType)enabledRemoteNotificationTypes;
- (void)unregisterForRemoteNotifications;

- (void)registerUserNotificationSettings:(UIUserNotificationSettings *)notificationSettings;
- (UIUserNotificationSettings *)currentUserNotificationSettings;

@property (nonatomic, readonly) NSTimeInterval backgroundTimeRemaining;

- (void)beginBackgroundTaskWithExpirationHandler:(void (^)(void))handler;
- (void)beginBackgroundTaskWithName:(NSString *)taskName expirationHandler:(void (^)(void))handler;
- (void)endBackgroundTask:(UIBackgroundTaskIdentifier)identifier;

@property (nonatomic, readonly) UIBackgroundRefreshStatus backgroundRefreshStatus;

- (void)setMinimumBackgroundFetchInterval:(NSTimeInterval)minimumBackgroundFetchInterval;

- (void)ignoreSnapshotOnNextAppLaunch;

- (void)setStatusBarHidden:(BOOL)hidden withAnimation:(UIStatusBarAnimation)animation;
@property (nonatomic, getter=isStatusBarHidden) BOOL statusBarHidden;
@property (nonatomic) UIStatusBarStyle statusBarStyle;

@property (nonatomic, readonly) UIInterfaceOrientation statusBarOrientation;

- (void)presentLocalNotificationNow:(UILocalNotification *)notification;
- (void)scheduleLocalNotification:(UILocalNotification *)notification;
- (void)cancelAllLocalNotifications;

@property (nonatomic, getter=isProtectedDataAvailable) BOOL protectedDataAvailable;

@property (nonatomic, readonly) UIUserInterfaceLayoutDirection userInterfaceLayoutDirection;

@property (nonatomic, strong) UIViewController *rootViewController;

- (void)beginReceivingRemoteControlEvents;
- (void)endReceivingRemoteControlEvents;

@property (nonatomic, readonly) BOOL isHandlingRemoteControlEvents;

@end

@protocol UIApplicationDelegate <NSObject>
@optional

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions;
- (void)applicationDidFinishLaunching:(UIApplication *)application;

- (void)applicationDidBecomeActive:(UIApplication *)application;
- (void)applicationWillResignActive:(UIApplication *)application;

- (void)applicationDidEnterBackground:(UIApplication *)application;
- (void)applicationWillEnterForeground:(UIApplication *)application;

- (void)applicationWillTerminate:(UIApplication *)application;

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application;

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation;
- (BOOL)application:(UIApplication *)app openURL:(NSURL *)url options:(NSDictionary *)options;

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken;
- (void)application:(UIApplication *)application didFailToRegisterForRemoteNotificationsWithError:(NSError *)error;
- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo;

- (void)application:(UIApplication *)application didRegisterUserNotificationSettings:(UIUserNotificationSettings *)notificationSettings;

- (void)application:(UIApplication *)application didReceiveLocalNotification:(UILocalNotification *)notification;

- (void)application:(UIApplication *)application handleActionWithIdentifier:(NSString *)identifier forLocalNotification:(UILocalNotification *)notification completionHandler:(void (^)(void))completionHandler;

- (void)application:(UIApplication *)application performFetchWithCompletionHandler:(void (^)(UIBackgroundFetchResult result))completionHandler;

- (void)application:(UIApplication *)application handleEventsForBackgroundURLSession:(NSString *)identifier completionHandler:(void (^)(void))completionHandler;

- (NSUInteger)application:(UIApplication *)application supportedInterfaceOrientationsForWindow:(UIWindow *)window;

@property (nonatomic, readonly, strong) UIWindow *window;

@end

@interface UILocalNotification : NSObject

@property (nonatomic, copy) NSDate *fireDate;
@property (nonatomic, copy) NSTimeZone *timeZone;
@property (nonatomic) NSCalendarUnit repeatInterval;
@property (nonatomic, copy) NSCalendar *repeatCalendar;
@property (nonatomic, copy) NSString *alertBody;
@property (nonatomic) BOOL hasAction;
@property (nonatomic, copy) NSString *alertAction;
@property (nonatomic, copy) NSString *alertLaunchImage;
@property (nonatomic, copy) NSString *alertTitle;
@property (nonatomic, copy) NSNumber *badge;
@property (nonatomic, copy) NSString *soundName;
@property (nonatomic, copy) NSDictionary *userInfo;
@property (nonatomic, copy) NSString *category;

@end

@interface UIUserNotificationSettings : NSObject {
    UIUserNotificationType _types;
    NSSet *_categories;
}

+ (UIUserNotificationSettings *)settingsForTypes:(UIUserNotificationType)types categories:(NSSet *)categories;

@property (nonatomic, readonly) UIUserNotificationType types;
@property (nonatomic, readonly, copy) NSSet *categories;

@end
