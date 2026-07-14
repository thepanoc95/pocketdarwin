/*
   Project: GNUKext

   Copyright (C) 2026 Free Software Foundation

   Author: milo

   Cross-platform IOKit registry entry implementation.

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

#import "GNUKextRegistry.h"

static GNUKextRegistryEntry *_rootEntry = nil;

@implementation GNUKextRegistryEntry

+ (void)initialize
{
    if (self == [GNUKextRegistryEntry class] && _rootEntry == nil) {
        _rootEntry = [[GNUKextRegistryEntry alloc] initWithName:@"root"];
    }
}

+ (instancetype)rootEntry
{
    return _rootEntry;
}

+ (instancetype)entryWithName:(NSString *)name
{
    return [[[self alloc] initWithName:name] autorelease];
}

+ (instancetype)entryWithPath:(NSString *)path
{
    if (path == nil) return nil;

    NSArray *components = [path componentsSeparatedByString:@"/"];
    GNUKextRegistryEntry *current = _rootEntry;

    for (NSString *component in components) {
        if ([component length] == 0) continue;

        GNUKextRegistryEntry *child = [current childWithName:component];
        if (child == nil) {
            child = [[[GNUKextRegistryEntry alloc] initWithName:component
                                                         parent:current] autorelease];
            [current addChild:child];
        }
        current = child;
    }

    return current;
}

- (instancetype)initWithName:(NSString *)name
{
    return [self initWithName:name parent:_rootEntry];
}

- (instancetype)initWithName:(NSString *)name parent:(GNUKextRegistryEntry *)parent
{
    self = [super init];
    if (self) {
        _name = [name copy];
        _properties = [[NSMutableDictionary alloc] init];
        _childEntries = [[NSMutableArray alloc] init];
        _parentEntry = parent;
        _entryPath = nil;
    }
    return self;
}

- (void)dealloc
{
    [_name release];
    [_properties release];
    [_childEntries release];
    [_entryPath release];
    [super dealloc];
}

- (NSString *)name
{
    return _name;
}

- (NSString *)entryPath
{
    if (_entryPath == nil) {
        NSMutableArray *parts = [NSMutableArray array];
        GNUKextRegistryEntry *current = self;
        while (current != nil && current->_name != nil) {
            [parts insertObject:current->_name atIndex:0];
            current = current->_parentEntry;
        }
        _entryPath = [[parts componentsJoinedByString:@"/"] copy];
    }
    return _entryPath;
}

- (id)propertyForKey:(NSString *)key
{
    if (key == nil) return nil;
    return [_properties objectForKey:key];
}

- (void)setProperty:(id)property forKey:(NSString *)key
{
    if (key == nil) return;
    if (property == nil) {
        [_properties removeObjectForKey:key];
    } else {
        [_properties setObject:property forKey:key];
    }
    [_entryPath release];
    _entryPath = nil;
}

- (void)removePropertyForKey:(NSString *)key
{
    [self setProperty:nil forKey:key];
}

- (NSDictionary *)allProperties
{
    return [[_properties copy] autorelease];
}

- (BOOL)hasProperty:(NSString *)key
{
    return [_properties objectForKey:key] != nil;
}

- (BOOL)hasProperty:(NSString *)key withValue:(id)value
{
    if (key == nil || value == nil) return NO;
    id actual = [_properties objectForKey:key];
    if (actual == nil) return NO;
    return [actual isEqual:value];
}

- (BOOL)hasProperty:(NSString *)key matchingPrefix:(NSString *)prefix
{
    if (key == nil || prefix == nil) return NO;
    id actual = [_properties objectForKey:key];
    if (actual == nil) return NO;
    if (![actual isKindOfClass:[NSString class]]) return NO;
    return [(NSString *)actual hasPrefix:prefix];
}

- (void)addChild:(GNUKextRegistryEntry *)child
{
    if (child == nil) return;

    for (GNUKextRegistryEntry *existing in _childEntries) {
        if ([existing->_name isEqualToString:child->_name]) {
            return;
        }
    }

    child->_parentEntry = self;
    [child->_entryPath release];
    child->_entryPath = nil;
    [_childEntries addObject:child];
}

- (void)removeChild:(GNUKextRegistryEntry *)child
{
    if (child == nil) return;
    [_childEntries removeObject:child];
    child->_parentEntry = nil;
    [child->_entryPath release];
    child->_entryPath = nil;
}

- (GNUKextRegistryEntry *)childWithName:(NSString *)name
{
    if (name == nil) return nil;

    for (GNUKextRegistryEntry *child in _childEntries) {
        if ([child->_name isEqualToString:name]) {
            return child;
        }
    }
    return nil;
}

- (NSArray *)childEntries
{
    return [[_childEntries copy] autorelease];
}

- (GNUKextRegistryEntry *)parentEntry
{
    return _parentEntry;
}

- (GNUKextRegistryEntry *)rootEntry
{
    return _rootEntry;
}

- (GNUKextRegistryEntry *)entryWithPath:(NSString *)path
{
    if (path == nil) return nil;

    NSArray *components = [path componentsSeparatedByString:@"/"];
    GNUKextRegistryEntry *current = self;

    for (NSString *component in components) {
        if ([component length] == 0) continue;
        GNUKextRegistryEntry *child = [current childWithName:component];
        if (child == nil) return nil;
        current = child;
    }

    return current;
}

@end
