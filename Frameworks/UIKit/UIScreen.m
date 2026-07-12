#import <UIKit/UIScreen.h>

static UIScreen *mainScreen = nil;

@implementation UIScreen

+ (UIScreen *)mainScreen {
    if (!mainScreen)
        mainScreen = [[self alloc] init];
    return mainScreen;
}

+ (NSArray *)screens {
    return @[[self mainScreen]];
}

+ (CGFloat)scale {
    return [[self mainScreen] scale];
}

- (instancetype)init {
    self = [super init];
    if (self) {
        _bounds = CGRectMake(0, 0, 320, 480);
        _applicationFrame = CGRectMake(0, 20, 320, 460);
        _scale = 2.0;
        _brightness = 0.5;
        _wantsSoftwareDimming = NO;
        _maximumFramesPerSecond = 60;
    }
    return self;
}

- (CGRect)bounds {
    return _bounds;
}

- (CGRect)applicationFrame {
    return _applicationFrame;
}

- (CGFloat)scale {
    return _scale;
}

- (CGFloat)nativeScale {
    return _scale;
}

- (CGSize)nativeBounds {
    return CGSizeMake(_bounds.size.width * _scale, _bounds.size.height * _scale);
}

- (CGFloat)maximumFramesPerSecond {
    return _maximumFramesPerSecond;
}

- (CGFloat)brightness {
    return _brightness;
}

- (void)setBrightness:(CGFloat)brightness {
    _brightness = brightness;
}

- (BOOL)wantsSoftwareDimming {
    return _wantsSoftwareDimming;
}

- (void)setWantsSoftwareDimming:(BOOL)wants {
    _wantsSoftwareDimming = wants;
}

- (void)dealloc {
    [super dealloc];
}

@end
