#import <UIKit/UIResponder.h>
#import <UIKit/UIEvent.h>
#import <UIKit/UIView.h>

@implementation UIResponder

- (UIResponder *)nextResponder {
    return nil;
}

- (BOOL)canBecomeFirstResponder {
    return NO;
}

- (BOOL)becomeFirstResponder {
    return NO;
}

- (BOOL)canResignFirstResponder {
    return YES;
}

- (BOOL)resignFirstResponder {
    return YES;
}

- (BOOL)isFirstResponder {
    return NO;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    [[self nextResponder] touchesBegan:touches withEvent:event];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    [[self nextResponder] touchesMoved:touches withEvent:event];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    [[self nextResponder] touchesEnded:touches withEvent:event];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
    [[self nextResponder] touchesCancelled:touches withEvent:event];
}

- (void)pressesBegan:(NSSet *)presses withEvent:(UIPressesEvent *)event {
    [[self nextResponder] pressesBegan:presses withEvent:event];
}

- (void)pressesChanged:(NSSet *)presses withEvent:(UIPressesEvent *)event {
    [[self nextResponder] pressesChanged:presses withEvent:event];
}

- (void)pressesEnded:(NSSet *)presses withEvent:(UIPressesEvent *)event {
    [[self nextResponder] pressesEnded:presses withEvent:event];
}

- (void)pressesCancelled:(NSSet *)presses withEvent:(UIPressesEvent *)event {
    [[self nextResponder] pressesCancelled:presses withEvent:event];
}

- (void)motionBegan:(UIEventSubtype)motion withEvent:(UIEvent *)event {
    [[self nextResponder] motionBegan:motion withEvent:event];
}

- (void)motionEnded:(UIEventSubtype)motion withEvent:(UIEvent *)event {
    [[self nextResponder] motionEnded:motion withEvent:event];
}

- (void)motionCancelled:(UIEventSubtype)motion withEvent:(UIEvent *)event {
    [[self nextResponder] motionCancelled:motion withEvent:event];
}

- (void)remoteControlReceivedWithEvent:(UIEvent *)event {
    [[self nextResponder] remoteControlReceivedWithEvent:event];
}

- (BOOL)canPerformAction:(SEL)action withSender:(id)sender {
    if ([[self nextResponder] respondsToSelector:@selector(canPerformAction:withSender:)])
        return [[self nextResponder] canPerformAction:action withSender:sender];
    return NO;
}

- (id)targetForAction:(SEL)action withSender:(id)sender {
    UIResponder *responder = self;
    while (responder) {
        if ([responder respondsToSelector:action])
            return responder;
        responder = [responder nextResponder];
    }
    return nil;
}

- (NSUndoManager *)undoManager {
    return [[self nextResponder] undoManager];
}

- (NSSet *)keyCommands {
    return nil;
}

- (BOOL)isEditing {
    return NO;
}

- (UIUserInterfaceLayoutDirection)userInterfaceLayoutDirection {
    return UIUserInterfaceLayoutDirectionLeftToRight;
}

@end

@implementation UIResponder (UIResponderInputViewAdditions)

- (UIView *)inputView {
    return nil;
}

- (UIView *)inputAccessoryView {
    return nil;
}

- (void)reloadInputViews {
}

@end
