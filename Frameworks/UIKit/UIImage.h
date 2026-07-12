#import <Foundation/Foundation.h>
#import <UIKit/UIKitExport.h>
#import <CoreGraphics/CGGeometry.h>
#import <CoreGraphics/CGImage.h>

@class UIImageAsset;

typedef CGFloat UIImageResizingMode;

enum {
    UIImageResizingModeTile,
    UIImageResizingModeStretch,
};

typedef NSInteger UIImageRenderingMode;

enum {
    UIImageRenderingModeAutomatic,
    UIImageRenderingModeAlwaysOriginal,
    UIImageRenderingModeAlwaysTemplate,
};

typedef NSInteger UIImageOrientation;

enum {
    UIImageOrientationUp,
    UIImageOrientationDown,
    UIImageOrientationLeft,
    UIImageOrientationRight,
    UIImageOrientationUpMirrored,
    UIImageOrientationDownMirrored,
    UIImageOrientationLeftMirrored,
    UIImageOrientationRightMirrored,
};

@interface UIImage : NSObject <NSSecureCoding> {
    CGImageRef _CGImage;
    UIImageOrientation _imageOrientation;
    CGFloat _scale;
    NSData *_pngData;
    NSData *_jpegData;
}

+ (UIImage *)imageNamed:(NSString *)name;
+ (UIImage *)imageNamed:(NSString *)name inBundle:(NSBundle *)bundle compatibleWithTraitCollection:(UITraitCollection *)traitCollection;

+ (UIImage *)imageWithContentsOfFile:(NSString *)path;
+ (UIImage *)imageWithData:(NSData *)data;
+ (UIImage *)imageWithData:(NSData *)data scale:(CGFloat)scale;
+ (UIImage *)imageWithCGImage:(CGImageRef)cgImage;
+ (UIImage *)imageWithCGImage:(CGImageRef)cgImage scale:(CGFloat)scale orientation:(UIImageOrientation)orientation;

+ (UIImage *)animatedImageWithImages:(NSArray *)images duration:(NSTimeInterval)duration;
+ (UIImage *)animatedImageNamed:(NSString *)name duration:(NSTimeInterval)duration;
+ (UIImage *)animatedResizableImageNamed:(NSString *)name capInsets:(UIEdgeInsets)capInsets duration:(NSTimeInterval)duration;
+ (UIImage *)animatedResizableImageNamed:(NSString *)name capInsets:(UIEdgeInsets)capInsets resizingMode:(UIImageResizingMode)resizingMode duration:(NSTimeInterval)duration;

- (instancetype)initWithContentsOfFile:(NSString *)path;
- (instancetype)initWithData:(NSData *)data;
- (instancetype)initWithData:(NSData *)data scale:(CGFloat)scale;
- (instancetype)initWithCGImage:(CGImageRef)cgImage;
- (instancetype)initWithCGImage:(CGImageRef)cgImage scale:(CGFloat)scale orientation:(UIImageOrientation)orientation;

@property (nonatomic, readonly) CGSize size;
@property (nonatomic, readonly) CGImageRef CGImage;
@property (nonatomic, readonly) UIImageOrientation imageOrientation;
@property (nonatomic, readonly) CGFloat scale;

@property (nonatomic, readonly) UIEdgeInsets capInsets;
@property (nonatomic, readonly) UIEdgeInsets alignmentRectInsets;
@property (nonatomic, readonly) UIImageResizingMode resizingMode;

@property (nonatomic, readonly) UIImageRenderingMode renderingMode;
- (UIImage *)imageWithRenderingMode:(UIImageRenderingMode)renderingMode;

- (UIImage *)imageWithAlignmentRectInsets:(UIEdgeInsets)alignmentInsets;

@property (nonatomic, readonly) NSArray *images;
@property (nonatomic, readonly) NSTimeInterval duration;

- (UIImage *)resizableImageWithCapInsets:(UIEdgeInsets)capInsets;
- (UIImage *)resizableImageWithCapInsets:(UIEdgeInsets)capInsets resizingMode:(UIImageResizingMode)resizingMode;

@property (nonatomic, readonly) UIImage *flippableImage;
- (UIImage *)imageFlippedForRightToLeftLayoutDirection;

- (UIImage *)stretchableImageWithLeftCapWidth:(NSInteger)leftCapWidth topCapHeight:(NSInteger)topCapHeight;
@property (nonatomic, readonly) NSInteger leftCapWidth;
@property (nonatomic, readonly) NSInteger topCapHeight;

- (void)drawAtPoint:(CGPoint)point;
- (void)drawAtPoint:(CGPoint)point blendMode:(CGBlendMode)blendMode alpha:(CGFloat)alpha;
- (void)drawInRect:(CGRect)rect;
- (void)drawInRect:(CGRect)rect blendMode:(CGBlendMode)blendMode alpha:(CGFloat)alpha;
- (void)drawAsPatternInRect:(CGRect)rect;

- (UIImage *)imageWithHorizontallyFlippedOrientation;

@property (nonatomic, readonly) UIImageAsset *imageAsset;
- (UIImage *)imageWithTintColor:(UIColor *)color;

@end
