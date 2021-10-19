#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mf_pjsip.h"

#define THIS_FILE	"main.c"

#define _TEST(a, fmt, ...)\
do{\
    if (a){\
        printf("\033[31m[ ERROR ] \033[0m"fmt, ##__VA_ARGS__);}\
    else{\
        printf("\033[32m[  OK   ] \033[0m"fmt, ##__VA_ARGS__);\
    }\
}while(0)

static int exit_flag = 0;

static void signal_handler(int sig)
{
    if (sig == SIGINT)
    {
        exit_flag = 1;
    }
}

static void setup_signal_handler(void)
{
    signal(SIGINT, &signal_handler);
    signal(SIGABRT, &signal_handler);
}

int main(int argc, char *argv[])
{
    int res = 0;
    setup_signal_handler();

    /** Init */
    if (mf_pjsip_init())
    {
        printf("pjsip init error!\n");
    }

#if 1
    /** 添加一个新账户 */
    mf_pjsip_acc_cfg_t acc_cfg = 
    {
        .id = "sip:100@myvoipapp.com",
        .reg_uri = "sip:192.168.0.115",
        .cred_info.realm = "*",
        .cred_info.uname = "100",
        .cred_info.passwd = "100"
    };
    mf_pjsip_add_new_account(&acc_cfg);
#endif

#if 0
    while(!exit_flag);
    exit_flag = 0; 

    /** 取消注册当前账户 */
    mf_pjsip_print_account_info();
    res = mf_pjsip_unregister();
    _TEST(res, "pjsip unregister\n");
    mf_pjsip_print_account_info();
#endif

#if 0
    /* 删除账户 */
    mf_pjsip_print_account_info();

    pjsua_acc_id acc_id = pjsua_acc_get_default();
    res = mf_pjsip_delete_account(acc_id);
    _TEST(res, "delete account:%d\n", acc_id);

    res = mf_pjsip_print_account_info();
    _TEST(res, "list account info\n");
#endif

#if 0
    while(!exit_flag);
    exit_flag = 0; 
    /* 拨打一个电话 */
    res = mf_pjsip_make_call("sip:101@192.168.0.115");
    _TEST(res, "mf_pjsip_make_call\n");
#endif

#if 0
    while(!exit_flag);
    exit_flag = 0; 
    res = mf_pjsip_answer_call(200);
    _TEST(res, "mf_pjsip_answer_call\n");
#endif

#if 0
    /* 设置声音 */
    res = mf_pjsip_adjust_audio_volume(0.3, 0.3);
    _TEST(res, "mf_pjsip_adjust_audio_volume\n");
#endif

#if 0
    while(!exit_flag);
    exit_flag = 0;
    /* 挂起电话 */
    res = mf_pjsip_hold_call();
    
    _TEST(res, "mf_pjsip_hold_call\n");
#endif

#if 0
    while(!exit_flag);
    exit_flag = 0;
    /* 释放挂起的电话 */
    res = mf_pjsip_reinvite_call();
    _TEST(res, "mf_pjsip_reinvite_call\n");
#endif

#if 0
    while(!exit_flag);
    exit_flag = 0; 
    /* 挂断电话 */
    res = mf_pjsip_hangup_call(1);    
    _TEST(res, "mf_pjsip_hangup_call\n");
#endif

#if 1
    while(!exit_flag);
    exit_flag = 0; 
    /**  Deinit */
    res = mf_pjsip_deinit();
    _TEST(res, "deinit\n");
#endif
    
    return 0;
}
