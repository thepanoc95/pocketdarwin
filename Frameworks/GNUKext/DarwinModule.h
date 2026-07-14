/*
   Project: GNUKext

   Copyright (C) 2026 Free Software Foundation

   Author: milo

   Darwin/XNU kernel extension injection via the native OSKext APIs.
   Loads kexts by Mach-O bundle path, unloads by identifier, installs
   by copying the .kext bundle into the system extensions directory.

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

#ifndef _DARWINMODULE_H_
#define _DARWINMODULE_H_

#import <Foundation/Foundation.h>

#if defined(__APPLE__)

extern NSString * const DarwinKextErrorDomain;

typedef enum {
    DarwinKextErrorLoadFailed = 4000,
    DarwinKextErrorUnloadFailed,
    DarwinKextErrorNotFound,
    DarwinKextErrorAlreadyLoaded,
    DarwinKextErrorDependencyMissing,
    DarwinKextErrorInstallFailed,
    DarwinKextErrorUninstallFailed,
    DarwinKextErrorNotAuthorized
} DarwinKextError;

@interface DarwinKext : NSObject
{
    NSString *_identifier;
    NSString *_bundlePath;
    BOOL      _loaded;
}

+ (BOOL)loadKextAtPath:(NSString *)bundlePath;
+ (BOOL)loadKextWithIdentifier:(NSString *)identifier;
+ (BOOL)unloadKextWithIdentifier:(NSString *)identifier;
+ (BOOL)isKextLoaded:(NSString *)identifier;
+ (NSArray *)allLoadedIdentifiers;
+ (NSString *)systemExtensionsPath;
+ (NSString *)localExtensionsPath;

+ (BOOL)installKext:(NSString *)bundlePath error:(NSError **)error;
+ (BOOL)uninstallKext:(NSString *)identifier error:(NSError **)error;

- (instancetype)initWithPath:(NSString *)bundlePath;
- (BOOL)load;
- (BOOL)unload;
- (BOOL)isLoaded;

@property (nonatomic, readonly, copy) NSString *identifier;
@property (nonatomic, readonly, copy) NSString *bundlePath;
@property (nonatomic, readonly, assign) BOOL loaded;

@end

#endif /* __APPLE__ */

#endif /* _DARWINMODULE_H_ */
