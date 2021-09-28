#ifndef __DEBUG_H
#define __DEBUG_H

#include <stdint.h>
#include <stdio.h>

#define _INFO(fmt, ...)     printf("\033[32m[INFO] \033[0m"fmt, ##__VA_ARGS__)
#define _ERR(fmt, ...)      printf("\033[31m[ERROR] \033[0m"fmt, ##__VA_ARGS__)
#define _WARNING(fmt, ...)  printf("\033[33m[WARNING] \033[0m"fmt, ##__VA_ARGS__)

static inline void _INFO_HEX(uint8_t *data, int size)
{
    printf("\033[32m[INFO]hex(%d):\033[0m", size);
    for (int i = 0; i < size; i ++)
    {
        printf("%.2X ", data[i]);
    }
    printf("\n");
}

#endif
