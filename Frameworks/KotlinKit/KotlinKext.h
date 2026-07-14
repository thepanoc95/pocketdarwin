/*
    Project: KotlinKit

    Author: milo

    Cross-platform GNUKext support for Kotlin. Provides Kotlin-friendly
    wrappers around GNUKext classes.

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

#ifndef _KOTLINKEXT_H_
#define _KOTLINKEXT_H_

#import <Foundation/Foundation.h>
#import <GNUKext/GNUKext.h>

/*
 * KotlinKextErrorDomain
 */
extern NSString * const KotlinKextErrorDomain;

typedef enum {
    KotlinKextErrorNotAvailable = 4100,
    KotlinKextErrorInvalidArgument,
    KotlinKextErrorLoadFailed,
    KotlinKextErrorUnloadFailed
} KotlinKextErrorCode;

/*
 * KotlinKext - Cross-platform kext wrapper for Kotlin
 */
@interface KotlinKext : NSObject

@property (nonatomic, readonly, copy) NSString *bundlePath;
@property (nonatomic, readonly, copy) NSString *bundleIdentifier;
@property (nonatomic, readonly, copy) NSString *bundleVersion;
@property (nonatomic, readonly, copy) NSString *bundleName;
@property (nonatomic, readonly, assign) BOOL loaded;
@property (nonatomic, readonly, copy) NSDictionary *personalities;
@property (nonatomic, readonly, copy) NSArray *dependencies;

+ (instancetype)kextWithPath:(NSString *)path;
+ (instancetype)kextWithIdentifier:(NSString *)identifier;
+ (NSArray *)allLoadedKexts;
+ (BOOL)loadKextWithIdentifier:(NSString *)identifier;
+ (BOOL)unloadKextWithIdentifier:(NSString *)identifier;

- (instancetype)initWithPath:(NSString *)path;
- (BOOL)load;
- (BOOL)unload;
- (BOOL)isLoaded;
- (NSDictionary *)mergedInfoDictionary;
- (NSArray *)driverInstances;

@end

/*
 * KotlinKextDriver - Cross-platform kext driver wrapper for Kotlin
 */
@interface KotlinKextDriver : NSObject

@property (nonatomic, readonly, assign) BOOL started;
@property (nonatomic, readonly, assign) BOOL terminated;
@property (nonatomic, readonly, strong) id provider;
@property (nonatomic, readonly, strong) id matchedPersonality;
@property (nonatomic, readonly, copy) NSDictionary *personalityInfo;

+ (NSArray *)personalityKeys;
+ (BOOL)doesMatchProvider:(id)provider withPersonality:(id)personality;

- (instancetype)initWithPersonality:(id)personality;
- (BOOL)start:(id)provider;
- (void)stop:(id)provider;
- (void)free;

@end

/*
 * KotlinKextPersonality - Cross-platform kext personality wrapper for Kotlin
 */
@interface KotlinKextPersonality : NSObject

@property (nonatomic, readonly, copy) NSString *personalityName;
@property (nonatomic, readonly, copy) NSDictionary *properties;

+ (instancetype)personalityWithName:(NSString *)name properties:(NSDictionary *)props;
- (instancetype)initWithName:(NSString *)name properties:(NSDictionary *)props;
- (NSString *)providerClass;
- (NSString *)ioClass;
- (NSDictionary *)propertyMatch;
- (NSDictionary *)propertyTable;
- (NSInteger)probeScore;

@end

#endif /* _KOTLINKEXT_H_ */