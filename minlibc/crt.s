.section __TEXT,__text

.align 2
.globl _start
.globl __start

_start:
__start:
    push {r7, lr}
    add r7, sp, #0

    mov r4, sp
    bic sp, sp, #0xF

    ldr r0, bss_start_ptr
    ldr r1, bss_end_ptr
    cmp r0, r1
    beq .bss_done
.bss_loop:
    mov r3, #0
    str r3, [r0], #4
    cmp r0, r1
    blt .bss_loop

.bss_done:
    ldr r0, ctors_list_ptr
    ldr r1, ctors_end_ptr
    cmp r0, r1
    beq .ctors_done

.ctors_loop:
    ldr r2, [r0], #4
    cmp r2, #0
    beq .ctors_done
    mov lr, pc
    bx r2
    b .ctors_loop

.ctors_done:
    push {r0, r1, r2, r3}
    pop {r0, r1, r2}

    bl _main

    mov r7, #1
    svc 0

    b .

bss_start_ptr: .long ___bss_start
bss_end_ptr:   .long ___bss_end
ctors_list_ptr: .long __CTOR_LIST__
ctors_end_ptr:  .long __CTOR_END__

.align 2
.data

.globl ___bss_start
.globl ___bss_end
.globl __CTOR_LIST__
.globl __CTOR_END__

___bss_start:
    .long 0
___bss_end:
    .long 0
__CTOR_LIST__:
    .long 0
__CTOR_END__:
    .long 0
