#include <stdio.h>
#include "common.h"
#include "tests.h"
#include "thr_pjsip.h"

#define THIS_FILE   __FILE__

int init(void)
{
    // app_init();
    thr_pjsip_init();

    // 添加一个账户，并连接
    thr_pjsip_acc_cfg_t acc_cfg = 
    {
        .id = "sip:100@192.168.0.38",
        .reg_uri = "sip:192.168.0.38",
        .cred_info.realm = "*",
        .cred_info.uname = "100",
        .cred_info.passwd = "100"
    };
    pj_status_t status = thr_pjsip_add_account(&acc_cfg);
    if (status != PJ_SUCCESS)
        pjsua_perror(THIS_FILE, "Error pjsip add account", status);

    // 打印好友列表
    thr_pjsip_print_buddy_list();

    // 拨出电话
#if 0
    status = thr_pjsip_make_call("sip:101@192.168.0.38");
    if (status != PJ_SUCCESS)
        pjsua_perror(THIS_FILE, "Error make call", status);
#endif

    

    return 0;
}

int loop(void)
{
    // 接收电话
    // thr_pjsip_answer_call(200);
    return 0;
}

int main(void)
{
    int res = 0;

    res = init();

    while (!loop());

    return res;
}

