/*
   Project: GNUKext

   Copyright (C) 2026 Free Software Foundation

   Author: milo

   Cross-platform kext driver base class. Subclass this to write a kext
   that compiles on Darwin, Linux, and BSD. The framework manages lifecycle,
   personality matching, and provider relationships.

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

#ifndef _GNUKEXTDRIVER_H_
#define _GNUKEXTDRIVER_H_

#import <Foundation/Foundation.h>

extern NSString * const GNUKextDriverErrorDomain;

typedef enum {
    GNUKextDriverErrorStartFailed = 5000,
    GNUKextDriverErrorStopFailed,
    GNUKextDriverErrorNotMatched,
    GNUKextDriverErrorProviderLost,
    GNUKextDriverErrorInitFailed
} GNUKextDriverErrorCode;

@class GNUKextDriver;
@class GNUKextRegistryEntry;
@class GNUKextPersonality;

@protocol GNUKextDriverMatching <NSObject>
@optional
- (BOOL)matchPersonality:(GNUKextPersonality *)personality
              withProvider:(GNUKextRegistryEntry *)provider;
@end

@interface GNUKextDriver : NSObject <GNUKextDriverMatching>
{
    GNUKextRegistryEntry *_provider;
    GNUKextPersonality   *_matchedPersonality;
    NSDictionary         *_personalityInfo;
    BOOL                  _started;
    BOOL                  _terminated;
}

+ (NSArray *)personalityKeys;

+ (BOOL)doesMatchProvider:(GNUKextRegistryEntry *)provider
             withPersonality:(GNUKextPersonality *)personality;

- (instancetype)initWithPersonality:(GNUKextPersonality *)personality;

- (BOOL)start:(GNUKextRegistryEntry *)provider;
- (void)stop:(GNUKextRegistryEntry *)provider;
- (void)free;

- (BOOL)isStarted;
- (BOOL)isTerminated;

@property (nonatomic, readonly, retain) GNUKextRegistryEntry *provider;
@property (nonatomic, readonly, retain) GNUKextPersonality *matchedPersonality;
@property (nonatomic, readonly, copy) NSDictionary *personalityInfo;

@end

#define GNUKextDriverDeclare(_class) \
    + (NSArray *)personalityKeys; \
    + (BOOL)doesMatchProvider:(GNUKextRegistryEntry *)provider \
              withPersonality:(GNUKextPersonality *)personality;

#define GNUKextDriverImplement(_class) \
    + (NSArray *)personalityKeys { return @[]; } \
    + (BOOL)doesMatchProvider:(GNUKextRegistryEntry *)provider \
              withPersonality:(GNUKextPersonality *)personality { \
        return YES; \
    }

#endif /* _GNUKEXTDRIVER_H_ */
