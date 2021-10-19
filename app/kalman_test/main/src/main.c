#include <stdio.h>
#include "common.h"

int init(void)
{
    return 0;
}

int loop(void)
{

    return 0;
}

int main(void)
{
    int res = 0;

    res = init();
    while (!(res = loop()));

    return res;
}

