/*
 * NextStd NSString - Modern String Class
 */

#ifndef _NEXTSTD_NSSTRING_H_
#define _NEXTSTD_NSSTRING_H_

#include "NSTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

@class NSData;

typedef unsigned short unichar;

typedef NSUInteger NSStringEncoding;

typedef NSUInteger NSStringCompareOptions;

extern NSStringEncoding const NSUTF8StringEncoding;
extern NSStringEncoding const NSASCIIStringEncoding;
extern NSStringEncoding const NSUnicodeStringEncoding;
extern NSStringEncoding const NSProprietaryStringEncoding;

typedef struct _NSZone NSZone;
typedef struct NSRange {
    NSUInteger location;
    NSUInteger length;
} NSRange;

@interface NSString : NSObject

+ (instancetype)string;
+ (instancetype)stringWithUTF8String:(const char *)utf8;
+ (instancetype)stringWithCString:(const char *)cString;
+ (instancetype)stringWithFormat:(NSString *)format, ...;

- (instancetype)initWithString:(NSString *)string;
- (instancetype)initWithUTF8String:(const char *)utf8;
- (instancetype)initWithFormat:(NSString *)format, ...;

- (NSUInteger)length;
- (unichar)characterAtIndex:(NSUInteger)index;

- (const char *)UTF8String;
- (BOOL)isEqualToString:(NSString *)string;

- (NSComparisonResult)compare:(NSString *)other;
- (BOOL)hasPrefix:(NSString *)prefix;
- (BOOL)hasSuffix:(NSString *)suffix;

- (NSString *)stringByAppendingString:(NSString *)string;
- (NSString *)stringByAppendingFormat:(NSString *)format, ...;
- (NSString *)stringByReplacingOccurrencesOfString:(NSString *)target withString:(NSString *)replacement;

- (NSArray *)componentsSeparatedByString:(NSString *)separator;

- (NSData *)dataUsingEncoding:(NSStringEncoding)encoding;

@end

@interface NSMutableString : NSString
- (void)appendString:(NSString *)string;
- (void)appendFormat:(NSString *)format, ...;
- (void)setString:(NSString *)string;
@end

#ifdef __cplusplus

inline NSString * operator+(const NSString *lhs, const NSString *rhs) {
    return [lhs stringByAppendingString:rhs];
}

inline bool operator==(const NSString *lhs, const NSString *rhs) {
    return [lhs isEqualToString:rhs];
}

inline bool operator!=(const NSString *lhs, const NSString *rhs) {
    return ![lhs isEqualToString:rhs];
}

inline NSString *NSStringFromCString(const char *cstr) {
    return [NSString stringWithCString:cstr];
}

inline NSString *NSStringFromStdString(const std::string &str) {
    return [NSString stringWithUTF8String:str.c_str()];
}

#endif

#endif /* _NEXTSTD_NSSTRING_H_ */