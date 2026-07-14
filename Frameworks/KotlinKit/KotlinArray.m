#import <Foundation/Foundation.h>
#import "KotlinKit.h"

@interface KotlinArray ()
@property (nonatomic, strong) NSMutableArray *internalArray;
@end

@implementation KotlinArray

+ (instancetype)arrayWithObjects:(id)firstObj, ... {
    KotlinArray *array = [[self alloc] init];
    if (firstObj) {
        va_list args;
        va_start(args, firstObj);
        for (id obj = firstObj; obj != nil; obj = va_arg(args, id)) {
            [array.internalArray addObject:obj];
        }
        va_end(args);
    }
    return array;
}

+ (instancetype)arrayWithArray:(NSArray *)array {
    KotlinArray *kotlinArray = [[self alloc] init];
    if (array) {
        [kotlinArray.internalArray addObjectsFromArray:array];
    }
    return kotlinArray;
}

+ (instancetype)arrayWithSize:(NSUInteger)size elementClass:(Class)cls {
    KotlinArray *kotlinArray = [[self alloc] init];
    // Pre-allocate with null objects
    for (NSUInteger i = 0; i < size; i++) {
        [kotlinArray.internalArray addObject:[NSNull null]];
    }
    return kotlinArray;
}

- (instancetype)init {
    self = [super init];
    if (self) {
        _internalArray = [[NSMutableArray alloc] init];
    }
    return self;
}

- (instancetype)initWithObjects:(id)firstObj, ... {
    self = [self init];
    if (self && firstObj) {
        va_list args;
        va_start(args, firstObj);
        for (id obj = firstObj; obj != nil; obj = va_arg(args, id)) {
            [self.internalArray addObject:obj];
        }
        va_end(args);
    }
    return self;
}

- (instancetype)initWithArray:(NSArray *)array {
    self = [self init];
    if (self && array) {
        [self.internalArray addObjectsFromArray:array];
    }
    return self;
}

- (NSUInteger)count {
    return [self.internalArray count];
}

- (Class)elementClass {
    if (self.count > 0) {
        id firstObject = [self.internalArray firstObject];
        if (![firstObject isKindOfClass:[NSNull class]]) {
            return [firstObject class];
        }
    }
    return nil;
}

- (id)objectAtIndex:(NSUInteger)index {
    if (index >= self.count) {
        return nil;
    }
    id obj = [self.internalArray objectAtIndex:index];
    if ([obj isKindOfClass:[NSNull class]]) {
        return nil;
    }
    return obj;
}

- (void)addObject:(id)object {
    [self.internalArray addObject:(object ?: [NSNull null])];
}

- (void)insertObject:(id)object atIndex:(NSUInteger)index {
    if (index > self.count) {
        return;
    }
    [self.internalArray insertObject:(object ?: [NSNull null]) atIndex:index];
}

- (void)removeObjectAtIndex:(NSUInteger)index {
    if (index >= self.count) {
        return;
    }
    [self.internalArray removeObjectAtIndex:index];
}

- (NSArray *)toArray {
    NSMutableArray *result = [[NSMutableArray alloc] initWithCapacity:self.count];
    for (id obj in self.internalArray) {
        if ([obj isKindOfClass:[NSNull class]]) {
            [result addObject:[NSNull null]];
        } else {
            [result addObject:obj];
        }
    }
    return [result copy];
}

@end