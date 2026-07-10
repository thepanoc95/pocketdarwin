#include <stdint.h>
struct lk2nd_fb_info {
    uint32_t base;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t depth;
};

struct boot_args {
    uint32_t    v_baseAddr;
    uint32_t    v_display;
    uint32_t    v_rowBytes;
    uint32_t    v_width;
    uint32_t    v_height;
    uint32_t    v_depth;
};

void get_active_fb(struct lk2nd_fb_info *fb) {
    fb->base    = target_get_fb_base();
    fb->width   = target_get_fb_width();
    fb->height  = target_get_fb_height();
    fb->stride  = target_get_fb_stride();
    fb->depth   = target_get_fb_depth();
}

void __start__(void) {
    __asm__ volatile ("bl loader");
    __asm__ volatile ("");
}

void jump2xnu(uint32_t entry_point, struct boot_args *args) {
    __asm__ volatile(
        "mov r0, %1 \n\t"
        "mov r1, #0 \n\t"
        "mov r2, #0 \n\t"
        "mov r3, # \n\t"
        "bx  %0     \n\t"
        :
        : "r"(entry_point), "r"(args)
        : "r0", "r1", "r2", "r3"
    );
}
