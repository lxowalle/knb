#include "mf_pjsip.h"
#include "pjsua_app_common.h"
#include "pjsua_app.h"
#include "pjsua_app_config.h"

#define THIS_FILE __FILE__

#define _DBG_LINE(a)                                            \
do{                                                         \
    printf("[DBG LINE]===================%d\n", __LINE__);  \
    if (a != 0)                                             \
        exit(a);                                            \
}while(0)

/************************************************************************************
 * 							MF API
************************************************************************************/
static pj_bool_t	    running = PJ_FALSE;
static pjsua_app_cfg_t	    cfg;

/**
 * @brief 初始化
*/
int mf_pjsip_init(void)
{
	int argc = 1;
	char *argv = "mf_pjsip_init";
	pj_status_t status = PJ_SUCCESS;

	if (running == PJ_TRUE)	return -1;

    pj_bzero(&cfg, sizeof(cfg));
    cfg.argc = argc;
    cfg.argv = &argv;

	status = pjsua_app_init(&cfg);
	if (status == PJ_SUCCESS)
	{
		status = pjsua_app_run(PJ_TRUE);
	}

	if (status != PJ_SUCCESS)
	{
		status = pjsua_app_destroy();
		pjsua_perror(THIS_FILE, "mf pjsip init", status);
		return -1;
	}

	running = PJ_TRUE;

	return 0;
}

/**
 * @brief 取消初始化
*/
int mf_pjsip_deinit(void)
{
	pj_status_t status = PJ_SUCCESS;

	if (running == PJ_FALSE)	return -1;

	status = pjsua_app_destroy();	
	if (status != PJ_SUCCESS)
	{
		return -1;
	}	

	running = PJ_FALSE;
	return 0;
}

/**
 * @brief 发起一个电话(TODO)
*/
int mf_pjsip_make_call(char *dst_url)
{
	pj_status_t status = PJ_SUCCESS;
	pj_str_t call_dst;
	pjsua_msg_data msg_data_;

	if (running == PJ_FALSE)	return -1;

	status = pjsua_verify_url(dst_url);
	if (status != PJ_SUCCESS)
	{
		pjsua_perror(THIS_FILE, "Invalid URL", status);
		return -1;
	}

	call_dst = pj_str(dst_url);

	pjsua_msg_data_init(&msg_data_);
	pjsua_call_setting_default(&call_opt);
	call_opt.aud_cnt = app_config.aud_cnt;
	call_opt.vid_cnt = app_config.vid.vid_cnt;
	
	status = pjsua_call_make_call(current_acc, &call_dst, &call_opt, NULL, &msg_data_, &current_call);
	if (status != PJ_SUCCESS)
	{
		pjsua_perror(THIS_FILE, "Make call", status);
		return -1;
	}

	return 0;
}

/**
 * @brief 发起多个呼叫(TODO)
*/
void mf_pjsip_make_multiple_calls(char *dst_url);

/**
 * @brief 挂断电话(TODO)
*/
int mf_pjsip_hangup_call(int all)
{
	pj_status_t status = PJ_SUCCESS;

	if (all)
	{
		pjsua_call_hangup_all();
		return 0;
	}
	else
	{
		status = pjsua_call_hangup(current_call, 0, NULL, NULL);
		if (status != PJ_SUCCESS)
			return -1;
	}

	return 0;
}

/**
 * @brief 挂起电话(TODO)
*/
int mf_pjsip_hold_call(void)
{
	pj_status_t status = PJ_SUCCESS;

	if (current_call != -1)
	{
		status = pjsua_call_set_hold(current_call, NULL);
		if (status == PJ_SUCCESS)
			return 0;
		else
			return -1;
	}
}

/**
 * @brief 释放挂起的电话(TODO)
*/
int mf_pjsip_reinvite_call(void)
{
	pj_status_t status = PJ_SUCCESS;

	status = pjsua_call_reinvite2(current_call, &call_opt, NULL);
	if (status == PJ_SUCCESS)
		return 0;
	else
		return 1;
}

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
 * @brief 调节声音音量(TODO)
*/
int mf_pjsip_adjust_audio_volume(float mic_vol, float spk_vol)
{
	pj_status_t status = PJ_SUCCESS;
	pj_status_t status2 = PJ_SUCCESS;

	app_config.mic_level = mic_vol;
	status = pjsua_conf_adjust_rx_level(0, app_config.mic_level);
	if (status != PJ_SUCCESS)
		pjsua_perror(THIS_FILE, "adjust mic level", status);

	app_config.speaker_level = spk_vol;
	status2 = pjsua_conf_adjust_tx_level(0, app_config.speaker_level);
	if (status2 != PJ_SUCCESS)
		pjsua_perror(THIS_FILE, "adjust mic level", status2);

	if (status != PJ_SUCCESS || status2 != PJ_SUCCESS)
		return -1;
	else
		return 0;
}

/**
 * @brief 设置编码器优先级(TODO)
*/
void mf_pjsip_set_codec_priorities(void);

/**
 * @brief 添加一个新账户(TODO)
*/
int mf_pjsip_add_new_account(mf_pjsip_acc_cfg_t * cfg)
{
    pjsua_acc_config acc_cfg;
    pj_status_t status;
	pjsua_transport_config *rtp_cfg = &app_config.rtp_cfg;

	if (running == PJ_FALSE)	return -1;

    pjsua_acc_config_default(&acc_cfg);
    acc_cfg.id = pj_str(cfg->id);
    acc_cfg.reg_uri = pj_str(cfg->reg_uri);
    acc_cfg.cred_count = 1;
    acc_cfg.cred_info[0].scheme = pj_str("Digest");
    acc_cfg.cred_info[0].realm = pj_str(cfg->cred_info.realm);
    acc_cfg.cred_info[0].username = pj_str(cfg->cred_info.uname);
    acc_cfg.cred_info[0].data_type = 0;
    acc_cfg.cred_info[0].data = pj_str(cfg->cred_info.passwd);

    acc_cfg.rtp_cfg = *rtp_cfg;
    app_config_init_video(&acc_cfg);

    status = pjsua_acc_add(&acc_cfg, PJ_TRUE, NULL);
    if (status != PJ_SUCCESS) 
	{
		pjsua_perror(THIS_FILE, "Error adding new account", status);
		return -1;
    }

	return 0;
}
/**
 * @brief 删除账户(TODO)
*/
int mf_pjsip_delete_account(pjsua_acc_id acc_id)
{
	pj_status_t status;
    if (!pjsua_acc_is_valid(acc_id)) {
		return -1;
    } 
	else 
	{
		status = pjsua_acc_del(acc_id);
		if (status != PJ_SUCCESS) 
			return 0;
		else
			return -1;
    }
}

/**
 * @brief 修改账户(TODO)
*/
void mf_pjsip_modify_account();

/**
 * @brief 重新注册账户(TODO)
*/
void mf_pjsip_re_register_account(void);

/**
 * @brief 取消注册账户(TODO)
*/
int mf_pjsip_unregister(void)
{
	pj_status_t status;
	status = pjsua_acc_set_registration(current_acc, PJ_TRUE);

	if (status == PJ_SUCCESS)
		return 0;
	else
		return -1;
}

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
void mf_pjsip_enable_video(void);

/**
 * @brief 显示当前video的会话配置(TODO)
*/
void mf_pjsip_vid_show_cfg(void);

/**
 * @brief 启动自动接收video数据(TODO)
*/
void mf_pjsip_vid_set_autorx(void);

/**
 * @brief 启动自动发送video数据(TODO)
*/
void mf_pjsip_vid_set_autotx(void);

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
void mf_pjsip_vid_set_rx_stream(void);

/**
 * @brief 设置当前视频会话的发送流(TODO)
*/
void mf_pjsip_vid_set_tx_stream(void);

/**
 * @brief 添加一个视频流到当前会话(TODO)
*/
void mf_pjsip_vid_add_stream(void);

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