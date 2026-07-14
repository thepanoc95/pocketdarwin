/*
   Project: GNUKext

   Copyright (C) 2026 Free Software Foundation

   Author: milo

   Created: 2026-07-13 19:39:55 -0300 by milo

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

#ifndef _LIBKERN_H_
#define _LIBKERN_H_

#import <Foundation/Foundation.h>

#if defined(__APPLE__)
  #include <libkern/OSKext.h>
  #include <libkern/OSKextLib.h>
#else

typedef enum {
    kKextInfoStateUnloaded = 0,
    kKextInfoStateLoading,
    kKextInfoStateLoaded,
    kKextInfoStateUnloading,
    kKextInfoStateFailed
} KextInfoState;

typedef enum {
    kKextTypeKext = 0,
    kKextTypeLibrary,
    kKextTypeKernelModule
} KextType;

typedef enum {
    kKextLoadResultSuccess = 0,
    kKextLoadResultNotFound,
    kKextLoadResultInvalidInfo,
    kKextLoadResultMissingDependencies,
    kKextLoadResultFailedToLoad,
    kKextLoadResultAlreadyLoaded
} KextLoadResult;

struct KextInfo {
    NSString           *bundleIdentifier;
    NSString           *version;
    NSString           *compatibleVersion;
    NSString           *path;
    NSString           *executablePath;
    NSString           *name;
    KextType            type;
    KextInfoState       state;
    KextLoadResult      lastLoadResult;
    NSDictionary       *infoPlist;
    NSDictionary       *personalities;
    NSArray            *dependencyIDs;
    BOOL                allowUserLoad;
    BOOL                isAuthenticated;
};

typedef struct KextInfo *KextInfoRef;

#ifdef __cplusplus
extern "C" {
#endif

KextInfoRef  KextInfoCreate(void);
void         KextInfoFree(KextInfoRef info);

KextInfoRef  KextInfoCreateFromPath(NSString *path);
KextInfoRef  KextInfoCreateFromIdentifier(NSString *identifier);

KextLoadResult KextInfoLoad(KextInfoRef info);
KextLoadResult KextInfoUnload(KextInfoRef info);

BOOL         KextInfoAddPersonalities(KextInfoRef info);
BOOL         KextInfoRemovePersonalities(KextInfoRef info);

KextInfoRef  KextInfoFindWithIdentifier(NSString *identifier);
NSArray     *KextInfoGetAllLoaded(void);

NSString    *KextInfoGetVersion(void);

#ifdef __cplusplus
}
#endif

#endif /* !__APPLE__ */
#endif /* _LIBKERN_H_ */
