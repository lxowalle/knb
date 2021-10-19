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
        printf("\033[31m[ERROR] \033[0m"fmt, ##__VA_ARGS__);}\
    else{\
        printf("\033[32m[OK] \033[0m"fmt, ##__VA_ARGS__);\
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
    setup_signal_handler();

    /** Init */
    if (mf_pjsip_init())
    {
        printf("pjsip init error!\n");
    }

    /** 添加一个新账户 */
    mf_pjsip_acc_cfg_t acc_cfg = 
    {
        .id = "sip:100@192.168.0.115",
        .reg_uri = "sip:192.168.0.115",
        .cred_info.realm = "*",
        .cred_info.uname = "100",
        .cred_info.passwd = "100"
    };
    mf_pjsip_add_new_account(&acc_cfg);

    
    while(!exit_flag);
    exit_flag = 0;

    /** 删除当前账户 */
    mf_pjsip_list_account_info();
    mf_pjsip_unregister();
    mf_pjsip_list_account_info();
    while(!exit_flag);
    
    /**  Deinit */
    mf_pjsip_deinit();

    _TEST(0, "ok:%s\n", "123");

    _TEST(1, "err:%s\n", "456");
    return 0;
}
