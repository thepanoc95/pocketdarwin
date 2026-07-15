/*
 * NextStd NSData - Modern Data Class
 */

#ifndef _NEXTSTD_NSDATA_H_
#define _NEXTSTD_NSDATA_H_

#include "NSTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

@interface NSData : NSObject

+ (instancetype)data;
+ (instancetype)dataWithBytes:(const void *)bytes length:(NSUInteger)length;
+ (instancetype)dataWithBytesNoCopy:(void *)bytes length:(NSUInteger)length;

- (instancetype)initWithBytes:(const void *)bytes length:(NSUInteger)length;
- (instancetype)initWithData:(NSData *)data;

- (NSUInteger)length;
- (const void *)bytes;

- (NSData *)subdataWithRange:(NSRange)range;

@end

@interface NSMutableData : NSData
- (void)appendBytes:(const void *)bytes length:(NSUInteger)length;
- (void)appendData:(NSData *)data;
- (void)setLength:(NSUInteger)length;
@end

#ifdef __cplusplus
inline NSData *NSDataFromBytes(const void * _Nonnull bytes, NSUInteger length) {
    return [NSData dataWithBytes:bytes length:length];
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* _NEXTSTD_NSDATA_H_ */