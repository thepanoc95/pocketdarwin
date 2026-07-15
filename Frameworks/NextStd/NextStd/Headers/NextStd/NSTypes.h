/*
 * NextStd Types
 */

#ifndef _NEXTSTD_NSTYPES_H_
#define _NEXTSTD_NSTYPES_H_

#include <stdint.h>
#include <stdbool.h>
#include <objc/objc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef intptr_t NSInteger;
typedef uintptr_t NSUInteger;
typedef float CGFloat;

typedef enum NSComparisonResult {
    NSOrderedAscending = -1,
    NSOrderedSame = 0,
    NSOrderedDescending = 1
} NSComparisonResult;

#define NSNotFound (NSInteger)(0x7FFFFFFF)
#define NSUIntegerMax (18446744073709551615ULL)

extern NSString *NSStringFromClass(Class cls);
extern Class NSClassFromString(NSString *name);

#ifdef __cplusplus
}
#endif

#endif /* _NEXTSTD_NSTYPES_H_ */