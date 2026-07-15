/*
 * ARM will not have PAL routines, sorry.
 */

#ifndef _ARM_PAL_ROUTINES_H_
#define _ARM_PAL_ROUTINES_H_

#define	pal_kernel_announce() do { } while(0)

extern void pal_dbg_set_task_name(task_t task);

#endif