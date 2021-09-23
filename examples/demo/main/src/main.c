#include <stdio.h>
#include "common.h"
#include "test_kalman.h"


int init(void)
{
    test_kalman_init();
    return 0;
}

int loop(void)
{

    return 0;
}

int main(void)
{
    _INFO("info\n");
    _ERR("err\n");
    _WARNING("warning\n");

    int res = 0;

    res = init();

    while (!loop());

    return res;
}

