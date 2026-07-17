#ifndef _OBJC_RUNTIME_H
#define _OBJC_RUNTIME_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct objc_class *Class;
typedef void *SEL;
typedef void *id;

static inline Class objc_getClass(const char *name) { return 0; }
static inline Class objc_getMetaClass(const char *name) { return 0; }

#ifdef __cplusplus
}
#endif

#endif