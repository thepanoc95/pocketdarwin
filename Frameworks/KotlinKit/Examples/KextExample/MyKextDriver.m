#import <Foundation/Foundation.h>
#import <KotlinKit/KotlinKit.h>
#import <GNUKext/GNUKext.h>

@interface MyKextDriver : GNUKextDriver
@end

@implementation MyKextDriver

+ (NSArray *)personalityKeys {
    return @[@"com.example.kotlin.kext"];
}

+ (BOOL)doesMatchProvider:(GNUKextRegistryEntry *)provider
           withPersonality:(GNUKextPersonality *)personality
{
    return YES;
}

- (instancetype)initWithPersonality:(GNUKextPersonality *)personality {
    self = [super initWithPersonality:personality];
    if (self) {
        NSLog(@"MyKextDriver initialized with personality: %@", [personality name]);
    }
    return self;
}

- (BOOL)start:(GNUKextRegistryEntry *)provider {
    NSLog(@"MyKextDriver starting on provider: %@", [provider entryPath]);
    return [super start:provider];
}

- (void)stop:(GNUKextRegistryEntry *)provider {
    NSLog(@"MyKextDriver stopping");
    [super stop:provider];
}

@end