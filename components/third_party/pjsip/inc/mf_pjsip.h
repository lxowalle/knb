#include "pjlib.h"
#include "pjlib-util.h"
#include "pjnath.h"
#include "pjsip.h"
#include "pjsip_ua.h"
#include "pjsip_simple.h"
#include "pjsua-lib/pjsua.h"
#include "pjmedia.h"
#include "pjmedia-codec.h"

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
}mf_pjsip_acc_cfg_t;

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
pj_status_t thr_pjsip_add_account(mf_pjsip_acc_cfg_t * cfg);

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

/**
 * @brief 初始化
*/
int mf_pjsip_init(void);

/**
 * @brief 取消初始化
*/
int mf_pjsip_deinit(void);

/**
 * @brief 发起一个电话
*/
int mf_pjsip_make_call(char *dst_url);

/**
 * @brief 发起多个呼叫(TODO)
*/
void mf_pjsip_make_multiple_calls(char *dst_url);

/**
 * @brief 应答电话
*/
int mf_pjsip_answer_call(int st_code);

/**
 * @brief 挂断电话
*/
int mf_pjsip_hangup_call(int all);

/**
 * @brief 挂起电话
*/
int mf_pjsip_hold_call(void);

/**
 * @brief 释放挂起的电话
*/
int mf_pjsip_reinvite_call(void);

/**
 * @brief 发起更新请求(TODO)
*/
void mf_pjsip_send_update(void);

/**
 * @brief 翻页(TODO)
*/
void mf_pjsip_select_call(void);

/**
 * @brief 呼叫转移(TODO)
*/
void mf_pjsip_xfer_call(void);

/**
 * @brief 呼叫替换(TODO)
*/
void mf_pjsip_xfer_with_replaces(void);

/**
 * @brief 发送DTMF(RFC238)(TODO)
*/
void mf_pjsip_send_dtmf_rfc2833(void);

/**
 * @brief 发送DTMF(SIP信令)(TODO)
*/
void mf_pjsip_send_dtmf_sip(void);

/**
 * @brief 获取当前通话质量(TODO)
*/
void mf_pjsip_dump_call_quality(void);

/**
 * @brief 向远程主机发送任意请求(TODO)
*/
void mf_pjsip_send_arbitrary_request(void);

/**
 * @brief 绑定好友(TODO)
*/
void mf_pjsip_add_new_buffy(void);

/**
 * @brief 删除好友(TODO)
*/
void mf_pjsip_delete_buddy(void);

/**
 * @brief 发送及时消息(TODO)
*/
void mf_pjsip_send_real_message(void);

/**
 * @brief 订阅好友状态(TODO)
*/
void mf_pjsip_subscribe_presence(void);

/** 
 * @brief 取消订阅好友状态(TODO)
*/
void mf_pjsip_unsubscribe_presence(void);

/**
 * @brief 切换在线状态(TODO)
*/
void mf_pjsip_toggle_online_status(void);

/**
 * @brief 设置在线状态(TODO)
*/
void mf_pjsip_set_online_status(void);

/**
 * @brief 列出端口(TODO)
*/
void mf_pjsip_list_ports(void);

/**
 * @brief 连接端口(TODO)
*/
void mf_pjsip_connect_port(void);

/**
 * @brief 断开端口连接(TODO)
*/
void mf_pjsip_disconnect_port(void);

/**
 * @brief 调节声音音量
*/
int mf_pjsip_adjust_audio_volume(float mic_vol, float spk_vol);

/**
 * @brief 设置编码器优先级(TODO)
*/
void mf_pjsip_set_codec_priorities(void);

/**
 * @brief 添加一个新账户
*/
int mf_pjsip_add_new_account(mf_pjsip_acc_cfg_t *cfg);

/**
 * @brief 删除账户
*/
int mf_pjsip_delete_account(pjsua_acc_id acc_id);

/**
 * @brief 打印账户信息
*/
int mf_pjsip_print_account_info(void);

/**
 * @brief 修改账户(TODO)
*/
void mf_pjsip_modify_account(void);

/**
 * @brief 重新注册账户(TODO)
*/
void mf_pjsip_re_register_account(void);

/**
 * @brief 取消注册当前账户(TODO)
*/
int mf_pjsip_unregister(void);

/**
 * @brief 下一个循环？(TODO)
*/
void mf_pjsip_cycle_next_ac(void);

/**
 * @brief 上一个循环？(TODO)
*/
void mf_pjsip_cycle_prev_ac(void);

/**
 * @brief 获取当前状态(TODO)
*/
void mf_pjsip_dump_status(void);

/**
 * @brief 获取当前状态详细(TODO)
*/
void mf_pjsip_dump_detailed(void);

/**
 * @brief 获取当前配置(TODO)
*/
void mf_pjsip_dump_config(void);

/**
 * @brief 保存配置(TODO)
*/
void mf_pjsip_save_config(void);

/**
 * @brief 开启video功能(TODO)
*/
int mf_pjsip_enable_video(int enable);

/**
 * @brief 显示当前video的会话配置(TODO)
*/
void mf_pjsip_vid_show_cfg(void);

/**
 * @brief 启动自动接收video数据(TODO)
*/
int mf_pjsip_vid_set_autorx(int on);

/**
 * @brief 启动自动发送video数据(TODO)
*/
int mf_pjsip_vid_set_autotx(int on);

/**
 * @brief 设置video默认捕获的设备号(TODO)
*/
void mf_pjsip_vid_set_default_capture_id(void);

/**
 * @brief 设置video默认播放的设备号(TODO)
*/
void mf_pjsip_vid_set_default_renderer_id(void);

/**
 * @brief 设置当前视频会话的接收流(TODO)
*/
int mf_pjsip_vid_set_rx_stream(int med_idx, int on);

/**
 * @brief 设置当前视频会话的发送流(TODO)
*/
int mf_pjsip_vid_set_tx_stream(int med_idx, int on);

/**
 * @brief 添加一个视频流到当前会话(TODO)
*/
int mf_pjsip_vid_add_stream(void);

/**
 * @brief 设置当前会话流的捕获设备id(TODO)
*/
void mf_pjsip_vid_set_stream_capture_id(void);

/**
 * @brief 获取设备列表(TODO)
*/
void mf_pjsip_vid_get_dev_list(void);

/**
 * @brief 刷新设备列表(TODO)
*/
void mf_pjsip_vid_refresh_dev_list(void);

/**
 * @brief 预览设备(TODO)
*/
void mf_pjsip_vid_prev_dev(void);

/**
 * @brief 获取设备编解码列表(TODO)
*/
void mf_pjsip_vid_get_codec_list(void);

/**
 * @brief 设置设备编解码器的优先级(TODO)
*/
void mf_pjsip_vid_set_codec_prio(void);

/**
 * @brief 设置设备编解码器的帧率(TODO)
*/
void mf_pjsip_vid_set_codec_fps(void);

/**
 * @brief 设置设备编解码器的比特率(TODO)
*/
void mf_pjsip_vid_set_codec_bw(void);

/**
 * @brief 设置设备编解码器的大小(TODO)
*/
void mf_pjsip_vid_set_codec_size(void);

/**
 * @brief 列出所有视频会议桥的端口(TODO)
*/
void mf_pjsip_vid_list_ports(void);

/**
 * @brief 连接视频会议桥的端口(TODO)
*/
void mf_pjsip_vid_connect_port(void);

/**
 * @brief 取消视频会议桥的端口连接(TODO)
*/
void mf_pjsip_vid_disconnect_port(void);
