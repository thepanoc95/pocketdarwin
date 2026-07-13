#import <Foundation/Foundation.h>
#import <UIKit/UIKitExport.h>
#import <CoreGraphics/CGGeometry.h>

@class UIView;
@class UIWindow;
@class UIGestureRecognizer;

typedef NSInteger UIEventType;

enum {
    UIEventTypeTouches,
    UIEventTypeMotion,
    UIEventTypeRemoteControl,
    UIEventTypePresses,
};

typedef NSInteger UIEventSubtype;

enum {
    UIEventSubtypeNone                              = 0,
    UIEventSubtypeMotionShake                       = 1,
    UIEventSubtypeRemoteControlPlay                 = 100,
    UIEventSubtypeRemoteControlPause                = 101,
    UIEventSubtypeRemoteControlStop                 = 102,
    UIEventSubtypeRemoteControlTogglePlayPause      = 103,
    UIEventSubtypeRemoteControlNextTrack            = 104,
    UIEventSubtypeRemoteControlPreviousTrack        = 105,
    UIEventSubtypeRemoteControlBeginSeekingBackward = 106,
    UIEventSubtypeRemoteControlEndSeekingBackward   = 107,
    UIEventSubtypeRemoteControlBeginSeekingForward  = 108,
    UIEventSubtypeRemoteControlEndSeekingForward    = 109,
};

typedef NSInteger UITouchPhase;

enum {
    UITouchPhaseBegan,
    UITouchPhaseMoved,
    UITouchPhaseStationary,
    UITouchPhaseEnded,
    UITouchPhaseCancelled,
};

@interface UITouch : NSObject {
    NSTimeInterval _timestamp;
    UITouchPhase _phase;
    NSUInteger _tapCount;
    UIWindow *_window;
    UIView *_view;
    CGPoint _locationInWindow;
    CGPoint _previousLocationInWindow;
}

@property (nonatomic, readonly) NSTimeInterval timestamp;
@property (nonatomic, readonly) UITouchPhase phase;
@property (nonatomic, readonly) NSUInteger tapCount;
@property (nonatomic, readonly, retain) UIWindow *window;
@property (nonatomic, readonly, retain) UIView *view;

- (CGPoint)locationInView:(UIView *)view;
- (CGPoint)previousLocationInView:(UIView *)view;

@property (nonatomic, readonly) CGFloat majorRadius;
@property (nonatomic, readonly) CGFloat majorRadiusTolerance;

@end

@interface UIEvent : NSObject {
    NSTimeInterval _timestamp;
    UIEventType _type;
    UIEventSubtype _subtype;
    NSSet *_touches;
}

@property (nonatomic, readonly) UIEventType type;
@property (nonatomic, readonly) UIEventSubtype subtype;
@property (nonatomic, readonly) NSTimeInterval timestamp;

- (NSSet *)allTouches;
- (NSSet *)touchesForWindow:(UIWindow *)window;
- (NSSet *)touchesForView:(UIView *)view;
- (NSSet *)touchesForGestureRecognizer:(UIGestureRecognizer *)gesture;

@end

@interface UIPressesEvent : UIEvent

@property (nonatomic, readonly) NSSet *allPresses;

@end
