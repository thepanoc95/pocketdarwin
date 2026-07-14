#import <Foundation/Foundation.h>
#import "KotlinKit.h"

@interface KotlinLong ()
@property (nonatomic, assign) int64_t longValue;
@end

@implementation KotlinLong

+ (instancetype)longWithInt64:(int64_t)value {
    return [[self alloc] initWithInt64:value];
}

+ (instancetype)longWithNumber:(NSNumber *)value {
    return [[self alloc] initWithNumber:value];
}

+ (instancetype)longWithString:(NSString *)value {
    return [[self alloc] initWithString:value];
}

- (instancetype)initWithInt64:(int64_t)value {
    self = [super init];
    if (self) {
        _longValue = value;
    }
    return self;
}

- (instancetype)initWithNumber:(NSNumber *)value {
    self = [super init];
    if (self) {
        _longValue = [value longLongValue];
    }
    return self;
}

- (instancetype)initWithString:(NSString *)value {
    self = [super init];
    if (self) {
        _longValue = value.longLongValue;
    }
    return self;
}

- (int64_t)longValue {
    return _longValue;
}

- (NSNumber *)toNSNumber {
    return @(_longValue);
}

- (NSString *)toString {
    return [NSString stringWithFormat:@"%lld", _longValue];
}

@end