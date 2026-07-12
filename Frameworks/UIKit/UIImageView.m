#import <UIKit/UIImageView.h>
#import <UIKit/UIImage.h>

@implementation UIImageView

- (instancetype)initWithImage:(UIImage *)image {
    CGSize size = [image size];
    self = [super initWithFrame:CGRectMake(0, 0, size.width, size.height)];
    if (self) {
        _image = [image retain];
        _highlightedImage = nil;
        _animationImages = nil;
        _highlightedAnimationImages = nil;
        _animationDuration = 0;
        _animationRepeatCount = 0;
        _highlighted = NO;
        _adjustsImageWhenAncestorFocused = NO;
        [self setContentMode:UIViewContentModeScaleToFill];
        [self setClipsToBounds:YES];
    }
    return self;
}

- (instancetype)initWithImage:(UIImage *)image highlightedImage:(UIImage *)highlightedImage {
    self = [self initWithImage:image];
    if (self) {
        _highlightedImage = [highlightedImage retain];
    }
    return self;
}

- (instancetype)initWithFrame:(CGRect)frame {
    return [self initWithImage:nil];
}

- (void)dealloc {
    [_image release];
    [_highlightedImage release];
    [_animationImages release];
    [_highlightedAnimationImages release];
    [super dealloc];
}

- (UIImage *)image {
    return _image;
}

- (void)setImage:(UIImage *)image {
    image = [image retain];
    [_image release];
    _image = image;
    [self setNeedsDisplay];
}

- (UIImage *)highlightedImage {
    return _highlightedImage;
}

- (void)setHighlightedImage:(UIImage *)image {
    image = [image retain];
    [_highlightedImage release];
    _highlightedImage = image;
}

- (BOOL)isHighlighted {
    return _highlighted;
}

- (void)setHighlighted:(BOOL)highlighted {
    _highlighted = highlighted;
    [self setNeedsDisplay];
}

- (NSArray *)animationImages {
    return _animationImages;
}

- (void)setAnimationImages:(NSArray *)images {
    images = [images copy];
    [_animationImages release];
    _animationImages = images;
}

- (NSArray *)highlightedAnimationImages {
    return _highlightedAnimationImages;
}

- (void)setHighlightedAnimationImages:(NSArray *)images {
    images = [images copy];
    [_highlightedAnimationImages release];
    _highlightedAnimationImages = images;
}

- (NSTimeInterval)animationDuration {
    return _animationDuration;
}

- (void)setAnimationDuration:(NSTimeInterval)duration {
    _animationDuration = duration;
}

- (NSInteger)animationRepeatCount {
    return _animationRepeatCount;
}

- (void)setAnimationRepeatCount:(NSInteger)count {
    _animationRepeatCount = count;
}

- (void)startAnimating {
}

- (void)stopAnimating {
}

- (BOOL)isAnimating {
    return NO;
}

- (BOOL)adjustsImageWhenAncestorFocused {
    return _adjustsImageWhenAncestorFocused;
}

- (void)setAdjustsImageWhenAncestorFocused:(BOOL)adjusts {
    _adjustsImageWhenAncestorFocused = adjusts;
}

- (void)drawRect:(CGRect)rect {
    UIImage *drawImage = (_highlighted && _highlightedImage) ? _highlightedImage : _image;
    if (drawImage) {
        [drawImage drawInRect:[self bounds]];
    }
}

@end
