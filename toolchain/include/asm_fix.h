#ifndef _ASM_FIX_H_
#define _ASM_FIX_H_

// Fix EnterThumb macro for Linux integrated assembler
// On Apple assembler, ; works as line separator. On Linux integrated assembler,
// .code 16 doesn't accept anything after it on the same line.
#ifdef __linux__
#undef EnterThumb
#define EnterThumb(function)              \
    .code 16                            ; \
    .thumb_func _ ##function            ; \
    .globl _ ##function                 ; \
    .align  4                           ; \
    _##function:                        ;

#undef EnterARM
#define EnterARM(function)                \
    .code 32                            ; \
    .globl _ ##function                 ; \
    .align  4                           ; \
    _##function:                        ;
#endif

#endif
