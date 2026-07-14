/*
   Project: GNUKext

   Copyright (C) 2026 Free Software Foundation

   Author: milo

   Cross-platform IOKit personality matching implementation.

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

#import "GNUKextPersonality.h"
#import "GNUKextRegistry.h"

@implementation GNUKextPersonality

+ (instancetype)personalityWithName:(NSString *)name dictionary:(NSDictionary *)dict
{
    return [[[self alloc] initWithName:name dictionary:dict] autorelease];
}

- (instancetype)initWithName:(NSString *)name dictionary:(NSDictionary *)dict
{
    self = [super init];
    if (self) {
        _name = [name copy];
        _dictionary = [dict copy];
    }
    return self;
}

- (void)dealloc
{
    [_name release];
    [_dictionary release];
    [super dealloc];
}

- (NSString *)name
{
    return _name;
}

- (NSDictionary *)dictionary
{
    return _dictionary;
}

- (id)objectForKey:(NSString *)key
{
    return [_dictionary objectForKey:key];
}

- (NSString *)providerClass
{
    return [_dictionary objectForKey:@"IOProviderClass"];
}

- (NSString *)ioClass
{
    return [_dictionary objectForKey:@"IOClass"];
}

- (NSDictionary *)propertyMatch
{
    return [_dictionary objectForKey:@"IOPropertyMatch"];
}

- (NSDictionary *)propertyTable
{
    return [_dictionary objectForKey:@"IOPropertyTable"];
}

- (NSInteger)probeScore
{
    id score = [_dictionary objectForKey:@"IOProbeScore"];
    if (score != nil && [score respondsToSelector:@selector(integerValue)]) {
        return [score integerValue];
    }
    return 0;
}

- (BOOL)_matchIOPropertyMatch:(GNUKextRegistryEntry *)provider
{
    NSDictionary *matchDict = [self propertyMatch];
    if (matchDict == nil) return YES;

    for (NSString *key in matchDict) {
        id matchValue = [matchDict objectForKey:key];
        id providerValue = [provider propertyForKey:key];

        if (providerValue == nil) return NO;

        if ([matchValue isKindOfClass:[NSDictionary class]] &&
            [providerValue isKindOfClass:[NSDictionary class]]) {
            BOOL allMatch = YES;
            for (NSString *subKey in (NSDictionary *)matchValue) {
                id subMatch = [(NSDictionary *)matchValue objectForKey:subKey];
                id subActual = [(NSDictionary *)providerValue objectForKey:subKey];
                if (subActual == nil || ![subActual isEqual:subMatch]) {
                    allMatch = NO;
                    break;
                }
            }
            if (!allMatch) return NO;
        } else {
            if (![providerValue isEqual:matchValue]) return NO;
        }
    }

    return YES;
}

- (BOOL)_matchIOPropertyTable:(GNUKextRegistryEntry *)provider
{
    NSDictionary *tableDict = [self propertyTable];
    if (tableDict == nil) return YES;

    for (NSString *key in tableDict) {
        id tableValue = [tableDict objectForKey:key];
        id providerValue = [provider propertyForKey:key];

        if (providerValue == nil) return NO;

        if ([tableValue isKindOfClass:[NSString class]] &&
            [providerValue isKindOfClass:[NSString class]]) {
            if (![(NSString *)providerValue isEqualToString:(NSString *)tableValue]) {
                return NO;
            }
        } else if ([tableValue isKindOfClass:[NSArray class]]) {
            BOOL found = NO;
            for (id acceptable in (NSArray *)tableValue) {
                if ([acceptable isEqual:providerValue]) {
                    found = YES;
                    break;
                }
            }
            if (!found) return NO;
        } else {
            if (![providerValue isEqual:tableValue]) return NO;
        }
    }

    return YES;
}

- (BOOL)_matchIOProviderClass:(GNUKextRegistryEntry *)provider
{
    NSString *providerClass = [self providerClass];
    if (providerClass == nil) return YES;

    NSString *entryClass = [provider propertyForKey:@"IOProviderClass"];
    if (entryClass == nil) return NO;

    return [entryClass isEqualToString:providerClass];
}

- (BOOL)_matchIONameMatch:(GNUKextRegistryEntry *)provider
{
    id nameMatch = [_dictionary objectForKey:@"IONameMatch"];
    if (nameMatch == nil) return YES;

    NSString *entryName = [provider propertyForKey:@"IOName"];
    if (entryName == nil) return NO;

    if ([nameMatch isKindOfClass:[NSArray class]]) {
        return [(NSArray *)nameMatch containsObject:entryName];
    } else if ([nameMatch isKindOfClass:[NSString class]]) {
        return [entryName isEqualToString:(NSString *)nameMatch];
    }

    return NO;
}

- (BOOL)_matchIOUserClientClass:(GNUKextRegistryEntry *)provider
{
    NSString *clientClass = [_dictionary objectForKey:@"IOUserClientClass"];
    if (clientClass == nil) return YES;

    NSString *entryClientClass = [provider propertyForKey:@"IOUserClientClass"];
    if (entryClientClass == nil) return YES;

    return [entryClientClass isEqualToString:clientClass];
}

- (BOOL)matchesProvider:(GNUKextRegistryEntry *)provider
{
    if (provider == nil) return NO;

    if (![self _matchIOProviderClass:provider]) return NO;
    if (![self _matchIOPropertyMatch:provider]) return NO;
    if (![self _matchIOPropertyTable:provider]) return NO;
    if (![self _matchIONameMatch:provider]) return NO;
    if (![self _matchIOUserClientClass:provider]) return NO;

    return YES;
}

@end
