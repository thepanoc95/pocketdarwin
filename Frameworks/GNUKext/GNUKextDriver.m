/*
   Project: GNUKext

   Copyright (C) 2026 Free Software Foundation

   Author: milo

   Cross-platform kext driver base class implementation.

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

#import "GNUKextDriver.h"
#import "GNUKextRegistry.h"
#import "GNUKextPersonality.h"

NSString * const GNUKextDriverErrorDomain = @"GNUKextDriverErrorDomain";

@implementation GNUKextDriver

+ (NSArray *)personalityKeys
{
    return @[];
}

+ (BOOL)doesMatchProvider:(GNUKextRegistryEntry *)provider
           withPersonality:(GNUKextPersonality *)personality
{
    return YES;
}

- (instancetype)initWithPersonality:(GNUKextPersonality *)personality
{
    self = [super init];
    if (self) {
        _matchedPersonality = [personality retain];
        _personalityInfo = [[personality dictionary] copy];
        _provider = nil;
        _started = NO;
        _terminated = NO;
    }
    return self;
}

- (void)dealloc
{
    if (_started) {
        [self stop:_provider];
    }
    [_provider release];
    [_matchedPersonality release];
    [_personalityInfo release];
    [super dealloc];
}

- (BOOL)start:(GNUKextRegistryEntry *)provider
{
    if (_started) return YES;
    if (provider == nil) return NO;
    if (_terminated) return NO;

    _provider = [provider retain];
    _started = YES;

    NSLog(@"%@: started on %@", [self class], [provider entryPath]);
    return YES;
}

- (void)stop:(GNUKextRegistryEntry *)provider
{
    if (!_started) return;

    NSLog(@"%@: stopped", [self class]);

    _started = NO;
    _terminated = YES;
    [_provider release];
    _provider = nil;
}

- (void)free
{
    [_provider release];
    _provider = nil;
    [_matchedPersonality release];
    _matchedPersonality = nil;
    [_personalityInfo release];
    _personalityInfo = nil;
}

- (BOOL)isStarted
{
    return _started;
}

- (BOOL)isTerminated
{
    return _terminated;
}

@end
