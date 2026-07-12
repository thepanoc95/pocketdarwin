#import <Foundation/Foundation.h>
#import <UIKit/UIKitExport.h>
#import <CoreGraphics/CGGeometry.h>

@class UIView;

@interface UIScreen : NSObject {
    CGRect _bounds;
    CGRect _applicationFrame;
    CGFloat _scale;
    CGFloat _brightness;
    BOOL _wantsSoftwareDimming;
}

+ (UIScreen *)mainScreen;
+ (NSArray *)screens;

@property (nonatomic, readonly) CGRect bounds;
@property (nonatomic, readonly) CGRect applicationFrame;
@property (nonatomic, readonly) CGFloat scale;
@property (nonatomic, readonly) CGFloat nativeScale;
@property (nonatomic, readonly) CGSize nativeBounds;
@property (nonatomic, readonly) CGFloat maximumFramesPerSecond;

+ (CGFloat)scale;

@property (nonatomic) CGFloat brightness;
@property (nonatomic) BOOL wantsSoftwareDimming;

@end
