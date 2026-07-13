#import <UIKit/UIView.h>
#import <UIKit/UIKitExport.h>

@class UIColor;
@class UIFont;
@class NSAttributedString;

typedef NSInteger NSTextAlignment;

enum {
    NSTextAlignmentLeft      = 0,
    NSTextAlignmentCenter    = 1,
    NSTextAlignmentRight     = 2,
    NSTextAlignmentNatural   = 3,
    NSTextAlignmentJustified = 4,
};

typedef NSInteger NSLineBreakMode;

enum {
    NSLineBreakByWordWrapping = 0,
    NSLineBreakByCharWrapping,
    NSLineBreakByClipping,
    NSLineBreakByTruncatingHead,
    NSLineBreakByTruncatingTail,
    NSLineBreakByTruncatingMiddle,
};

typedef NSInteger UIBaselineAdjustment;

enum {
    UIBaselineAdjustmentAlignBaselines,
    UIBaselineAdjustmentAlignCenters,
    UIBaselineAdjustmentNone,
};

@interface UILabel : UIView {
    NSString *_text;
    NSAttributedString *_attributedText;
    UIFont *_font;
    UIColor *_textColor;
    UIColor *_shadowColor;
    UIColor *_highlightedTextColor;
    NSTextAlignment _textAlignment;
    NSLineBreakMode _lineBreakMode;
    NSInteger _numberOfLines;
    BOOL _adjustsFontSizeToFitWidth;
    CGFloat _minimumScaleFactor;
    BOOL _enabled;
    BOOL _highlighted;
    CGSize _shadowOffset;
    CGFloat _preferredMaxLayoutWidth;
    BOOL _adjustsLetterSpacingToFitWidth;
    BOOL _baselineAdjustment;
    CGFloat _minimumFontSize;
}

- (instancetype)initWithFrame:(CGRect)frame;

@property (nonatomic, copy) NSString *text;
@property (nonatomic, retain) UIFont *font;
@property (nonatomic, retain) UIColor *textColor;
@property (nonatomic, retain) UIColor *shadowColor;
@property (nonatomic) CGSize shadowOffset;
@property (nonatomic) NSTextAlignment textAlignment;
@property (nonatomic) NSLineBreakMode lineBreakMode;
@property (nonatomic, getter=isEnabled) BOOL enabled;

@property (nonatomic) NSInteger numberOfLines;
@property (nonatomic) BOOL adjustsFontSizeToFitWidth;
@property (nonatomic) CGFloat minimumScaleFactor;
@property (nonatomic) CGFloat preferredMaxLayoutWidth;

@property (nonatomic, getter=isHighlighted) BOOL highlighted;
@property (nonatomic, retain) UIColor *highlightedTextColor;

@property (nonatomic) UIBaselineAdjustment baselineAdjustment;
@property (nonatomic) CGFloat minimumFontSize;
@property (nonatomic) BOOL adjustsLetterSpacingToFitWidth;

- (CGRect)textRectForBounds:(CGRect)bounds limitedToNumberOfLines:(NSInteger)numberOfLines;
- (void)drawTextInRect:(CGRect)rect;

@property (nonatomic, readonly) CGFloat _baselineOffsetFromBottom;

@property (nonatomic, copy) NSAttributedString *attributedText;

@end
