/*
 * NextStd NSObject Implementation
 */

#include "NSObject.h"
#include "NSString.h"

@implementation NSObject

+ (instancetype)alloc {
    return NSAllocateObject(self, 0, NULL);
}

+ (instancetype)new {
    return [[self alloc] init];
}

- (instancetype)init {
    return self;
}

- (void)dealloc {
    NSDeallocateObject(self);
}

- (BOOL)isKindOfClass:(Class)cls {
    Class current = object_getClass(self);
    while (current) {
        if (current == cls) return YES;
        current = class_getSuperclass(current);
    }
    return NO;
}

- (BOOL)isMemberOfClass:(Class)cls {
    return object_getClass(self) == cls;
}

- (BOOL)respondsToSelector:(SEL)selector {
    return class_respondsToSelector(object_getClass(self), selector);
}

- (BOOL)conformsToProtocol:(Protocol *)protocol {
    return class_conformsToProtocol(object_getClass(self), protocol);
}

- (NSUInteger)hash {
    return (NSUInteger)self >> 4;
}

- (BOOL)isEqual:(id)object {
    return self == object;
}

- (NSString *)description {
    return [NSString stringWithFormat:@"<%@ 0x%lx>", NSStringFromClass(object_getClass(self)), (unsigned long)self];
}

- (NSString *)debugDescription {
    return [self description];
}

- (id)performSelector:(SEL)selector {
    IMP imp = class_getMethodImplementation(object_getClass(self), selector);
    return ((id (*)(id, SEL))imp)(self, selector);
}

- (id)performSelector:(SEL)selector withObject:(id)object {
    IMP imp = class_getMethodImplementation(object_getClass(self), selector);
    return ((id (*)(id, SEL, id))imp)(self, selector, object);
}

- (id)copy {
    return [(id <NSCopying>)self copyWithZone:NULL];
}

- (id)copyWithZone:(NSZone *)zone {
    return [[self class] new];
}

- (id)mutableCopy {
    return [self copy];
}

@end