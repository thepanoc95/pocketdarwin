/*
 * PureDarwin libSystem.B.dylib initializer -- the real dyld<->libSystem
 * handshake, not a hack.
 *
 * This constructor MUST live in libSystem.B.dylib itself, not in libdyld.dylib
 * (where it was originally, alongside dyldLibSystemGlue.c/pd_dladdr.c -- an
 * understandable first guess since it's dyld-shaped code). Real dyld hardcodes
 * a check (ImageLoaderMachO.cpp doModInitFunctions): until
 * gProcessInfo->libSystemInitialized is true, the ONLY image allowed to run
 * ANY initializer is the one whose install path exactly equals
 * LIBSYSTEM_DYLIB_PATH ("/usr/lib/libSystem.B.dylib") -- every other dylib's
 * initializer throws "initializer in image (...) that does not link with
 * libSystem.dylib". Since THIS constructor is what flips gLibSystemHelpers
 * from NULL to non-NULL (which is what sets libSystemInitialized = true in
 * the first place), it has to be the FIRST initializer dyld runs, and per
 * that check that means it has to be compiled into libSystem.B.dylib's own
 * object files, with libSystem.B.dylib's own install path -- exactly mirroring
 * real Darwin, where this handshake is libSystem_initializer(), part of
 * libSystem.B.dylib, not of any re-exported child.
 *
 * At load time dyld fills the __DATA,__dyld section (in dyldLibSystemGlue.c,
 * still built into libdyld.dylib -- that part IS dyld-side, not moved) with
 * its _dyld_func_lookup implementation. This constructor looks up dyld's
 * "__dyld_register_thread_helpers" entry point and hands it a LibSystemHelpers
 * table. Once registered, dyld's gLibSystemHelpers is non-NULL, which is what
 * lets dyld launch an LC_MAIN main executable (it needs
 * helpers->startGlueToCallExit + version >= 9; see dyld2.cpp:_main).
 *
 * We deliberately provide only the helper table + start glue, not the full
 * dyldAPIsInLibSystem.cpp API surface (dlopen/NSModule/...), which launchd does
 * not use. The struct layout mirrors dyld/src/dyldLibSystemInterface.h exactly
 * (version 13) -- that is the ABI dyld reads, so it must not drift.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <mach/mach.h>
#include <malloc/malloc.h>

struct __cxa_range_t { const void *addr; size_t length; };

struct ProgramVars {
	void *mh;
	int *NXArgcPtr;
	char ***NXArgvPtr;
	char ***environPtr;
	char **__prognamePtr;
};

/* Mirrors dyld::LibSystemHelpers (dyld/src/dyldLibSystemInterface.h). */
struct LibSystemHelpers {
	uintptr_t version;
	void   (*acquireGlobalDyldLock)(void);
	void   (*releaseGlobalDyldLock)(void);
	char  *(*getThreadBufferFor_dlerror)(size_t sizeRequired);
	void  *(*malloc)(size_t);
	void   (*free)(void *);
	int    (*cxa_atexit)(void (*)(void *), void *, void *);
	void   (*dyld_shared_cache_missing)(void);
	void   (*dyld_shared_cache_out_of_date)(void);
	void   (*acquireDyldInitializerLock)(void);
	void   (*releaseDyldInitializerLock)(void);
	int    (*pthread_key_create)(pthread_key_t *, void (*destructor)(void *));
	int    (*pthread_setspecific)(pthread_key_t, const void *);
	size_t (*malloc_size)(const void *ptr);
	void  *(*pthread_getspecific)(pthread_key_t);
	void   (*cxa_finalize)(const void *);
	void  *startGlueToCallExit;
	bool   (*hasPerThreadBufferFor_dlerror)(void);
	bool   (*isLaunchdOwned)(void);
	kern_return_t (*vm_alloc)(vm_map_t task, vm_address_t *addr, vm_size_t size, int flags);
	void  *(*mmap)(void *addr, size_t len, int prot, int flags, int fd, off_t offset);
	void   (*cxa_finalize_ranges)(const struct __cxa_range_t ranges[], int count);
};

extern void *pd_libdyld_getStartGlueToCallExit(void);
extern int _dyld_func_lookup(const char *name, void **address);
extern void _pthread_set_self(void *p);   /* p == NULL selects the VARIANT_DYLD seed path */
extern void _program_vars_init(const struct ProgramVars *vars);
extern void __atexit_init(void);
extern void _init_clock_port(void);
extern void __chk_init(void);
extern void __xlocale_init(void);
extern void __guard_setup(const char *apple[]);
extern void _subsystem_init(const char *apple[]);
extern void __malloc_init(const char *apple[]);
extern int mach_init(void);

static pthread_mutex_t sGlobalDyldLock = PTHREAD_MUTEX_INITIALIZER;
static void acquireGlobalDyldLock(void) { pthread_mutex_lock(&sGlobalDyldLock); }
static void releaseGlobalDyldLock(void) { pthread_mutex_unlock(&sGlobalDyldLock); }
static char sDlerrorBuffer[512];
static char *getThreadBufferFor_dlerror(size_t sizeRequired)
{
	return (sizeRequired <= sizeof(sDlerrorBuffer)) ? sDlerrorBuffer : NULL;
}
static bool hasPerThreadBufferFor_dlerror(void) { return false; }

static void noop_void(void) {}

static bool isLaunchdOwned(void) { return getpid() == 1; }

extern int  __cxa_atexit(void (*)(void *), void *, void *);
extern void __cxa_finalize(const void *);
extern void __cxa_finalize_ranges(const struct __cxa_range_t ranges[], int count);

static struct LibSystemHelpers sHelpers = {
	.version                     = 13,
	.acquireGlobalDyldLock       = &acquireGlobalDyldLock,
	.releaseGlobalDyldLock       = &releaseGlobalDyldLock,
	.getThreadBufferFor_dlerror  = &getThreadBufferFor_dlerror,
	.malloc                      = &malloc,
	.free                        = &free,
	.cxa_atexit                  = &__cxa_atexit,
	.dyld_shared_cache_missing   = &noop_void,
	.dyld_shared_cache_out_of_date = &noop_void,
	.acquireDyldInitializerLock  = NULL,
	.releaseDyldInitializerLock  = NULL,
	.pthread_key_create          = &pthread_key_create,
	.pthread_setspecific         = &pthread_setspecific,
	.malloc_size                 = &malloc_size,
	.pthread_getspecific         = &pthread_getspecific,
	.cxa_finalize                = &__cxa_finalize,
	.startGlueToCallExit         = NULL,  /* filled at runtime below -- see pd_libSystem_initializer() */
	.hasPerThreadBufferFor_dlerror = &hasPerThreadBufferFor_dlerror,
	.isLaunchdOwned              = &isLaunchdOwned,
	.vm_alloc                    = &vm_allocate,
	.mmap                        = &mmap,
	.cxa_finalize_ranges         = &__cxa_finalize_ranges,
};

__attribute__((constructor))
static void pd_libSystem_initializer(int argc, const char *argv[], const char *envp[],
    const char *apple[], const struct ProgramVars *vars)
{
	static int fallback_argc;
	static const char **fallback_argv;
	static const char **fallback_envp;
	static char *fallback_progname;
	static struct ProgramVars fallback_vars;

	if (vars == NULL) {
		fallback_argc = argc;
		fallback_argv = argv;
		fallback_envp = envp;
		fallback_progname = (argv && argv[0]) ? (char *)argv[0] : NULL;
		fallback_vars.mh = NULL;
		fallback_vars.NXArgcPtr = &fallback_argc;
		fallback_vars.NXArgvPtr = (char ***)&fallback_argv;
		fallback_vars.environPtr = (char ***)&fallback_envp;
		fallback_vars.__prognamePtr = &fallback_progname;
		vars = &fallback_vars;
	}

	_program_vars_init(vars);
	mach_init();
	_pthread_set_self(NULL);
	__atexit_init();
	_init_clock_port();
	__chk_init();
	__xlocale_init();
	__guard_setup(apple);
	_subsystem_init(apple);
	__malloc_init(apple);

	sHelpers.startGlueToCallExit = pd_libdyld_getStartGlueToCallExit();

	typedef void (*register_fn)(struct LibSystemHelpers *);
	register_fn reg = NULL;
	if (_dyld_func_lookup("__dyld_register_thread_helpers", (void **)&reg) && reg != NULL)
		reg(&sHelpers);
}
