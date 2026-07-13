#pragma once
#include "cpu.h"
#include <stddef.h>

/* MSM8916 Memory Map */
#define MSM8916_RAM_BASE      0x80000000
#define MSM8916_RAM_SIZE      0x10000000  /* 256MB (enough for boot + kernel) */

/* Peripheral base addresses */
#define MSM8916_GIC_DIST      0xB000000
#define MSM8916_GIC_CPU       0xB002000
#define MSM8916_GIC_VCPU      0xB004000
#define MSM8916_GIC_HYP      0xB005000
#define MSM8916_GIC_VCPUFIB   0xB006000

#define MSM8916_BLSP1_UART2   0x78B0000
#define MSM8916_BLSP1_UART2_SIZE 0x200

#define MSM8916_CLK_CTL       0x1800000
#define MSM8916_CLK_CTL_SIZE  0x80000

#define MSM8916_GPIO          0x1000000
#define MSM8916_GPIO_SIZE     0x1000

#define MSM8916_SDC1          0x07824000
#define MSM8916_SDC2          0x07864000

#define MSM8916_MMCC_BASE     0x02400000

#define MSM8916_QUP_BASE      0x78B0000

/* Watchdog */
#define MSM8916_WDT_BASE      0xB017000

/* SPMI */
#define MSM8916_SPMI_BASE     0x200F000

/* Timer */
#define MSM8916_TIMER_BASE    0xB020000
#define MSM8916_TIMER_SIZE    0x1000

#define MSM8916_DLOAD_MSG     0x00058000
#define MSM8916_DLOAD_MSG_SIZE 0x10000

/* Platform state */
typedef struct {
    /* Flat memory */
    uint8_t *ram;
    uint32_t ram_base;
    uint32_t ram_size;

    /* UART state */
    uint8_t uart_status;
    uint8_t uart_fifo[64];
    int uart_fifo_head;
    int uart_fifo_tail;

    /* Timer */
    uint32_t timer_value;

    /* GIC (stub) */
    uint8_t gic_dist[0x1000];
    uint8_t gic_cpu[0x1000];

    /* Generic stub regions */
    uint8_t clk_ctl[0x1000];
    uint8_t gpio[0x1000];

    /* CPU reference */
    arm_cpu_t *cpu;
} msm8916_platform_t;

void platform_init(msm8916_platform_t *plat, arm_cpu_t *cpu);
void platform_destroy(msm8916_platform_t *plat);
int  platform_load_elf(msm8916_platform_t *plat, const char *path, uint32_t *entry_out);
