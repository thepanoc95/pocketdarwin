/*
 * NextStd NSEnumerator - Modern Enumerator Class
 */

#ifndef _NEXTSTD_NSENUMERATOR_H_
#define _NEXTSTD_NSENUMERATOR_H_

#include "NSTypes.h"

@interface NSEnumerator : NSObject
+ (instancetype)enumeratorWithObjects:(id *)objects count:(NSUInteger)count;
- (id)nextObject;
@end

#endif /* _NEXTSTD_NSENUMERATOR_H_ */