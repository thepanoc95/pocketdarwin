#import <Foundation/Foundation.h>
#import "KotlinKit.h"

@interface KotlinFloat ()
@property (nonatomic, assign) float floatValue;
@end

@implementation KotlinFloat

+ (instancetype)floatWithFloat:(float)value {
    return [[self alloc] initWithFloat:value];
}

+ (instancetype)floatWithNumber:(NSNumber *)value {
    return [[self alloc] initWithNumber:value];
}

+ (instancetype)floatWithString:(NSString *)value {
    return [[self alloc] initWithString:value];
}

- (instancetype)initWithFloat:(float)value {
    self = [super init];
    if (self) {
        _floatValue = value;
    }
    return self;
}

- (instancetype)initWithNumber:(NSNumber *)value {
    self = [super init];
    if (self) {
        _floatValue = [value floatValue];
    }
    return self;
}

- (instancetype)initWithString:(NSString *)value {
    self = [super init];
    if (self) {
        _floatValue = value.floatValue;
    }
    return self;
}

- (float)floatValue {
    return _floatValue;
}

- (NSNumber *)toNSNumber {
    return @(_floatValue);
}

- (NSString *)toString {
    return [NSString stringWithFormat:@"%f", _floatValue];
}

@end