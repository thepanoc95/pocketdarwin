#ifndef _MINC_CONVERT_H
#define _MINC_CONVERT_H

#define minc_convert_32_to_64(x) ((uint64_t)(uint32_t)(x))
#define minc_convert_64_to_32(x) ((uint32_t)(uint64_t)(x))

// write(num, str, num) -> printf(str, args)
#define minc_convert_write(fd, buf, count) printf("%.*s", (int)(count), (const char *)(buf))

#define printf(fmt, ...) write(1, fmt, sizeof(fmt) - 1); \
    write(1, "\n", 1); \
    (void)0

#if defined(__DARWIN__)
    #if defined(__MINLIBC__) 
        #include <unistd.h>
    #endif
#endif


#endif // _MINC_CONVERT_H