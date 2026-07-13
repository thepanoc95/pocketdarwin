#import <UIKit/UIView.h>
#import <UIKit/UIColor.h>
#import <UIKit/UIGestureRecognizer.h>
#import <UIKit/UIViewController.h>
#import <UIKit/UIWindow.h>
#import <Foundation/NSKeyedArchiver.h>
#import <Foundation/NSException.h>

typedef NSInteger NSWindowOrderingMode;
enum {
    NSWindowBelow = -1,
    NSWindowAbove = 1,
};

static BOOL UIViewAnimationsEnabled = YES;
static NSMutableDictionary *UIViewAnimationContext = nil;

@interface UIView ()
- (void)_setWindow:(UIWindow *)window;
@end

@implementation UIView

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super init];
    if (self) {
        _frame = frame;
        _bounds = CGRectMake(0, 0, frame.size.width, frame.size.height);
        _center = CGPointMake(CGRectGetMidX(frame), CGRectGetMidY(frame));
        _transform = CGAffineTransformIdentity;
        _alpha = 1.0;
        _hidden = NO;
        _clipsToBounds = NO;
        _clearsContextBeforeDrawing = YES;
        _autoresizesSubviews = YES;
        _autoresizingMask = UIViewAutoresizingNone;
        _contentMode = UIViewContentModeScaleToFill;
        _backgroundColor = nil;
        _superview = nil;
        _subviews = [[NSMutableArray alloc] init];
        _window = nil;
        _tag = 0;
        _userInteractionEnabled = YES;
        _multipleTouchEnabled = NO;
        _exclusiveTouch = NO;
        _gestureRecognizers = [[NSMutableArray alloc] init];
        _layoutMargins = UIEdgeInsetsMake(8, 8, 8, 8);
        _preservesSuperviewLayoutMargins = NO;
        _cornerRadius = 0;
        _masksToBounds = NO;
        _borderWidth = 0;
        _borderColor = nil;
    }
    return self;
}

- (instancetype)init {
    return [self initWithFrame:CGRectZero];
}

- (void)dealloc {
    [_subviews release];
    [_backgroundColor release];
    [_gestureRecognizers release];
    [_tintColor release];
    [_borderColor release];
    [super dealloc];
}

- (CGRect)frame {
    return _frame;
}

- (void)setFrame:(CGRect)frame {
    _frame = frame;
    _bounds.size = frame.size;
    _center = CGPointMake(CGRectGetMidX(frame), CGRectGetMidY(frame));
    [self setNeedsLayout];
}

- (CGRect)bounds {
    return _bounds;
}

- (void)setBounds:(CGRect)bounds {
    _bounds = bounds;
    [self setNeedsLayout];
}

- (CGPoint)center {
    return _center;
}

- (void)setCenter:(CGPoint)center {
    _center = center;
    _frame.origin.x = center.x - _frame.size.width / 2;
    _frame.origin.y = center.y - _frame.size.height / 2;
    [self setNeedsLayout];
}

- (CGAffineTransform)transform {
    return _transform;
}

- (void)setTransform:(CGAffineTransform)transform {
    _transform = transform;
}

- (UIView *)superview {
    return _superview;
}

- (NSArray *)subviews {
    return [[_subviews copy] autorelease];
}

- (UIWindow *)window {
    return _window;
}

- (CGFloat)alpha {
    return _alpha;
}

- (void)setAlpha:(CGFloat)alpha {
    _alpha = alpha;
}

- (BOOL)isHidden {
    return _hidden;
}

- (void)setHidden:(BOOL)hidden {
    _hidden = hidden;
}

- (BOOL)clipsToBounds {
    return _clipsToBounds;
}

- (void)setClipsToBounds:(BOOL)clipsToBounds {
    _clipsToBounds = clipsToBounds;
}

- (BOOL)clearsContextBeforeDrawing {
    return _clearsContextBeforeDrawing;
}

- (void)setClearsContextBeforeDrawing:(BOOL)clears {
    _clearsContextBeforeDrawing = clears;
}

- (BOOL)autoresizesSubviews {
    return _autoresizesSubviews;
}

- (void)setAutoresizesSubviews:(BOOL)autoresizes {
    _autoresizesSubviews = autoresizes;
}

- (UIViewAutoresizing)autoresizingMask {
    return _autoresizingMask;
}

- (void)setAutoresizingMask:(UIViewAutoresizing)mask {
    _autoresizingMask = mask;
}

- (CGRect)contentStretch {
    return CGRectMake(0, 0, 1, 1);
}

- (void)setContentStretch:(CGRect)stretch {
}

- (UIViewContentMode)contentMode {
    return _contentMode;
}

- (void)setContentMode:(UIViewContentMode)mode {
    _contentMode = mode;
}

- (BOOL)isUserInteractionEnabled {
    return _userInteractionEnabled;
}

- (void)setUserInteractionEnabled:(BOOL)enabled {
    _userInteractionEnabled = enabled;
}

- (BOOL)isMultipleTouchEnabled {
    return _multipleTouchEnabled;
}

- (void)setMultipleTouchEnabled:(BOOL)enabled {
    _multipleTouchEnabled = enabled;
}

- (BOOL)isExclusiveTouch {
    return _exclusiveTouch;
}

- (void)setExclusiveTouch:(BOOL)exclusive {
    _exclusiveTouch = exclusive;
}

- (UIColor *)backgroundColor {
    return _backgroundColor;
}

- (void)setBackgroundColor:(UIColor *)color {
    color = [color retain];
    [_backgroundColor release];
    _backgroundColor = color;
    [self setNeedsDisplay];
}

- (UIColor *)tintColor {
    if (_tintColor)
        return _tintColor;
    return [_superview tintColor];
}

- (void)setTintColor:(UIColor *)color {
    color = [color retain];
    [_tintColor release];
    _tintColor = color;
}

- (UIViewTintAdjustmentMode)tintAdjustmentMode {
    return UIViewTintAdjustmentModeAutomatic;
}

- (void)setTintAdjustmentMode:(UIViewTintAdjustmentMode)mode {
}

- (NSInteger)tag {
    return _tag;
}

- (void)setTag:(NSInteger)tag {
    _tag = tag;
}

- (NSArray *)gestureRecognizers {
    return _gestureRecognizers;
}

- (void)setGestureRecognizers:(NSArray *)gestureRecognizers {
    gestureRecognizers = [gestureRecognizers copy];
    [_gestureRecognizers release];
    _gestureRecognizers = [gestureRecognizers mutableCopy];
    [gestureRecognizers release];
}

- (void)addGestureRecognizer:(UIGestureRecognizer *)gestureRecognizer {
    [_gestureRecognizers addObject:gestureRecognizer];
    [gestureRecognizer _setView:self];
}

- (void)removeGestureRecognizer:(UIGestureRecognizer *)gestureRecognizer {
    [_gestureRecognizers removeObject:gestureRecognizer];
    [gestureRecognizer _setView:nil];
}

- (UIEdgeInsets)layoutMargins {
    return _layoutMargins;
}

- (void)setLayoutMargins:(UIEdgeInsets)layoutMargins {
    _layoutMargins = layoutMargins;
}

- (BOOL)preservesSuperviewLayoutMargins {
    return _preservesSuperviewLayoutMargins;
}

- (void)setPreservesSuperviewLayoutMargins:(BOOL)preserves {
    _preservesSuperviewLayoutMargins = preserves;
}

- (CGFloat)cornerRadius {
    return _cornerRadius;
}

- (void)setCornerRadius:(CGFloat)cornerRadius {
    _cornerRadius = cornerRadius;
}

- (BOOL)masksToBounds {
    return _masksToBounds;
}

- (void)setMasksToBounds:(BOOL)masksToBounds {
    _masksToBounds = masksToBounds;
}

- (CGFloat)borderWidth {
    return _borderWidth;
}

- (void)setBorderWidth:(CGFloat)borderWidth {
    _borderWidth = borderWidth;
}

- (UIColor *)borderColor {
    return _borderColor;
}

- (void)setBorderColor:(UIColor *)borderColor {
    borderColor = [borderColor retain];
    [_borderColor release];
    _borderColor = borderColor;
}

- (void)addSubview:(UIView *)view {
    [self addSubview:view positioned:NSWindowAbove relativeTo:nil];
}

- (void)addSubview:(UIView *)view positioned:(NSWindowOrderingMode)ordering relativeTo:(UIView *)relativeTo {
    if (view->_superview) {
        [view removeFromSuperview];
    }
    [view willMoveToSuperview:self];
    [view willMoveToWindow:[self window]];

    if (ordering == NSWindowAbove && relativeTo == nil) {
        [_subviews addObject:view];
    } else if (ordering == NSWindowBelow && relativeTo == nil) {
        [_subviews insertObject:view atIndex:0];
    } else if (relativeTo) {
        NSUInteger index = [_subviews indexOfObject:relativeTo];
        if (ordering == NSWindowAbove) {
            [_subviews insertObject:view atIndex:index + 1];
        } else {
            [_subviews insertObject:view atIndex:index];
        }
    }

    view->_superview = self;
    [view _setWindow:[self window]];

    [view didMoveToSuperview];
    [view didMoveToWindow];
    [self didAddSubview:view];
    [self setNeedsLayout];
}

- (void)insertSubview:(UIView *)view atIndex:(NSInteger)index {
    if (view->_superview) {
        [view removeFromSuperview];
    }
    [view willMoveToSuperview:self];
    [_subviews insertObject:view atIndex:index];
    view->_superview = self;
    [view didMoveToSuperview];
    [self didAddSubview:view];
    [self setNeedsLayout];
}

- (void)insertSubview:(UIView *)view aboveSubview:(UIView *)siblingSubview {
    [self addSubview:view positioned:NSWindowAbove relativeTo:siblingSubview];
}

- (void)insertSubview:(UIView *)view belowSubview:(UIView *)siblingSubview {
    [self addSubview:view positioned:NSWindowBelow relativeTo:siblingSubview];
}

- (void)bringSubviewToFront:(UIView *)view {
    if ([_subviews containsObject:view]) {
        [view retain];
        [_subviews removeObject:view];
        [_subviews addObject:view];
        [view release];
    }
}

- (void)sendSubviewToBack:(UIView *)view {
    if ([_subviews containsObject:view]) {
        [view retain];
        [_subviews removeObject:view];
        [_subviews insertObject:view atIndex:0];
        [view release];
    }
}

- (void)removeFromSuperview {
    UIView *oldSuperview = _superview;
    if (oldSuperview) {
        [self willMoveToSuperview:nil];
        [self willMoveToWindow:nil];
        [_superview willRemoveSubview:self];
        [_superview->_subviews removeObject:self];
        _superview = nil;
        [self _setWindow:nil];
        [self didMoveToSuperview];
        [self didMoveToWindow];
        [oldSuperview setNeedsLayout];
    }
}

- (void)didAddSubview:(UIView *)subview {
}

- (void)willRemoveSubview:(UIView *)subview {
}

- (void)willMoveToSuperview:(UIView *)newSuperview {
}

- (void)didMoveToSuperview {
}

- (void)willMoveToWindow:(UIWindow *)newWindow {
}

- (void)didMoveToWindow {
}

- (BOOL)isDescendantOfView:(UIView *)view {
    UIView *current = self;
    while (current) {
        if (current == view)
            return YES;
        current = [current superview];
    }
    return NO;
}

- (UIView *)viewWithTag:(NSInteger)tag {
    if (_tag == tag)
        return self;
    for (UIView *subview in _subviews) {
        UIView *result = [subview viewWithTag:tag];
        if (result)
            return result;
    }
    return nil;
}

- (void)setNeedsLayout {
}

- (void)layoutIfNeeded {
    [self layoutSubviews];
}

- (void)layoutSubviews {
}

- (void)setNeedsDisplay {
}

- (void)setNeedsDisplayInRect:(CGRect)rect {
}

- (void)drawRect:(CGRect)rect {
}

- (CGSize)sizeThatFits:(CGSize)size {
    return _frame.size;
}

- (void)sizeToFit {
    CGSize size = [self sizeThatFits:_frame.size];
    _frame.size = size;
    _bounds.size = size;
}

- (CGRect)convertRect:(CGRect)rect toView:(UIView *)view {
    if (view == self)
        return rect;
    CGPoint origin = [self convertPoint:rect.origin toView:view];
    return CGRectMake(origin.x, origin.y, rect.size.width, rect.size.height);
}

- (CGRect)convertRect:(CGRect)rect fromView:(UIView *)view {
    if (view == self)
        return rect;
    CGPoint origin = [self convertPoint:rect.origin fromView:view];
    return CGRectMake(origin.x, origin.y, rect.size.width, rect.size.height);
}

- (CGPoint)convertPoint:(CGPoint)point toView:(UIView *)view {
    if (view == self)
        return point;

    CGFloat x = point.x;
    CGFloat y = point.y;

    UIView *current = self;
    while (current && current != view) {
        x += current->_frame.origin.x;
        y += current->_frame.origin.y;
        current = [current superview];
    }

    if (current == nil && view != nil) {
        CGPoint viewOrigin = [view convertPoint:CGPointZero toView:self];
        x -= viewOrigin.x;
        y -= viewOrigin.y;
    }

    return CGPointMake(x, y);
}

- (CGPoint)convertPoint:(CGPoint)point fromView:(UIView *)view {
    if (view == self)
        return point;

    if (view == nil)
        return [self convertPoint:point toView:nil];

    return [view convertPoint:point toView:self];
}

- (UIEdgeInsets)safeAreaInsets {
    return UIEdgeInsetsZero;
}

- (void)safeAreaInsetsDidChange {
}

- (UIViewController *)viewController {
    UIResponder *responder = self;
    while (responder) {
        if ([responder isKindOfClass:[UIViewController class]])
            return (UIViewController *)responder;
        responder = [responder nextResponder];
    }
    return nil;
}

- (void)_setWindow:(UIWindow *)window {
    _window = window;
    for (UIView *subview in _subviews) {
        [subview _setWindow:window];
    }
}

- (NSArray *)constraints {
    return @[];
}

- (void)addConstraint:(NSLayoutConstraint *)constraint {
}

- (void)addConstraints:(NSArray *)constraints {
}

- (void)removeConstraint:(NSLayoutConstraint *)constraint {
}

- (void)removeConstraints:(NSArray *)constraints {
}

- (BOOL)translatesAutoresizingMaskIntoConstraints {
    return _translatesAutoresizingMaskIntoConstraints;
}

- (void)setTranslatesAutoresizingMaskIntoConstraints:(BOOL)flag {
    _translatesAutoresizingMaskIntoConstraints = flag;
}

- (BOOL)needsUpdateConstraints {
    return NO;
}

- (void)setNeedsUpdateConstraints {
}

- (void)updateConstraints {
}

- (void)invalidateIntrinsicContentSize {
}

- (CGSize)intrinsicContentSize {
    return _frame.size;
}

- (CGFloat)scale {
    return [[UIScreen mainScreen] scale];
}

- (BOOL)hasAmbiguousLayout {
    return NO;
}

- (UIView *)exerciseAmbiguityInLayout {
    return nil;
}

- (NSString *)accessibilityIdentifier {
    return nil;
}

- (void)setAccessibilityIdentifier:(NSString *)identifier {
}

- (NSString *)accessibilityLabel {
    return nil;
}

- (void)setAccessibilityLabel:(NSString *)label {
}

- (NSString *)accessibilityHint {
    return nil;
}

- (void)setAccessibilityHint:(NSString *)hint {
}

- (NSString *)accessibilityValue {
    return nil;
}

- (void)setAccessibilityValue:(NSString *)value {
}

- (UIAccessibilityTraits)accessibilityTraits {
    return 0;
}

- (void)setAccessibilityTraits:(UIAccessibilityTraits)traits {
}

- (CGRect)accessibilityFrame {
    return CGRectZero;
}

- (void)setAccessibilityFrame:(CGRect)frame {
}

- (BOOL)isAccessibilityElement {
    return NO;
}

- (void)setIsAccessibilityElement:(BOOL)element {
}

- (void)encodeWithCoder:(NSCoder *)coder {
}

- (instancetype)initWithCoder:(NSCoder *)coder {
    return [self initWithFrame:CGRectZero];
}

@end

@implementation UIView (UIViewAnimationWithBlocks)

static NSTimeInterval _animationDuration = 0.0;
static NSTimeInterval _animationDelay = 0.0;
static UIViewAnimationOptions _animationOptions = 0;
static CGFloat _animationDampingRatio = 0.0;
static CGFloat _animationSpringVelocity = 0.0;
static BOOL _animating = NO;

+ (void)animateWithDuration:(NSTimeInterval)duration animations:(void (^)(void))animations {
    [self animateWithDuration:duration delay:0 options:0 animations:animations completion:nil];
}

+ (void)animateWithDuration:(NSTimeInterval)duration animations:(void (^)(void))animations completion:(void (^)(BOOL finished))completion {
    [self animateWithDuration:duration delay:0 options:0 animations:animations completion:completion];
}

+ (void)animateWithDuration:(NSTimeInterval)duration delay:(NSTimeInterval)delay options:(UIViewAnimationOptions)options animations:(void (^)(void))animations completion:(void (^)(BOOL finished))completion {
    if (animations) {
        _animationDuration = duration;
        _animationDelay = delay;
        _animationOptions = options;
        _animating = YES;
        animations();
        _animating = NO;
        _animationDuration = 0;
        _animationDelay = 0;
        _animationOptions = 0;
        if (completion)
            completion(YES);
    }
}

+ (void)animateWithDuration:(NSTimeInterval)duration delay:(NSTimeInterval)delay usingSpringWithDamping:(CGFloat)dampingRatio initialSpringVelocity:(CGFloat)velocity options:(UIViewAnimationOptions)options animations:(void (^)(void))animations completion:(void (^)(BOOL finished))completion {
    _animationDampingRatio = dampingRatio;
    _animationSpringVelocity = velocity;
    [self animateWithDuration:duration delay:delay options:options animations:animations completion:completion];
    _animationDampingRatio = 0;
    _animationSpringVelocity = 0;
}

+ (void)transitionWithView:(UIView *)view duration:(NSTimeInterval)duration options:(UIViewAnimationOptions)options animations:(void (^)(void))animations completion:(void (^)(BOOL finished))completion {
    if (animations)
        animations();
    if (completion)
        completion(YES);
}

+ (void)transitionFromView:(UIView *)fromView toView:(UIView *)toView duration:(NSTimeInterval)duration options:(UIViewAnimationOptions)options completion:(void (^)(BOOL finished))completion {
    UIView *superview = [fromView superview];
    if (superview) {
        [fromView removeFromSuperview];
        [superview addSubview:toView];
    }
    if (completion)
        completion(YES);
}

+ (void)performWithoutAnimation:(void (^)(void))actionsWithoutAnimation {
    BOOL saved = UIViewAnimationsEnabled;
    UIViewAnimationsEnabled = NO;
    if (actionsWithoutAnimation)
        actionsWithoutAnimation();
    UIViewAnimationsEnabled = saved;
}

+ (void)animateKeyframesWithDuration:(NSTimeInterval)duration delay:(NSTimeInterval)delay options:(UIViewKeyframeAnimationOptions)options animations:(void (^)(void))animations completion:(void (^)(BOOL finished))completion {
    if (animations)
        animations();
    if (completion)
        completion(YES);
}

+ (void)addKeyframeWithRelativeStartTime:(double)frameStartTime relativeDuration:(double)frameDuration animations:(void (^)(void))animations {
    if (animations)
        animations();
}

+ (void)performSystemAnimation:(UISystemAnimation)animation onViews:(NSArray *)views options:(UIViewAnimationOptions)options animations:(void (^)(void))parallelAnimations completion:(void (^)(BOOL finished))completion {
    if (parallelAnimations)
        parallelAnimations();
    if (completion)
        completion(YES);
}

+ (BOOL)areAnimationsEnabled {
    return UIViewAnimationsEnabled;
}

+ (void)setAnimationsEnabled:(BOOL)enabled {
    UIViewAnimationsEnabled = enabled;
}

@end
