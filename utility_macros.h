#ifndef _UTILITY_MACROS
#define _UTILITY_MACROS

#define MIN(x, y) ((x < y) ? x : y)
#define MAX(x, y) ((x > y) ? x : y)
#define CLAMP(x, min, max) (MAX(MIN(x, max), min))
#define COUNT(arr) (sizeof arr / sizeof *arr)
#define STR(x) #x
#define HEX(r, g, b, a)                                                        \
    (vec4){(float)r / 0xff, (float)g / 0xff, (float)b / 0xff, (float)a / 0xff}

#endif
