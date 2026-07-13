#import <UIKit/UIView.h>
#import <UIKit/UIKitExport.h>

@class UIImage;

@interface UIImageView : UIView {
    UIImage *_image;
    UIImage *_highlightedImage;
    NSArray *_animationImages;
    NSArray *_highlightedAnimationImages;
    NSTimeInterval _animationDuration;
    NSInteger _animationRepeatCount;
    BOOL _highlighted;
    BOOL _adjustsImageWhenAncestorFocused;
}

- (instancetype)initWithImage:(UIImage *)image;
- (instancetype)initWithImage:(UIImage *)image highlightedImage:(UIImage *)highlightedImage;

@property (nonatomic, retain) UIImage *image;
@property (nonatomic, retain) UIImage *highlightedImage;

@property (nonatomic, getter=isHighlighted) BOOL highlighted;

@property (nonatomic, copy) NSArray *animationImages;
@property (nonatomic, copy) NSArray *highlightedAnimationImages;
@property (nonatomic) NSTimeInterval animationDuration;
@property (nonatomic) NSInteger animationRepeatCount;

- (void)startAnimating;
- (void)stopAnimating;
- (BOOL)isAnimating;

@property (nonatomic) BOOL adjustsImageWhenAncestorFocused;

@end
