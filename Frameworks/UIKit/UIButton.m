#import <UIKit/UIButton.h>
#import <UIKit/UILabel.h>
#import <UIKit/UIImageView.h>
#import <UIKit/UIImage.h>
#import <UIKit/UIColor.h>
#import <UIKit/UIFont.h>
#import <UIKit/UIEvent.h>

@interface UIButton ()
- (UIControlState)_currentState;
@end

@implementation UIButton

+ (UIButton *)buttonWithType:(UIButtonType)buttonType {
    UIButton *button = [[self alloc] initWithFrame:CGRectZero];
    button->_buttonType = buttonType;
    return [button autorelease];
}

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        _buttonType = UIButtonTypeCustom;
        _title = nil;
        _titleColor = [[UIColor blackColor] retain];
        _titleShadowColor = nil;
        _image = nil;
        _backgroundImage = nil;
        _font = [[UIFont systemFontOfSize:15] retain];
        _titleLabel = nil;
        _imageView = nil;
        _contentEdgeInsets = UIEdgeInsetsZero;
        _titleEdgeInsets = UIEdgeInsetsZero;
        _imageEdgeInsets = UIEdgeInsetsZero;
        _state = UIControlStateNormal;
        _contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;
        _contentHorizontalAlignment = UIControlContentHorizontalAlignmentCenter;
        _reversesTitleShadowWhenHighlighted = NO;
        _adjustsImageWhenHighlighted = YES;
        _adjustsImageWhenDisabled = YES;
        _showsTouchWhenHighlighted = NO;
        _targetActions = [[NSMutableDictionary alloc] init];
        _selected = NO;
        _highlighted = NO;
        _enabled = YES;

        _titleLabel = [[UILabel alloc] initWithFrame:CGRectZero];
        [_titleLabel setBackgroundColor:[UIColor clearColor]];
        [_titleLabel setTextAlignment:NSTextAlignmentCenter];
        [self addSubview:_titleLabel];

        _imageView = [[UIImageView alloc] initWithFrame:CGRectZero];
        [self addSubview:_imageView];
    }
    return self;
}

- (void)dealloc {
    [_title release];
    [_titleColor release];
    [_titleShadowColor release];
    [_image release];
    [_backgroundImage release];
    [_font release];
    [_titleLabel release];
    [_imageView release];
    [_targetActions release];
    [super dealloc];
}

- (UIButtonType)buttonType {
    return _buttonType;
}

- (UIControlState)_currentState {
    UIControlState state = UIControlStateNormal;
    if (_highlighted)
        state |= UIControlStateHighlighted;
    if (!_enabled)
        state |= UIControlStateDisabled;
    if (_selected)
        state |= UIControlStateSelected;
    return state;
}

- (void)setTitle:(NSString *)title forState:(UIControlState)state {
    if (state == UIControlStateNormal) {
        title = [title copy];
        [_title release];
        _title = title;
        [_titleLabel setText:title];
    }
}

- (void)setTitleColor:(UIColor *)color forState:(UIControlState)state {
    if (state == UIControlStateNormal) {
        color = [color retain];
        [_titleColor release];
        _titleColor = color;
        [_titleLabel setTextColor:color];
    }
}

- (void)setTitleShadowColor:(UIColor *)color forState:(UIControlState)state {
    if (state == UIControlStateNormal) {
        color = [color retain];
        [_titleShadowColor release];
        _titleShadowColor = color;
    }
}

- (void)setImage:(UIImage *)image forState:(UIControlState)state {
    if (state == UIControlStateNormal) {
        image = [image retain];
        [_image release];
        _image = image;
        [_imageView setImage:image];
    }
}

- (void)setBackgroundImage:(UIImage *)image forState:(UIControlState)state {
    if (state == UIControlStateNormal) {
        image = [image retain];
        [_backgroundImage release];
        _backgroundImage = image;
    }
}

- (NSString *)titleForState:(UIControlState)state {
    if (state == UIControlStateNormal)
        return _title;
    return nil;
}

- (UIColor *)titleColorForState:(UIControlState)state {
    if (state == UIControlStateNormal)
        return _titleColor;
    return nil;
}

- (UIColor *)titleShadowColorForState:(UIControlState)state {
    if (state == UIControlStateNormal)
        return _titleShadowColor;
    return nil;
}

- (UIImage *)imageForState:(UIControlState)state {
    if (state == UIControlStateNormal)
        return _image;
    return nil;
}

- (UIImage *)backgroundImageForState:(UIControlState)state {
    if (state == UIControlStateNormal)
        return _backgroundImage;
    return nil;
}

- (void)setAttributedTitle:(NSAttributedString *)title forState:(UIControlState)state {
}

- (NSAttributedString *)attributedTitleForState:(UIControlState)state {
    return nil;
}

- (UILabel *)titleLabel {
    return _titleLabel;
}

- (UIImageView *)imageView {
    return _imageView;
}

- (UIEdgeInsets)contentEdgeInsets {
    return _contentEdgeInsets;
}

- (void)setContentEdgeInsets:(UIEdgeInsets)insets {
    _contentEdgeInsets = insets;
}

- (UIEdgeInsets)titleEdgeInsets {
    return _titleEdgeInsets;
}

- (void)setTitleEdgeInsets:(UIEdgeInsets)insets {
    _titleEdgeInsets = insets;
}

- (UIEdgeInsets)imageEdgeInsets {
    return _imageEdgeInsets;
}

- (void)setImageEdgeInsets:(UIEdgeInsets)insets {
    _imageEdgeInsets = insets;
}

- (BOOL)reversesTitleShadowWhenHighlighted {
    return _reversesTitleShadowWhenHighlighted;
}

- (void)setReversesTitleShadowWhenHighlighted:(BOOL)reverses {
    _reversesTitleShadowWhenHighlighted = reverses;
}

- (BOOL)adjustsImageWhenHighlighted {
    return _adjustsImageWhenHighlighted;
}

- (void)setAdjustsImageWhenHighlighted:(BOOL)adjusts {
    _adjustsImageWhenHighlighted = adjusts;
}

- (BOOL)adjustsImageWhenDisabled {
    return _adjustsImageWhenDisabled;
}

- (void)setAdjustsImageWhenDisabled:(BOOL)adjusts {
    _adjustsImageWhenDisabled = adjusts;
}

- (BOOL)showsTouchWhenHighlighted {
    return _showsTouchWhenHighlighted;
}

- (void)setShowsTouchWhenHighlighted:(BOOL)shows {
    _showsTouchWhenHighlighted = shows;
}

- (BOOL)isSelected {
    return _selected;
}

- (void)setSelected:(BOOL)selected {
    _selected = selected;
    _state = [self _currentState];
}

- (BOOL)isHighlighted {
    return _highlighted;
}

- (void)setHighlighted:(BOOL)highlighted {
    _highlighted = highlighted;
    _state = [self _currentState];
}

- (BOOL)isEnabled {
    return _enabled;
}

- (void)setEnabled:(BOOL)enabled {
    _enabled = enabled;
    _state = [self _currentState];
}

- (UIControlState)state {
    return _state;
}

- (UIControlContentVerticalAlignment)contentVerticalAlignment {
    return _contentVerticalAlignment;
}

- (void)setContentVerticalAlignment:(UIControlContentVerticalAlignment)alignment {
    _contentVerticalAlignment = alignment;
}

- (UIControlContentHorizontalAlignment)contentHorizontalAlignment {
    return _contentHorizontalAlignment;
}

- (void)setContentHorizontalAlignment:(UIControlContentHorizontalAlignment)alignment {
    _contentHorizontalAlignment = alignment;
}

- (NSString *)currentTitle {
    return _title;
}

- (UIColor *)currentTitleColor {
    return _titleColor;
}

- (UIColor *)currentTitleShadowColor {
    return _titleShadowColor;
}

- (UIImage *)currentImage {
    return _image;
}

- (UIImage *)currentBackgroundImage {
    return _backgroundImage;
}

- (NSAttributedString *)currentAttributedTitle {
    return nil;
}

- (UIFont *)font {
    return _font;
}

- (void)setFont:(UIFont *)font {
    font = [font retain];
    [_font release];
    _font = font;
    [_titleLabel setFont:font];
}

- (void)addTarget:(id)target action:(SEL)action forControlEvents:(UIControlEvents)controlEvents {
    NSString *key = [NSString stringWithFormat:@"%lu", (unsigned long)controlEvents];
    NSMutableArray *actions = [_targetActions objectForKey:key];
    if (!actions) {
        actions = [NSMutableArray array];
        [_targetActions setObject:actions forKey:key];
    }
    NSDictionary *entry = @{@"target": target, @"action": NSStringFromSelector(action)};
    [actions addObject:entry];
}

- (void)removeTarget:(id)target action:(SEL)action forControlEvents:(UIControlEvents)controlEvents {
    NSString *key = [NSString stringWithFormat:@"%lu", (unsigned long)controlEvents];
    NSMutableArray *actions = [_targetActions objectForKey:key];
    if (actions) {
        NSMutableArray *toRemove = [NSMutableArray array];
        for (NSDictionary *entry in actions) {
            if (entry[@"target"] == target) {
                if (!action || NSSelectorFromString(entry[@"action"]) == action)
                    [toRemove addObject:entry];
            }
        }
        [actions removeObjectsInArray:toRemove];
    }
}

- (NSSet *)allTargets {
    NSMutableSet *targets = [NSMutableSet set];
    for (NSArray *actions in [_targetActions allValues]) {
        for (NSDictionary *entry in actions) {
            [targets addObject:entry[@"target"]];
        }
    }
    return targets;
}

- (UIControlEvents)allControlEvents {
    UIControlEvents events = 0;
    for (NSString *key in _targetActions) {
        if ([[_targetActions objectForKey:key] count] > 0)
            events |= [key integerValue];
    }
    return events;
}

- (NSArray *)actionsForTarget:(id)target forControlEvent:(UIControlEvents)controlEvent {
    NSString *key = [NSString stringWithFormat:@"%lu", (unsigned long)controlEvent];
    NSMutableArray *actions = [_targetActions objectForKey:key];
    NSMutableArray *result = [NSMutableArray array];
    for (NSDictionary *entry in actions) {
        if (entry[@"target"] == target)
            [result addObject:entry[@"action"]];
    }
    return result;
}

- (void)sendAction:(SEL)action to:(id)target forEvent:(UIEvent *)event {
    [target performSelector:action withObject:self];
}

- (void)sendActionsForControlEvents:(UIControlEvents)controlEvents {
    NSString *key = [NSString stringWithFormat:@"%lu", (unsigned long)controlEvents];
    NSArray *actions = [_targetActions objectForKey:key];
    for (NSDictionary *entry in actions) {
        id target = entry[@"target"];
        SEL action = NSSelectorFromString(entry[@"action"]);
        [target performSelector:action withObject:self];
    }
}

- (CGRect)backgroundRectForBounds:(CGRect)bounds {
    return bounds;
}

- (CGRect)contentRectForBounds:(CGRect)bounds {
    return UIEdgeInsetsInsetRect(bounds, _contentEdgeInsets);
}

- (CGRect)titleRectForContentRect:(CGRect)contentRect {
    return contentRect;
}

- (CGRect)imageRectForContentRect:(CGRect)contentRect {
    return contentRect;
}

- (void)layoutSubviews {
    [super layoutSubviews];
    CGRect bounds = [self bounds];
    [_titleLabel setFrame:bounds];
    [_imageView setFrame:bounds];
}

- (void)drawRect:(CGRect)rect {
    if (_backgroundImage)
        [_backgroundImage drawInRect:[self bounds]];
}

- (CGSize)sizeThatFits:(CGSize)size {
    if (_title) {
        CGSize textSize = [_title sizeWithFont:_font];
        return CGSizeMake(textSize.width + 16, textSize.height + 8);
    }
    return [super sizeThatFits:size];
}

@end
