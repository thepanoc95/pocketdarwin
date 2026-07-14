#import <Foundation/Foundation.h>
#import "KotlinKit.h"

@interface KotlinChar ()
@property (nonatomic, assign) unichar charValue;
@end

@implementation KotlinChar

+ (instancetype)charWithUnichar:(unichar)value {
    return [[self alloc] initWithUnichar:value];
}

+ (instancetype)charWithString:(NSString *)value {
    return [[self alloc] initWithString:value];
}

- (instancetype)initWithUnichar:(unichar)value {
    self = [super init];
    if (self) {
        _charValue = value;
    }
    return self;
}

- (instancetype)initWithString:(NSString *)value {
    self = [super init];
    if (self && value.length > 0) {
        _charValue = [value characterAtIndex:0];
    }
    return self;
}

- (unichar)charValue {
    return _charValue;
}

- (NSString *)toString {
    return [NSString stringWithCharacters:&_charValue length:1];
}

@end