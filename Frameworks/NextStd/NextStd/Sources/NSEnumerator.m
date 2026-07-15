/*
 * NextStd NSEnumerator Implementation
 */

#import "NSEnumerator.h"

@interface NSEnumerator ()
@property (nonatomic, assign) id *_objects;
@property (nonatomic, assign) NSUInteger _count;
@property (nonatomic, assign) NSUInteger _index;
@end

@implementation NSEnumerator

+ (instancetype)enumeratorWithObjects:(id *)objects count:(NSUInteger)count {
    NSEnumerator *en = [self alloc];
    en->_objects = objects;
    en->_count = count;
    en->_index = 0;
    return en;
}

- (id)nextObject {
    if (_index >= _count) return nil;
    return _objects[_index++];
}

@end