#include "pjlib.h"
#include "pjlib-util.h"
#include "pjnath.h"
#include "pjsip.h"
#include "pjsip_ua.h"
#include "pjsip_simple.h"
#include "pjsua-lib/pjsua.h"
#include "pjmedia.h"
#include "pjmedia-codec.h"

#include "pjsua_app_common.h"
#include "pjsua_app.h"
#include "pjsua_app_config.h"

typedef struct
{
    char	    id[50];         // 账户完整的sip url，例如"sip:account@serviceprovider"
    char        reg_uri[50];    // 作为注册请求的uri,录入"sip:serviceprovider"
    /**
     * 证书信息
    */
    struct
    {
        char realm[50];
        char uname[50];
        char passwd[30]; 
    }cred_info;
}thr_pjsip_acc_cfg_t;

pj_status_t thr_pjsip_init(void);
pj_status_t thr_pjsip_loop(void);

/**
 * @brief 添加一个账户
 * @details
 * @param [in]  cfg
 * @return
*/
pj_status_t thr_pjsip_add_account(thr_pjsip_acc_cfg_t * cfg);

/**
 * @brief 拨出电话
 * @details
 * @param [in]  url 对方的url
 * @return
*/
pj_status_t thr_pjsip_make_call(char *url);

/**
 * @brief 回应电话
 * @details
 * @param [in]  st_code 接听电话时返回的代码，200表示接听
 * @return
*/
pj_status_t thr_pjsip_answer_call(int st_code);

/**
 * @brief 打印好友列表
*/
void thr_pjsip_print_buddy_list(void);