/*
   Project: GNUKext

   Copyright (C) 2026 Free Software Foundation

   Author: milo

   Cross-platform IOKit registry entry equivalent. Provides a property
   tree that kext drivers can walk, matching device properties against
   personality dictionaries.

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

#ifndef _GNUKEXTREGISTRY_H_
#define _GNUKEXTREGISTRY_H_

#import <Foundation/Foundation.h>

@interface GNUKextRegistryEntry : NSObject
{
    NSString              *_name;
    NSString              *_entryPath;
    NSMutableDictionary   *_properties;
    NSMutableArray        *_childEntries;
    GNUKextRegistryEntry  *_parentEntry;
}

+ (instancetype)rootEntry;
+ (instancetype)entryWithName:(NSString *)name;
+ (instancetype)entryWithPath:(NSString *)path;

- (instancetype)initWithName:(NSString *)name;
- (instancetype)initWithName:(NSString *)name parent:(GNUKextRegistryEntry *)parent;

- (id)propertyForKey:(NSString *)key;
- (void)setProperty:(id)property forKey:(NSString *)key;
- (void)removePropertyForKey:(NSString *)key;
- (NSDictionary *)allProperties;

- (BOOL)hasProperty:(NSString *)key;
- (BOOL)hasProperty:(NSString *)key withValue:(id)value;
- (BOOL)hasProperty:(NSString *)key matchingPrefix:(NSString *)prefix;

- (void)addChild:(GNUKextRegistryEntry *)child;
- (void)removeChild:(GNUKextRegistryEntry *)child;
- (GNUKextRegistryEntry *)childWithName:(NSString *)name;
- (NSArray *)childEntries;

- (GNUKextRegistryEntry *)parentEntry;
- (GNUKextRegistryEntry *)rootEntry;
- (NSString *)entryPath;
- (NSString *)name;

- (GNUKextRegistryEntry *)entryWithPath:(NSString *)path;

@property (nonatomic, readonly, copy) NSString *name;
@property (nonatomic, readonly, copy) NSString *entryPath;

@end

#endif /* _GNUKEXTREGISTRY_H_ */
