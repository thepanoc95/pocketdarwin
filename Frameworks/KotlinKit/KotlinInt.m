#import <Foundation/Foundation.h>
#import "KotlinKit.h"

@interface KotlinInt ()
@property (nonatomic, assign) int32_t intValue;
@end

@implementation KotlinInt

+ (instancetype)intWithInt32:(int32_t)value {
    return [[self alloc] initWithInt32:value];
}

+ (instancetype)intWithNumber:(NSNumber *)value {
    return [[self alloc] initWithNumber:value];
}

+ (instancetype)intWithString:(NSString *)value {
    return [[self alloc] initWithString:value];
}

- (instancetype)initWithInt32:(int32_t)value {
    self = [super init];
    if (self) {
        _intValue = value;
    }
    return self;
}

- (instancetype)initWithNumber:(NSNumber *)value {
    self = [super init];
    if (self) {
        _intValue = [value intValue];
    }
    return self;
}

- (instancetype)initWithString:(NSString *)value {
    self = [super init];
    if (self) {
        _intValue = value.intValue;
    }
    return self;
}

- (int32_t)intValue {
    return _intValue;
}

- (NSNumber *)toNSNumber {
    return @(_intValue);
}

- (NSString *)toString {
    return [NSString stringWithFormat:@"%d", _intValue];
}

@end