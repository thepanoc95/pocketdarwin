#import <Foundation/Foundation.h>
#import "KotlinKit.h"

@implementation KotlinUnit

+ (instancetype)unit {
    static KotlinUnit *sharedUnit = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedUnit = [[KotlinUnit alloc] init];
    });
    return sharedUnit;
}

@end