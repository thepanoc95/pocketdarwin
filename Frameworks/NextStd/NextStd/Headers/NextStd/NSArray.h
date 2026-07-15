/*
 * NextStd NSArray - Modern Array Class
 */

#ifndef _NEXTSTD_NSARRAY_H_
#define _NEXTSTD_NSARRAY_H_

#include "NSTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

@class NSEnumerator;

@interface NSArray : NSObject

+ (instancetype)array;
+ (instancetype)arrayWithObject:(id)object;
+ (instancetype)arrayWithObjects:(id)firstObject, ... nil;
+ (instancetype)arrayWithArray:(NSArray *)array;
#ifdef __cplusplus
+ (instancetype)arrayWithObjects:(std::initializer_list<id>)objects;
#endif

- (instancetype)initWithObjects:(id)firstObject, ... nil;
- (NSUInteger)count;

- (id)objectAtIndex:(NSUInteger)index;
- (id)firstObject;
- (id)lastObject;

- (NSUInteger)indexOfObject:(id)object;
- (BOOL)containsObject:(id)object;

- (NSArray *)arrayByAddingObject:(id)object;
- (NSArray *)arrayByAddingObjectsFromArray:(NSArray *)other;

- (NSEnumerator *)objectEnumerator;

@end

@interface NSMutableArray : NSArray
- (void)addObject:(id)object;
- (void)removeObjectAtIndex:(NSUInteger)index;
- (void)removeLastObject;
- (void)insertObject:(id)object atIndex:(NSUInteger)index;
- (void)replaceObjectAtIndex:(NSUInteger)index withObject:(id)object;
- (void)removeAllObjects;
- (void)removeObject:(id)object;
@end

#ifdef __cplusplus
inline NSArray *operator+(const NSArray *lhs, const NSArray *rhs) {
    return [lhs arrayByAddingObjectsFromArray:rhs];
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* _NEXTSTD_NSARRAY_H_ */