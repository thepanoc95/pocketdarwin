/*
 * minlibc - syscall_compat.h
 * Cross-platform syscall definitions for ARM EABI
 * Supports: Linux, FreeBSD, macOS (XNU)
 */

#ifndef _MINLIBC_SYSCALL_COMPAT_H
#define _MINLIBC_SYSCALL_COMPAT_H

/* ============================================================================
 * Platform Detection
 * ============================================================================ */

#if defined(__linux__)
  #define MINLIBC_LINUX 1
#elif defined(__FreeBSD__)
  #define MINLIBC_BSD 1
#elif defined(__APPLE__)
  #define MINLIBC_APPLE 1
#else
  #error "Unsupported OS: Only Linux, FreeBSD, and Darwin are supported"
#endif

/* ============================================================================
 * ARM EABI Syscall Numbers by Platform
 * ============================================================================
 *
 * Linux ARM EABI:    https://github.com/torvalds/linux/blob/master/arch/arm/tools/syscall.tbl
 * FreeBSD ARM EABI:  https://svn.freebsd.org/base/head/sys/kern/syscalls.master
 * macOS ARM EABI:    https://opensource.apple.com/source/xnu/
 */

/* ---- Linux ARM EABI ---- */
#ifdef MINLIBC_LINUX

#define SYS_restart_syscall  0
#define SYS_exit             1
#define SYS_fork             2
#define SYS_read             3
#define SYS_write            4
#define SYS_open             5
#define SYS_close            6
#define SYS_waitpid          7
#define SYS_creat            8
#define SYS_link             9
#define SYS_unlink           10
#define SYS_execve           11
#define SYS_chdir            12
#define SYS_mknod            14
#define SYS_chmod            15
#define SYS_lchown           16
#define SYS_lseek            19
#define SYS_getpid           20
#define SYS_mount            21
#define SYS_umount2          22
#define SYS_setuid           23
#define SYS_getuid           24
#define SYS_stime            25
#define SYS_ptrace           26
#define SYS_alarm            27
#define SYS_pause            29
#define SYS_utime            30
#define SYS_access           33
#define SYS_nice             34
#define SYS_kill             37
#define SYS_rename           38
#define SYS_mkdir            39
#define SYS_rmdir            40
#define SYS_dup              41
#define SYS_pipe             42
#define SYS_times            43
#define SYS_brk              45
#define SYS_getgroups       47
#define SYS_setgroups       48
#define SYS_dup2            63
#define SYS_getppid         64
#define SYS_getpgrp         65
#define SYS_setsid          66
#define SYS_sigaction       67
#define SYS_sigpending      73
#define SYS_sethostname     74
#define SYS_setrlimit       75
#define SYS_getrlimit       76
#define SYS_getrusage       77
#define SYS_gettimeofday    78
#define SYS_settimeofday    79
#define SYS_symlink         83
#define SYS_readlink        85
#define SYS_uselib          86
#define SYS_swapon          87
#define SYS_reboot          88
#define SYS_munmap          91
#define SYS_truncate        92
#define SYS_ftruncate       93
#define SYS_fchmod          94
#define SYS_getpriority     96
#define SYS_setpriority     97
#define SYS_fsync           118
#define SYS_clone           120
#define SYS_setdomainname   121
#define SYS_uname           122
#define SYS_mprotect        125
#define SYS_getpgid         132
#define SYS_fchdir          133
#define SYS_bdflush         134
#define SYS_sysfs           135
#define SYS_personality     136
#define SYS__llseek         140
#define SYS_getdents        141
#define SYS__newselect      142
#define SYS_flock           143
#define SYS_msync           144
#define SYS_readv           145
#define SYS_writev          146
#define SYS_getsid          147
#define SYS_fdatasync       148
#define SYS__sysctl         149
#define SYS_mlock           150
#define SYS_munlock         151
#define SYS_mlockall        152
#define SYS_munlockall      153
#define SYS_sched_setparam  154
#define SYS_sched_getparam  155
#define SYS_sched_setscheduler 156
#define SYS_sched_getscheduler 157
#define SYS_prctl           172
#define SYS_rt_sigaction    174
#define SYS_rt_sigprocmask  175
#define SYS_rt_sigpending   176
#define SYS_rt_sigtimedwait 177
#define SYS_rt_sigaction    178
#define SYS_pread64         180
#define SYS_pwrite64        181
#define SYS_getcwd          183
#define SYS_capget          184
#define SYS_capset          185
#define SYS_sigaltstack     186
#define SYS_sendfile        187
#define SYS_getpmsg         188
#define SYS_putpmsg         189
#define SYS_vfork           190
#define SYS_getrlimit       191
#define SYS_mmap2           192
#define SYS_truncate64      193
#define SYS_ftruncate64     194
#define SYS_stat64          195
#define SYS_lstat64         196
#define SYS_fstat64         197
#define SYS_lchown32        198
#define SYS_getuid32        199
#define SYS_getgid32        200
#define SYS_geteuid32       201
#define SYS_getegid32       202
#define SYS_setreuid32      203
#define SYS_setregid32      204
#define SYS_getgroups32     205
#define SYS_setgroups32     206
#define SYS_fchown32        207
#define SYS_setresuid32     208
#define SYS_getresuid32     209
#define SYS_setresgid32     210
#define SYS_getresgid32     211
#define SYS_chown32         212
#define SYS_setuid32        213
#define SYS_setgid32        214
#define SYS_setfsuid32      215
#define SYS_setfsgid32      216
#define SYS_getdents64      217
#define SYS_pivot_root      218
#define SYS_mincore         219
#define SYS_madvise         220

#endif /* MINLIBC_LINUX */

/* ---- FreeBSD ARM EABI ---- */
#ifdef MINLIBC_BSD

#define SYS_syscall         0
#define SYS_exit            1
#define SYS_fork            2
#define SYS_read            3
#define SYS_write           4
#define SYS_open            5
#define SYS_close           6
#define SYS_wait4           7
#define SYS_creat           8
#define SYS_link            9
#define SYS_unlink          10
#define SYS_execv           11
#define SYS_chdir           12
#define SYS_fchdir          13
#define SYS_mknod           14
#define SYS_chmod           15
#define SYS_chown           16
#define SYS_break           17
#define SYS_getfsstat       18
#define SYS_lseek           19
#define SYS_getpid          20
#define SYS_mount           21
#define SYS_unmount         22
#define SYS_setuid          23
#define SYS_getuid          24
#define SYS_geteuid         25
#define SYS_ptrace          26
#define SYS_recvmsg         27
#define SYS_sendmsg         28
#define SYS_recvfrom        29
#define SYS_accept          30
#define SYS_getpeername     31
#define SYS_getsockname     32
#define SYS_access          33
#define SYS_chflags         34
#define SYS_fchflags        35
#define SYS_sync            36
#define SYS_kill            37
#define SYS_getppid         39
#define SYS_dup             41
#define SYS_pipe            42
#define SYS_getegid         43
#define SYS_profil          44
#define SYS_ktrace          45
#define SYS_sigaction       46
#define SYS_getgid          47
#define SYS_sigprocmask     48
#define SYS_getlogin        49
#define SYS_setlogin        50
#define SYS_acct            51
#define SYS_sigpending      52
#define SYS_sigaltstack     53
#define SYS_ioctl           54
#define SYS_reboot          55
#define SYS_revoke          56
#define SYS_symlink         57
#define SYS_readlink        58
#define SYS_execve          59
#define SYS_umask           60
#define SYS_chroot          61
#define SYS_msync           65
#define SYS_vfork           66
#define SYS_munmap          73
#define SYS_mprotect        74
#define SYS_madvise         75
#define SYS_mincore         78
#define SYS_getgroups       79
#define SYS_setgroups       80
#define SYS_getpgrp         81
#define SYS_setpgid         82
#define SYS_setitimer       83
#define SYS_swapon          85
#define SYS_getitimer       86
#define SYS_dup2            63
#define SYS_fcntl           92
#define SYS_select          93
#define SYS_fsync           95
#define SYS_setpriority     96
#define SYS_getpriority     100
#define SYS_fsync           95
#define SYS_getrusage       117
#define SYS_gettimeofday    116
#define SYS_settimeofday    79
#define SYS_getcwd          326

#endif /* MINLIBC_BSD */

/* ---- macOS ARM EABI (XNU) ---- */
#ifdef MINLIBC_APPLE

#define SYS_exit            1
#define SYS_fork            2
#define SYS_read            3
#define SYS_write           4
#define SYS_open            5
#define SYS_close           6
#define SYS_wait4           114
#define SYS_link            9
#define SYS_unlink          10
#define SYS_chdir           12
#define SYS_fchdir          13
#define SYS_mknod           14
#define SYS_chmod           15
#define SYS_chown           16
#define SYS_brk             45
#define SYS_getpid          20
#define SYS_getuid          24
#define SYS_geteuid         25
#define SYS_ptrace          26
#define SYS_recvmsg         27
#define SYS_sendmsg         28
#define SYS_recvfrom        29
#define SYS_accept          30
#define SYS_getpeername     31
#define SYS_getsockname     32
#define SYS_access          33
#define SYS_chflags         34
#define SYS_fchflags        35
#define SYS_sync            36
#define SYS_kill            37
#define SYS_getppid         39
#define SYS_dup             41
#define SYS_pipe            42
#define SYS_getegid         43
#define SYS_getgid          47
#define SYS_sigaction       46
#define SYS_sigprocmask     48
#define SYS_acct            51
#define SYS_sigpending      52
#define SYS_sigaltstack     53
#define SYS_ioctl           54
#define SYS_reboot          55
#define SYS_revoke          56
#define SYS_symlink         57
#define SYS_readlink        58
#define SYS_execve          59
#define SYS_umask           60
#define SYS_chroot          61
#define SYS_msync           65
#define SYS_vfork           66
#define SYS_dup2            63
#define SYS_fcntl           92
#define SYS_select          93
#define SYS_fsync           95
#define SYS_setpriority     96
#define SYS_getpriority     100
#define SYS_getrusage       117
#define SYS_gettimeofday    116
#define SYS_getitimer       86
#define SYS_setitimer       83
#define SYS_getgroups       79
#define SYS_setgroups       80
#define SYS_getpgrp         81
#define SYS_setpgid         82
#define SYS_setsid          147
#define SYS_munmap          73
#define SYS_mprotect        74
#define SYS_madvise         75
#define SYS_mincore         78
#define SYS_mmap            197
#define SYS_getcwd          326
#define SYS_stat            188
#define SYS_fstat           189
#define SYS_lstat           190
#define SYS_isatty          54

#endif /* MINLIBC_APPLE */

/* ============================================================================
 * Validate Syscall Definition
 * ============================================================================ */

#if !defined(SYS_exit) || !defined(SYS_fork) || !defined(SYS_read)
  #error "Syscall definitions not properly set for this platform"
#endif

#endif /* _MINLIBC_SYSCALL_COMPAT_H */