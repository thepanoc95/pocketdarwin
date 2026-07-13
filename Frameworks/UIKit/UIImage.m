#import <UIKit/UIImage.h>
#import <UIKit/UIColor.h>
#import <UIKit/UIScreen.h>
#import <CoreGraphics/CGImage.h>
#import <CoreGraphics/CGContext.h>
#import <CoreGraphics/CGDataProvider.h>

static NSMutableDictionary *imageCache = nil;

@implementation UIImage

+ (UIImage *)imageNamed:(NSString *)name {
    if (!imageCache)
        imageCache = [[NSMutableDictionary alloc] init];

    UIImage *cached = [imageCache objectForKey:name];
    if (cached)
        return cached;

    NSString *path = nil;
    NSArray *extensions = @[@"", @"png", @"jpg", @"jpeg", @"gif", @"tiff", @"bmp"];
    for (NSString *ext in extensions) {
        NSString *fullName = [ext length] ? [name stringByAppendingPathExtension:ext] : name;
        path = [[NSBundle mainBundle] pathForResource:fullName ofType:nil];
        if (path)
            break;
    }

    if (path) {
        UIImage *image = [[[UIImage alloc] initWithContentsOfFile:path] autorelease];
        if (image) {
            [imageCache setObject:image forKey:name];
            return image;
        }
    }

    return [[[UIImage alloc] init] autorelease];
}

+ (UIImage *)imageNamed:(NSString *)name inBundle:(NSBundle *)bundle compatibleWithTraitCollection:(UITraitCollection *)traitCollection {
    return [self imageNamed:name];
}

+ (UIImage *)imageWithContentsOfFile:(NSString *)path {
    return [[[self alloc] initWithContentsOfFile:path] autorelease];
}

+ (UIImage *)imageWithData:(NSData *)data {
    return [[[self alloc] initWithData:data] autorelease];
}

+ (UIImage *)imageWithData:(NSData *)data scale:(CGFloat)scale {
    return [[[self alloc] initWithData:data scale:scale] autorelease];
}

+ (UIImage *)imageWithCGImage:(CGImageRef)cgImage {
    return [[[self alloc] initWithCGImage:cgImage] autorelease];
}

+ (UIImage *)imageWithCGImage:(CGImageRef)cgImage scale:(CGFloat)scale orientation:(UIImageOrientation)orientation {
    return [[[self alloc] initWithCGImage:cgImage scale:scale orientation:orientation] autorelease];
}

+ (UIImage *)animatedImageWithImages:(NSArray *)images duration:(NSTimeInterval)duration {
    return [images firstObject];
}

+ (UIImage *)animatedImageNamed:(NSString *)name duration:(NSTimeInterval)duration {
    return [UIImage imageNamed:name];
}

+ (UIImage *)animatedResizableImageNamed:(NSString *)name capInsets:(UIEdgeInsets)capInsets duration:(NSTimeInterval)duration {
    return [UIImage imageNamed:name];
}

+ (UIImage *)animatedResizableImageNamed:(NSString *)name capInsets:(UIEdgeInsets)capInsets resizingMode:(UIImageResizingMode)resizingMode duration:(NSTimeInterval)duration {
    return [UIImage imageNamed:name];
}

- (instancetype)init {
    self = [super init];
    if (self) {
        _CGImage = NULL;
        _imageOrientation = UIImageOrientationUp;
        _scale = [UIScreen mainScreen].scale;
    }
    return self;
}

- (instancetype)initWithContentsOfFile:(NSString *)path {
    self = [super init];
    if (self) {
        _CGImage = NULL;
        _imageOrientation = UIImageOrientationUp;
        _scale = [UIScreen mainScreen].scale;

        NSData *data = [NSData dataWithContentsOfFile:path];
        if (data) {
            _pngData = [data retain];
        }
    }
    return self;
}

- (instancetype)initWithData:(NSData *)data {
    return [self initWithData:data scale:[UIScreen mainScreen].scale];
}

- (instancetype)initWithData:(NSData *)data scale:(CGFloat)scale {
    self = [super init];
    if (self) {
        _CGImage = NULL;
        _imageOrientation = UIImageOrientationUp;
        _scale = scale;
        _pngData = [data retain];
    }
    return self;
}

- (instancetype)initWithCGImage:(CGImageRef)cgImage {
    return [self initWithCGImage:cgImage scale:[UIScreen mainScreen].scale orientation:UIImageOrientationUp];
}

- (instancetype)initWithCGImage:(CGImageRef)cgImage scale:(CGFloat)scale orientation:(UIImageOrientation)orientation {
    self = [super init];
    if (self) {
        _CGImage = CGImageRetain(cgImage);
        _imageOrientation = orientation;
        _scale = scale;
    }
    return self;
}

- (void)dealloc {
    if (_CGImage)
        CGImageRelease(_CGImage);
    [_pngData release];
    [_jpegData release];
    [super dealloc];
}

- (CGSize)size {
    if (_CGImage) {
        return CGSizeMake(CGImageGetWidth(_CGImage) / _scale, CGImageGetHeight(_CGImage) / _scale);
    }
    return CGSizeZero;
}

- (CGImageRef)CGImage {
    return _CGImage;
}

- (UIImageOrientation)imageOrientation {
    return _imageOrientation;
}

- (CGFloat)scale {
    return _scale;
}

- (UIEdgeInsets)capInsets {
    return UIEdgeInsetsZero;
}

- (UIEdgeInsets)alignmentRectInsets {
    return UIEdgeInsetsZero;
}

- (UIImageResizingMode)resizingMode {
    return UIImageResizingModeStretch;
}

- (UIImageRenderingMode)renderingMode {
    return UIImageRenderingModeAutomatic;
}

- (UIImage *)imageWithRenderingMode:(UIImageRenderingMode)renderingMode {
    return self;
}

- (UIImage *)imageWithAlignmentRectInsets:(UIEdgeInsets)alignmentInsets {
    return self;
}

- (NSArray *)images {
    return nil;
}

- (NSTimeInterval)duration {
    return 0;
}

- (UIImage *)resizableImageWithCapInsets:(UIEdgeInsets)capInsets {
    return self;
}

- (UIImage *)resizableImageWithCapInsets:(UIEdgeInsets)capInsets resizingMode:(UIImageResizingMode)resizingMode {
    return self;
}

- (UIImage *)flippableImage {
    return self;
}

- (UIImage *)imageFlippedForRightToLeftLayoutDirection {
    return self;
}

- (UIImage *)stretchableImageWithLeftCapWidth:(NSInteger)leftCapWidth topCapHeight:(NSInteger)topCapHeight {
    return self;
}

- (NSInteger)leftCapWidth {
    return 0;
}

- (NSInteger)topCapHeight {
    return 0;
}

- (void)drawAtPoint:(CGPoint)point {
}

- (void)drawAtPoint:(CGPoint)point blendMode:(CGBlendMode)blendMode alpha:(CGFloat)alpha {
}

- (void)drawInRect:(CGRect)rect {
}

- (void)drawInRect:(CGRect)rect blendMode:(CGBlendMode)blendMode alpha:(CGFloat)alpha {
}

- (void)drawAsPatternInRect:(CGRect)rect {
}

- (UIImage *)imageWithHorizontallyFlippedOrientation {
    return self;
}

- (UIImageAsset *)imageAsset {
    return nil;
}

- (UIImage *)imageWithTintColor:(UIColor *)color {
    return self;
}

- (void)encodeWithCoder:(NSCoder *)coder {
}

- (instancetype)initWithCoder:(NSCoder *)coder {
    return [self init];
}

+ (BOOL)supportsSecureCoding {
    return YES;
}

@end
