/*
 * NextStd NSArray Implementation
 */

#import "NSArray.h"
#import "NSEnumerator.h"

@interface NSArray ()
@property (nonatomic, assign) NSUInteger _count;
@property (nonatomic, assign) id *_objects;
@end

@implementation NSArray

+ (instancetype)array {
    return [[self alloc] init];
}

+ (instancetype)arrayWithObject:(id)object {
    NSArray *arr = [self alloc];
    arr->_count = 1;
    arr->_objects = (id *)malloc(sizeof(id));
    arr->_objects[0] = [object retain];
    return arr;
}

+ (instancetype)arrayWithObjects:(id)first, ... {
    if (!first) return [self array];
    
    va_list args;
    va_start(args, first);
    
    NSUInteger count = 1;
    id obj;
    while ((obj = va_arg(args, id)) != nil) {
        count++;
    }
    va_end(args);
    
    NSArray *arr = [self alloc];
    if (arr) {
        arr->_objects = (id *)malloc(sizeof(id) * count);
        arr->_count = count;
        arr->_objects[0] = [first retain];
        
        va_start(args, first);
        NSUInteger i = 1;
        while ((obj = va_arg(args, id)) != nil) {
            arr->_objects[i++] = [obj retain];
        }
        va_end(args);
    }
    return arr;
}

- (instancetype)init {
    _count = 0;
    _objects = NULL;
    return self;
}

- (NSUInteger)count {
    return _count;
}

- (id)objectAtIndex:(NSUInteger)index {
    if (index >= _count) return nil;
    return _objects[index];
}

- (id)firstObject {
    return _count > 0 ? _objects[0] : nil;
}

- (id)lastObject {
    return _count > 0 ? _objects[_count - 1] : nil;
}

- (NSUInteger)indexOfObject:(id)object {
    NSUInteger i;
    for (i = 0; i < _count; i++) {
        if (_objects[i] == object) return i;
    }
    return NSNotFound;
}

- (BOOL)containsObject:(id)object {
    return [self indexOfObject:object] != NSNotFound;
}

- (NSArray *)arrayByAddingObject:(id)object {
    NSArray *newArr = [NSArray alloc];
    newArr->_objects = (id *)malloc(sizeof(id) * (_count + 1));
    newArr->_count = _count + 1;
    NSUInteger i;
    for (i = 0; i < _count; i++) {
        newArr->_objects[i] = [_objects[i] retain];
    }
    newArr->_objects[_count] = [object retain];
    return newArr;
}

- (NSArray *)arrayByAddingObjectsFromArray:(NSArray *)other {
    NSMutableArray *result = [NSMutableArray arrayWithArray:self];
    for (id obj in [other objectEnumerator]) {
        [result addObject:obj];
    }
    return [result copy];
}

- (NSEnumerator *)objectEnumerator {
    return [NSEnumerator enumeratorWithObjects:_objects count:_count];
}

- (void)dealloc {
    NSUInteger i;
    for (i = 0; i < _count; i++) {
        [_objects[i] release];
    }
    free(_objects);
    NSDeallocateObject(self);
}

@end

@implementation NSMutableArray

- (void)addObject:(id)object {
    id *newObjects = (id *)realloc(_objects, sizeof(id) * (_count + 1));
    if (newObjects) {
        _objects = newObjects;
        _objects[_count] = [object retain];
        _count++;
    }
}

- (void)removeObjectAtIndex:(NSUInteger)index {
    if (index >= _count) return;
    [_objects[index] release];
    NSUInteger i;
    for (i = index; i < _count - 1; i++) {
        _objects[i] = _objects[i + 1];
    }
    _count--;
}

- (void)removeLastObject {
    if (_count == 0) return;
    _count--;
    [_objects[_count] release];
}

- (void)insertObject:(id)object atIndex:(NSUInteger)index {
    if (index > _count) return;
    id *newObjects = (id *)realloc(_objects, sizeof(id) * (_count + 1));
    if (newObjects) {
        _objects = newObjects;
        memmove(_objects + index + 1, _objects + index, sizeof(id) * (_count - index));
        _objects[index] = [object retain];
        _count++;
    }
}

- (void)replaceObjectAtIndex:(NSUInteger)index withObject:(id)object {
    if (index >= _count) return;
    [_objects[index] release];
    _objects[index] = [object retain];
}

- (void)removeAllObjects {
    NSUInteger i;
    for (i = 0; i < _count; i++) {
        [_objects[i] release];
    }
    free(_objects);
    _objects = NULL;
    _count = 0;
}

- (void)removeObject:(id)object {
    NSUInteger idx = [self indexOfObject:object];
    if (idx != NSNotFound) {
        [self removeObjectAtIndex:idx];
    }
}

@end