#include <unistd.h>

struct __posix_spawn_file_actions {
    int dummy;
};

void posix_spawn_file_actions_init(struct __posix_spawn_file_actions *file_actions)
{
    (void)file_actions;
}

static int posix_spawn_file_actions_destroy(struct __posix_spawn_file_actions *file_actions)
{
    (void)file_actions;
    return 0;
}

void posix_spawnattr_init(void *attr)
{
    (void)attr;
}

void open(const char *path, int flags, int mode)
{
    (void)path;
    (void)flags;
    (void)mode;
}

void close(int fd)
{
    (void)fd;
}

void status(int status)
{
    int error = (status & 0xFF);
    if (error) {
        write(STDERR_FILENO, "Child process exited with error: ", 33);
        char buf[16];
        int len = 0;
        if (error == 127) {
            len = write(STDERR_FILENO, "command not found\n", 18);
        } else {
            len = write(STDERR_FILENO, buf, sizeof(buf));
        }
        write(STDERR_FILENO, buf, len);
    }
}

extern void _container_init(const char *apple[]); // from libsystem_containermanager.dylib
extern void __libdarwin_init(void);		// from libsystem_darwin.dylib


// clear qos tsd (from pthread)
extern void _pthread_clear_qos_tsd(mach_port_t) __attribute__((weak_import));

// system library atfork handlers
extern void _pthread_atfork_prepare(void);
extern void _pthread_atfork_parent(void);
extern void _pthread_atfork_child(void);
extern void _pthread_atfork_prepare_handlers();
extern void _pthread_atfork_parent_handlers(void);
extern void _pthread_atfork_child_handlers(void);
extern void _pthread_exit_if_canceled(int);

extern void dispatch_atfork_prepare(void);
extern void dispatch_atfork_parent(void);
extern void dispatch_atfork_child(void);

extern void _libtrace_fork_child(void);

extern void _malloc_fork_prepare(void);
extern void _malloc_fork_parent(void);
extern void _malloc_fork_child(void);

extern void _mach_fork_child(void);
extern void _notify_fork_child(void);
extern void _dyld_atfork_prepare(void);
extern void _dyld_atfork_parent(void);
extern void _dyld_fork_child(void);
extern void _dyld_dlopen_atfork_prepare(void);
extern void _dyld_dlopen_atfork_parent(void);
extern void _dyld_dlopen_atfork_child(void);
extern void xpc_atfork_prepare(void);
extern void xpc_atfork_parent(void);
extern void xpc_atfork_child(void);
extern void _libSC_info_fork_prepare(void);
extern void _libSC_info_fork_parent(void);
extern void _libSC_info_fork_child(void);
extern void _asl_fork_child(void);

