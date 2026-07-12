#import <UIKit/UIEvent.h>
#import <UIKit/UIWindow.h>
#import <UIKit/UIView.h>
#import <UIKit/UIGestureRecognizer.h>

@implementation UITouch

- (instancetype)init {
    self = [super init];
    if (self) {
        _timestamp = [NSDate timeIntervalSinceReferenceDate];
        _phase = UITouchPhaseBegan;
        _tapCount = 1;
        _window = nil;
        _view = nil;
        _locationInWindow = CGPointZero;
        _previousLocationInWindow = CGPointZero;
    }
    return self;
}

- (NSTimeInterval)timestamp {
    return _timestamp;
}

- (UITouchPhase)phase {
    return _phase;
}

- (NSUInteger)tapCount {
    return _tapCount;
}

- (UIWindow *)window {
    return _window;
}

- (UIView *)view {
    return _view;
}

- (CGPoint)locationInView:(UIView *)view {
    if (!view)
        return _locationInWindow;
    return [view convertPoint:_locationInWindow fromView:nil];
}

- (CGPoint)previousLocationInView:(UIView *)view {
    if (!view)
        return _previousLocationInWindow;
    return [view convertPoint:_previousLocationInWindow fromView:nil];
}

- (CGFloat)majorRadius {
    return 0.0;
}

- (CGFloat)majorRadiusTolerance {
    return 0.0;
}

@end

@implementation UIEvent

- (instancetype)init {
    self = [super init];
    if (self) {
        _timestamp = [NSDate timeIntervalSinceReferenceDate];
        _type = UIEventTypeTouches;
        _subtype = UIEventSubtypeNone;
        _touches = [[NSSet alloc] init];
    }
    return self;
}

- (void)dealloc {
    [_touches release];
    [super dealloc];
}

- (UIEventType)type {
    return _type;
}

- (UIEventSubtype)subtype {
    return _subtype;
}

- (NSTimeInterval)timestamp {
    return _timestamp;
}

- (NSSet *)allTouches {
    return _touches;
}

- (NSSet *)touchesForWindow:(UIWindow *)window {
    NSMutableSet *result = [NSMutableSet set];
    for (UITouch *touch in _touches) {
        if ([touch window] == window)
            [result addObject:touch];
    }
    return result;
}

- (NSSet *)touchesForView:(UIView *)view {
    NSMutableSet *result = [NSMutableSet set];
    for (UITouch *touch in _touches) {
        if ([touch view] == view)
            [result addObject:touch];
    }
    return result;
}

- (NSSet *)touchesForGestureRecognizer:(UIGestureRecognizer *)gesture {
    return _touches;
}

@end

@implementation UIPressesEvent

- (NSSet *)allPresses {
    return [NSSet set];
}

@end
