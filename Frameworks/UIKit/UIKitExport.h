#ifdef __clang__
#define UIKIT_DLLEXPORT
#define UIKIT_DLLIMPORT
#else
#define UIKIT_DLLEXPORT __declspec(dllexport)
#define UIKIT_DLLIMPORT __declspec(dllimport)
#endif

#ifdef __cplusplus

#if defined(__WIN32__)
#if defined(UIKIT_INSIDE_BUILD)
#define UIKIT_EXPORT extern "C" UIKIT_DLLEXPORT
#else
#define UIKIT_EXPORT extern "C" UIKIT_DLLIMPORT
#endif
#else
#define UIKIT_EXPORT extern "C"
#endif

#else

#if defined(__WIN32__)
#if defined(UIKIT_INSIDE_BUILD)
#define UIKIT_EXPORT UIKIT_DLLEXPORT extern
#else
#define UIKIT_EXPORT UIKIT_DLLIMPORT extern
#endif
#else
#define UIKIT_EXPORT extern
#endif

#endif
