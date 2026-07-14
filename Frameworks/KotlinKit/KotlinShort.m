#import <Foundation/Foundation.h>
#import "KotlinKit.h"

@interface KotlinShort ()
@property (nonatomic, assign) int16_t shortValue;
@end

@implementation KotlinShort

+ (instancetype)shortWithInt16:(int16_t)value {
    return [[self alloc] initWithInt16:value];
}

+ (instancetype)shortWithNumber:(NSNumber *)value {
    return [[self alloc] initWithNumber:value];
}

- (instancetype)initWithInt16:(int16_t)value {
    self = [super init];
    if (self) {
        _shortValue = value;
    }
    return self;
}

- (instancetype)initWithNumber:(NSNumber *)value {
    self = [super init];
    if (self) {
        int32_t intValue = [value intValue];
        // Clamp to short range (-32768 to 32767)
        if (intValue > 32767) intValue = 32767;
        if (intValue < -32768) intValue = -32768;
        _shortValue = (int16_t)intValue;
    }
    return self;
}

- (int16_t)shortValue {
    return _shortValue;
}

- (NSNumber *)toNSNumber {
    return @(_shortValue);
}

@end