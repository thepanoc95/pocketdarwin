/*
   Project: GNUKext

   Copyright (C) 2026 Free Software Foundation

   Author: milo

   Created: 2026-07-13 20:11:26 -0300 by milo

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

#ifndef _LINUXMODULE_H_
#define _LINUXMODULE_H_

#import <Foundation/Foundation.h>

#if defined(__linux__)

extern NSString * const LinuxKernelModuleErrorDomain;

typedef enum {
    LinuxKernelModuleErrorNotFound = 1000,
    LinuxKernelModuleErrorAlreadyLoaded,
    LinuxKernelModuleErrorNotLoaded,
    LinuxKernelModuleErrorInitFailed,
    LinuxKernelModuleErrorSyscallFailed,
    LinuxKernelModuleErrorDependencyMissing,
    LinuxKernelModuleErrorSymlinkFailed
} LinuxKernelModuleError;

@interface LinuxKernelModule : NSObject
{
    NSString *_name;
    NSString *_path;
    BOOL     _loaded;
    void    *_handle;
}

+ (BOOL)loadModuleAtPath:(NSString *)path;
+ (BOOL)unloadModuleAtPath:(NSString *)path;
+ (BOOL)loadModuleNamed:(NSString *)name;
+ (BOOL)unloadModuleNamed:(NSString *)name;
+ (NSArray *)loadedModules;
+ (NSString *)kernelVersion;

+ (BOOL)installKext:(NSString *)kextPath toModulesPath:(NSString *)modulesPath
              error:(NSError **)error;
+ (BOOL)uninstallKext:(NSString *)kextName
          modulesPath:(NSString *)modulesPath
                error:(NSError **)error;

- (instancetype)initWithPath:(NSString *)path;
- (BOOL)load;
- (BOOL)unload;
- (BOOL)isLoaded;

@property (nonatomic, readonly, copy) NSString *name;
@property (nonatomic, readonly, copy) NSString *path;
@property (nonatomic, readonly, assign) BOOL loaded;

@end

#endif /* __linux__ */

#endif /* _LINUXMODULE_H_ */
