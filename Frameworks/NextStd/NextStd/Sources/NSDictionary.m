/*
 * NextStd NSDictionary Implementation
 */

#import "NSDictionary.h"
#import "NSEnumerator.h"
#import <stdlib.h>
#import <string.h>

@interface NSDictionary ()
@property (nonatomic, assign) NSUInteger _count;
@property (nonatomic, assign) id *_keys;
@property (nonatomic, assign) id *_values;
@end

@implementation NSDictionary

+ (instancetype)dictionary {
    NSDictionary *obj = [self alloc];
    obj->_count = 0;
    obj->_keys = NULL;
    obj->_values = NULL;
    return obj;
}

+ (instancetype)dictionaryWithObject:(id)value forKey:(id)key {
    NSDictionary *obj = [self alloc];
    obj->_count = 1;
    obj->_keys = malloc(sizeof(id));
    obj->_values = malloc(sizeof(id));
    obj->_keys[0] = [key retain];
    obj->_values[0] = [value retain];
    return obj;
}

+ (instancetype)dictionaryWithObjects:(id)first, forKeys:(id)firstKey, ... {
    if (!first || !firstKey) return [self dictionary];
    
    va_list args;
    va_start(args, firstKey);
    
    NSUInteger count = 1;
    id obj;
    while ((obj = va_arg(args, id)) != nil) {
        count++;
    }
    va_end(args);
    
    NSDictionary *dict = [self alloc];
    if (dict) {
        dict->_count = count;
        dict->_keys = malloc(sizeof(id) * count);
        dict->_values = malloc(sizeof(id) * count);
        dict->_keys[0] = [firstKey retain];
        dict->_values[0] = [first retain];
        
        va_start(args, firstKey);
        NSUInteger i = 1;
        while ((obj = va_arg(args, id)) != nil) {
            id value = va_arg(args, id);
            dict->_keys[i] = [obj retain];
            dict->_values[i] = [value retain];
            i++;
        }
        va_end(args);
    }
    return dict;
}

- (instancetype)initWithDictionary:(NSDictionary *)dict {
    _count = [dict count];
    _keys = malloc(sizeof(id) * _count);
    _values = malloc(sizeof(id) * _count);
    NSUInteger i;
    for (i = 0; i < _count; i++) {
        _keys[i] = [[[dict allKeys] objectAtIndex:i] retain];
        _values[i] = [[dict objectForKey:[_keys[i]] retain];
    }
    return self;
}

- (NSUInteger)count {
    return _count;
}

- (NSArray *)allKeys {
    NSMutableArray *keys = [NSMutableArray array];
    NSUInteger i;
    for (i = 0; i < _count; i++) {
        [keys addObject:_keys[i]];
    }
    return keys;
}

- (NSArray *)allValues {
    NSMutableArray *values = [NSMutableArray array];
    NSUInteger i;
    for (i = 0; i < _count; i++) {
        [values addObject:_values[i]];
    }
    return values;
}

- (id)objectForKey:(id)key {
    NSUInteger i;
    for (i = 0; i < _count; i++) {
        if ([_keys[i] isEqual:key]) return _values[i];
    }
    return nil;
}

- (NSEnumerator *)keyEnumerator {
    return [NSEnumerator enumeratorWithObjects:_keys count:_count];
}

- (NSEnumerator *)objectEnumerator {
    return [NSEnumerator enumeratorWithObjects:_values count:_count];
}

- (void)dealloc {
    NSUInteger i;
    for (i = 0; i < _count; i++) {
        [_keys[i] release];
        [_values[i] release];
    }
    free(_keys);
    free(_values);
    NSDeallocateObject(self);
}

@end

@implementation NSMutableDictionary

- (void)setObject:(id)object forKey:(id)key {
    NSUInteger i;
    for (i = 0; i < _count; i++) {
        if ([_keys[i] isEqual:key]) {
            [_values[i] release];
            _values[i] = [object retain];
            return;
        }
    }
    
    id *newKeys = realloc(_keys, sizeof(id) * (_count + 1));
    id *newValues = realloc(_values, sizeof(id) * (_count + 1));
    if (newKeys && newValues) {
        _keys = newKeys;
        _values = newValues;
        _keys[_count] = [key retain];
        _values[_count] = [object retain];
        _count++;
    }
}

- (void)removeObjectForKey:(id)key {
    NSUInteger i;
    for (i = 0; i < _count; i++) {
        if ([_keys[i] isEqual:key]) {
            [_keys[i] release];
            [_values[i] release];
            memmove(_keys + i, _keys + i + 1, sizeof(id) * (_count - i - 1));
            memmove(_values + i, _values + i + 1, sizeof(id) * (_count - i - 1));
            _count--;
            return;
        }
    }
}

- (void)removeAllObjects {
    NSUInteger i;
    for (i = 0; i < _count; i++) {
        [_keys[i] release];
        [_values[i] release];
    }
    free(_keys);
    free(_values);
    _count = 0;
}

@end