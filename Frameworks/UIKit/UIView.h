#import <UIKit/UIResponder.h>
#import <UIKit/UIKitExport.h>
#import <UIKit/UIGeometry.h>
#import <CoreGraphics/CGGeometry.h>
#import <CoreGraphics/CGAffineTransform.h>

@class UIColor;
@class UIGestureRecognizer;
@class UIMotionEffect;
@class UIViewController;
@class UIWindow;

typedef NSInteger UIViewContentMode;

enum {
    UIViewContentModeScaleToFill,
    UIViewContentModeScaleAspectFit,
    UIViewContentModeScaleAspectFill,
    UIViewContentModeRedraw,
    UIViewContentModeCenter,
    UIViewContentModeTop,
    UIViewContentModeBottom,
    UIViewContentModeLeft,
    UIViewContentModeRight,
    UIViewContentModeTopLeft,
    UIViewContentModeTopRight,
    UIViewContentModeBottomLeft,
    UIViewContentModeBottomRight,
};

typedef NSInteger UIViewAnimationCurve;

enum {
    UIViewAnimationCurveEaseInOut,
    UIViewAnimationCurveEaseIn,
    UIViewAnimationCurveEaseOut,
    UIViewAnimationCurveLinear,
};

typedef NSInteger UIViewAnimationTransition;

enum {
    UIViewAnimationTransitionNone,
    UIViewAnimationTransitionFlipFromLeft,
    UIViewAnimationTransitionFlipFromRight,
    UIViewAnimationTransitionCurlUp,
    UIViewAnimationTransitionCurlDown,
};

typedef NSInteger UIViewAutoresizing;

enum {
    UIViewAutoresizingNone                 = 0,
    UIViewAutoresizingFlexibleLeftMargin   = 1 << 0,
    UIViewAutoresizingFlexibleWidth        = 1 << 1,
    UIViewAutoresizingFlexibleRightMargin  = 1 << 2,
    UIViewAutoresizingFlexibleTopMargin    = 1 << 3,
    UIViewAutoresizingFlexibleHeight       = 1 << 4,
    UIViewAutoresizingFlexibleBottomMargin = 1 << 5,
};

typedef NSInteger UIViewAnimationOptions;

enum {
    UIViewAnimationOptionLayoutSubviews            = 1 <<  0,
    UIViewAnimationOptionAllowUserInteraction      = 1 <<  1,
    UIViewAnimationOptionBeginFromCurrentState     = 1 <<  2,
    UIViewAnimationOptionRepeat                    = 1 <<  3,
    UIViewAnimationOptionAutoreverse               = 1 <<  4,
    UIViewAnimationOptionOverrideInheritedDuration = 1 <<  5,
    UIViewAnimationOptionOverrideInheritedCurve    = 1 <<  6,
    UIViewAnimationOptionAllowAnimatedContent      = 1 <<  7,
    UIViewAnimationOptionShowHideTransitionViews   = 1 <<  8,
    UIViewAnimationOptionOverrideInheritedOptions  = 1 <<  9,
    UIViewAnimationOptionCurveEaseInOut            = 0 << 16,
    UIViewAnimationOptionCurveEaseIn               = 1 << 16,
    UIViewAnimationOptionCurveEaseOut              = 2 << 16,
    UIViewAnimationOptionCurveLinear               = 3 << 16,
    UIViewAnimationOptionTransitionNone            = 0 << 20,
    UIViewAnimationOptionTransitionFlipFromLeft    = 1 << 20,
    UIViewAnimationOptionTransitionFlipFromRight   = 2 << 20,
    UIViewAnimationOptionTransitionCurlUp          = 3 << 20,
    UIViewAnimationOptionTransitionCurlDown        = 4 << 20,
    UIViewAnimationOptionTransitionCrossDissolve   = 5 << 20,
    UIViewAnimationOptionTransitionFlipFromTop     = 6 << 20,
    UIViewAnimationOptionTransitionFlipFromBottom  = 7 << 20,
};

typedef NSInteger UIViewTintAdjustmentMode;

enum {
    UIViewTintAdjustmentModeAutomatic,
    UIViewTintAdjustmentModeNormal,
    UIViewTintAdjustmentModeDimmed,
};

@interface UIView : UIResponder <NSCoding, UIAppearance, UIAppearanceContainer, UIDynamicItem> {
    CGRect _frame;
    CGRect _bounds;
    CGPoint _center;
    CGAffineTransform _transform;
    CGFloat _alpha;
    BOOL _hidden;
    BOOL _clipsToBounds;
    BOOL _autoresizesSubviews;
    UIViewAutoresizing _autoresizingMask;
    UIViewContentMode _contentMode;
    UIColor *_backgroundColor;
    UIView *_superview;
    NSMutableArray *_subviews;
    UIWindow *_window;
    NSInteger _tag;
    UIColor *_tintColor;
    NSMutableArray *_gestureRecognizers;
    UIEdgeInsets _layoutMargins;
    BOOL _userInteractionEnabled;
    BOOL _multipleTouchEnabled;
    BOOL _exclusiveTouch;
    CGFloat _cornerRadius;
    BOOL _masksToBounds;
    CGFloat _borderWidth;
    UIColor *_borderColor;
}

- (instancetype)initWithFrame:(CGRect)frame;

@property (nonatomic, assign) CGRect frame;
@property (nonatomic, assign) CGRect bounds;
@property (nonatomic, assign) CGPoint center;
@property (nonatomic, assign) CGAffineTransform transform;

@property (nonatomic, readonly) UIView *superview;
@property (nonatomic, readonly, copy) NSArray *subviews;
@property (nonatomic, readonly) UIWindow *window;

@property (nonatomic) CGFloat alpha;
@property (nonatomic, getter=isHidden) BOOL hidden;
@property (nonatomic) BOOL clipsToBounds;
@property (nonatomic) BOOL clearsContextBeforeDrawing;

@property (nonatomic) BOOL autoresizesSubviews;
@property (nonatomic) UIViewAutoresizing autoresizingMask;

@property (nonatomic) CGRect contentStretch;

@property (nonatomic) UIViewContentMode contentMode;

@property (nonatomic, getter=isUserInteractionEnabled) BOOL userInteractionEnabled;
@property (nonatomic, getter=isMultipleTouchEnabled) BOOL multipleTouchEnabled;
@property (nonatomic, getter=isExclusiveTouch) BOOL exclusiveTouch;

@property (nonatomic, copy) UIColor *backgroundColor;
@property (nonatomic, copy) UIColor *tintColor;
@property (nonatomic) UIViewTintAdjustmentMode tintAdjustmentMode;
@property (nonatomic, readonly, strong) UIColor *tintColor;

@property (nonatomic) NSInteger tag;

@property (nonatomic, copy) NSArray *gestureRecognizers;

- (void)addGestureRecognizer:(UIGestureRecognizer *)gestureRecognizer;
- (void)removeGestureRecognizer:(UIGestureRecognizer *)gestureRecognizer;

@property (nonatomic) UIEdgeInsets layoutMargins;
@property (nonatomic) BOOL preservesSuperviewLayoutMargins;

@property (nonatomic) CGFloat cornerRadius;
@property (nonatomic) BOOL masksToBounds;
@property (nonatomic) CGFloat borderWidth;
@property (nonatomic, strong) UIColor *borderColor;

- (void)addSubview:(UIView *)view;
- (void)insertSubview:(UIView *)view atIndex:(NSInteger)index;
- (void)insertSubview:(UIView *)view aboveSubview:(UIView *)siblingSubview;
- (void)insertSubview:(UIView *)view belowSubview:(UIView *)siblingSubview;

- (void)bringSubviewToFront:(UIView *)view;
- (void)sendSubviewToBack:(UIView *)view;

- (void)removeFromSuperview;

- (void)didAddSubview:(UIView *)subview;
- (void)willRemoveSubview:(UIView *)subview;

- (void)willMoveToSuperview:(UIView *)newSuperview;
- (void)didMoveToSuperview;
- (void)willMoveToWindow:(UIWindow *)newWindow;
- (void)didMoveToWindow;

- (BOOL)isDescendantOfView:(UIView *)view;
- (UIView *)viewWithTag:(NSInteger)tag;

- (void)setNeedsLayout;
- (void)layoutIfNeeded;
- (void)layoutSubviews;

- (void)setNeedsDisplay;
- (void)setNeedsDisplayInRect:(CGRect)rect;

- (void)drawRect:(CGRect)rect;

- (CGSize)sizeThatFits:(CGSize)size;
- (void)sizeToFit;

- (CGRect)convertRect:(CGRect)rect toView:(UIView *)view;
- (CGRect)convertRect:(CGRect)rect fromView:(UIView *)view;
- (CGPoint)convertPoint:(CGPoint)point toView:(UIView *)view;
- (CGPoint)convertPoint:(CGPoint)point fromView:(UIView *)view;

@property (nonatomic, readonly) UIEdgeInsets safeAreaInsets;
- (void)safeAreaInsetsDidChange;

@property (nonatomic, readonly) UIViewController *viewController;

@property (nonatomic, readonly, copy) NSArray *constraints;
- (void)addConstraint:(NSLayoutConstraint *)constraint;
- (void)addConstraints:(NSArray *)constraints;
- (void)removeConstraint:(NSLayoutConstraint *)constraint;
- (void)removeConstraints:(NSArray *)constraints;

@property (nonatomic) BOOL translatesAutoresizingMaskIntoConstraints;

- (BOOL)needsUpdateConstraints;
- (void)setNeedsUpdateConstraints;
- (void)updateConstraints;

- (void)invalidateIntrinsicContentSize;
- (CGSize)intrinsicContentSize;

@property (nonatomic, readonly) CGFloat scale;

@property (nonatomic, readonly) BOOL hasAmbiguousLayout;
- (UIView *)exerciseAmbiguityInLayout;

@property (nonatomic, copy) NSString *accessibilityIdentifier;
@property (nonatomic, copy) NSString *accessibilityLabel;
@property (nonatomic, copy) NSString *accessibilityHint;
@property (nonatomic, copy) NSString *accessibilityValue;
@property (nonatomic) UIAccessibilityTraits accessibilityTraits;
@property (nonatomic) CGRect accessibilityFrame;
@property (nonatomic) BOOL isAccessibilityElement;

@end

@interface UIView (UIViewAnimationWithBlocks)

+ (void)animateWithDuration:(NSTimeInterval)duration animations:(void (^)(void))animations;
+ (void)animateWithDuration:(NSTimeInterval)duration animations:(void (^)(void))animations completion:(void (^)(BOOL finished))completion;
+ (void)animateWithDuration:(NSTimeInterval)duration delay:(NSTimeInterval)delay options:(UIViewAnimationOptions)options animations:(void (^)(void))animations completion:(void (^)(BOOL finished))completion;
+ (void)animateWithDuration:(NSTimeInterval)duration delay:(NSTimeInterval)delay usingSpringWithDamping:(CGFloat)dampingRatio initialSpringVelocity:(CGFloat)velocity options:(UIViewAnimationOptions)options animations:(void (^)(void))animations completion:(void (^)(BOOL finished))completion;
+ (void)transitionWithView:(UIView *)view duration:(NSTimeInterval)duration options:(UIViewAnimationOptions)options animations:(void (^)(void))animations completion:(void (^)(BOOL finished))completion;
+ (void)transitionFromView:(UIView *)fromView toView:(UIView *)toView duration:(NSTimeInterval)duration options:(UIViewAnimationOptions)options completion:(void (^)(BOOL finished))completion;

+ (void)performWithoutAnimation:(void (^)(void))actionsWithoutAnimation;

+ (void)animateKeyframesWithDuration:(NSTimeInterval)duration delay:(NSTimeInterval)delay options:(UIViewKeyframeAnimationOptions)options animations:(void (^)(void))animations completion:(void (^)(BOOL finished))completion;
+ (void)addKeyframeWithRelativeStartTime:(double)frameStartTime relativeDuration:(double)frameDuration animations:(void (^)(void))animations;

+ (void)performSystemAnimation:(UISystemAnimation)animation onViews:(NSArray *)views options:(UIViewAnimationOptions)options animations:(void (^)(void))parallelAnimations completion:(void (^)(BOOL finished))completion;

+ (BOOL)areAnimationsEnabled;
+ (void)setAnimationsEnabled:(BOOL)enabled;

+ (void)animateWithDuration:(NSTimeInterval)duration delay:(NSTimeInterval)delay options:(UIViewAnimationOptions)options animations:(void (^)(void))animations completion:(void (^)(BOOL finished))completion;

@end
