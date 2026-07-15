/*
 * NextStd NSString Implementation
 */

#import "NSString.h"
#import "NSData.h"
#import "NSArray.h"
#import <string.h>
#import <stdlib.h>
#import <stdio.h>
#import <stdarg.h>

@interface NSString ()
@property (nonatomic, assign) NSUInteger _length;
@property (nonatomic, assign) char *_cString;
@end

@implementation NSString

static NSStringEncoding _defaultCStringEncoding = NSUTF8StringEncoding;

+ (NSStringEncoding)defaultCStringEncoding {
    return _defaultCStringEncoding;
}

+ (instancetype)string {
    NSString *obj = [self alloc];
    obj->_length = 0;
    obj->_cString = strdup("");
    return obj;
}

+ (instancetype)stringWithUTF8String:(const char *)utf8 {
    if (!utf8) return nil;
    NSString *obj = [self alloc];
    obj->_length = strlen(utf8);
    obj->_cString = strdup(utf8);
    return obj;
}

+ (instancetype)stringWithCString:(const char *)cString {
    return [self stringWithUTF8String:cString];
}

+ (instancetype)stringWithFormat:(NSString *)format, ... {
    if (!format) return nil;
    va_list args;
    va_start(args, format);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), [format UTF8String], args);
    va_end(args);
    return [self stringWithUTF8String:buffer];
}

- (instancetype)init {
    _length = 0;
    _cString = strdup("");
    return self;
}

- (NSUInteger)length {
    return _length;
}

- (unichar)characterAtIndex:(NSUInteger)index {
    if (index >= _length) {
        [NSException raise:@"NSRangeException" format:@"index beyond length"];
    }
    return (unichar)(unsigned char)_cString[index];
}

- (void)getCharacters:(unichar *)buffer {
    NSUInteger i;
    for (i = 0; i < _length; i++) {
        buffer[i] = (unichar)(unsigned char)_cString[i];
    }
}

- (const char *)UTF8String {
    return _cString;
}

- (BOOL)isEqualToString:(NSString *)string {
    if (!string) return NO;
    return strcmp(_cString, [string UTF8String]) == 0;
}

- (NSComparisonResult)compare:(NSString *)other {
    int result = strcmp(_cString, [other UTF8String]);
    if (result < 0) return NSOrderedAscending;
    if (result > 0) return NSOrderedDescending;
    return NSOrderedSame;
}

- (BOOL)hasPrefix:(NSString *)prefix {
    NSUInteger prefixLen = [prefix length];
    if (prefixLen > _length) return NO;
    return strncmp(_cString, [prefix UTF8String], prefixLen) == 0;
}

- (BOOL)hasSuffix:(NSString *)suffix {
    NSUInteger suffixLen = [suffix length];
    if (suffixLen > _length) return NO;
    return strcmp(_cString + _length - suffixLen, [suffix UTF8String]) == 0;
}

- (NSString *)stringByAppendingString:(NSString *)string {
    if (!string) return self;
    NSString *result = [NSString alloc];
    result->_length = _length + [string length];
    result->_cString = malloc(_length + [string length] + 1);
    strcpy(result->_cString, _cString);
    strcat(result->_cString, [string UTF8String]);
    return result;
}

- (NSString *)stringByAppendingFormat:(NSString *)format, ... {
    if (!format) return self;
    va_list args;
    va_start(args, format);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), [format UTF8String], args);
    va_end(args);
    return [self stringByAppendingString:[NSString stringWithUTF8String:buffer]];
}

- (NSString *)stringByReplacingOccurrencesOfString:(NSString *)target withString:(NSString *)replacement {
    return self;
}

- (NSArray *)componentsSeparatedByString:(NSString *)separator {
    return [NSArray arrayWithObject:self];
}

- (NSData *)dataUsingEncoding:(NSStringEncoding)encoding {
    return [NSData dataWithBytes:_cString length:_length];
}

- (NSString *)description {
    return [NSString stringWithFormat:@"%@", _cString];
}

- (void)dealloc {
    free(_cString);
    NSDeallocateObject(self);
}

@end

@implementation NSMutableString

- (void)appendString:(NSString *)string {
    if (!string) return;
    NSUInteger newLen = _length + [string length];
    char *newStr = realloc(_cString, newLen + 1);
    if (newStr) {
        _cString = newStr;
        strcat(_cString, [string UTF8String]);
        _length = newLen;
    }
}

- (void)appendFormat:(NSString *)format, ... {
    if (!format) return;
    va_list args;
    va_start(args, format);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), [format UTF8String], args);
    va_end(args);
    [self appendString:[NSString stringWithUTF8String:buffer]];
}

- (void)setString:(NSString *)string {
    if (!string) return;
    free(_cString);
    _length = [string length];
    _cString = strdup([string UTF8String]);
}

@end