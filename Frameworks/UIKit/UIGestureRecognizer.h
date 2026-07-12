#import <Foundation/Foundation.h>
#import <UIKit/UIKitExport.h>

@class UIView;
@class UIEvent;
@class UITouch;

typedef NSInteger UIGestureRecognizerState;

enum {
    UIGestureRecognizerStatePossible,
    UIGestureRecognizerStateBegan,
    UIGestureRecognizerStateChanged,
    UIGestureRecognizerStateEnded,
    UIGestureRecognizerStateCancelled,
    UIGestureRecognizerStateFailed,
    UIGestureRecognizerStateRecognized = UIGestureRecognizerStateEnded,
};

@interface UIGestureRecognizer : NSObject {
    UIView *_view;
    id _target;
    SEL _action;
    UIGestureRecognizerState _state;
    BOOL _enabled;
    BOOL _cancelsTouchesInView;
    BOOL _delaysTouchesBegan;
    BOOL _delaysTouchesEnded;
    NSMutableArray *_delayedTouches;
    NSMutableSet *_touches;
    NSUInteger _numberOfTouchesRequired;
    NSMutableArray *_recognizersToWaitFor;
    NSMutableArray *_recognizersToFailFor;
    BOOL _requiresExclusiveTouchType;
}

- (instancetype)initWithTarget:(id)target action:(SEL)action;

@property (nonatomic, readonly) UIGestureRecognizerState state;
@property (nonatomic, getter=isEnabled) BOOL enabled;
@property (nonatomic, readonly) UIView *view;

- (void)addTarget:(id)target action:(SEL)action;
- (void)removeTarget:(id)target action:(SEL)action;

@property (nonatomic) BOOL cancelsTouchesInView;
@property (nonatomic) BOOL delaysTouchesBegan;
@property (nonatomic) BOOL delaysTouchesEnded;

@property (nonatomic, copy) NSArray *allowedTouchTypes;

- (void)requireGestureRecognizerToFail:(UIGestureRecognizer *)otherGestureRecognizer;

- (NSUInteger)numberOfTouches;

- (CGPoint)locationInView:(UIView *)view;
- (CGPoint)locationOfTouch:(NSUInteger)touchIndex inView:(UIView *)view;

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event;

- (void)pressesBegan:(NSSet *)presses withEvent:(UIPressesEvent *)event;
- (void)pressesChanged:(NSSet *)presses withEvent:(UIPressesEvent *)event;
- (void)pressesEnded:(NSSet *)presses withEvent:(UIPressesEvent *)event;
- (void)pressesCancelled:(NSSet *)presses withEvent:(UIPressesEvent *)event;

- (void)ignoreTouch:(UITouch *)touch forEvent:(UIEvent *)event;
- (void)ignorePress:(UIPress *)press forEvent:(UIPressesEvent *)event;

- (void)reset;

@property (nonatomic) BOOL requiresExclusiveTouchType;

@end

@interface UITapGestureRecognizer : UIGestureRecognizer

@property (nonatomic) NSUInteger numberOfTapsRequired;
@property (nonatomic) NSUInteger numberOfTouchesRequired;

@end

@interface UIPinchGestureRecognizer : UIGestureRecognizer

@property (nonatomic) CGFloat scale;
@property (nonatomic, readonly) CGFloat velocity;

@end

@interface UIRotationGestureRecognizer : UIGestureRecognizer

@property (nonatomic) CGFloat rotation;
@property (nonatomic, readonly) CGFloat velocity;

@end

@interface UISwipeGestureRecognizer : UIGestureRecognizer

@property (nonatomic) UISwipeGestureRecognizerDirection direction;
@property (nonatomic) NSUInteger numberOfTouchesRequired;

typedef NS_OPTIONS(NSUInteger, UISwipeGestureRecognizerDirection) {
    UISwipeGestureRecognizerDirectionRight = 1 << 0,
    UISwipeGestureRecognizerDirectionLeft  = 1 << 1,
    UISwipeGestureRecognizerDirectionUp    = 1 << 2,
    UISwipeGestureRecognizerDirectionDown  = 1 << 3,
};

@end

@interface UIPanGestureRecognizer : UIGestureRecognizer

@property (nonatomic) NSUInteger minimumNumberOfTouches;
@property (nonatomic) NSUInteger maximumNumberOfTouches;

- (CGPoint)translationInView:(UIView *)view;
- (void)setTranslation:(CGPoint)translation inView:(UIView *)view;
- (CGPoint)velocityInView:(UIView *)view;

@end

@interface UIScreenEdgePanGestureRecognizer : UIPanGestureRecognizer

@property (nonatomic) UIRectEdge edges;

@end

@interface UILongPressGestureRecognizer : UIGestureRecognizer

@property (nonatomic) NSUInteger numberOfTapsRequired;
@property (nonatomic) NSUInteger numberOfTouchesRequired;
@property (nonatomic) CFTimeInterval minimumPressDuration;
@property (nonatomic) CGFloat allowableMovement;

@end
