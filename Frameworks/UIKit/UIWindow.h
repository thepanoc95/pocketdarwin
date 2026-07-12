#import <UIKit/UIView.h>
#import <UIKit/UIKitExport.h>
#import <CoreGraphics/CGGeometry.h>

@class UIViewController;
@class UIScreen;
@class UIEvent;

typedef CGFloat UIWindowLevel;

UIKIT_EXPORT const UIWindowLevel UIWindowLevelNormal;
UIKIT_EXPORT const UIWindowLevel UIWindowLevelAlert;
UIKIT_EXPORT const UIWindowLevel UIWindowLevelStatusBar;

@interface UIWindow : UIView {
    UIViewController *_rootViewController;
    UIScreen *_screen;
    UIWindowLevel _windowLevel;
    BOOL _keyWindow;
}

- (instancetype)initWithFrame:(CGRect)frame;

@property (nonatomic, retain) UIViewController *rootViewController;
@property (nonatomic, retain) UIScreen *screen;
@property (nonatomic) UIWindowLevel windowLevel;

- (BOOL)isKeyWindow;
- (void)makeKeyWindow;
- (void)makeKeyAndVisible;

- (void)resignKeyWindow;
- (void)becomeKeyWindow;

- (CGPoint)convertPoint:(CGPoint)point toWindow:(UIWindow *)window;
- (CGPoint)convertPoint:(CGPoint)point fromWindow:(UIWindow *)window;
- (CGRect)convertRect:(CGRect)rect toWindow:(UIWindow *)window;
- (CGRect)convertRect:(CGRect)rect fromWindow:(UIWindow *)window;

- (void)sendEvent:(UIEvent *)event;

@end
