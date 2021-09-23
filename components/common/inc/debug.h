#ifndef __DEBUG_H
#define __DEBUG_H

#define _INFO(fmt, ...)     printf("\033[32m[INFO] \033[0m"fmt, ##__VA_ARGS__)
#define _ERR(fmt, ...)      printf("\033[31m[ERROR] \033[0m"fmt, ##__VA_ARGS__)
#define _WARNING(fmt, ...)  printf("\033[33m[WARNING] \033[0m"fmt, ##__VA_ARGS__)

#endif
