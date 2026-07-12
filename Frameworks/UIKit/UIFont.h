#import <Foundation/Foundation.h>
#import <UIKit/UIKitExport.h>
#import <CoreGraphics/CGGeometry.h>

@interface UIFont : NSObject {
    NSString *_fontName;
    CGFloat _pointSize;
    CGFloat _ascender;
    CGFloat _descender;
    CGFloat _leading;
    CGFloat _lineHeight;
    CGFloat _xHeight;
    CGFloat _capHeight;
}

+ (UIFont *)fontWithName:(NSString *)fontName size:(CGFloat)fontSize;
+ (UIFont *)fontWithDescriptor:(UIFontDescriptor *)fontDescriptor size:(CGFloat)pointSize;

+ (NSArray *)familyNames;
+ (NSArray *)fontNamesForFamilyName:(NSString *)familyName;

+ (UIFont *)systemFontOfSize:(CGFloat)fontSize;
+ (UIFont *)boldSystemFontOfSize:(CGFloat)fontSize;
+ (UIFont *)italicSystemFontOfSize:(CGFloat)fontSize;

+ (UIFont *)preferredFontForTextStyle:(NSString *)style;

@property (nonatomic, readonly) NSString *fontName;
@property (nonatomic, readonly) CGFloat pointSize;
@property (nonatomic, readonly) CGFloat ascender;
@property (nonatomic, readonly) CGFloat descender;
@property (nonatomic, readonly) CGFloat leading;
@property (nonatomic, readonly) CGFloat lineHeight;
@property (nonatomic, readonly) CGFloat xHeight;
@property (nonatomic, readonly) CGFloat capHeight;

- (UIFont *)fontWithSize:(CGFloat)fontSize;

- (UIFontDescriptor *)fontDescriptor;

@property (nonatomic, readonly, getter=isFixedPitch) BOOL fixedPitch;

@end

@interface UIFont (UIFontTextStyle)

UIKIT_EXPORT NSString *const UIFontTextStyleHeadline;
UIKIT_EXPORT NSString *const UIFontTextStyleBody;
UIKIT_EXPORT NSString *const UIFontTextStyleSubheadline;
UIKIT_EXPORT NSString *const UIFontTextStyleFootnote;
UIKIT_EXPORT NSString *const UIFontTextStyleCaption1;
UIKIT_EXPORT NSString *const UIFontTextStyleCaption2;
UIKIT_EXPORT NSString *const UIFontTextStyleTitle1;
UIKIT_EXPORT NSString *const UIFontTextStyleTitle2;
UIKIT_EXPORT NSString *const UIFontTextStyleTitle3;
UIKIT_EXPORT NSString *const UIFontTextStyleCallout;
UIKIT_EXPORT NSString *const UIFontTextStyleLargeTitle;

@end

@interface UIFontDescriptor : NSObject

+ (UIFontDescriptor *)fontDescriptorWithFontAttributes:(NSDictionary *)attributes;
+ (UIFontDescriptor *)fontDescriptorWithName:(NSString *)fontName size:(CGFloat)size;
+ (UIFontDescriptor *)fontDescriptorWithName:(NSString *)fontName matrix:(CGAffineTransform)matrix;

- (instancetype)initWithFontAttributes:(NSDictionary *)attributes;

@property (nonatomic, readonly) UIFontDescriptorSymbolicTraits symbolicTraits;
@property (nonatomic, readonly) NSDictionary *fontAttributes;

- (UIFontDescriptor *)fontDescriptorWithSize:(CGFloat)newPointSize;
- (UIFontDescriptor *)fontDescriptorWithSymbolicTraits:(UIFontDescriptorSymbolicTraits)symbolicTraits;
- (UIFontDescriptor *)fontDescriptorWithFamily:(NSString *)newFamily;

@property (nonatomic, readonly) CGAffineTransform matrix;

- (id)objectForKey:(NSString *)anAttribute;

@end

typedef NSInteger UIFontDescriptorSymbolicTraits;

enum {
    UIFontDescriptorTraitItalic       = 1u << 0,
    UIFontDescriptorTraitBold         = 1u << 1,
    UIFontDescriptorTraitExpanded     = 1u << 5,
    UIFontDescriptorTraitCondensed    = 1u << 6,
    UIFontDescriptorTraitMonoSpace    = 1u << 10,
    UIFontDescriptorTraitVertical     = 1u << 11,
    UIFontDescriptorTraitUIOptimized  = 1u << 12,
    UIFontDescriptorTraitTightLeading = 1u << 15,
    UIFontDescriptorTraitLooseLeading = 1u << 16,
};

UIKIT_EXPORT NSString *const UIFontDescriptorFamilyAttribute;
UIKIT_EXPORT NSString *const UIFontDescriptorNameAttribute;
UIKIT_EXPORT NSString *const UIFontDescriptorPointSizeAttribute;
UIKIT_EXPORT NSString *const UIFontDescriptorSizeAttribute;
UIKIT_EXPORT NSString *const UIFontDescriptorMatrixAttribute;
UIKIT_EXPORT NSString *const UIFontDescriptorCharacterSetAttribute;
UIKIT_EXPORT NSString *const UIFontDescriptorTraitsAttribute;
UIKIT_EXPORT NSString *const UIFontDescriptorFaceAttribute;
UIKIT_EXPORT NSString *const UIFontDescriptorVisibleNameAttribute;
