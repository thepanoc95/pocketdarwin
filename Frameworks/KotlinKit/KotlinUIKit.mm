#import <Foundation/Foundation.h>
#import "KotlinUIKit.h"
#import <UIKit/UIKit.h>

@implementation KotlinUIApplication

+ (KotlinUIApplication *)sharedInstance {
    UIApplication *shared = [UIApplication sharedApplication];
    KotlinUIApplication *kotlinApp = [[KotlinUIApplication alloc] init];
    return kotlinApp;
}

+ (void)runMain:(int)argc argv:(char **)argv principalClass:(NSString *)principalClassName delegateClass:(NSString *)delegateClassName {
    UIApplicationMain(argc, argv, principalClassName, delegateClassName);
}

- (NSArray *)windows {
    UIApplication *app = [UIApplication sharedApplication];
    return [app windows];
}

- (void)addWindow:(id)window {
    UIApplication *app = [UIApplication sharedApplication];
    if ([window isKindOfClass:[UIWindow class]]) {
        [app addWindow:(UIWindow *)window];
    }
}

- (void)terminate {
    [[[UIApplication sharedApplication] windows] makeObjectsPerformSelector:@selector(close)];
}

@end

@implementation KotlinUIView

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super init];
    if (self) {
        _x = frame.origin.x;
        _y = frame.origin.y;
        _width = frame.size.width;
        _height = frame.size.height;
    }
    return self;
}

- (void)addSubview:(KotlinUIView *)view {
    // Would add to underlying UIView
}

- (void)removeFromSuperview {
    // Would remove from underlying UIView
}

- (KotlinUIView *)viewWithTag:(NSInteger)tag {
    return nil; // Would search subviews for matching tag
}

- (void)setNeedsDisplay {
    // Would mark underlying UIView for redraw
}

- (void)setNeedsLayout {
    // Would mark underlying UIView for layout
}

@end

@implementation KotlinUIViewController

- (instancetype)init {
    self = [super init];
    if (self) {
        _view = nil;
    }
    return self;
}

- (void)viewDidLoad {
    // Override in subclass
}

- (void)setView:(KotlinUIView *)view {
    _view = [view retain];
}

- (KotlinUIView *)view {
    return _view;
}

- (void)dealloc {
    [_view release];
    [_title release];
    [super dealloc];
}

@end

@implementation KotlinUIColor

+ (instancetype)colorWithRed:(CGFloat)r green:(CGFloat)g blue:(CGFloat)b alpha:(CGFloat)a {
    return [[self alloc] initWithRed:r green:g blue:b alpha:a];
}

+ (instancetype)blackColor {
    return [[self alloc] initWithRed:0 green:0 blue:0 alpha:1];
}

+ (instancetype)whiteColor {
    return [[self alloc] initWithRed:1 green:1 blue:1 alpha:1];
}

+ (instancetype)clearColor {
    return [[self alloc] initWithRed:0 green:0 blue:0 alpha:0];
}

- (instancetype)initWithRed:(CGFloat)r green:(CGFloat)g blue:(CGFloat)b alpha:(CGFloat)a {
    self = [super init];
    if (self) {
        _red = r;
        _green = g;
        _blue = b;
        _alpha = a;
    }
    return self;
}

@end