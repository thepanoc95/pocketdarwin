#import <Foundation/Foundation.h>
#import "KotlinKit.h"

@interface KotlinByte ()
@property (nonatomic, assign) int8_t byteValue;
@end

@implementation KotlinByte

+ (instancetype)byteWithInt8:(int8_t)value {
    return [[self alloc] initWithInt8:value];
}

+ (instancetype)byteWithNumber:(NSNumber *)value {
    return [[self alloc] initWithNumber:value];
}

- (instancetype)initWithInt8:(int8_t)value {
    self = [super init];
    if (self) {
        // Clamp to byte range (-128 to 127)
        _byteValue = value;
    }
    return self;
}

- (instancetype)initWithNumber:(NSNumber *)value {
    self = [super init];
    if (self) {
        int32_t intValue = [value intValue];
        // Clamp to byte range
        if (intValue > 127) intValue = 127;
        if (intValue < -128) intValue = -128;
        _byteValue = (int8_t)intValue;
    }
    return self;
}

- (int8_t)byteValue {
    return _byteValue;
}

- (NSNumber *)toNSNumber {
    return @(_byteValue);
}

@end