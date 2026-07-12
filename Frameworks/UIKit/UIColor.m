#import <UIKit/UIColor.h>
#import <UIKit/UIImage.h>
#import <CoreGraphics/CGColor.h>
#import <CoreGraphics/CGColorSpace.h>

@implementation UIColor

+ (UIColor *)colorWithRed:(CGFloat)red green:(CGFloat)green blue:(CGFloat)blue alpha:(CGFloat)alpha {
    return [[[self alloc] initWithRed:red green:green blue:blue alpha:alpha] autorelease];
}

+ (UIColor *)colorWithWhite:(CGFloat)white alpha:(CGFloat)alpha {
    return [[[self alloc] initWithWhite:white alpha:alpha] autorelease];
}

+ (UIColor *)colorWithHue:(CGFloat)hue saturation:(CGFloat)saturation brightness:(CGFloat)brightness alpha:(CGFloat)alpha {
    return [[[self alloc] initWithHue:hue saturation:saturation brightness:brightness alpha:alpha] autorelease];
}

+ (UIColor *)colorWithCGColor:(CGColorRef)cgColor {
    return [[[self alloc] initWithCGColor:cgColor] autorelease];
}

+ (UIColor *)colorWithPatternImage:(UIImage *)image {
    return [[[self alloc] initWithPatternImage:image] autorelease];
}

static UIColor *BlackColor = nil;
static UIColor *WhiteColor = nil;
static UIColor *RedColor = nil;
static UIColor *GreenColor = nil;
static UIColor *BlueColor = nil;
static UIColor *ClearColor = nil;
static UIColor *GrayColor = nil;
static UIColor *DarkGrayColor = nil;
static UIColor *LightGrayColor = nil;
static UIColor *CyanColor = nil;
static UIColor *YellowColor = nil;
static UIColor *MagentaColor = nil;
static UIColor *OrangeColor = nil;
static UIColor *PurpleColor = nil;
static UIColor *BrownColor = nil;

+ (UIColor *)blackColor {
    if (!BlackColor)
        BlackColor = [[self alloc] initWithRed:0 green:0 blue:0 alpha:1];
    return BlackColor;
}

+ (UIColor *)darkGrayColor {
    if (!DarkGrayColor)
        DarkGrayColor = [[self alloc] initWithWhite:0.333 alpha:1];
    return DarkGrayColor;
}

+ (UIColor *)lightGrayColor {
    if (!LightGrayColor)
        LightGrayColor = [[self alloc] initWithWhite:0.667 alpha:1];
    return LightGrayColor;
}

+ (UIColor *)whiteColor {
    if (!WhiteColor)
        WhiteColor = [[self alloc] initWithRed:1 green:1 blue:1 alpha:1];
    return WhiteColor;
}

+ (UIColor *)grayColor {
    if (!GrayColor)
        GrayColor = [[self alloc] initWithWhite:0.5 alpha:1];
    return GrayColor;
}

+ (UIColor *)redColor {
    if (!RedColor)
        RedColor = [[self alloc] initWithRed:1 green:0 blue:0 alpha:1];
    return RedColor;
}

+ (UIColor *)greenColor {
    if (!GreenColor)
        GreenColor = [[self alloc] initWithRed:0 green:1 blue:0 alpha:1];
    return GreenColor;
}

+ (UIColor *)blueColor {
    if (!BlueColor)
        BlueColor = [[self alloc] initWithRed:0 green:0 blue:1 alpha:1];
    return BlueColor;
}

+ (UIColor *)cyanColor {
    if (!CyanColor)
        CyanColor = [[self alloc] initWithRed:0 green:1 blue:1 alpha:1];
    return CyanColor;
}

+ (UIColor *)yellowColor {
    if (!YellowColor)
        YellowColor = [[self alloc] initWithRed:1 green:1 blue:0 alpha:1];
    return YellowColor;
}

+ (UIColor *)magentaColor {
    if (!MagentaColor)
        MagentaColor = [[self alloc] initWithRed:1 green:0 blue:1 alpha:1];
    return MagentaColor;
}

+ (UIColor *)orangeColor {
    if (!OrangeColor)
        OrangeColor = [[self alloc] initWithRed:1 green:0.5 blue:0 alpha:1];
    return OrangeColor;
}

+ (UIColor *)purpleColor {
    if (!PurpleColor)
        PurpleColor = [[self alloc] initWithRed:0.5 green:0 blue:0.5 alpha:1];
    return PurpleColor;
}

+ (UIColor *)brownColor {
    if (!BrownColor)
        BrownColor = [[self alloc] initWithRed:0.6 green:0.4 blue:0.2 alpha:1];
    return BrownColor;
}

+ (UIColor *)clearColor {
    if (!ClearColor)
        ClearColor = [[self alloc] initWithRed:0 green:0 blue:0 alpha:0];
    return ClearColor;
}

+ (UIColor *)lightTextColor {
    return [UIColor whiteColor];
}

+ (UIColor *)darkTextColor {
    return [UIColor blackColor];
}

+ (UIColor *)groupTableViewBackgroundColor {
    return [UIColor colorWithRed:0.937 green:0.937 blue:0.957 alpha:1];
}

+ (UIColor *)viewFlipsideBackgroundColor {
    return [UIColor colorWithRed:0.121 green:0.121 blue:0.141 alpha:1];
}

+ (UIColor *)scrollViewTexturedBackgroundColor {
    return [UIColor colorWithRed:0.435 green:0.435 blue:0.447 alpha:1];
}

+ (UIColor *)underPageBackgroundColor {
    return [UIColor colorWithWhite:0.176 alpha:1];
}

- (instancetype)initWithRed:(CGFloat)red green:(CGFloat)green blue:(CGFloat)blue alpha:(CGFloat)alpha {
    self = [super init];
    if (self) {
        CGFloat components[4] = {red, green, blue, alpha};
        CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();
        _cgColor = CGColorCreate(colorspace, components);
        CGColorSpaceRelease(colorspace);
    }
    return self;
}

- (instancetype)initWithWhite:(CGFloat)white alpha:(CGFloat)alpha {
    return [self initWithRed:white green:white blue:white alpha:alpha];
}

- (instancetype)initWithHue:(CGFloat)hue saturation:(CGFloat)saturation brightness:(CGFloat)brightness alpha:(CGFloat)alpha {
    CGFloat red = 0, green = 0, blue = 0;
    if (saturation == 0) {
        red = green = blue = brightness;
    } else {
        CGFloat h = hue * 6;
        int i = (int)h;
        CGFloat f = h - i;
        CGFloat p = brightness * (1 - saturation);
        CGFloat q = brightness * (1 - saturation * f);
        CGFloat t = brightness * (1 - saturation * (1 - f));
        switch (i % 6) {
            case 0: red = brightness; green = t; blue = p; break;
            case 1: red = q; green = brightness; blue = p; break;
            case 2: red = p; green = brightness; blue = t; break;
            case 3: red = p; green = q; blue = brightness; break;
            case 4: red = t; green = p; blue = brightness; break;
            case 5: red = brightness; green = p; blue = q; break;
        }
    }
    return [self initWithRed:red green:green blue:blue alpha:alpha];
}

- (instancetype)initWithCGColor:(CGColorRef)cgColor {
    self = [super init];
    if (self) {
        _cgColor = CGColorRetain(cgColor);
    }
    return self;
}

- (instancetype)initWithPatternImage:(UIImage *)image {
    return [self initWithRed:1 green:1 blue:1 alpha:1];
}

- (void)dealloc {
    if (_cgColor)
        CGColorRelease(_cgColor);
    [super dealloc];
}

- (void)set {
    [self setFill];
}

- (void)setFill {
}

- (void)setStroke {
}

- (CGColorRef)CGColor {
    return _cgColor;
}

- (CGFloat *)components {
    return (CGFloat *)CGColorGetComponents(_cgColor);
}

- (NSInteger)numberOfComponents {
    return CGColorGetNumberOfComponents(_cgColor);
}

- (BOOL)getRed:(CGFloat *)red green:(CGFloat *)green blue:(CGFloat *)blue alpha:(CGFloat *)alpha {
    const CGFloat *c = [self components];
    if ([self numberOfComponents] >= 4) {
        *red = c[0];
        *green = c[1];
        *blue = c[2];
        *alpha = c[3];
        return YES;
    }
    return NO;
}

- (BOOL)getWhite:(CGFloat *)white alpha:(CGFloat *)alpha {
    const CGFloat *c = [self components];
    if ([self numberOfComponents] >= 2) {
        *white = c[0];
        *alpha = c[1];
        return YES;
    }
    return NO;
}

- (BOOL)getHue:(CGFloat *)hue saturation:(CGFloat *)saturation brightness:(CGFloat *)brightness alpha:(CGFloat *)alpha {
    CGFloat red, green, blue;
    if (![self getRed:&red green:&green blue:&blue alpha:alpha])
        return NO;

    CGFloat max = MAX(MAX(red, green), blue);
    CGFloat min = MIN(MIN(red, green), blue);
    *brightness = max;
    *saturation = (max == 0) ? 0 : (max - min) / max;

    if (*saturation == 0) {
        *hue = 0;
    } else {
        CGFloat delta = max - min;
        if (red == max)
            *hue = (green - blue) / delta + (green < blue ? 6 : 0);
        else if (green == max)
            *hue = (blue - red) / delta + 2;
        else
            *hue = (red - green) / delta + 4;
        *hue /= 6;
    }
    return YES;
}

- (CGFloat)alphaComponent {
    const CGFloat *c = [self components];
    return c[[self numberOfComponents] - 1];
}

- (CGFloat)whiteComponent {
    const CGFloat *c = [self components];
    return c[0];
}

- (CGFloat)redComponent {
    const CGFloat *c = [self components];
    return c[0];
}

- (CGFloat)greenComponent {
    const CGFloat *c = [self components];
    return c[1];
}

- (CGFloat)blueComponent {
    const CGFloat *c = [self components];
    return c[2];
}

- (CGFloat)hueComponent {
    CGFloat h, s, b, a;
    [self getHue:&h saturation:&s brightness:&b alpha:&a];
    return h;
}

- (CGFloat)saturationComponent {
    CGFloat h, s, b, a;
    [self getHue:&h saturation:&s brightness:&b alpha:&a];
    return s;
}

- (CGFloat)brightnessComponent {
    CGFloat h, s, b, a;
    [self getHue:&h saturation:&s brightness:&b alpha:&a];
    return b;
}

- (NSString *)colorSpaceName {
    return @"NSCalibratedRGBColorSpace";
}

- (UIColor *)colorWithAlphaComponent:(CGFloat)alpha {
    CGFloat r, g, b, a;
    [self getRed:&r green:&g blue:&b alpha:&a];
    return [UIColor colorWithRed:r green:g blue:b alpha:alpha];
}

- (UIColor *)blendedColorWithFraction:(CGFloat)fraction ofColor:(UIColor *)color {
    CGFloat r1, g1, b1, a1, r2, g2, b2, a2;
    [self getRed:&r1 green:&g1 blue:&b1 alpha:&a1];
    [color getRed:&r2 green:&g2 blue:&b2 alpha:&a2];
    return [UIColor colorWithRed:r1 * (1 - fraction) + r2 * fraction
                           green:g1 * (1 - fraction) + g2 * fraction
                            blue:b1 * (1 - fraction) + b2 * fraction
                           alpha:a1 * (1 - fraction) + a2 * fraction];
}

- (BOOL)isEqual:(id)object {
    if (![object isKindOfClass:[UIColor class]])
        return NO;
    UIColor *other = (UIColor *)object;
    return CGColorEqualToColor(_cgColor, other->_cgColor);
}

- (NSUInteger)hash {
    CGFloat r, g, b, a;
    [self getRed:&r green:&g blue:&b alpha:&a];
    NSUInteger hash = (NSUInteger)(r * 255) << 24;
    hash |= (NSUInteger)(g * 255) << 16;
    hash |= (NSUInteger)(b * 255) << 8;
    hash |= (NSUInteger)(a * 255);
    return hash;
}

- (void)encodeWithCoder:(NSCoder *)coder {
}

- (instancetype)initWithCoder:(NSCoder *)coder {
    return [self initWithRed:0 green:0 blue:0 alpha:1];
}

- (id)copyWithZone:(NSZone *)zone {
    return [self retain];
}

@end
