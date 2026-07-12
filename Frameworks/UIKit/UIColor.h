#import <Foundation/Foundation.h>
#import <UIKit/UIKitExport.h>
#import <CoreGraphics/CGColor.h>
#import <CoreGraphics/CGGeometry.h>

@class UIImage;

typedef CGFloat (^UIColorPatternImage)(CGRect rect);

@interface UIColor : NSObject <NSCoding, NSCopying> {
    CGColorRef _cgColor;
}

+ (UIColor *)colorWithRed:(CGFloat)red green:(CGFloat)green blue:(CGFloat)blue alpha:(CGFloat)alpha;
+ (UIColor *)colorWithWhite:(CGFloat)white alpha:(CGFloat)alpha;
+ (UIColor *)colorWithHue:(CGFloat)hue saturation:(CGFloat)saturation brightness:(CGFloat)brightness alpha:(CGFloat)alpha;
+ (UIColor *)colorWithCGColor:(CGColorRef)cgColor;
+ (UIColor *)colorWithPatternImage:(UIImage *)image;

+ (UIColor *)blackColor;
+ (UIColor *)darkGrayColor;
+ (UIColor *)lightGrayColor;
+ (UIColor *)whiteColor;
+ (UIColor *)grayColor;
+ (UIColor *)redColor;
+ (UIColor *)greenColor;
+ (UIColor *)blueColor;
+ (UIColor *)cyanColor;
+ (UIColor *)yellowColor;
+ (UIColor *)magentaColor;
+ (UIColor *)orangeColor;
+ (UIColor *)purpleColor;
+ (UIColor *)brownColor;
+ (UIColor *)clearColor;

+ (UIColor *)lightTextColor;
+ (UIColor *)darkTextColor;

+ (UIColor *)groupTableViewBackgroundColor;
+ (UIColor *)viewFlipsideBackgroundColor;
+ (UIColor *)scrollViewTexturedBackgroundColor;
+ (UIColor *)underPageBackgroundColor;

- (instancetype)initWithRed:(CGFloat)red green:(CGFloat)green blue:(CGFloat)blue alpha:(CGFloat)alpha;
- (instancetype)initWithWhite:(CGFloat)white alpha:(CGFloat)alpha;
- (instancetype)initWithHue:(CGFloat)hue saturation:(CGFloat)saturation brightness:(CGFloat)brightness alpha:(CGFloat)alpha;
- (instancetype)initWithCGColor:(CGColorRef)cgColor;
- (instancetype)initWithPatternImage:(UIImage *)image;

- (void)set;
- (void)setFill;
- (void)setStroke;

@property (nonatomic, readonly) CGColorRef CGColor;
@property (nonatomic, readonly) CGFloat *components;
@property (nonatomic, readonly) NSInteger numberOfComponents;

- (BOOL)getRed:(CGFloat *)red green:(CGFloat *)green blue:(CGFloat *)blue alpha:(CGFloat *)alpha;
- (BOOL)getWhite:(CGFloat *)white alpha:(CGFloat *)alpha;
- (BOOL)getHue:(CGFloat *)hue saturation:(CGFloat *)saturation brightness:(CGFloat *)brightness alpha:(CGFloat *)alpha;

@property (nonatomic, readonly) CGFloat alphaComponent;
@property (nonatomic, readonly) CGFloat whiteComponent;
@property (nonatomic, readonly) CGFloat redComponent;
@property (nonatomic, readonly) CGFloat greenComponent;
@property (nonatomic, readonly) CGFloat blueComponent;
@property (nonatomic, readonly) CGFloat hueComponent;
@property (nonatomic, readonly) CGFloat saturationComponent;
@property (nonatomic, readonly) CGFloat brightnessComponent;

@property (nonatomic, readonly) NSString *colorSpaceName;

- (UIColor *)colorWithAlphaComponent:(CGFloat)alpha;
- (UIColor *)blendedColorWithFraction:(CGFloat)fraction ofColor:(UIColor *)color;

- (BOOL)isEqual:(id)object;
- (NSUInteger)hash;

@end
