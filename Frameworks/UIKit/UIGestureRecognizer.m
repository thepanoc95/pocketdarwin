#import <UIKit/UIGestureRecognizer.h>
#import <UIKit/UIView.h>
#import <UIKit/UIEvent.h>

@implementation UIGestureRecognizer

- (instancetype)initWithTarget:(id)target action:(SEL)action {
    self = [super init];
    if (self) {
        _target = target;
        _action = action;
        _state = UIGestureRecognizerStatePossible;
        _enabled = YES;
        _cancelsTouchesInView = YES;
        _delaysTouchesBegan = NO;
        _delaysTouchesEnded = YES;
        _touches = [[NSMutableSet alloc] init];
        _delayedTouches = nil;
        _recognizersToWaitFor = [[NSMutableArray alloc] init];
        _recognizersToFailFor = [[NSMutableArray alloc] init];
        _numberOfTouchesRequired = 1;
        _requiresExclusiveTouchType = NO;
    }
    return self;
}

- (void)dealloc {
    [_touches release];
    [_delayedTouches release];
    [_recognizersToWaitFor release];
    [_recognizersToFailFor release];
    [super dealloc];
}

- (UIGestureRecognizerState)state {
    return _state;
}

- (void)setState:(UIGestureRecognizerState)state {
    _state = state;
    if (state == UIGestureRecognizerStateRecognized) {
        if (_target && _action)
            [_target performSelector:_action withObject:self];
    }
}

- (BOOL)isEnabled {
    return _enabled;
}

- (void)setEnabled:(BOOL)enabled {
    _enabled = enabled;
    if (!enabled)
        _state = UIGestureRecognizerStatePossible;
}

- (UIView *)view {
    return _view;
}

- (void)_setView:(UIView *)view {
    _view = view;
}

- (void)addTarget:(id)target action:(SEL)action {
    _target = target;
    _action = action;
}

- (void)removeTarget:(id)target action:(SEL)action {
    if (_target == target && _action == action) {
        _target = nil;
        _action = NULL;
    }
}

- (BOOL)cancelsTouchesInView {
    return _cancelsTouchesInView;
}

- (void)setCancelsTouchesInView:(BOOL)cancels {
    _cancelsTouchesInView = cancels;
}

- (BOOL)delaysTouchesBegan {
    return _delaysTouchesBegan;
}

- (void)setDelaysTouchesBegan:(BOOL)delays {
    _delaysTouchesBegan = delays;
}

- (BOOL)delaysTouchesEnded {
    return _delaysTouchesEnded;
}

- (void)setDelaysTouchesEnded:(BOOL)delays {
    _delaysTouchesEnded = delays;
}

- (NSArray *)allowedTouchTypes {
    return nil;
}

- (void)setAllowedTouchTypes:(NSArray *)types {
}

- (void)requireGestureRecognizerToFail:(UIGestureRecognizer *)otherGestureRecognizer {
    [_recognizersToFailFor addObject:otherGestureRecognizer];
}

- (NSUInteger)numberOfTouches {
    return [_touches count];
}

- (CGPoint)locationInView:(UIView *)view {
    return CGPointZero;
}

- (CGPoint)locationOfTouch:(NSUInteger)touchIndex inView:(UIView *)view {
    return CGPointZero;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    if (!_enabled)
        return;
    _state = UIGestureRecognizerStateBegan;
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    if (!_enabled)
        return;
    _state = UIGestureRecognizerStateChanged;
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    if (!_enabled)
        return;
    _state = UIGestureRecognizerStateEnded;
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
    if (!_enabled)
        return;
    _state = UIGestureRecognizerStateCancelled;
}

- (void)pressesBegan:(NSSet *)presses withEvent:(UIPressesEvent *)event {
}

- (void)pressesChanged:(NSSet *)presses withEvent:(UIPressesEvent *)event {
}

- (void)pressesEnded:(NSSet *)presses withEvent:(UIPressesEvent *)event {
}

- (void)pressesCancelled:(NSSet *)presses withEvent:(UIPressesEvent *)event {
}

- (void)ignoreTouch:(UITouch *)touch forEvent:(UIEvent *)event {
}

- (void)ignorePress:(UIPress *)press forEvent:(UIPressesEvent *)event {
}

- (void)reset {
    _state = UIGestureRecognizerStatePossible;
    [_touches removeAllObjects];
}

- (BOOL)requiresExclusiveTouchType {
    return _requiresExclusiveTouchType;
}

- (void)setRequiresExclusiveTouchType:(BOOL)requires {
    _requiresExclusiveTouchType = requires;
}

@end

@implementation UITapGestureRecognizer

- (instancetype)initWithTarget:(id)target action:(SEL)action {
    self = [super initWithTarget:target action:action];
    if (self) {
        _numberOfTapsRequired = 1;
        _numberOfTouchesRequired = 1;
    }
    return self;
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    [self setState:UIGestureRecognizerStateRecognized];
}

- (void)reset {
    [super reset];
}

@end

@implementation UIPinchGestureRecognizer

- (instancetype)initWithTarget:(id)target action:(SEL)action {
    self = [super initWithTarget:target action:action];
    if (self) {
        _scale = 1.0;
    }
    return self;
}

@end

@implementation UIRotationGestureRecognizer

- (instancetype)initWithTarget:(id)target action:(SEL)action {
    self = [super initWithTarget:target action:action];
    if (self) {
        _rotation = 0;
    }
    return self;
}

@end

@implementation UISwipeGestureRecognizer

- (instancetype)initWithTarget:(id)target action:(SEL)action {
    self = [super initWithTarget:target action:action];
    if (self) {
        _direction = UISwipeGestureRecognizerDirectionRight;
        _numberOfTouchesRequired = 1;
    }
    return self;
}

@end

@implementation UIPanGestureRecognizer

- (instancetype)initWithTarget:(id)target action:(SEL)action {
    self = [super initWithTarget:target action:action];
    if (self) {
        _minimumNumberOfTouches = 1;
        _maximumNumberOfTouches = NSUIntegerMax;
    }
    return self;
}

- (CGPoint)translationInView:(UIView *)view {
    return CGPointZero;
}

- (void)setTranslation:(CGPoint)translation inView:(UIView *)view {
}

- (CGPoint)velocityInView:(UIView *)view {
    return CGPointZero;
}

@end

@implementation UIScreenEdgePanGestureRecognizer

- (instancetype)initWithTarget:(id)target action:(SEL)action {
    self = [super initWithTarget:target action:action];
    if (self) {
        _edges = UIRectEdgeNone;
    }
    return self;
}

@end

@implementation UILongPressGestureRecognizer

- (instancetype)initWithTarget:(id)target action:(SEL)action {
    self = [super initWithTarget:target action:action];
    if (self) {
        _numberOfTapsRequired = 0;
        _numberOfTouchesRequired = 1;
        _minimumPressDuration = 0.5;
        _allowableMovement = 10;
    }
    return self;
}

@end
