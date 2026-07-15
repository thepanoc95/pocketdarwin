/*
 * NextStd NSObject - Modern Foundation Base Class
 */

#ifndef _NEXTSTD_NSOBJECT_H_
#define _NEXTSTD_NSOBJECT_H_

#include "NSTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

@class NSString;

@interface NSObject <NSCopying>
- (instancetype)init;
+ (instancetype)new;
+ (instancetype)alloc;
- (void)dealloc;

- (BOOL)isKindOfClass:(Class)cls;
- (BOOL)isMemberOfClass:(Class)cls;
- (BOOL)respondsToSelector:(SEL)selector;
- (BOOL)conformsToProtocol:(Protocol *)protocol;

- (NSUInteger)hash;
- (BOOL)isEqual:(id)object;

- (NSString *)description;
- (NSString *)debugDescription;

- (id)performSelector:(SEL)selector;
- (id)performSelector:(SEL)selector withObject:(id)object;

@end

@interface NSObject (NSCopying)
- (id)copy;
- (id)copyWithZone:(struct _NSZone *)zone;
- (id)mutableCopy;
@end

@interface NSObject (NSCoding)
- (void)encodeWithCoder:(id)coder;
- (instancetype)initWithCoder:(id)coder;
@end

#ifdef __cplusplus
}
#endif

#endif /* _NEXTSTD_NSOBJECT_H_ */