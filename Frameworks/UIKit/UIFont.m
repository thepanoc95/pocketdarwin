#import <UIKit/UIFont.h>

NSString *const UIFontTextStyleHeadline     = @"UICTFontTextStyleHeadline";
NSString *const UIFontTextStyleBody         = @"UICTFontTextStyleBody";
NSString *const UIFontTextStyleSubheadline  = @"UICTFontTextStyleSubheadline";
NSString *const UIFontTextStyleFootnote     = @"UICTFontTextStyleFootnote";
NSString *const UIFontTextStyleCaption1     = @"UICTFontTextStyleCaption1";
NSString *const UIFontTextStyleCaption2     = @"UICTFontTextStyleCaption2";
NSString *const UIFontTextStyleTitle1       = @"UICTFontTextStyleTitle1";
NSString *const UIFontTextStyleTitle2       = @"UICTFontTextStyleTitle2";
NSString *const UIFontTextStyleTitle3       = @"UICTFontTextStyleTitle3";
NSString *const UIFontTextStyleCallout      = @"UICTFontTextStyleCallout";
NSString *const UIFontTextStyleLargeTitle   = @"UICTFontTextStyleLargeTitle";

NSString *const UIFontDescriptorFamilyAttribute           = @"NSFontFamilyAttribute";
NSString *const UIFontDescriptorNameAttribute             = @"NSFontNameAttribute";
NSString *const UIFontDescriptorPointSizeAttribute        = @"NSFontPointSizeAttribute";
NSString *const UIFontDescriptorSizeAttribute             = @"NSFontSizeAttribute";
NSString *const UIFontDescriptorMatrixAttribute           = @"NSFontMatrixAttribute";
NSString *const UIFontDescriptorCharacterSetAttribute     = @"NSFontCharacterSetAttribute";
NSString *const UIFontDescriptorTraitsAttribute           = @"NSFontTraitsAttribute";
NSString *const UIFontDescriptorFaceAttribute             = @"NSFontFaceAttribute";
NSString *const UIFontDescriptorVisibleNameAttribute      = @"NSFontVisibleNameAttribute";

@implementation UIFont

+ (UIFont *)fontWithName:(NSString *)fontName size:(CGFloat)fontSize {
    return [[[self alloc] init] autorelease];
}

+ (UIFont *)fontWithDescriptor:(UIFontDescriptor *)fontDescriptor size:(CGFloat)pointSize {
    return [[[self alloc] init] autorelease];
}

+ (NSArray *)familyNames {
    return @[@"Helvetica", @"Helvetica Neue", @"Times New Roman", @"Courier", @"Arial", @"Georgia"];
}

+ (NSArray *)fontNamesForFamilyName:(NSString *)familyName {
    return @[familyName];
}

+ (UIFont *)systemFontOfSize:(CGFloat)fontSize {
    return [self fontWithName:@"Helvetica" size:fontSize];
}

+ (UIFont *)boldSystemFontOfSize:(CGFloat)fontSize {
    return [self fontWithName:@"Helvetica-Bold" size:fontSize];
}

+ (UIFont *)italicSystemFontOfSize:(CGFloat)fontSize {
    return [self fontWithName:@"Helvetica-Oblique" size:fontSize];
}

+ (UIFont *)preferredFontForTextStyle:(NSString *)style {
    if ([style isEqualToString:UIFontTextStyleLargeTitle])
        return [self systemFontOfSize:34];
    if ([style isEqualToString:UIFontTextStyleTitle1])
        return [self systemFontOfSize:28];
    if ([style isEqualToString:UIFontTextStyleTitle2])
        return [self systemFontOfSize:22];
    if ([style isEqualToString:UIFontTextStyleTitle3])
        return [self systemFontOfSize:20];
    if ([style isEqualToString:UIFontTextStyleHeadline])
        return [self boldSystemFontOfSize:17];
    if ([style isEqualToString:UIFontTextStyleBody])
        return [self systemFontOfSize:17];
    if ([style isEqualToString:UIFontTextStyleCallout])
        return [self systemFontOfSize:16];
    if ([style isEqualToString:UIFontTextStyleSubheadline])
        return [self systemFontOfSize:15];
    if ([style isEqualToString:UIFontTextStyleFootnote])
        return [self systemFontOfSize:13];
    if ([style isEqualToString:UIFontTextStyleCaption1])
        return [self systemFontOfSize:12];
    if ([style isEqualToString:UIFontTextStyleCaption2])
        return [self systemFontOfSize:11];
    return [self systemFontOfSize:17];
}

- (instancetype)init {
    self = [super init];
    if (self) {
        _fontName = @"Helvetica";
        _pointSize = 12;
        _ascender = _pointSize * 0.8;
        _descender = -_pointSize * 0.2;
        _leading = 2;
        _lineHeight = _ascender - _descender + _leading;
        _xHeight = _pointSize * 0.45;
        _capHeight = _pointSize * 0.7;
    }
    return self;
}

- (NSString *)fontName {
    return _fontName;
}

- (CGFloat)pointSize {
    return _pointSize;
}

- (CGFloat)ascender {
    return _ascender;
}

- (CGFloat)descender {
    return _descender;
}

- (CGFloat)leading {
    return _leading;
}

- (CGFloat)lineHeight {
    return _lineHeight;
}

- (CGFloat)xHeight {
    return _xHeight;
}

- (CGFloat)capHeight {
    return _capHeight;
}

- (UIFont *)fontWithSize:(CGFloat)fontSize {
    return [[self class] fontWithName:_fontName size:fontSize];
}

- (UIFontDescriptor *)fontDescriptor {
    NSDictionary *attributes = @{
        UIFontDescriptorNameAttribute: _fontName,
        UIFontDescriptorSizeAttribute: @(_pointSize),
    };
    return [UIFontDescriptor fontDescriptorWithFontAttributes:attributes];
}

- (BOOL)isFixedPitch {
    return [_fontName isEqualToString:@"Courier"];
}

- (NSString *)description {
    return [NSString stringWithFormat:@"<%@: 0x%p fontName: %@ pointSize: %g>", [self class], self, _fontName, _pointSize];
}

@end

@implementation UIFontDescriptor

+ (UIFontDescriptor *)fontDescriptorWithFontAttributes:(NSDictionary *)attributes {
    return [[[self alloc] initWithFontAttributes:attributes] autorelease];
}

+ (UIFontDescriptor *)fontDescriptorWithName:(NSString *)fontName size:(CGFloat)size {
    return [self fontDescriptorWithFontAttributes:@{
        UIFontDescriptorNameAttribute: fontName,
        UIFontDescriptorSizeAttribute: @(size),
    }];
}

+ (UIFontDescriptor *)fontDescriptorWithName:(NSString *)fontName matrix:(CGAffineTransform)matrix {
    return [self fontDescriptorWithFontAttributes:@{
        UIFontDescriptorNameAttribute: fontName,
        UIFontDescriptorMatrixAttribute: [NSValue valueWithBytes:&matrix objCType:@encode(CGAffineTransform)],
    }];
}

- (instancetype)initWithFontAttributes:(NSDictionary *)attributes {
    self = [super init];
    if (self) {
        _fontAttributes = [attributes copy];
    }
    return self;
}

- (UIFontDescriptorSymbolicTraits)symbolicTraits {
    return 0;
}

- (NSDictionary *)fontAttributes {
    return _fontAttributes;
}

- (UIFontDescriptor *)fontDescriptorWithSize:(CGFloat)newPointSize {
    NSMutableDictionary *attrs = [[_fontAttributes mutableCopy] autorelease];
    attrs[UIFontDescriptorSizeAttribute] = @(newPointSize);
    return [[self class] fontDescriptorWithFontAttributes:attrs];
}

- (UIFontDescriptor *)fontDescriptorWithSymbolicTraits:(UIFontDescriptorSymbolicTraits)symbolicTraits {
    return self;
}

- (UIFontDescriptor *)fontDescriptorWithFamily:(NSString *)newFamily {
    NSMutableDictionary *attrs = [[_fontAttributes mutableCopy] autorelease];
    attrs[UIFontDescriptorFamilyAttribute] = newFamily;
    return [[self class] fontDescriptorWithFontAttributes:attrs];
}

- (CGAffineTransform)matrix {
    return CGAffineTransformIdentity;
}

- (id)objectForKey:(NSString *)anAttribute {
    return _fontAttributes[anAttribute];
}

- (void)dealloc {
    [_fontAttributes release];
    [super dealloc];
}

@end
