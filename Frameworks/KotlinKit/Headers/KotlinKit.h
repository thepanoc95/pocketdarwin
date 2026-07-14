/*
   Project: KotlinKit

   Copyright (C) 2026 Free Software Foundation

   Author: milo

   Created: 2026-07-14

   KotlinKit - A library for making GNUstep and Kotlin work together.
   This framework provides Objective-C wrappers for Kotlin types and
   enables interoperability between Kotlin and GNUstep/Objective-C.

   This application is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This application is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 31 Milk Street #960789 Boston, MA 02196 USA.
*/

#ifndef _KOTLINKIT_H_
#define _KOTLINKIT_H_

#import <Foundation/Foundation.h>
#import "KotlinKitTypes.h"

/*
 * Kotlin - Main class for Kotlin interoperability
 */
@interface Kotlin : NSObject

@property (class, nonatomic, readonly) NSString *version;

+ (BOOL)isKotlinClass:(id)object;
+ (void)JIGSClassMethod:(NSString *)methodName;

@end

/*
 * KotlinArray - Represents a Kotlin array
 */
@interface KotlinArray : NSObject

@property (nonatomic, readonly, assign) NSUInteger count;
@property (nonatomic, readonly, strong) Class elementClass;

+ (instancetype)arrayWithObjects:(id)firstObj, ... NS_REQUIRES_NIL_TERMINATION;
+ (instancetype)arrayWithArray:(NSArray *)array;
+ (instancetype)arrayWithSize:(NSUInteger)size elementClass:(Class)cls;

- (instancetype)initWithObjects:(id)firstObj, ... NS_REQUIRES_NIL_TERMINATION;
- (instancetype)initWithArray:(NSArray *)array;
- (id)objectAtIndex:(NSUInteger)index;
- (void)addObject:(id)object;
- (void)insertObject:(id)object atIndex:(NSUInteger)index;
- (void)removeObjectAtIndex:(NSUInteger)index;
- (NSArray *)toArray;

@end

/*
 * KotlinBoolean - Represents Kotlin's Boolean type
 */
@interface KotlinBoolean : NSObject

@property (nonatomic, readonly, assign) BOOL booleanValue;

+ (instancetype)TRUE;
+ (instancetype)FALSE;
+ (instancetype)booleanWithBOOL:(BOOL)value;
+ (instancetype)booleanWithValue:(NSNumber *)value;

- (instancetype)initWithBOOL:(BOOL)value;
- (instancetype)initWithValue:(NSNumber *)value;
- (BOOL)booleanValue;
- (NSNumber *)toNSNumber;

@end

/*
 * KotlinByte - Represents Kotlin's Byte type
 */
@interface KotlinByte : NSObject

@property (nonatomic, readonly, assign) int8_t byteValue;

+ (instancetype)byteWithInt8:(int8_t)value;
+ (instancetype)byteWithNumber:(NSNumber *)value;

- (instancetype)initWithInt8:(int8_t)value;
- (instancetype)initWithNumber:(NSNumber *)value;
- (int8_t)byteValue;
- (NSNumber *)toNSNumber;

@end

/*
 * KotlinChar - Represents Kotlin's Char type
 */
@interface KotlinChar : NSObject

@property (nonatomic, readonly, assign) unichar charValue;

+ (instancetype)charWithUnichar:(unichar)value;
+ (instancetype)charWithString:(NSString *)value;

- (instancetype)initWithUnichar:(unichar)value;
- (instancetype)initWithString:(NSString *)value;
- (unichar)charValue;
- (NSString *)toString;

@end

/*
 * KotlinDouble - Represents Kotlin's Double type
 */
@interface KotlinDouble : NSObject

@property (nonatomic, readonly, assign) double doubleValue;

+ (instancetype)doubleWithDouble:(double)value;
+ (instancetype)doubleWithNumber:(NSNumber *)value;
+ (instancetype)doubleWithString:(NSString *)value;

- (instancetype)initWithDouble:(double)value;
- (instancetype)initWithNumber:(NSNumber *)value;
- (double)doubleValue;
- (NSNumber *)toNSNumber;
- (NSString *)toString;

@end

/*
 * KotlinFloat - Represents Kotlin's Float type
 */
@interface KotlinFloat : NSObject

@property (nonatomic, readonly, assign) float floatValue;

+ (instancetype)floatWithFloat:(float)value;
+ (instancetype)floatWithNumber:(NSNumber *)value;
+ (instancetype)floatWithString:(NSString *)value;

- (instancetype)initWithFloat:(float)value;
- (instancetype)initWithNumber:(NSNumber *)value;
- (float)floatValue;
- (NSNumber *)toNSNumber;
- (NSString *)toString;

@end

/*
 * KotlinInt - Represents Kotlin's Int type
 */
@interface KotlinInt : NSObject

@property (nonatomic, readonly, assign) int32_t intValue;

+ (instancetype)intWithInt32:(int32_t)value;
+ (instancetype)intWithNumber:(NSNumber *)value;
+ (instancetype)intWithString:(NSString *)value;

- (instancetype)initWithInt32:(int32_t)value;
- (instancetype)initWithNumber:(NSNumber *)value;
- (int32_t)intValue;
- (NSNumber *)toNSNumber;
- (NSString *)toString;

@end

/*
 * KotlinLong - Represents Kotlin's Long type
 */
@interface KotlinLong : NSObject

@property (nonatomic, readonly, assign) int64_t longValue;

+ (instancetype)longWithInt64:(int64_t)value;
+ (instancetype)longWithNumber:(NSNumber *)value;
+ (instancetype)longWithString:(NSString *)value;

- (instancetype)initWithInt64:(int64_t)value;
- (instancetype)initWithNumber:(NSNumber *)value;
- (int64_t)longValue;
- (NSNumber *)toNSNumber;
- (NSString *)toString;

@end

/*
 * KotlinShort - Represents Kotlin's Short type
 */
@interface KotlinShort : NSObject

@property (nonatomic, readonly, assign) int16_t shortValue;

+ (instancetype)shortWithInt16:(int16_t)value;
+ (instancetype)shortWithNumber:(NSNumber *)value;

- (instancetype)initWithInt16:(int16_t)value;
- (instancetype)initWithNumber:(NSNumber *)value;
- (int16_t)shortValue;
- (NSNumber *)toNSNumber;

@end

/*
 * KotlinUnit - Represents Kotlin's Unit type (void equivalent)
 */
@interface KotlinUnit : NSObject

+ (instancetype)unit;

@end

/*
 * KotlinFuncs - Utility functions for Kotlin interoperability
 */
@interface KotlinFuncs : NSObject

+ (NSString *)kotlinVersion;
+ (BOOL)isKotlinAvailable;
+ (id)callKotlinStaticMethod:(NSString *)className 
                   methodName:(NSString *)methodName 
                        args:(NSArray *)arguments;
+ (id)callKotlinInstanceMethod:(id)instance 
                    methodName:(NSString *)methodName 
                         args:(NSArray *)arguments;

@end

/*
 * KotlinKext - Kotlin-friendly wrapper for GNUKext
 * Note: Full implementation in KotlinKext.h/KotlinKext.m
 */
@interface KotlinKext : NSObject
@end

/*
 * KotlinKextDriver - Kotlin-friendly wrapper for GNUKextDriver
 * Note: Full implementation in KotlinKext.h/KotlinKext.m
 */
@interface KotlinKextDriver : NSObject
@end

/*
 * KotlinKextPersonality - Kotlin-friendly wrapper for GNUKextPersonality
 * Note: Full implementation in KotlinKext.h/KotlinKext.m
 */
@interface KotlinKextPersonality : NSObject
@end

/*
 * KotlinUIKit - UIKit extensions for Kotlin
 * Note: Full implementation in KotlinUIKit.h/KotlinUIKit.mm
 */
@interface KotlinUIKit : NSObject
@end

#endif /* _KOTLINKIT_H_ */