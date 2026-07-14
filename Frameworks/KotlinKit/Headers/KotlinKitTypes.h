/*
   Project: KotlinKit

   Copyright (C) 2026 Free Software Foundation

   Author: milo

   Created: 2026-07-14

   KotlinKitTypes - Type definitions for KotlinKit framework.
   Provides type aliases and constants for Kotlin interoperability.

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

#ifndef _KOTLINKIT_TYPES_H_
#define _KOTLINKIT_TYPES_H_

#import <Foundation/Foundation.h>

/* Kotlin version string */
extern NSString * const KotlinVersion;

/* Kotlin type identifiers */
extern NSString * const KotlinBooleanType;
extern NSString * const KotlinByteType;
extern NSString * const KotlinCharType;
extern NSString * const KotlinShortType;
extern NSString * const KotlinIntType;
extern NSString * const KotlinLongType;
extern NSString * const KotlinFloatType;
extern NSString * const KotlinDoubleType;

/* Kotlin array type identifier */
extern NSString * const KotlinArrayType;

/* Error domain for KotlinKit */
extern NSString * const KotlinKitErrorDomain;

/* Error codes */
typedef NS_ENUM(NSInteger, KotlinKitErrorCode) {
    KotlinKitErrorNotAvailable = 4000,
    KotlinKitErrorInvalidArgument,
    KotlinKitErrorTypeMismatch,
    KotlinKitErrorMethodNotFound,
    KotlinKitErrorInvocationFailed
};

/* Block type for Kotlin callbacks */
typedef void (^KotlinCallback)(id result, NSError *error);

/* Block type for Kotlin function references */
typedef id (^KotlinFunction)(NSArray *arguments);

#endif /* _KOTLINKIT_TYPES_H_ */