/*
   Project: GNUKext

   Copyright (C) 2026 Free Software Foundation

   Author: milo

   Created: 2026-07-13 19:36:24 -0300 by milo

   Cross-platform Kernel Extension framework. Build kexts with this
   framework and they compile on Darwin, Linux, and BSD.

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

#ifndef _GNUKEXT_H_
#define _GNUKEXT_H_

#import <Foundation/Foundation.h>
#import "libkern.h"
#import "GNUKextDriver.h"
#import "GNUKextRegistry.h"
#import "GNUKextPersonality.h"
#import "LinuxModule.h"
#import "BSDModule.h"
#import "DarwinModule.h"
#import "kIOPlatformHandler.h"

extern NSString * const GNUKextErrorDomain;

typedef enum {
    GNUKextErrorNotLoaded = 3000,
    GNUKextErrorAlreadyLoaded,
    GNUKextErrorInvalidBundle,
    GNUKextErrorLoadFailed,
    GNUKextErrorUnloadFailed,
    GNUKextErrorDependencyMissing,
    GNUKextErrorNotFound
} GNUKextErrorCode;

@interface GNUKext : NSObject
{
    NSString    *_bundlePath;
    NSString    *_bundleIdentifier;
    NSString    *_bundleVersion;
    NSString    *_bundleName;
    NSDictionary *_infoPlist;
    NSDictionary *_personalities;
    NSArray     *_dependencies;
    BOOL         _loaded;
    KextInfoRef  _kextInfo;
    NSMutableArray *_driverInstances;
}

+ (instancetype)kextWithPath:(NSString *)path;
+ (instancetype)kextWithIdentifier:(NSString *)identifier;
+ (NSArray *)allLoadedKexts;
+ (BOOL)loadKextWithIdentifier:(NSString *)identifier;
+ (BOOL)unloadKextWithIdentifier:(NSString *)identifier;
+ (NSString *)libkernVersion;

+ (void)registerDriverClass:(Class)driverClass;
+ (void)registerDriverClass:(Class)driverClass forPersonality:(NSString *)personalityName;
+ (NSArray *)registeredDriverClasses;
+ (void)matchAndStartDrivers;

- (instancetype)initWithPath:(NSString *)path;
- (BOOL)load;
- (BOOL)unload;
- (BOOL)isLoaded;
- (BOOL)addPersonalities;
- (BOOL)removePersonalities;
- (BOOL)startMatchingWithProvider:(GNUKextRegistryEntry *)provider;
- (NSDictionary *)mergedInfoDictionary;
- (NSArray *)driverInstances;

@property (nonatomic, readonly, copy) NSString *bundlePath;
@property (nonatomic, readonly, copy) NSString *bundleIdentifier;
@property (nonatomic, readonly, copy) NSString *bundleVersion;
@property (nonatomic, readonly, copy) NSString *bundleName;
@property (nonatomic, readonly, copy) NSDictionary *infoPlist;
@property (nonatomic, readonly, copy) NSDictionary *personalities;
@property (nonatomic, readonly, copy) NSArray *dependencies;
@property (nonatomic, readonly, assign) BOOL loaded;

@end

@interface libkernDriver : NSObject

+ (NSDictionary *)driverPlistForKext:(NSString *)kextPath;
+ (NSDictionary *)driverPlistForIdentifier:(NSString *)identifier;
+ (BOOL)writeDriverPlist:(NSDictionary *)plist toPath:(NSString *)path;
+ (NSString *)defaultModulesPath;

@end

#endif /* _GNUKEXT_H_ */
