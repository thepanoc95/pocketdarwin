#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <KotlinKit/KotlinKit.h>
#import <KotlinKit/KotlinUIKit.h>

@interface KotlinViewController : UIViewController
{
    UILabel *_titleLabel;
    UIButton *_helloButton;
}
@end

@implementation KotlinViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    // Create a label using UIKit
    _titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(20, 40, 200, 30)];
    [_titleLabel setText:@"Hello from KotlinKit!"];
    [_titleLabel setBackgroundColor:[UIColor clearColor]];
    [self.view addSubview:_titleLabel];
    
    // Create a button
    _helloButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [_helloButton setFrame:CGRectMake(20, 80, 100, 30)];
    [_helloButton setTitle:@"Press Me" forState:UIControlStateNormal];
    [_helloButton addTarget:self action:@selector(buttonPressed:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:_helloButton];
}

- (void)buttonPressed:(id)sender
{
    NSLog(@"Button pressed - can call back into Kotlin!");
}

@end

@interface KotlinAppDelegate : NSObject <UIApplicationDelegate>
{
    UIWindow *_window;
    KotlinViewController *_viewController;
}
@end

@implementation KotlinAppDelegate

- (void)applicationDidFinishLaunching:(UIApplication *)application
{
    _window = [[UIWindow alloc] initWithFrame:CGRectMake(80, 80, 320, 200)];
    _viewController = [[KotlinViewController alloc] init];
    [_window setTitle:@"KotlinKit UIKit Example"];
    [_window setRootViewController:_viewController];
    [_window makeKeyAndVisible];
    [_window setBackgroundColor:[UIColor whiteColor]];
}

- (void)dealloc
{
    [_window release];
    [_viewController release];
    [super dealloc];
}

@end

int main(int argc, char **argv)
{
    return UIApplicationMain(argc, argv, nil, @"KotlinAppDelegate");
}