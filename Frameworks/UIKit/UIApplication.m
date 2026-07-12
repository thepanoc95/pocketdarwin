#import <UIKit/UIApplication.h>
#import <UIKit/UIWindow.h>
#import <UIKit/UIViewController.h>

static UIApplication *sharedApplication = nil;

@implementation UIApplication

+ (UIApplication *)sharedApplication {
    if (!sharedApplication)
        sharedApplication = [[self alloc] init];
    return sharedApplication;
}

+ (void)initialize {
    if (self == [UIApplication class]) {
        sharedApplication = nil;
    }
}

- (instancetype)init {
    self = [super init];
    if (self) {
        _windows = [[NSMutableArray alloc] init];
        _delegate = nil;
        _applicationState = UIApplicationStateInactive;
        _idleTimerDisabled = NO;
        _networkActivityIndicatorVisible = NO;
        _statusBarHidden = NO;
        _statusBarStyle = UIStatusBarStyleDefault;
        _backgroundTimeRemaining = 0;
        _backgroundRefreshStatus = UIBackgroundRefreshStatusAvailable;
        _applicationSupportsShakeToEdit = NO;
        _isHandlingRemoteControlEvents = NO;
    }
    return self;
}

- (void)dealloc {
    [_windows release];
    [_keyWindow release];
    [super dealloc];
}

- (void)setDelegate:(id<UIApplicationDelegate>)delegate {
    _delegate = delegate;
}

- (id<UIApplicationDelegate>)delegate {
    return _delegate;
}

- (void)sendEvent:(UIEvent *)event {
    if (event.type == UIEventTypeTouches) {
        NSSet *touches = [event allTouches];
        UITouch *touch = [touches anyObject];
        UIView *view = [touch view];

        if (view) {
            UIResponder *responder = view;
            while (responder) {
                if ([responder respondsToSelector:@selector(touchesBegan:withEvent:)]) {
                    [responder touchesBegan:touches withEvent:event];
                    break;
                }
                responder = [responder nextResponder];
            }
        }
    }
}

- (UIWindow *)keyWindow {
    return _keyWindow;
}

- (void)setKeyWindow:(UIWindow *)window {
    window = [window retain];
    [_keyWindow release];
    _keyWindow = window;
}

- (NSArray *)windows {
    return _windows;
}

- (BOOL)sendAction:(SEL)action to:(id)target from:(id)sender forEvent:(UIEvent *)event {
    if (target)
        return [target performSelector:action withObject:sender];
    return [[self keyWindow] performSelector:action withObject:sender];
}

- (BOOL)isNetworkActivityIndicatorVisible {
    return _networkActivityIndicatorVisible;
}

- (void)setNetworkActivityIndicatorVisible:(BOOL)visible {
    _networkActivityIndicatorVisible = visible;
}

- (BOOL)applicationSupportsShakeToEdit {
    return _applicationSupportsShakeToEdit;
}

- (void)setApplicationSupportsShakeToEdit:(BOOL)supports {
    _applicationSupportsShakeToEdit = supports;
}

- (UIApplicationState)applicationState {
    return _applicationState;
}

- (void)beginIgnoringInteractionEvents {
}

- (void)endIgnoringInteractionEvents {
}

- (BOOL)isIgnoringInteractionEvents {
    return NO;
}

- (BOOL)isIdleTimerDisabled {
    return _idleTimerDisabled;
}

- (void)setIdleTimerDisabled:(BOOL)disabled {
    _idleTimerDisabled = disabled;
}

- (BOOL)openURL:(NSURL *)url {
    return NO;
}

- (BOOL)openURL:(NSURL *)url options:(NSDictionary *)options completionHandler:(void (^)(BOOL))completion {
    if (completion)
        completion(NO);
    return NO;
}

- (BOOL)canOpenURL:(NSURL *)url {
    return NO;
}

- (void)registerForRemoteNotificationTypes:(UIRemoteNotificationType)types {
}

- (UIRemoteNotificationType)enabledRemoteNotificationTypes {
    return UIRemoteNotificationTypeNone;
}

- (void)unregisterForRemoteNotifications {
}

- (void)registerUserNotificationSettings:(UIUserNotificationSettings *)settings {
}

- (UIUserNotificationSettings *)currentUserNotificationSettings {
    return nil;
}

- (NSTimeInterval)backgroundTimeRemaining {
    return _backgroundTimeRemaining;
}

- (void)beginBackgroundTaskWithExpirationHandler:(void (^)(void))handler {
}

- (void)beginBackgroundTaskWithName:(NSString *)taskName expirationHandler:(void (^)(void))handler {
}

- (void)endBackgroundTask:(UIBackgroundTaskIdentifier)identifier {
}

- (UIBackgroundRefreshStatus)backgroundRefreshStatus {
    return _backgroundRefreshStatus;
}

- (void)setMinimumBackgroundFetchInterval:(NSTimeInterval)interval {
}

- (void)ignoreSnapshotOnNextAppLaunch {
}

- (void)setStatusBarHidden:(BOOL)hidden withAnimation:(UIStatusBarAnimation)animation {
    _statusBarHidden = hidden;
}

- (BOOL)isStatusBarHidden {
    return _statusBarHidden;
}

- (void)setStatusBarHidden:(BOOL)hidden {
    _statusBarHidden = hidden;
}

- (UIStatusBarStyle)statusBarStyle {
    return _statusBarStyle;
}

- (void)setStatusBarStyle:(UIStatusBarStyle)style {
    _statusBarStyle = style;
}

- (UIInterfaceOrientation)statusBarOrientation {
    return UIInterfaceOrientationPortrait;
}

- (void)presentLocalNotificationNow:(UILocalNotification *)notification {
}

- (void)scheduleLocalNotification:(UILocalNotification *)notification {
}

- (void)cancelAllLocalNotifications {
}

- (BOOL)isProtectedDataAvailable {
    return YES;
}

- (UIUserInterfaceLayoutDirection)userInterfaceLayoutDirection {
    return UIUserInterfaceLayoutDirectionLeftToRight;
}

- (void)beginReceivingRemoteControlEvents {
    _isHandlingRemoteControlEvents = YES;
}

- (void)endReceivingRemoteControlEvents {
    _isHandlingRemoteControlEvents = NO;
}

- (BOOL)isHandlingRemoteControlEvents {
    return _isHandlingRemoteControlEvents;
}

@end

@implementation UILocalNotification

- (instancetype)init {
    self = [super init];
    if (self) {
    }
    return self;
}

- (void)dealloc {
    [_fireDate release];
    [_timeZone release];
    [_repeatCalendar release];
    [_alertBody release];
    [_alertAction release];
    [_alertLaunchImage release];
    [_alertTitle release];
    [_badge release];
    [_soundName release];
    [_userInfo release];
    [_category release];
    [super dealloc];
}

@end

@implementation UIUserNotificationSettings

+ (UIUserNotificationSettings *)settingsForTypes:(UIUserNotificationType)types categories:(NSSet *)categories {
    UIUserNotificationSettings *settings = [[self alloc] init];
    settings->_types = types;
    settings->_categories = [categories copy];
    return [settings autorelease];
}

- (UIUserNotificationType)types {
    return _types;
}

- (NSSet *)categories {
    return _categories;
}

- (void)dealloc {
    [_categories release];
    [super dealloc];
}

@end
