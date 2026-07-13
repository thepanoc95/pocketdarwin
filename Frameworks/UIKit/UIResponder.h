#import <Foundation/Foundation.h>
#import <UIKit/UIKitExport.h>

@class UIEvent;
@class UIKeyCommand;
@class UIPress;
@class UIPressesEvent;
@class UIMenuController;
@class UIView;

typedef NSInteger UIEventSubtype;
typedef NSInteger UIUserInterfaceLayoutDirection;

typedef NSInteger UIKeyModifierFlags;

enum {
    UIKeyModifierAlphaShift     = 1 << 16,
    UIKeyModifierShift          = 1 << 17,
    UIKeyModifierControl        = 1 << 18,
    UIKeyModifierAlternate      = 1 << 19,
    UIKeyModifierCommand        = 1 << 20,
    UIKeyModifierNumericPad     = 1 << 21,
};

@interface UIResponder : NSObject

- (UIResponder *)nextResponder;

- (BOOL)canBecomeFirstResponder;
- (BOOL)becomeFirstResponder;
- (BOOL)canResignFirstResponder;
- (BOOL)resignFirstResponder;
- (BOOL)isFirstResponder;

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event;

- (void)pressesBegan:(NSSet *)presses withEvent:(UIPressesEvent *)event;
- (void)pressesChanged:(NSSet *)presses withEvent:(UIPressesEvent *)event;
- (void)pressesEnded:(NSSet *)presses withEvent:(UIPressesEvent *)event;
- (void)pressesCancelled:(NSSet *)presses withEvent:(UIPressesEvent *)event;

- (void)motionBegan:(UIEventSubtype)motion withEvent:(UIEvent *)event;
- (void)motionEnded:(UIEventSubtype)motion withEvent:(UIEvent *)event;
- (void)motionCancelled:(UIEventSubtype)motion withEvent:(UIEvent *)event;

- (void)remoteControlReceivedWithEvent:(UIEvent *)event;

- (BOOL)canPerformAction:(SEL)action withSender:(id)sender;
- (id)targetForAction:(SEL)action withSender:(id)sender;

@property (nonatomic, readonly, weak) UIResponder *nextResponder;
@property (nonatomic, readonly) NSUndoManager *undoManager;

@property (nonatomic, readonly) NSSet *keyCommands;

@property (nonatomic, readonly, getter=isEditing) BOOL editing;

@property (nonatomic, readonly) UIUserInterfaceLayoutDirection userInterfaceLayoutDirection;

@end

@interface UIResponder (UIResponderInputViewAdditions)

@property (nonatomic, readonly, retain) UIView *inputView;
@property (nonatomic, readonly, retain) UIView *inputAccessoryView;

- (void)reloadInputViews;

@end
