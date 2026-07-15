/*
 * NextStd NSValue - Modern Value Wrapper
 */

#ifndef _NEXTSTD_NSVALUE_H_
#define _NEXTSTD_NSVALUE_H_

#include "NSTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

@interface NSValue : NSObject

+ (instancetype)valueWithBytes:(const void *)value objCType:(const char *)type;
+ (instancetype)valueWithNonretainedObject:(id)anObject;
+ (instancetype)valueWithPointer:(const void *)pointer;

- (instancetype)initWithBytes:(const void *)value objCType:(const char *)type;

- (const char *)objCType;
- (void)getValue:(void *)buffer;
- (const void *)pointerValue;

@end

@interface NSNumber : NSValue
+ (instancetype)numberWithInteger:(NSInteger)value;
+ (instancetype)numberWithUnsignedInteger:(NSUInteger)value;
+ (instancetype)numberWithFloat:(CGFloat)value;
+ (instancetype)numberWithDouble:(double)value;
+ (instancetype)numberWithBool:(BOOL)value;

- (NSInteger)integerValue;
- (NSUInteger)unsignedIntegerValue;
- (CGFloat)floatValue;
- (double)doubleValue;
- (BOOL)boolValue;

- (NSString *)stringValue;
@end

#ifdef __cplusplus
inline NSNumber *NSNumberFromInt(int value) {
    return [NSNumber numberWithInteger:value];
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* _NEXTSTD_NSVALUE_H_ */