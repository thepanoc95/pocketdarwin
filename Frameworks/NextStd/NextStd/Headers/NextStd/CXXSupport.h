/*
 * NextStd C++ Support - Modern C++ Interoperability
 */

#ifndef _NEXTSTD_CXXSUPPORT_H_
#define _NEXTSTD_CXXSUPPORT_H_

#ifdef __cplusplus

#include <string>
#include <memory>
#include "NSString.h"

inline NSString *operator""_ns(const char *str, size_t) {
    return [NSString stringWithUTF8String:str];
}

template<typename T>
class ns_ptr {
    T *_obj;
public:
    ns_ptr(T *obj) : _obj(obj) {}
    ~ns_ptr() { if (_obj) [_obj release]; }
    T *get() const { return _obj; }
    T *operator->() const { return _obj; }
    T &operator*() const { return *_obj; }
};

#endif

#endif /* _NEXTSTD_CXXSUPPORT_H_ */