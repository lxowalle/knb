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

typedef enum
{
    THR_PJSIP_CALL_TIMEOUT_CB,
    THR_PJSIP_CALL_INCOMING_CB
}thr_pjsip_callback_e;

typedef pj_status_t (*on_call_timeout_cb)(pj_timer_heap_t *timer_heap,struct pj_timer_entry *entry);
typedef pj_status_t (*on_call_state_change_cb)(pjsua_call_id call_id, pjsip_event *e);
typedef pj_status_t (*on_call_incoming_cb)(pjsua_acc_id acc_id, pjsua_call_id call_id,pjsip_rx_data *rdata);
typedef pj_status_t (*on_call_generic_media_state_cb)(pjsua_call_info *ci, unsigned mi,pj_bool_t *has_error);
typedef pj_status_t (*on_call_audio_state_cb)(pjsua_call_info *ci, unsigned mi,pj_bool_t *has_error);
typedef pj_status_t (*on_call_video_state_cb)(pjsua_call_info *ci, unsigned mi,pj_bool_t *has_error);
typedef pj_status_t (*on_call_media_state_cb)(pjsua_call_id call_id);
typedef pj_status_t (*registration_state_change_cb)(pjsua_acc_id acc_id);
typedef pj_status_t (*buddy_state_change_cb)(pjsua_buddy_id buddy_id);
typedef pj_status_t (*buddy_evsub_state_change_cb)(pjsua_buddy_id buddy_id,pjsip_evsub *sub,pjsip_event *event);

typedef struct
{
    on_call_timeout_cb cb_on_call_timeout;
    on_call_incoming_cb cb_on_call_incoming;
}thr_pjsip_t;

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
 * @brief 注册回调函数
*/
pj_status_t thr_pjsip_reg_callback(thr_pjsip_callback_e cb_state, void *cb);

/**
 * @brief 打印好友列表
*/
void thr_pjsip_print_buddy_list(void);