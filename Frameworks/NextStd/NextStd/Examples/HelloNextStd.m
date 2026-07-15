// HelloNextStd.m - Example showing NextStd usage
#import <NextStd/NextStd.h>

#ifdef __cplusplus
#import <NextStd/CXXSupport.h>
#import <iostream>

int main() {
    // C++ style usage
    auto str = NSStringFromStdString("Hello, NextStd!");
    auto arr = NSArray::arrayWithObjects(@"one"_ns, @"two"_ns, @"three"_ns, nil);
    
    std::cout << "String: " << [[str UTF8String] UTF8String] << std::endl;
    std::cout << "Array count: " << [arr count] << std::endl;
    
    return 0;
}
#endif

// Objective-C style usage
int mainObjC() {
    NSString *str = [NSString stringWithUTF8String:"Hello, NextStd!"];
    NSArray *arr = [NSArray arrayWithObjects:@"one", @"two", @"three", nil];
    
    NSLog(@"String: %@", str);
    NSLog(@"Array count: %lu", (unsigned long)[arr count]);
    
    return 0;
}