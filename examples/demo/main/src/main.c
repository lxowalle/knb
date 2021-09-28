#include <stdio.h>
#include "common.h"
#include "tests.h"
#include "thr_pjsip.h"

// pj_status_t app_init(void)
// {
//     pj_status_t status;

//     status = pjsua_create();
//     if (PJ_SUCCESS != status)
//         return status;
// }

int init(void)
{
    // app_init();
    thr_pjsip_init();
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

    while (!loop());

    return res;
}

