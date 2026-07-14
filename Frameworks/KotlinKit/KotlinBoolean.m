#import <Foundation/Foundation.h>
#import "KotlinKit.h"

@interface KotlinBoolean ()
@property (nonatomic, assign) BOOL booleanValue;
@end

@implementation KotlinBoolean

+ (instancetype)TRUE {
    return [[self alloc] initWithBOOL:YES];
}

+ (instancetype)FALSE {
    return [[self alloc] initWithBOOL:NO];
}

+ (instancetype)booleanWithBOOL:(BOOL)value {
    return [[self alloc] initWithBOOL:value];
}

+ (instancetype)booleanWithValue:(NSNumber *)value {
    return [[self alloc] initWithValue:value];
}

- (instancetype)initWithBOOL:(BOOL)value {
    self = [super init];
    if (self) {
        _booleanValue = value;
    }
    return self;
}

- (instancetype)initWithValue:(NSNumber *)value {
    self = [super init];
    if (self) {
        _booleanValue = [value boolValue];
    }
    return self;
}

- (BOOL)booleanValue {
    return _booleanValue;
}

- (NSNumber *)toNSNumber {
    return @(_booleanValue);
}

@end