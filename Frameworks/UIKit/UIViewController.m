#import <UIKit/UIViewController.h>
#import <UIKit/UIView.h>
#import <UIKit/UIApplication.h>
#import <UIKit/UIScreen.h>

@implementation UIViewController

- (instancetype)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    self = [super init];
    if (self) {
        _view = nil;
        _title = nil;
        _parentViewController = nil;
        _presentingViewController = nil;
        _presentedViewController = nil;
        _childViewControllers = [[NSArray alloc] init];
        _navigationItem = nil;
        _tabBarItem = nil;
        _viewLoaded = NO;
        _viewAppearing = NO;
        _viewAppeared = NO;
        _extendedLayoutIncludesOpaqueBars = NO;
        _automaticallyAdjustsScrollViewInsets = YES;
        _additionalSafeAreaInsets = UIEdgeInsetsZero;
        _modalTransitionStyle = UIModalTransitionStyleCoverVertical;
        _modalPresentationStyle = UIModalPresentationFullScreen;
        _definesPresentationContext = NO;
        _providesPresentationContextTransitionStyle = NO;
        _wantsFullScreenLayout = NO;
        _hidesBottomBarWhenPushed = NO;
        _nibName = [nibNameOrNil copy];
        _nibBundle = [nibBundleOrNil retain];
        _preferredContentSize = CGSizeZero;
    }
    return self;
}

- (instancetype)init {
    return [self initWithNibName:nil bundle:nil];
}

- (void)dealloc {
    [_view release];
    [_title release];
    [_childViewControllers release];
    [_navigationItem release];
    [_tabBarItem release];
    [_nibName release];
    [_nibBundle release];
    [super dealloc];
}

- (UIView *)view {
    if (!_view) {
        [self loadView];
        _viewLoaded = YES;
    }
    return _view;
}

- (void)setView:(UIView *)view {
    view = [view retain];
    [_view release];
    _view = view;
    _viewLoaded = (view != nil);
}

- (NSString *)title {
    return _title;
}

- (void)setTitle:(NSString *)title {
    title = [title copy];
    [_title release];
    _title = title;
}

- (void)loadView {
    CGRect frame = [[UIScreen mainScreen] bounds];
    UIView *v = [[UIView alloc] initWithFrame:frame];
    [v setBackgroundColor:[UIColor whiteColor]];
    [self setView:v];
    [v release];
}

- (void)viewDidLoad {
}

- (void)viewWillAppear:(BOOL)animated {
}

- (void)viewDidAppear:(BOOL)animated {
}

- (void)viewWillDisappear:(BOOL)animated {
}

- (void)viewDidDisappear:(BOOL)animated {
}

- (void)viewWillLayoutSubviews {
}

- (void)viewDidLayoutSubviews {
}

- (void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)coordinator {
}

- (BOOL)isViewLoaded {
    return _viewLoaded;
}

- (UIViewController *)parentViewController {
    return _parentViewController;
}

- (UIViewController *)presentingViewController {
    return _presentingViewController;
}

- (UIViewController *)presentedViewController {
    return _presentedViewController;
}

- (UINavigationController *)navigationController {
    UIViewController *parent = _parentViewController;
    while (parent) {
        if ([parent isKindOfClass:[UINavigationController class]])
            return (UINavigationController *)parent;
        parent = [parent parentViewController];
    }
    return nil;
}

- (UISplitViewController *)splitViewController {
    UIViewController *parent = _parentViewController;
    while (parent) {
        if ([parent isKindOfClass:[UISplitViewController class]])
            return (UISplitViewController *)parent;
        parent = [parent parentViewController];
    }
    return nil;
}

- (UINavigationItem *)navigationItem {
    if (!_navigationItem)
        _navigationItem = [[UINavigationItem alloc] initWithTitle:_title];
    return _navigationItem;
}

- (UITabBarItem *)tabBarItem {
    return _tabBarItem;
}

- (void)setTabBarItem:(UITabBarItem *)item {
    item = [item retain];
    [_tabBarItem release];
    _tabBarItem = item;
}

- (NSArray *)childViewControllers {
    return _childViewControllers;
}

- (void)setChildViewControllers:(NSArray *)childViewControllers {
    childViewControllers = [childViewControllers copy];
    [_childViewControllers release];
    _childViewControllers = childViewControllers;
}

- (void)addChildViewController:(UIViewController *)childController {
    if (!_childViewControllers) {
        _childViewControllers = [[NSArray alloc] init];
    }
    if (![_childViewControllers containsObject:childController]) {
        _childViewControllers = [[_childViewControllers arrayByAddingObject:childController] retain];
        [childController willMoveToParentViewController:self];
        childController->_parentViewController = self;
        [childController didMoveToParentViewController:self];
    }
}

- (void)removeFromParentViewController {
    [self willMoveToParentViewController:nil];
    if (_parentViewController) {
        NSMutableArray *children = [[_parentViewController->_childViewControllers mutableCopy] autorelease];
        [children removeObject:self];
        _parentViewController->_childViewControllers = [children copy];
        _parentViewController = nil;
    }
    [self didMoveToParentViewController:nil];
}

- (void)transitionFromViewController:(UIViewController *)fromViewController toViewController:(UIViewController *)toViewController duration:(NSTimeInterval)duration options:(UIViewAnimationOptions)options animations:(void (^)(void))animations completion:(void (^)(BOOL finished))completion {
    [fromViewController willMoveToParentViewController:nil];
    [self addChildViewController:toViewController];

    if (animations)
        animations();

    [fromViewController removeFromParentViewController];
    [toViewController didMoveToParentViewController:self];

    if (completion)
        completion(YES);
}

- (void)willMoveToParentViewController:(UIViewController *)parent {
}

- (void)didMoveToParentViewController:(UIViewController *)parent {
}

- (void)presentViewController:(UIViewController *)viewControllerToPresent animated:(BOOL)flag completion:(void (^)(void))completion {
    viewControllerToPresent->_presentingViewController = self;
    _presentedViewController = viewControllerToPresent;

    if (flag) {
        [UIView animateWithDuration:0.35 animations:^{
            [[viewControllerToPresent view] setFrame:[[self view] bounds]];
        } completion:^(BOOL finished) {
            if (completion)
                completion();
        }];
    } else {
        if (completion)
            completion();
    }
}

- (void)dismissViewControllerAnimated:(BOOL)flag completion:(void (^)(void))completion {
    [_presentedViewController viewWillDisappear:flag];
    [_presentedViewController->_presentingViewController viewWillAppear:flag];

    _presentedViewController->_presentingViewController = nil;
    _presentedViewController = nil;

    if (completion)
        completion();
}

- (BOOL)definesPresentationContext {
    return _definesPresentationContext;
}

- (void)setDefinesPresentationContext:(BOOL)defines {
    _definesPresentationContext = defines;
}

- (BOOL)providesPresentationContextTransitionStyle {
    return _providesPresentationContextTransitionStyle;
}

- (void)setProvidesPresentationContextTransitionStyle:(BOOL)provides {
    _providesPresentationContextTransitionStyle = provides;
}

- (UIModalTransitionStyle)modalTransitionStyle {
    return _modalTransitionStyle;
}

- (void)setModalTransitionStyle:(UIModalTransitionStyle)style {
    _modalTransitionStyle = style;
}

- (UIModalPresentationStyle)modalPresentationStyle {
    return _modalPresentationStyle;
}

- (void)setModalPresentationStyle:(UIModalPresentationStyle)style {
    _modalPresentationStyle = style;
}

- (BOOL)extendedLayoutIncludesOpaqueBars {
    return _extendedLayoutIncludesOpaqueBars;
}

- (void)setExtendedLayoutIncludesOpaqueBars:(BOOL)includes {
    _extendedLayoutIncludesOpaqueBars = includes;
}

- (BOOL)automaticallyAdjustsScrollViewInsets {
    return _automaticallyAdjustsScrollViewInsets;
}

- (void)setAutomaticallyAdjustsScrollViewInsets:(BOOL)adjusts {
    _automaticallyAdjustsScrollViewInsets = adjusts;
}

- (UIEdgeInsets)additionalSafeAreaInsets {
    return _additionalSafeAreaInsets;
}

- (void)setAdditionalSafeAreaInsets:(UIEdgeInsets)insets {
    _additionalSafeAreaInsets = insets;
}

- (UIEdgeInsets)safeAreaInsets {
    return UIEdgeInsetsZero;
}

- (void)viewSafeAreaInsetsDidChange {
}

- (BOOL)wantsFullScreenLayout {
    return _wantsFullScreenLayout;
}

- (void)setWantsFullScreenLayout:(BOOL)fullScreen {
    _wantsFullScreenLayout = fullScreen;
}

- (UIInterfaceOrientation)preferredInterfaceOrientationForPresentation {
    return UIInterfaceOrientationPortrait;
}

- (NSUInteger)supportedInterfaceOrientations {
    return UIInterfaceOrientationMaskAll;
}

- (BOOL)shouldAutorotate {
    return YES;
}

- (BOOL)shouldAutomaticallyForwardRotationMethods {
    return YES;
}

- (BOOL)shouldAutomaticallyForwardAppearanceMethods {
    return YES;
}

- (BOOL)hidesBottomBarWhenPushed {
    return _hidesBottomBarWhenPushed;
}

- (void)setHidesBottomBarWhenPushed:(BOOL)hides {
    _hidesBottomBarWhenPushed = hides;
}

- (NSString *)nibName {
    return _nibName;
}

- (NSBundle *)nibBundle {
    return _nibBundle;
}

- (CGSize)preferredContentSize {
    return _preferredContentSize;
}

- (void)setPreferredContentSize:(CGSize)size {
    _preferredContentSize = size;
}

- (void)setNeedsStatusBarAppearanceUpdate {
}

- (UIStatusBarStyle)preferredStatusBarStyle {
    return UIStatusBarStyleDefault;
}

- (BOOL)prefersStatusBarHidden {
    return NO;
}

- (UIStatusBarAnimation)preferredStatusBarUpdateAnimation {
    return UIStatusBarAnimationFade;
}

- (UIViewController *)childViewControllerForStatusBarStyle {
    return nil;
}

- (UIViewController *)childViewControllerForStatusBarHidden {
    return nil;
}

- (void)showViewController:(UIViewController *)vc sender:(id)sender {
    [[self navigationController] pushViewController:vc animated:YES];
}

- (void)showDetailViewController:(UIViewController *)vc sender:(id)sender {
}

- (void)encodeWithCoder:(NSCoder *)coder {
}

- (instancetype)initWithCoder:(NSCoder *)coder {
    return [self initWithNibName:nil bundle:nil];
}

@end

@implementation UIViewController (UIViewControllerTransitioning)

- (UIPresentationController *)presentationController {
    return nil;
}

- (void)setPresentationController:(UIPresentationController *)pc {
}

- (id<UIViewControllerTransitioningDelegate>)transitioningDelegate {
    return nil;
}

- (void)setTransitioningDelegate:(id<UIViewControllerTransitioningDelegate>)delegate {
}

@end

@implementation UIViewController (UIViewControllerRestoration)

- (NSString *)restorationIdentifier {
    return nil;
}

- (void)setRestorationIdentifier:(NSString *)identifier {
}

- (Class<UIViewControllerRestoration>)restorationClass {
    return nil;
}

- (void)setRestorationClass:(Class<UIViewControllerRestoration>)cls {
}

@end
