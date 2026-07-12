/* minlibc - crt0 startup code */
/* Supports: x86_64, i386, ARM EABI */

/* ============================================================================
 * x86_64
 * ============================================================================ */
#if defined(__x86_64__)

#ifdef __APPLE__
#define CMAIN   _main
#define CEXIT   _exit
#define CBSSS   ___bss_start
#define CBSSD   ___bss_end
#define CCTORL  __CTOR_LIST__
#define CCTORE  __CTOR_END__
#define CEND    __end
#else
#define CMAIN   main
#define CEXIT   exit
#define CBSSS   __bss_start
#define CBSSD   __bss_end
#define CCTORL  __CTOR_LIST__
#define CCTORE  __CTOR_END__
#define CEND    _end
#endif

.text
.globl _start
_start:
    lea CBSSS(%rip), %rdi
    lea CBSSD(%rip), %rcx
    cmp %rdi, %rcx
    je .xbss_done
.xbss_loop:
    movq $0, (%rdi)
    add $8, %rdi
    cmp %rdi, %rcx
    ja .xbss_loop
.xbss_done:

    lea CCTORL(%rip), %rsi
    lea CCTORE(%rip), %rcx
    cmp %rsi, %rcx
    je .xctors_done
.xctors_loop:
    mov (%rsi), %rax
    test %rax, %rax
    jz .xctors_done
    call *%rax
    add $8, %rsi
    jmp .xctors_loop
.xctors_done:

    mov (%rsp), %rdi
    lea 8(%rsp), %rsi
    mov %rdi, %rdx
    inc %rdx
    shl $3, %rdx
    add %rsi, %rdx
    call CMAIN

    mov %rax, %rdi
    call CEXIT
    jmp .

.align 8
.data
.globl CBSSS
.globl CBSSD
.globl CCTORL
.globl CCTORE
.globl CEND
CBSSS:  .quad 0
CBSSD:  .quad 0
CCTORL: .quad 0
CCTORE: .quad 0
CEND:   .quad 0

/* ============================================================================
 * i386
 * ============================================================================ */
#elif defined(__i386__)

#ifdef __APPLE__
#define CMAIN   _main
#define CEXIT   _exit
#define CBSSS   ___bss_start
#define CBSSD   ___bss_end
#define CCTORL  __CTOR_LIST__
#define CCTORE  __CTOR_END__
#define CEND    __end
#else
#define CMAIN   main
#define CEXIT   exit
#define CBSSS   __bss_start
#define CBSSD   __bss_end
#define CCTORL  __CTOR_LIST__
#define CCTORE  __CTOR_END__
#define CEND    _end
#endif

.text
.globl _start
_start:
    mov $CBSSS, %edi
    mov $CBSSD, %ecx
    cmp %edi, %ecx
    je .ibss_done
.ibss_loop:
    movl $0, (%edi)
    add $4, %edi
    cmp %edi, %ecx
    ja .ibss_loop
.ibss_done:

    mov $CCTORL, %esi
    mov $CCTORE, %ecx
    cmp %esi, %ecx
    je .ictors_done
.ictors_loop:
    mov (%esi), %eax
    test %eax, %eax
    jz .ictors_done
    call *%eax
    add $4, %esi
    jmp .ictors_loop
.ictors_done:

    mov (%esp), %eax
    lea 4(%esp), %ecx
    pushl $0
    pushl %ecx
    pushl %eax
    call CMAIN
    add $12, %esp

    pushl %eax
    call CEXIT
    jmp .

.align 4
.data
.globl CBSSS
.globl CBSSD
.globl CCTORL
.globl CCTORE
.globl CEND
CBSSS:  .long 0
CBSSD:  .long 0
CCTORL: .long 0
CCTORE: .long 0
CEND:   .long 0

/* ============================================================================
 * ARM EABI
 * ============================================================================ */
#elif defined(__arm__) || defined(__arm64__)

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
end_ptr:        .long ___end

.align 2
.data

.globl ___bss_start
.globl ___bss_end
.globl __CTOR_LIST__
.globl __CTOR_END__
.globl ___end

___bss_start:
    .long 0
___bss_end:
    .long 0
__CTOR_LIST__:
    .long 0
__CTOR_END__:
    .long 0
___end:
    .long 0

#endif
