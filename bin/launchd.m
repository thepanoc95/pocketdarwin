#import <Foundation/Foundation.h>

#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef RB_POWER_OFF
#define RB_POWER_OFF RB_HALT
#endif

static volatile sig_atomic_t shutting_down;
static volatile sig_atomic_t want_poweroff;
static volatile sig_atomic_t want_reboot;

@interface TTYService : NSObject
{
	NSString *_tty;
	NSMutableArray *_arguments;
	pid_t _pid;
}
@property (nonatomic, retain) NSString *tty;
@property (nonatomic, retain) NSMutableArray *arguments;
@property (nonatomic, assign) pid_t pid;
@end

@implementation TTYService
@synthesize tty = _tty;
@synthesize arguments = _arguments;
@synthesize pid = _pid;

- (instancetype)init {
	self = [super init];
	if (self) {
		_arguments = [[NSMutableArray alloc] init];
		_pid = -1;
	}
	return self;
}

- (void)dealloc {
	[_tty release];
	[_arguments release];
	[super dealloc];
}
@end

static NSMutableArray *ttyServices = nil;

static void
on_signal(int sig)
{
	switch (sig) {
	case SIGTERM:
	case SIGINT:
		want_poweroff = 1;
		break;
	case SIGUSR1:
		want_reboot = 1;
		break;
	}
}

static void
install_signal_handlers(void)
{
	struct sigaction sa;
	struct sigaction chld;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = on_signal;
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGUSR1, &sa, NULL);

	memset(&chld, 0, sizeof(chld));
	chld.sa_handler = SIG_DFL;
	sigaction(SIGCHLD, &chld, NULL);
}

static void
mkdir_p(NSString *path, mode_t mode)
{
	if (mkdir([path UTF8String], mode) < 0 && errno != EEXIST) {
		perror([path UTF8String]);
	}
}

static void
try_mount(NSString *src, NSString *target, int flags, void *data)
{
	mkdir_p(target, 0755);
	if (mount([src UTF8String], [target UTF8String], flags, data) < 0 && errno != EBUSY) {
		fprintf(stderr, "mount %s on %s failed: ", [src UTF8String], [target UTF8String]);
		perror("");
	}
}

static int
open_tty_strict(NSString *tty)
{
	return open([tty UTF8String], O_RDWR | O_NOCTTY);
}

static int
open_console_fallback(void)
{
	return open("/dev/console", O_RDWR | O_NOCTTY);
}

static void
attach_fd_stdio(int fd)
{
	if (fd < 0) {
		return;
	}

	ioctl(fd, TIOCSCTTY, 1);
	dup2(fd, STDIN_FILENO);
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);

	if (fd > STDERR_FILENO) {
		close(fd);
	}
}

static void
attach_tty_stdio_strict(NSString *tty)
{
	attach_fd_stdio(open_tty_strict(tty));
}

static void
attach_console_stdio(void)
{
	attach_fd_stdio(open_console_fallback());
}

static void
console_write(NSString *msg)
{
	int fd = open("/dev/console", O_WRONLY | O_NOCTTY);

	if (fd >= 0) {
		write(fd, [msg UTF8String], [msg lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
		close(fd);
	}
}

static void
init_log(NSString *msg)
{
	console_write(msg);
}

static void
add_default_ttys(void)
{
	TTYService *svc = [[[TTYService alloc] init] autorelease];
	svc.tty = @"/dev/console";
	[svc.arguments addObject:@"/bin/sh"];
	[svc.arguments addObject:@"-i"];
	[ttyServices addObject:svc];
}

static void
load_ttys(NSString *path)
{
	NSError *error = nil;
	NSString *content = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:&error];
	if (content == nil) {
		init_log(@"[init] no /etc/ttys; using defaults\n");
		add_default_ttys();
		return;
	}

	NSArray *lines = [content componentsSeparatedByCharactersInSet:[NSCharacterSet newlineCharacterSet]];
	for (NSString *line in lines) {
		NSString *trimmed = [line stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
		if ([trimmed length] == 0 || [trimmed hasPrefix:@"#"]) {
			continue;
		}

		NSMutableArray *tokens = [NSMutableArray array];
		NSArray *rawTokens = [trimmed componentsSeparatedByCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
		for (NSString *tok in rawTokens) {
			if ([tok length] == 0) {
				continue;
			}
			if ([tok hasPrefix:@"#"]) {
				break;
			}
			[tokens addObject:tok];
		}

		if ([tokens count] == 0) {
			continue;
		}

		TTYService *svc = [[[TTYService alloc] init] autorelease];
		svc.tty = [tokens objectAtIndex:0];
		if ([tokens count] > 1) {
			for (NSUInteger i = 1; i < [tokens count]; i++) {
				[svc.arguments addObject:[tokens objectAtIndex:i]];
			}
		} else {
			[svc.arguments addObject:@"/bin/sh"];
			[svc.arguments addObject:@"-i"];
		}
		[ttyServices addObject:svc];
	}

	if ([ttyServices count] == 0) {
		init_log(@"[init] /etc/ttys empty; using defaults\n");
		add_default_ttys();
	}
}

static pid_t
spawn_tty(TTYService *svc)
{
	if (access([svc.tty UTF8String], R_OK | W_OK) < 0) {
		return -1;
	}

	pid_t pid = fork();
	if (pid < 0) {
		perror("fork");
		return -1;
	}

	if (pid == 0) {
		setsid();
		attach_tty_stdio_strict(svc.tty);

		NSUInteger count = [svc.arguments count];
		char **c_argv = calloc(count + 1, sizeof(char *));
		for (NSUInteger i = 0; i < count; i++) {
			c_argv[i] = (char *)[[svc.arguments objectAtIndex:i] UTF8String];
		}
		c_argv[count] = NULL;

		execv(c_argv[0], c_argv);
		perror(c_argv[0]);
		_exit(127);
	}

	return pid;
}

static int
run_wait(NSArray *arguments)
{
	pid_t pid = fork();
	int status = 0;

	if (pid < 0) {
		perror("fork");
		return -1;
	}

	if (pid == 0) {
		setsid();
		attach_console_stdio();

		NSUInteger count = [arguments count];
		char **c_argv = calloc(count + 1, sizeof(char *));
		for (NSUInteger i = 0; i < count; i++) {
			c_argv[i] = (char *)[[arguments objectAtIndex:i] UTF8String];
		}
		c_argv[count] = NULL;

		execv(c_argv[0], c_argv);
		perror(c_argv[0]);
		_exit(127);
	}

	while (waitpid(pid, &status, 0) < 0) {
		if (errno == EINTR) {
			continue;
		}
		perror("waitpid");
		return -1;
	}

	return status;
}

static int
reap_some_children(void)
{
	int reaped = 0;

	for (;;) {
		pid_t pid = waitpid(-1, NULL, WNOHANG);
		if (pid <= 0) {
			break;
		}
		reaped++;
	}

	return reaped;
}

static void
wait_for_children_ms(int timeout_ms)
{
	int waited_ms = 0;

	while (waited_ms < timeout_ms) {
		reap_some_children();
		if (waitpid(-1, NULL, WNOHANG) < 0 && errno == ECHILD) {
			return;
		}
		usleep(50 * 1000);
		waited_ms += 50;
	}
}

static void
kill_everything(void)
{
	kill(-1, SIGTERM);
	wait_for_children_ms(1000);
	kill(-1, SIGKILL);
	wait_for_children_ms(500);
}

static void
shutdown_common(void)
{
	init_log(@"[init] shutdown requested\n");
	if (access("/etc/init/rc.shutdown", X_OK) == 0) {
		run_wait([NSArray arrayWithObject:@"/etc/init/rc.shutdown"]);
	}

	kill_everything();
	sync();

	init_log(@"[init] unmounting /dev\n");
	unmount("/dev", 0);
	sync();
}

static void
maybe_shutdown(void)
{
	if (!want_poweroff && !want_reboot) {
		return;
	}

	shutting_down = 1;
	shutdown_common();

	if (want_reboot) {
		reboot(RB_AUTOBOOT);
	} else {
		reboot(RB_POWER_OFF);
	}

	init_log(@"[init] reboot syscall returned; halting\n");
	for (;;) {
		pause();
	}
}

int
main(void)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	if (getpid() != 1) {
		fprintf(stderr, "init: must be PID 1\n");
		[pool release];
		return 1;
	}

	install_signal_handlers();

	mkdir_p(@"/dev", 0755);
	try_mount(@"devfs", @"/dev", 0, NULL);

	if (access("/etc/init/rc.boot", X_OK) == 0) {
		run_wait([NSArray arrayWithObject:@"/etc/init/rc.boot"]);
	}

	setenv("HOME", "/root", 1);
	setenv("USER", "root", 1);
	setenv("LOGNAME", "root", 1);
	setenv("SHELL", "/bin/sh", 1);
	setenv("PATH", "/bin:/sbin:/usr/bin:/usr/sbin", 1);

	ttyServices = [[NSMutableArray alloc] init];

	load_ttys(@"/etc/ttys");

	for (TTYService *svc in ttyServices) {
		svc.pid = spawn_tty(svc);
	}

	for (;;) {
		int status = 0;
		pid_t pid = waitpid(-1, &status, 0);

		if (pid < 0) {
			if (errno == EINTR) {
				maybe_shutdown();
				continue;
			}

			if (errno == ECHILD) {
				if (!shutting_down) {
					sleep(1);
					for (TTYService *svc in ttyServices) {
						svc.pid = spawn_tty(svc);
					}
				}
				continue;
			}

			perror("waitpid");
			sleep(1);
			continue;
		}

		for (TTYService *svc in ttyServices) {
			if (pid == svc.pid) {
				if (!shutting_down) {
					sleep(1);
					svc.pid = spawn_tty(svc);
				}
				break;
			}
		}

		maybe_shutdown();
	}

	[ttyServices release];
	[pool release];
	return 0;
}
