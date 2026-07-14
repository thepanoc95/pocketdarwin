#import <Foundation/Foundation.h>
#import "KotlinKit.h"

@interface KotlinFuncs ()
@end

@implementation KotlinFuncs

+ (NSString *)kotlinVersion {
    return KotlinVersion;
}

+ (BOOL)isKotlinAvailable {
    // Check if Kotlin runtime/JVM is available
    // For now, return YES as a stub
    return YES;
}

+ (id)callKotlinStaticMethod:(NSString *)className 
                  methodName:(NSString *)methodName 
                         args:(NSArray *)arguments {
    // This would interface with Kotlin through JIGS or JNI
    // For now, return nil as a stub
    NSLog(@"KotlinFuncs: Calling static method %@.%@", className, methodName);
    return nil;
}

+ (id)callKotlinInstanceMethod:(id)instance 
                     methodName:(NSString *)methodName 
                          args:(NSArray *)arguments {
    // This would interface with Kotlin through JIGS or JNI
    // For now, return nil as a stub
    NSLog(@"KotlinFuncs: Calling instance method %@ on %@", methodName, instance);
    return nil;
}

@end