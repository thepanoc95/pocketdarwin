/*
 * NextStd NSDictionary - Modern Dictionary Class
 */

#ifndef _NEXTSTD_NSDICTIONARY_H_
#define _NEXTSTD_NSDICTIONARY_H_

#include "NSTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

@class NSEnumerator;

@interface NSDictionary : NSObject

+ (instancetype)dictionary;
+ (instancetype)dictionaryWithObject:(id)value forKey:(id)key;
+ (instancetype)dictionaryWithObjects:(id)first, forKeys:(id)firstKey, ...;

- (instancetype)initWithDictionary:(NSDictionary *)dict;

- (NSUInteger)count;
- (NSArray *)allKeys;
- (NSArray *)allValues;

- (id)objectForKey:(id)key;
- (id)objectForKeyedSubscript:(id)key;

- (NSEnumerator *)keyEnumerator;
- (NSEnumerator *)objectEnumerator;

@end

@interface NSMutableDictionary : NSDictionary
- (void)setObject:(id)object forKey:(id)key;
- (void)setObject:(id)object forKeyedSubscript:(id)key;
- (void)removeObjectForKey:(id)key;
- (void)removeAllObjects;
@end

#ifdef __cplusplus
}
#endif

#endif /* _NEXTSTD_NSDICTIONARY_H_ */