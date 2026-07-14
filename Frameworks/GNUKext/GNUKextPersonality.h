/*
   Project: GNUKext

   Copyright (C) 2026 Free Software Foundation

   Author: milo

   Cross-platform IOKit personality matching. Wraps a single entry from
   an IOKitPersonalities dictionary and compares it against registry entries.

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

#ifndef _GNUKEXTPERSONALITY_H_
#define _GNUKEXTPERSONALITY_H_

#import <Foundation/Foundation.h>

@class GNUKextRegistryEntry;

@interface GNUKextPersonality : NSObject
{
    NSString     *_name;
    NSDictionary *_dictionary;
}

+ (instancetype)personalityWithName:(NSString *)name dictionary:(NSDictionary *)dict;

- (instancetype)initWithName:(NSString *)name dictionary:(NSDictionary *)dict;

- (BOOL)matchesProvider:(GNUKextRegistryEntry *)provider;

- (NSString *)name;
- (NSDictionary *)dictionary;
- (id)objectForKey:(NSString *)key;
- (NSString *)providerClass;
- (NSString *)ioClass;
- (NSDictionary *)propertyMatch;
- (NSDictionary *)propertyTable;
- (NSInteger)probeScore;

@property (nonatomic, readonly, copy) NSString *name;
@property (nonatomic, readonly, copy) NSDictionary *dictionary;

@end

#endif /* _GNUKEXTPERSONALITY_H_ */
