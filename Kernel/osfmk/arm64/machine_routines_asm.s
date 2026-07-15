/*
 * Copyright 2013-2024, The PocketDarwin Project
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
 * ARM64 machine routines - inline implementations
 */

#include <arm64/asm.h>
#include <arm/asm_help.h>
#include <assym.s>

/*
 * get_preemption_level - returns current preemption level
 */
.text
.p2align 3
.globl _get_preemption_level
.p2align 2
_get_preemption_level:
	mrs	x12, TPIDR_EL1
	cbz	x12, .Lget_preempt_zero
	add	x13, x12, MACHINE_THREAD_PREEMPT_COUNT
	ldr	w0, [x13]
	ret
.Lget_preempt_zero:
	mov	w0, #0
	ret

/*
 * disable_preemption / _disable_preemption
 */
.globl ___disable_preemption
.globl __disable_preemption
.globl _disable_preemption
.globl disable_preemption
___disable_preemption:
__disable_preemption:
_disable_preemption:
disable_preemption:
	mrs	x12, TPIDR_EL1
	cbz	x12, .Ldisable_preempt_out
	add	x13, x12, MACHINE_THREAD_PREEMPT_COUNT
	ldr	w2, [x13]
	add	w2, w2, #1
	str	w2, [x13]
.Ldisable_preempt_out:
	ret

/*
 * enable_preemption / _enable_preemption
 */
.globl ___enable_preemption
.globl __enable_preemption
.globl _enable_preemption
.globl enable_preemption
___enable_preemption:
__enable_preemption:
_enable_preemption:
enable_preemption:
	mrs	x12, TPIDR_EL1
	cbz	x12, .Lenable_preempt_out
	add	x13, x12, MACHINE_THREAD_PREEMPT_COUNT
	ldr	w2, [x13]
	subs	w2, w2, #1
	str	w2, [x13]
	b.ne	.Lenable_preempt_out
.Lenable_preempt_out:
	ret

/*
 * ml_set_interrupts_enabled
 */
.globl _ml_set_interrupts_enabled
.p2align 2
_ml_set_interrupts_enabled:
	cbz	w0, .Ldisable_irq
	msr	DAIFClr, #0xf
	mov	w0, #1
	ret
.Ldisable_irq:
	msr	DAIFSet, #0xf
	mov	w0, #0
	ret

/*
 * ml_get_interrupts_enabled
 */
.globl _ml_get_interrupts_enabled
.p2align 2
_ml_get_interrupts_enabled:
	mrs	x1, DAIF
	mov	w0, #1
	tst	x1, #0xf0
	csel	w0, wzr, w0, ne
	ret

/*
 * machine_idle
 */
.globl _machine_idle
.p2align 2
_machine_idle:
	wfi
	ret

/*
 * current_thread
 */
.globl _current_thread
.p2align 2
_current_thread:
	mrs	x0, TPIDR_EL1
	ret