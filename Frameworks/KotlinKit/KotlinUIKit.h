/*
    Project: KotlinKit

    Copyright (C) 2026 Free Software Foundation

    Author: milo

    UIKit extensions for Kotlin. Provides Kotlin-friendly wrappers around
    UIKit classes for building cross-platform UI applications with Kotlin.

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

#ifndef _KOTLINUIKIT_H_
#define _KOTLINUIKIT_H_

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

/*
 * KotlinUIApplication - UIApplication wrapper for Kotlin
 */
@interface KotlinUIApplication : NSObject

@property (nonatomic, readonly, strong) KotlinUIApplication *shared;
@property (nonatomic, strong) id delegate;

+ (KotlinUIApplication *)sharedInstance;
+ (void)runMain:(int)argc argv:(char **)argv principalClass:(NSString *)principalClassName delegateClass:(NSString *)delegateClassName;

- (NSArray *)windows;
- (void)addWindow:(id)window;
- (void)terminate;

@end

/*
 * KotlinUIView - UIView wrapper for Kotlin
 */
@interface KotlinUIView : NSObject

@property (nonatomic, assign) CGFloat x, y, width, height;
@property (nonatomic, strong) id backgroundColor;
@property (nonatomic, assign) BOOL hidden;
@property (nonatomic, assign) CGFloat alpha;
@property (nonatomic, assign) NSInteger tag;
@property (nonatomic, strong) NSArray *subviews;
@property (nonatomic, strong) KotlinUIView *superview;

- (instancetype)initWithFrame:(CGRect)frame;
- (void)addSubview:(KotlinUIView *)view;
- (void)removeFromSuperview;
- (KotlinUIView *)viewWithTag:(NSInteger)tag;
- (void)setNeedsDisplay;
- (void)setNeedsLayout;

@end

/*
 * KotlinUIViewController - UIViewController wrapper for Kotlin
 */
@interface KotlinUIViewController : NSObject

@property (nonatomic, strong) KotlinUIView *view;
@property (nonatomic, strong) NSString *title;

- (instancetype)init;
- (void)viewDidLoad;
- (void)setView:(KotlinUIView *)view;
- (KotlinUIView *)view;

@end

/*
 * KotlinUIColor - UIColor wrapper for Kotlin
 */
@interface KotlinUIColor : NSObject

@property (nonatomic, readonly) CGFloat red, green, blue, alpha;

+ (instancetype)colorWithRed:(CGFloat)r green:(CGFloat)g blue:(CGFloat)b alpha:(CGFloat)a;
+ (instancetype)blackColor;
+ (instancetype)whiteColor;
+ (instancetype)clearColor;

- (instancetype)initWithRed:(CGFloat)r green:(CGFloat)g blue:(CGFloat)b alpha:(CGFloat)a;

@end

#endif /* _KOTLINUIKIT_H_ */