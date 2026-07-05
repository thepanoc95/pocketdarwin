.section __TEXT,__text
.align 4

.globl _xnu_kernel_entry
_xnu_kernel_entry:
    mov x19, x0
    mov x20, x1
    mov x21, x2
    mov x22, x3

    ldr x0, =stack_top
    mov sp, x0

    mov x0, x19
    mov x1, x20
    mov x2, x21
    mov x3, x22

    br x0

.align 6
stack_bottom:
    .space 0x4000
stack_top:

.globl _exception_vector
.align 11
_exception_vector:
    b .
    .align 7
    b .
    .align 7
    b .
    .align 7
    b .
    .align 7
    b .
    .align 7
    b .
    .align 7
    b .
    .align 7
    b .
    .align 7
    b .
    .align 7
    b .
    .align 7
    b .
    .align 7
    b .
    .align 7
    b .
    .align 7
    b .
    .align 7
    b .
    .align 7
    b .
