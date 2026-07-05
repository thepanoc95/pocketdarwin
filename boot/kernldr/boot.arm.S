.section __TEXT,__text
.align 4

.globl _xnu_kernel_entry
_xnu_kernel_entry:
    mov r4, r0
    mov r5, r1
    mov r6, r2
    mov r7, r3

    ldr sp, =stack_top

    mov r0, r4
    mov r1, r5
    mov r2, r6
    mov r3, r7

    bx r0

.align 6
stack_bottom:
    .space 0x2000
stack_top:
