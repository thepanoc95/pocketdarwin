.section __TEXT,__text

.align 2
.globl _start
.global __start

_start:
__start:
   push {r7, lr}
   add r7, sp, #0

   mov r4, sp
   bic sp, sp, #0xF

   ldr r0, =___bss_start
   ldr r1, =___bss_end
   cmp r0, r1
   beq .bss_done
.bss_loop:
    str r0, [r0], #4
    cmp r0, r1
    blt .bss_loop

.bss_done:
    ldr r0, =__CTOR_LIST__
    ldr r1, =__CTOR_END__
    cmp r0, r1
    beq .ctors_done

.ctors_loop:
    ldr r2, [r0], #4
    cmp r2, #0
    beq .ctors_done
    blx r2
    b .ctors_loop

.ctors_done:
    push {r0, r1, r2, r3}
    pop {r0, r1, r2}

    bl _main

    mov r7, #1
    svc 0

    b .

.align 2
.data

.globl ___bss_start
.globl ___bss_end

___bss_start:
    .space 0

___bss_end:
    .space 0

.globl __CTOR_LIST__
.globl __CTOR_END__

__CTOR_LIST__:
    .space 0

__CTOR_END__:
    .space 0
