#import <Foundation/Foundation.h>
#import "KotlinKit.h"

NSString * const KotlinVersion = @"1.0.0";

@implementation Kotlin

+ (NSString *)version {
    return KotlinVersion;
}

+ (BOOL)isKotlinClass:(id)object {
    if (object == nil) {
        return NO;
    }
    Class cls = [object class];
    while (cls) {
        NSString *className = NSStringFromClass(cls);
        if ([className hasPrefix:@"Kotlin"] || [className isEqualToString:@"Kotlin"]) {
            return YES;
        }
        cls = [cls superclass];
    }
    return NO;
}

+ (void)JIGSClassMethod:(NSString *)methodName {
    // Implementation for JIGS (Java Interface Generator for GNUstep) class method
    // This would interface with Kotlin/JVM through JIGS bridge
    NSLog(@"KotlinKit: JIGS method call - %@", methodName);
}

@end

