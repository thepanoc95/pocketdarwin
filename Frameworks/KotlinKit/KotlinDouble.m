#import <Foundation/Foundation.h>
#import "KotlinKit.h"

@interface KotlinDouble ()
@property (nonatomic, assign) double doubleValue;
@end

@implementation KotlinDouble

+ (instancetype)doubleWithDouble:(double)value {
    return [[self alloc] initWithDouble:value];
}

+ (instancetype)doubleWithNumber:(NSNumber *)value {
    return [[self alloc] initWithNumber:value];
}

+ (instancetype)doubleWithString:(NSString *)value {
    return [[self alloc] initWithString:value];
}

- (instancetype)initWithDouble:(double)value {
    self = [super init];
    if (self) {
        _doubleValue = value;
    }
    return self;
}

- (instancetype)initWithNumber:(NSNumber *)value {
    self = [super init];
    if (self) {
        _doubleValue = [value doubleValue];
    }
    return self;
}

- (instancetype)initWithString:(NSString *)value {
    self = [super init];
    if (self) {
        _doubleValue = value.doubleValue;
    }
    return self;
}

- (double)doubleValue {
    return _doubleValue;
}

- (NSNumber *)toNSNumber {
    return @(_doubleValue);
}

- (NSString *)toString {
    return [NSString stringWithFormat:@"%f", _doubleValue];
}

@end