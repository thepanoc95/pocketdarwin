#include <sys/types.h>
#include <unistd.h>

extern pid_t __pd_sys_fork(void) __asm("___fork");

pid_t
fork(void)
{
	return __pd_sys_fork();
}
