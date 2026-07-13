#import <Foundation/Foundation.h>
#import <CoreGraphics/CGGeometry.h>

@interface NSShadow : NSObject
@property (nonatomic, retain) id shadowColor;
@property (nonatomic) CGSize shadowOffset;
@property (nonatomic) CGFloat shadowBlurRadius;
@end

@interface NSMutableParagraphStyle : NSObject
@property (nonatomic) NSInteger alignment;
@property (nonatomic) NSInteger lineBreakMode;
@property (nonatomic) CGFloat lineSpacing;
@property (nonatomic) CGFloat paragraphSpacing;
@property (nonatomic) CGFloat firstLineHeadIndent;
@property (nonatomic) CGFloat headIndent;
@property (nonatomic) CGFloat tailIndent;
@property (nonatomic) CGFloat minimumLineHeight;
@property (nonatomic) CGFloat maximumLineHeight;
@property (nonatomic) CGFloat paragraphSpacingBefore;
@property (nonatomic) float hyphenationFactor;
@end

extern NSString *const NSFontAttributeName;
extern NSString *const NSForegroundColorAttributeName;
extern NSString *const NSParagraphStyleAttributeName;
extern NSString *const NSShadowAttributeName;

typedef NSUInteger NSStringDrawingOptions;
enum {
    NSStringDrawingUsesLineFragmentOrigin = (1 << 0),
    NSStringDrawingUsesFontLeading = (1 << 1),
    NSStringDrawingUsesDeviceMetrics = (1 << 2),
    NSStringDrawingTruncatesLastVisibleLine = (1 << 3),
};

@interface NSString (UIStringDrawing)
- (CGSize)sizeWithAttributes:(NSDictionary *)attributes;
- (CGSize)sizeWithFont:(UIFont *)font;
- (CGRect)boundingRectWithSize:(CGSize)size options:(NSStringDrawingOptions)options attributes:(NSDictionary *)attributes context:(void *)context;
- (void)drawInRect:(CGRect)rect withAttributes:(NSDictionary *)attributes;
- (void)drawAtPoint:(CGPoint)point withAttributes:(NSDictionary *)attributes;
@end

@interface NSAttributedString (UIStringDrawing2)
- (CGSize)sizeWithAttributes:(NSDictionary *)attributes;
- (CGRect)boundingRectWithSize:(CGSize)size options:(NSStringDrawingOptions)options context:(void *)context;
- (void)drawInRect:(CGRect)rect;
- (void)drawAtPoint:(CGPoint)point;
@end
