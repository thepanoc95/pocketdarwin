#import <UIKit/UIResponder.h>
#import <UIKit/UIKitExport.h>
#import <UIKit/UIApplication.h>
#import <UIKit/UIGeometry.h>
#import <UIKit/UIView.h>

@class UIView;
@class UITabBarItem;
@class UINavigationItem;
@class UIBarButtonItem;
@class UISearchDisplayController;
@class UIRefreshControl;
@class UINavigationController;
@class UISplitViewController;
@class UIPresentationController;
@class UIPopoverPresentationController;

@protocol UIViewControllerTransitionCoordinator <NSObject>
@end

@protocol UIViewControllerTransitioningDelegate <NSObject>
@end

@protocol UIViewControllerRestoration <NSObject>
@end

typedef NSInteger UIModalTransitionStyle;

enum {
    UIModalTransitionStyleCoverVertical = 0,
    UIModalTransitionStyleFlipHorizontal,
    UIModalTransitionStyleCrossDissolve,
    UIModalTransitionStylePartialCurl,
};

typedef NSInteger UIModalPresentationStyle;

enum {
    UIModalPresentationFullScreen = 0,
    UIModalPresentationPageSheet,
    UIModalPresentationFormSheet,
    UIModalPresentationCurrentContext,
    UIModalPresentationCustom,
    UIModalPresentationOverFullScreen,
    UIModalPresentationOverCurrentContext,
    UIModalPresentationPopover,
    UIModalPresentationNone = -1,
};

@interface UIViewController : UIResponder <NSCoding, UIAppearanceContainer> {
    UIView *_view;
    NSString *_title;
    UIViewController *_parentViewController;
    UIViewController *_presentingViewController;
    UIViewController *_presentedViewController;
    NSArray *_childViewControllers;
    UINavigationItem *_navigationItem;
    UITabBarItem *_tabBarItem;
    BOOL _viewLoaded;
    BOOL _viewAppearing;
    BOOL _viewAppeared;
    BOOL _extendedLayoutIncludesOpaqueBars;
    BOOL _automaticallyAdjustsScrollViewInsets;
    UIEdgeInsets _additionalSafeAreaInsets;
    UIModalTransitionStyle _modalTransitionStyle;
    UIModalPresentationStyle _modalPresentationStyle;
    BOOL _definesPresentationContext;
    BOOL _providesPresentationContextTransitionStyle;
    BOOL _wantsFullScreenLayout;
    BOOL _hidesBottomBarWhenPushed;
    NSString *_nibName;
    NSBundle *_nibBundle;
    CGSize _preferredContentSize;
    Class _restorationClass;
}

- (instancetype)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil;

@property (nonatomic, retain) UIView *view;
@property (nonatomic, copy) NSString *title;

- (void)loadView;
- (void)viewDidLoad;
- (void)viewWillAppear:(BOOL)animated;
- (void)viewDidAppear:(BOOL)animated;
- (void)viewWillDisappear:(BOOL)animated;
- (void)viewDidDisappear:(BOOL)animated;

- (void)viewWillLayoutSubviews;
- (void)viewDidLayoutSubviews;

- (void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)coordinator;

@property (nonatomic, readonly, getter=isViewLoaded) BOOL viewLoaded;

@property (nonatomic, readonly) UIViewController *parentViewController;
@property (nonatomic, readonly) UIViewController *presentingViewController;
@property (nonatomic, readonly) UIViewController *presentedViewController;

@property (nonatomic, readonly) UINavigationController *navigationController;
@property (nonatomic, readonly) UISplitViewController *splitViewController;

@property (nonatomic, readonly) UINavigationItem *navigationItem;
@property (nonatomic, retain) UITabBarItem *tabBarItem;

@property (nonatomic, copy) NSArray *childViewControllers;

- (void)addChildViewController:(UIViewController *)childController;
- (void)removeFromParentViewController;
- (void)transitionFromViewController:(UIViewController *)fromViewController toViewController:(UIViewController *)toViewController duration:(NSTimeInterval)duration options:(UIViewAnimationOptions)options animations:(void (^)(void))animations completion:(void (^)(BOOL finished))completion;

- (void)willMoveToParentViewController:(UIViewController *)parent;
- (void)didMoveToParentViewController:(UIViewController *)parent;

- (void)presentViewController:(UIViewController *)viewControllerToPresent animated:(BOOL)flag completion:(void (^)(void))completion;
- (void)dismissViewControllerAnimated:(BOOL)flag completion:(void (^)(void))completion;

@property (nonatomic) BOOL definesPresentationContext;
@property (nonatomic) BOOL providesPresentationContextTransitionStyle;

@property (nonatomic) UIModalTransitionStyle modalTransitionStyle;
@property (nonatomic) UIModalPresentationStyle modalPresentationStyle;

@property (nonatomic) BOOL extendedLayoutIncludesOpaqueBars;
@property (nonatomic) BOOL automaticallyAdjustsScrollViewInsets;
@property (nonatomic) UIEdgeInsets additionalSafeAreaInsets;

@property (nonatomic, readonly) UIEdgeInsets safeAreaInsets;
- (void)viewSafeAreaInsetsDidChange;

@property (nonatomic) BOOL wantsFullScreenLayout;

@property (nonatomic, readonly) UIInterfaceOrientation preferredInterfaceOrientationForPresentation;
@property (nonatomic, readonly) NSUInteger supportedInterfaceOrientations;
@property (nonatomic, readonly) BOOL shouldAutorotate;

- (BOOL)shouldAutomaticallyForwardRotationMethods;
- (BOOL)shouldAutomaticallyForwardAppearanceMethods;

@property (nonatomic) BOOL hidesBottomBarWhenPushed;

@property (nonatomic, readonly) NSString *nibName;
@property (nonatomic, readonly) NSBundle *nibBundle;

@property (nonatomic) CGSize preferredContentSize;

- (void)setNeedsStatusBarAppearanceUpdate;
@property (nonatomic, readonly) UIStatusBarStyle preferredStatusBarStyle;
@property (nonatomic, readonly) BOOL prefersStatusBarHidden;
@property (nonatomic, readonly) UIStatusBarAnimation preferredStatusBarUpdateAnimation;

@property (nonatomic, readonly) UIViewController *childViewControllerForStatusBarStyle;
@property (nonatomic, readonly) UIViewController *childViewControllerForStatusBarHidden;

- (void)showViewController:(UIViewController *)vc sender:(id)sender;
- (void)showDetailViewController:(UIViewController *)vc sender:(id)sender;

@end

@interface UIViewController (UIViewControllerTransitioning)

@property (nonatomic, retain) UIPresentationController *presentationController;
@property (nonatomic, retain) id<UIViewControllerTransitioningDelegate> transitioningDelegate;

@end

@interface UIViewController (UIViewControllerRestoration)

@property (nonatomic, copy) NSString *restorationIdentifier;
@property (nonatomic, retain) Class<UIViewControllerRestoration> restorationClass;

@end
