#import <UIKit/UILabel.h>
#import <UIKit/UIFont.h>
#import <UIKit/UIColor.h>
#import <UIKit/UIStringDrawing.h>

@implementation UILabel

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        _text = nil;
        _font = [[UIFont systemFontOfSize:17] retain];
        _textColor = [[UIColor blackColor] retain];
        _shadowColor = nil;
        _highlightedTextColor = nil;
        _textAlignment = NSTextAlignmentNatural;
        _lineBreakMode = NSLineBreakByTruncatingTail;
        _numberOfLines = 1;
        _adjustsFontSizeToFitWidth = NO;
        _minimumScaleFactor = 0;
        _enabled = YES;
        _highlighted = NO;
        _shadowOffset = CGSizeZero;
        _preferredMaxLayoutWidth = 0;
        _adjustsLetterSpacingToFitWidth = NO;
        _baselineAdjustment = UIBaselineAdjustmentAlignBaselines;
        _minimumFontSize = 0;
        [self setContentMode:UIViewContentModeRedraw];
        [self setOpaque:NO];
        [self setBackgroundColor:[UIColor clearColor]];
    }
    return self;
}

- (void)dealloc {
    [_text release];
    [_font release];
    [_textColor release];
    [_shadowColor release];
    [_highlightedTextColor release];
    [_attributedText release];
    [super dealloc];
}

- (NSString *)text {
    return _text;
}

- (void)setText:(NSString *)text {
    text = [text copy];
    [_text release];
    _text = text;
    [self setNeedsDisplay];
    [self invalidateIntrinsicContentSize];
}

- (UIFont *)font {
    return _font;
}

- (void)setFont:(UIFont *)font {
    font = [font retain];
    [_font release];
    _font = font;
    [self setNeedsDisplay];
    [self invalidateIntrinsicContentSize];
}

- (UIColor *)textColor {
    return _textColor;
}

- (void)setTextColor:(UIColor *)color {
    color = [color retain];
    [_textColor release];
    _textColor = color;
    [self setNeedsDisplay];
}

- (UIColor *)shadowColor {
    return _shadowColor;
}

- (void)setShadowColor:(UIColor *)color {
    color = [color retain];
    [_shadowColor release];
    _shadowColor = color;
    [self setNeedsDisplay];
}

- (CGSize)shadowOffset {
    return _shadowOffset;
}

- (void)setShadowOffset:(CGSize)offset {
    _shadowOffset = offset;
    [self setNeedsDisplay];
}

- (NSTextAlignment)textAlignment {
    return _textAlignment;
}

- (void)setTextAlignment:(NSTextAlignment)alignment {
    _textAlignment = alignment;
    [self setNeedsDisplay];
}

- (NSLineBreakMode)lineBreakMode {
    return _lineBreakMode;
}

- (void)setLineBreakMode:(NSLineBreakMode)mode {
    _lineBreakMode = mode;
    [self setNeedsDisplay];
}

- (BOOL)isEnabled {
    return _enabled;
}

- (void)setEnabled:(BOOL)enabled {
    _enabled = enabled;
    [self setNeedsDisplay];
}

- (NSInteger)numberOfLines {
    return _numberOfLines;
}

- (void)setNumberOfLines:(NSInteger)lines {
    _numberOfLines = lines;
    [self setNeedsDisplay];
    [self invalidateIntrinsicContentSize];
}

- (BOOL)adjustsFontSizeToFitWidth {
    return _adjustsFontSizeToFitWidth;
}

- (void)setAdjustsFontSizeToFitWidth:(BOOL)adjusts {
    _adjustsFontSizeToFitWidth = adjusts;
}

- (CGFloat)minimumScaleFactor {
    return _minimumScaleFactor;
}

- (void)setMinimumScaleFactor:(CGFloat)factor {
    _minimumScaleFactor = factor;
}

- (CGFloat)preferredMaxLayoutWidth {
    return _preferredMaxLayoutWidth;
}

- (void)setPreferredMaxLayoutWidth:(CGFloat)width {
    _preferredMaxLayoutWidth = width;
}

- (BOOL)isHighlighted {
    return _highlighted;
}

- (void)setHighlighted:(BOOL)highlighted {
    _highlighted = highlighted;
    [self setNeedsDisplay];
}

- (UIColor *)highlightedTextColor {
    return _highlightedTextColor;
}

- (void)setHighlightedTextColor:(UIColor *)color {
    color = [color retain];
    [_highlightedTextColor release];
    _highlightedTextColor = color;
}

- (UIBaselineAdjustment)baselineAdjustment {
    return _baselineAdjustment;
}

- (void)setBaselineAdjustment:(UIBaselineAdjustment)adjustment {
    _baselineAdjustment = adjustment;
}

- (CGFloat)minimumFontSize {
    return _minimumFontSize;
}

- (void)setMinimumFontSize:(CGFloat)size {
    _minimumFontSize = size;
}

- (BOOL)adjustsLetterSpacingToFitWidth {
    return _adjustsLetterSpacingToFitWidth;
}

- (void)setAdjustsLetterSpacingToFitWidth:(BOOL)adjusts {
    _adjustsLetterSpacingToFitWidth = adjusts;
}

- (CGRect)textRectForBounds:(CGRect)bounds limitedToNumberOfLines:(NSInteger)numberOfLines {
    return bounds;
}

- (void)drawTextInRect:(CGRect)rect {
}

- (CGFloat)_baselineOffsetFromBottom {
    return 0;
}

- (NSAttributedString *)attributedText {
    return _attributedText;
}

- (void)setAttributedText:(NSAttributedString *)text {
    text = [text copy];
    [_attributedText release];
    _attributedText = text;
    [self setNeedsDisplay];
}

- (CGSize)sizeThatFits:(CGSize)size {
    if (!_text && !_attributedText)
        return size;
    NSString *string = _text ? _text : [_attributedText string];
    CGFloat width = size.width > 0 ? size.width : 320;
    CGFloat maxHeight = CGFLOAT_MAX;
    if (_numberOfLines > 0)
        maxHeight = [_font lineHeight] * _numberOfLines;

    NSMutableParagraphStyle *style = [[NSMutableParagraphStyle alloc] init];
    [style setLineBreakMode:_lineBreakMode];
    NSDictionary *attributes = @{
        NSFontAttributeName: _font,
        NSParagraphStyleAttributeName: style,
    };
    CGRect textRect = [string boundingRectWithSize:CGSizeMake(width, maxHeight)
                                           options:NSStringDrawingUsesLineFragmentOrigin
                                        attributes:attributes
                                           context:nil];
    [style release];
    return CGSizeMake(ceil(textRect.size.width), ceil(textRect.size.height));
}

- (CGSize)intrinsicContentSize {
    return [self sizeThatFits:CGSizeMake(_preferredMaxLayoutWidth > 0 ? _preferredMaxLayoutWidth : 320, CGFLOAT_MAX)];
}

- (void)drawRect:(CGRect)rect {
    if (!_text)
        return;
    UIColor *drawColor = _enabled ? _textColor : [UIColor lightGrayColor];
    if (_highlighted && _highlightedTextColor)
        drawColor = _highlightedTextColor;

    NSMutableParagraphStyle *style = [[NSMutableParagraphStyle alloc] init];
    [style setAlignment:_textAlignment];
    [style setLineBreakMode:_lineBreakMode];

    NSDictionary *attributes = @{
        NSFontAttributeName: _font,
        NSForegroundColorAttributeName: drawColor,
        NSParagraphStyleAttributeName: style,
    };

    if (_shadowColor) {
        NSShadow *shadow = [[NSShadow alloc] init];
        [shadow setShadowColor:_shadowColor];
        [shadow setShadowOffset:_shadowOffset];
        NSMutableDictionary *mdict = [[attributes mutableCopy] autorelease];
        [mdict setObject:shadow forKey:NSShadowAttributeName];
        attributes = mdict;
        [shadow release];
    }

    [_text drawInRect:rect withAttributes:attributes];
    [style release];
}

@end
