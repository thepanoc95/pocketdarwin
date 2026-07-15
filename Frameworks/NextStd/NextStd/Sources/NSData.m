/*
 * NextStd NSData Implementation
 */

#import "NSData.h"
#import <string.h>
#import <stdlib.h>

@interface NSData ()
@property (nonatomic, assign) NSUInteger _length;
@property (nonatomic, assign) unsigned char *_bytes;
@end

@implementation NSData

+ (instancetype)data {
    NSData *obj = [self alloc];
    obj->_length = 0;
    obj->_bytes = NULL;
    return obj;
}

+ (instancetype)dataWithBytes:(const void *)bytes length:(NSUInteger)length {
    NSData *obj = [self alloc];
    obj->_length = length;
    obj->_bytes = (unsigned char *)malloc(length);
    memcpy(obj->_bytes, bytes, length);
    return obj;
}

+ (instancetype)dataWithBytesNoCopy:(void *)bytes length:(NSUInteger)length {
    NSData *obj = [self alloc];
    obj->_length = length;
    obj->_bytes = (unsigned char *)bytes;
    return obj;
}

- (instancetype)initWithBytes:(const void *)bytes length:(NSUInteger)length {
    _length = length;
    _bytes = (unsigned char *)malloc(length);
    memcpy(_bytes, bytes, length);
    return self;
}

- (instancetype)initWithData:(NSData *)data {
    return [self initWithBytes:[data bytes] length:[data length]];
}

- (NSUInteger)length {
    return _length;
}

- (const void *)bytes {
    return _bytes;
}

- (NSData *)subdataWithRange:(NSRange)range {
    if (range.location + range.length > _length) return nil;
    return [NSData dataWithBytes:_bytes + range.location length:range.length];
}

- (void)dealloc {
    free(_bytes);
    NSDeallocateObject(self);
}

@end

@implementation NSMutableData

- (void)appendBytes:(const void *)bytes length:(NSUInteger)length {
    unsigned char *newBytes = (unsigned char *)realloc(_bytes, _length + length);
    if (newBytes) {
        _bytes = newBytes;
        memcpy(_bytes + _length, bytes, length);
        _length += length;
    }
}

- (void)appendData:(NSData *)data {
    [self appendBytes:[data bytes] length:[data length]];
}

- (void)setLength:(NSUInteger)length {
    unsigned char *newBytes = (unsigned char *)realloc(_bytes, length);
    if (newBytes) {
        _bytes = newBytes;
        _length = length;
    }
}

@end