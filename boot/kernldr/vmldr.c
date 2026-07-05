#include <stdint.h>

#pragma mark Virtual Memory Loader

#define MAX_KERNEL_SIZE 0x10000000

const char* VMLdrKernelBinName = "mach_kernel";

#pragma mark Setup reserved VFS (fat16)
