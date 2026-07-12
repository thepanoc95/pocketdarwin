#import <UIKit/UIView.h>
#import <UIKit/UIKitExport.h>

@class UIColor;
@class UIFont;
@class UIImage;
@class UILabel;

typedef NSInteger UIButtonType;

enum {
    UIButtonTypeCustom = 0,
    UIButtonTypeSystem,
    UIButtonTypeDetailDisclosure,
    UIButtonTypeInfoLight,
    UIButtonTypeInfoDark,
    UIButtonTypeContactAdd,
    UIButtonTypePlain,
    UIButtonTypeRoundedRect = UIButtonTypeSystem,
};

typedef NSInteger UIControlState;

enum {
    UIControlStateNormal       = 0,
    UIControlStateHighlighted  = 1 << 0,
    UIControlStateDisabled     = 1 << 1,
    UIControlStateSelected     = 1 << 2,
    UIControlStateFocused      = 1 << 3,
    UIControlStateApplication  = 0x00FF0000,
    UIControlStateReserved     = 0xFF000000,
};

typedef NSInteger UIControlEvents;

enum {
    UIControlEventTouchDown           = 1 <<  0,
    UIControlEventTouchDownRepeat     = 1 <<  1,
    UIControlEventTouchDragInside     = 1 <<  2,
    UIControlEventTouchDragOutside    = 1 <<  3,
    UIControlEventTouchDragEnter      = 1 <<  4,
    UIControlEventTouchDragExit       = 1 <<  5,
    UIControlEventTouchUpInside       = 1 <<  6,
    UIControlEventTouchUpOutside      = 1 <<  7,
    UIControlEventTouchCancel         = 1 <<  8,
    UIControlEventValueChanged        = 1 << 12,
    UIControlEventPrimaryActionTriggered = 1 << 13,
    UIControlEventEditingDidBegin     = 1 << 16,
    UIControlEventEditingChanged      = 1 << 17,
    UIControlEventEditingDidEnd       = 1 << 18,
    UIControlEventEditingDidEndOnExit = 1 << 19,
    UIControlEventAllTouchEvents      = 0x00000FFF,
    UIControlEventAllEditingEvents    = 0x000F0000,
    UIControlEventApplicationReserved = 0x00FF0000,
    UIControlEventSystemReserved      = 0xF0000000,
    UIControlEventAllEvents           = 0xFFFFFFFF,
};

typedef NSInteger UIControlContentVerticalAlignment;

enum {
    UIControlContentVerticalAlignmentCenter,
    UIControlContentVerticalAlignmentTop,
    UIControlContentVerticalAlignmentBottom,
    UIControlContentVerticalAlignmentFill,
};

typedef NSInteger UIControlContentHorizontalAlignment;

enum {
    UIControlContentHorizontalAlignmentCenter,
    UIControlContentHorizontalAlignmentLeft,
    UIControlContentHorizontalAlignmentRight,
    UIControlContentHorizontalAlignmentFill,
};

@interface UIButton : UIView {
    UIButtonType _buttonType;
    NSString *_title;
    UIColor *_titleColor;
    UIColor *_titleShadowColor;
    UIImage *_image;
    UIImage *_backgroundImage;
    UIFont *_font;
    UILabel *_titleLabel;
    UIImageView *_imageView;
    UIEdgeInsets _contentEdgeInsets;
    UIEdgeInsets _titleEdgeInsets;
    UIEdgeInsets _imageEdgeInsets;
    UIControlState _state;
    UIControlContentVerticalAlignment _contentVerticalAlignment;
    UIControlContentHorizontalAlignment _contentHorizontalAlignment;
    BOOL _reversesTitleShadowWhenHighlighted;
    BOOL _adjustsImageWhenHighlighted;
    BOOL _adjustsImageWhenDisabled;
    BOOL _showsTouchWhenHighlighted;
    NSMutableDictionary *_targetActions;
    BOOL _selected;
    BOOL _highlighted;
    BOOL _enabled;
}

+ (UIButton *)buttonWithType:(UIButtonType)buttonType;

- (void)setTitle:(NSString *)title forState:(UIControlState)state;
- (void)setTitleColor:(UIColor *)color forState:(UIControlState)state;
- (void)setTitleShadowColor:(UIColor *)color forState:(UIControlState)state;
- (void)setImage:(UIImage *)image forState:(UIControlState)state;
- (void)setBackgroundImage:(UIImage *)image forState:(UIControlState)state;

- (NSString *)titleForState:(UIControlState)state;
- (UIColor *)titleColorForState:(UIControlState)state;
- (UIColor *)titleShadowColorForState:(UIControlState)state;
- (UIImage *)imageForState:(UIControlState)state;
- (UIImage *)backgroundImageForState:(UIControlState)state;

- (void)setAttributedTitle:(NSAttributedString *)title forState:(UIControlState)state;
- (NSAttributedString *)attributedTitleForState:(UIControlState)state;

@property (nonatomic, readonly) UILabel *titleLabel;
@property (nonatomic, readonly) UIImageView *imageView;

@property (nonatomic) UIEdgeInsets contentEdgeInsets;
@property (nonatomic) UIEdgeInsets titleEdgeInsets;
@property (nonatomic) UIEdgeInsets imageEdgeInsets;

@property (nonatomic) BOOL reversesTitleShadowWhenHighlighted;
@property (nonatomic) BOOL adjustsImageWhenHighlighted;
@property (nonatomic) BOOL adjustsImageWhenDisabled;
@property (nonatomic) BOOL showsTouchWhenHighlighted;

@property (nonatomic, getter=isSelected) BOOL selected;
@property (nonatomic, getter=isHighlighted) BOOL highlighted;
@property (nonatomic, getter=isEnabled) BOOL enabled;

@property (nonatomic, readonly) UIControlState state;

@property (nonatomic) UIControlContentVerticalAlignment contentVerticalAlignment;
@property (nonatomic) UIControlContentHorizontalAlignment contentHorizontalAlignment;

@property (nonatomic, readonly) NSString *currentTitle;
@property (nonatomic, readonly) UIColor *currentTitleColor;
@property (nonatomic, readonly) UIColor *currentTitleShadowColor;
@property (nonatomic, readonly) UIImage *currentImage;
@property (nonatomic, readonly) UIImage *currentBackgroundImage;
@property (nonatomic, readonly) NSAttributedString *currentAttributedTitle;

@property (nonatomic) UIFont *font;

- (void)addTarget:(id)target action:(SEL)action forControlEvents:(UIControlEvents)controlEvents;
- (void)removeTarget:(id)target action:(SEL)action forControlEvents:(UIControlEvents)controlEvents;

- (NSSet *)allTargets;
- (UIControlEvents)allControlEvents;

- (NSArray *)actionsForTarget:(id)target forControlEvent:(UIControlEvents)controlEvent;

- (void)sendAction:(SEL)action to:(id)target forEvent:(UIEvent *)event;
- (void)sendActionsForControlEvents:(UIControlEvents)controlEvents;

- (void)setTitle:(NSString *)title forState:(UIControlState)state;
- (void)setTitleColor:(UIColor *)color forState:(UIControlState)state;

- (CGRect)backgroundRectForBounds:(CGRect)bounds;
- (CGRect)contentRectForBounds:(CGRect)bounds;
- (CGRect)titleRectForContentRect:(CGRect)contentRect;
- (CGRect)imageRectForContentRect:(CGRect)contentRect;

@end
