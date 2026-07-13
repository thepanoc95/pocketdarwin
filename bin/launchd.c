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

#define MAX_TTYS 16
#define MAX_TTY_ARGS 16
#define MAX_LINE 256

struct tty_service {
	char tty[64];
	char *argv[MAX_TTY_ARGS];
	pid_t pid;
};

static struct tty_service ttys[MAX_TTYS];
static int tty_count;

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
mkdir_p(const char *path, mode_t mode)
{
	if (mkdir(path, mode) < 0 && errno != EEXIST) {
		perror(path);
	}
}

static char *
skip_ws(char *s)
{
	while (*s == ' ' || *s == '\t') {
		s++;
	}
	return s;
}

static void
strip_newline(char *s)
{
	size_t len = strlen(s);

	while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
		s[--len] = '\0';
	}
}

static void
try_mount(const char *src, const char *target, int flags, void *data)
{
	mkdir_p(target, 0755);
	if (mount(src, target, flags, data) < 0 && errno != EBUSY) {
		fprintf(stderr, "mount %s on %s failed: ", src, target);
		perror("");
	}
}

static int
open_tty_strict(const char *tty)
{
	return open(tty, O_RDWR | O_NOCTTY);
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
attach_tty_stdio_strict(const char *tty)
{
	attach_fd_stdio(open_tty_strict(tty));
}

static void
attach_console_stdio(void)
{
	attach_fd_stdio(open_console_fallback());
}

static void
console_write(const char *msg)
{
	int fd = open("/dev/console", O_WRONLY | O_NOCTTY);

	if (fd >= 0) {
		write(fd, msg, strlen(msg));
		close(fd);
	}
}

static void
init_log(const char *msg)
{
	console_write(msg);
}

static int
parse_tty_line(char *line, struct tty_service *svc)
{
	char *p;
	char *tok;
	int argc = 0;

	strip_newline(line);
	p = skip_ws(line);
	if (*p == '\0' || *p == '#') {
		return 0;
	}

	tok = strtok(p, " \t");
	if (tok == NULL) {
		return 0;
	}

	snprintf(svc->tty, sizeof(svc->tty), "%s", tok);
	svc->pid = -1;

	while (argc < MAX_TTY_ARGS - 1) {
		tok = strtok(NULL, " \t");
		if (tok == NULL || tok[0] == '#') {
			break;
		}
		svc->argv[argc] = strdup(tok);
		if (svc->argv[argc] == NULL) {
			return -1;
		}
		argc++;
	}

	svc->argv[argc] = NULL;
	if (argc == 0) {
		svc->argv[0] = strdup("/bin/sh");
		svc->argv[1] = strdup("-i");
		svc->argv[2] = NULL;
		if (svc->argv[0] == NULL || svc->argv[1] == NULL) {
			return -1;
		}
	}

	return 1;
}

static void
add_default_ttys(void)
{
	const char *defaults[] = {
		"/dev/console /bin/sh -i",
	};

	for (size_t i = 0; i < sizeof(defaults) / sizeof(defaults[0]); i++) {
		char line[MAX_LINE];

		if (tty_count >= MAX_TTYS) {
			return;
		}
		snprintf(line, sizeof(line), "%s", defaults[i]);
		if (parse_tty_line(line, &ttys[tty_count]) > 0) {
			tty_count++;
		}
	}
}

static void
load_ttys(const char *path)
{
	FILE *f = fopen(path, "r");
	char line[MAX_LINE];

	if (f == NULL) {
		init_log("[init] no /etc/ttys; using defaults\n");
		add_default_ttys();
		return;
	}

	while (fgets(line, sizeof(line), f)) {
		int ret;

		if (tty_count >= MAX_TTYS) {
			break;
		}
		ret = parse_tty_line(line, &ttys[tty_count]);
		if (ret > 0) {
			tty_count++;
		}
	}

	fclose(f);
	if (tty_count == 0) {
		init_log("[init] /etc/ttys empty; using defaults\n");
		add_default_ttys();
	}
}

static pid_t
spawn_tty(const char *tty, char *const argv[])
{
	pid_t pid;

	if (access(tty, R_OK | W_OK) < 0) {
		return -1;
	}

	pid = fork();
	if (pid < 0) {
		perror("fork");
		return -1;
	}

	if (pid == 0) {
		setsid();
		attach_tty_stdio_strict(tty);
		execv(argv[0], argv);
		perror(argv[0]);
		_exit(127);
	}

	return pid;
}

static int
run_wait(char *const argv[])
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
		execv(argv[0], argv);
		perror(argv[0]);
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
	char *shutdown_argv[] = { "/etc/init/rc.shutdown", NULL };

	init_log("[init] shutdown requested\n");
	if (access("/etc/init/rc.shutdown", X_OK) == 0) {
		run_wait(shutdown_argv);
	}

	kill_everything();
	sync();

	init_log("[init] unmounting /dev\n");
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

	init_log("[init] reboot syscall returned; halting\n");
	for (;;) {
		pause();
	}
}

int
main(void)
{
	char *boot_argv[] = { "/etc/init/rc.boot", NULL };

	if (getpid() != 1) {
		fprintf(stderr, "init: must be PID 1\n");
		return 1;
	}

	install_signal_handlers();

	mkdir_p("/dev", 0755);
	try_mount("devfs", "/dev", 0, NULL);

	if (access("/etc/init/rc.boot", X_OK) == 0) {
		run_wait(boot_argv);
	}

	setenv("HOME", "/root", 1);
	setenv("USER", "root", 1);
	setenv("LOGNAME", "root", 1);
	setenv("SHELL", "/bin/sh", 1);
	setenv("PATH", "/bin:/sbin:/usr/bin:/usr/sbin", 1);

	load_ttys("/etc/ttys");

	for (int i = 0; i < tty_count; i++) {
		ttys[i].pid = spawn_tty(ttys[i].tty, ttys[i].argv);
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
					for (int i = 0; i < tty_count; i++) {
						ttys[i].pid = spawn_tty(ttys[i].tty, ttys[i].argv);
					}
				}
				continue;
			}

			perror("waitpid");
			sleep(1);
			continue;
		}

		for (int i = 0; i < tty_count; i++) {
			if (pid == ttys[i].pid) {
				if (!shutting_down) {
					sleep(1);
					ttys[i].pid = spawn_tty(ttys[i].tty, ttys[i].argv);
				}
				break;
			}
		}

		maybe_shutdown();
	}
}
