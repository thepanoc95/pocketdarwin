#import <UIKit/UIWindow.h>
#import <UIKit/UIApplication.h>
#import <UIKit/UIScreen.h>
#import <UIKit/UIViewController.h>
#import <UIKit/UIEvent.h>

const UIWindowLevel UIWindowLevelNormal    = 0.0;
const UIWindowLevel UIWindowLevelAlert     = 1000.0;
const UIWindowLevel UIWindowLevelStatusBar = 2000.0;

@implementation UIWindow

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        _screen = [UIScreen mainScreen];
        _windowLevel = UIWindowLevelNormal;
        _keyWindow = NO;
        _rootViewController = nil;
        [self setOpaque:NO];
        [self setBackgroundColor:[UIColor whiteColor]];
    }
    return self;
}

- (void)dealloc {
    [_rootViewController release];
    [_screen release];
    [super dealloc];
}

- (UIViewController *)rootViewController {
    return _rootViewController;
}

- (void)setRootViewController:(UIViewController *)controller {
    controller = [controller retain];
    [_rootViewController release];
    _rootViewController = controller;
    [[_rootViewController view] setFrame:[self bounds]];
    if ([_subviews count] == 0) {
        [self addSubview:[_rootViewController view]];
    }
}

- (UIScreen *)screen {
    return _screen;
}

- (void)setScreen:(UIScreen *)screen {
    screen = [screen retain];
    [_screen release];
    _screen = screen;
}

- (UIWindowLevel)windowLevel {
    return _windowLevel;
}

- (void)setWindowLevel:(UIWindowLevel)level {
    _windowLevel = level;
}

- (BOOL)isKeyWindow {
    return _keyWindow;
}

- (void)makeKeyWindow {
    if (!_keyWindow) {
        UIApplication *app = [UIApplication sharedApplication];
        UIWindow *oldKey = [app keyWindow];
        [oldKey resignKeyWindow];
        _keyWindow = YES;
        [app setKeyWindow:self];
        [self becomeKeyWindow];
    }
}

- (void)makeKeyAndVisible {
    [self setHidden:NO];
    [self makeKeyWindow];
}

- (void)resignKeyWindow {
    if (_keyWindow) {
        _keyWindow = NO;
    }
}

- (void)becomeKeyWindow {
}

- (CGPoint)convertPoint:(CGPoint)point toWindow:(UIWindow *)window {
    return point;
}

- (CGPoint)convertPoint:(CGPoint)point fromWindow:(UIWindow *)window {
    return point;
}

- (CGRect)convertRect:(CGRect)rect toWindow:(UIWindow *)window {
    return rect;
}

- (CGRect)convertRect:(CGRect)rect fromWindow:(UIWindow *)window {
    return rect;
}

- (void)sendEvent:(UIEvent *)event {
    [[UIApplication sharedApplication] sendEvent:event];
}

@end
