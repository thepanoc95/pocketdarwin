/*
 * Copyright (c) 2007 Apple Inc. All rights reserved.
 * ARM64 DTrace support - stub implementation
 */
/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS/LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

#ifndef _MACH_ARM64_SDT_ISA_H
#define	_MACH_ARM64_SDT_ISA_H

/* ARM64 DTrace stubs - actual implementation needs DTrace provider */
#define DTRACE_CALL0ARGS(provider, name)
#define DTRACE_CALL1ARG(provider, name)
#define DTRACE_CALL2ARGS(provider, name)
#define DTRACE_CALL3ARGS(provider, name)
#define DTRACE_CALL4ARGS(provider, name)
#define DTRACE_CALL5ARGS(provider, name)
#define DTRACE_CALL6ARGS(provider, name)
#define DTRACE_CALL7ARGS(provider, name)
#define DTRACE_CALL8ARGS(provider, name)
#define DTRACE_CALL9ARGS(provider, name)
#define DTRACE_CALL10ARGS(provider, name)

#define ARG1_EXTENT	1
#define ARGS2_EXTENT	2
#define ARGS3_EXTENT	3
#define ARGS4_EXTENT	4
#define ARGS5_EXTENT	5
#define ARGS6_EXTENT	6
#define ARGS7_EXTENT	7
#define ARGS8_EXTENT	8
#define ARGS9_EXTENT	9
#define ARGS10_EXTENT	10

#endif /* _MACH_ARM64_SDT_ISA_H */