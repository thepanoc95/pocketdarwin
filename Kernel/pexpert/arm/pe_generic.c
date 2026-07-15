/*
 * Copyright 2024, The PocketDarwin Team.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * Generic ARM platform expert stub.
 *
 * Used when no specific SoC platform expert is available.
 * Provides minimal defaults for bringup.
 *
 * Falls back to this implementation only when no board-specific
 * pe_*.c has already provided PE_init_SocSupport_stub.
 */

#ifndef PE_INIT_SOC_SUPPORT_STUB_ALREADY_DEFINED

#include <mach/mach_types.h>
#include <pexpert/pexpert.h>
#include <pexpert/arm/protos.h>
#include <pexpert/arm/boot.h>
#include <machine/machine_routines.h>
#include <vm/pmap.h>
#include <arm/pmap.h>

extern void *pmap_steal_memory(vm_size_t size);
extern vm_offset_t pmap_extract(pmap_t pmap, vm_offset_t virt);

#define KPRINTF_PREFIX  "PE_Generic: "

static void generic_putc(int c __unused)
{
    return;
}

static int generic_getc(void)
{
    return -1;
}

static void generic_uart_init(void)
{
    return;
}

static void generic_interrupt_init(void)
{
    ml_set_interrupts_enabled(FALSE);
}

static uint64_t generic_timer_value(void)
{
    return 0;
}

static void generic_timer_enabled(int enable __unused)
{
    return;
}

static void generic_timebase_init(void)
{
    gPEClockFrequencyInfo.timebase_frequency_hz = 1000000000;
    ml_set_interrupts_enabled(TRUE);
}

static uint64_t generic_get_timebase(void)
{
    return 0;
}

static void generic_handle_interrupt(void *context __unused)
{
    return;
}

static void generic_framebuffer_init(void)
{
    void *framebuffer = (void *)pmap_steal_memory(1024 * 768 * 4);
    void *framebuffer_phys = (void *)pmap_extract(kernel_pmap, (vm_offset_t)framebuffer);

    PE_state.video.v_baseAddr = (unsigned long)framebuffer_phys;
    PE_state.video.v_rowBytes = 1024 * 2;
    PE_state.video.v_width = 1024;
    PE_state.video.v_height = 768;
    PE_state.video.v_depth = 16;

    bzero(framebuffer, 1024 * 768 * 4);
    initialize_screen((void *)&PE_state.video, kPETextMode);
}

void PE_init_SocSupport_generic(void)
{
    gPESocDispatch.uart_getc = generic_getc;
    gPESocDispatch.uart_putc = generic_putc;
    gPESocDispatch.uart_init = generic_uart_init;

    gPESocDispatch.interrupt_init = generic_interrupt_init;
    gPESocDispatch.timebase_init = generic_timebase_init;

    gPESocDispatch.get_timebase = generic_get_timebase;

    gPESocDispatch.handle_interrupt = generic_handle_interrupt;

    gPESocDispatch.timer_value = generic_timer_value;
    gPESocDispatch.timer_enabled = generic_timer_enabled;

    gPESocDispatch.framebuffer_init = generic_framebuffer_init;

    generic_framebuffer_init();
}

void PE_init_SocSupport_stub(void)
{
    PE_early_puts("PE_init_SocSupport: Using generic ARM platform expert\n");
    PE_init_SocSupport_generic();
}

#endif /* !PE_INIT_SOC_SUPPORT_STUB_ALREADY_DEFINED */
