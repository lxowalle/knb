#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "common.h"
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

void video_call()
{
    // status = pjmedia_endpt_create_sdp( g_med_endpt,    
    //         dlg->pool,    
    //         MAX_MEDIA_CNT,  
    //         g_sock_info,    
    //         &local_sdp);

    pjmedia_sdp_attr *sdp_attr = NULL;
    sdp_attr = pjmedia_sdp_media_find_attr(1, "a", NULL);
    if (sdp_attr == NULL)   
    {
        printf("========= NULL ============\n");
        return;
    }
    // printf("\n\n================ name:%s  str:%s\n\n", sdp_attr->name.ptr, sdp_attr->value.ptr);
    // PJ_ASSERT_RETURN(status == PJ_SUCCESS, 1);
    // {
    //     pjmedia_sdp_attr* a;
    //     pjmedia_sdp_attr* a1;
    //     pjmedia_sdp_media *sdp_m=local_sdp->media[1];
    //     pj_str_t  tem=pj_str("96 CIF=4;QCIF=4");
        
    //     pjmedia_sdp_media_remove_all_attr(sdp_m, "sendrecv");
    //     pjmedia_sdp_media_remove_all_attr(sdp_m, "sendonly");
    //     pjmedia_sdp_media_remove_all_attr(sdp_m, "recvonly");
    //     pjmedia_sdp_media_remove_all_attr(sdp_m, "inactive");
    //     pjmedia_sdp_media_remove_all_attr(sdp_m, "fmtp");


    //     a = pjmedia_sdp_attr_create(pool, "sendonly", NULL);
    //     pjmedia_sdp_media_add_attr(sdp_m, a);
    
    //     a1 = pjmedia_sdp_attr_create(pool, "fmtp", &tem);
    //     pjmedia_sdp_media_add_attr(sdp_m, a1);
    
    // }
}

void print_vid(void)
{
    pj_status_t res = PJ_SUCCESS;
    pjmedia_vid_dev_index vid_dev_index_max;
    vid_dev_index_max = pjsua_vid_dev_count();
    _INFO("Get video num:%d\n", vid_dev_index_max);
    pjmedia_vid_dev_info vid_dev_info;
    for (int i = 0; i < vid_dev_index_max; i ++)
    {
        res = pjsua_vid_dev_get_info(i, &vid_dev_info);
        if (res != PJ_SUCCESS)
        {
            _ERR("Error for %d[%d]\n", __LINE__, i);
            pjsua_perror("-", "Error", res);
            continue;
        }
            
        _INFO(  "[vid:%d]id:%d name:%s\n"
                "        dir:%d caps:%d\n"
                "        driver:%s preview:%s\n"
                "        active:%s\n", 
                i,
                vid_dev_info.id, 
                vid_dev_info.name,
                vid_dev_info.dir,
                vid_dev_info.caps, 
                vid_dev_info.driver, pjsua_vid_preview_has_native(i)?"Yes":"No",
                pjsua_vid_dev_is_active(i)?"Yes":"No");
    }    
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
    res = mf_pjsip_add_new_account(&acc_cfg);
    _TEST(res, "mf_pjsip_add_new_account\n");

    res = mf_pjsip_enable_video(1);
    _TEST(res, "mf_pjsip_enable_video\n");

    
#endif

#if 0
    /* codec */
    // 枚举所有支持的视频编解码器
    pjsua_codec_info codec_info_list[10];
    int codec_info_cnt = sizeof(codec_info_list) / sizeof(codec_info_list[0]);
    res = pjsua_vid_enum_codecs(codec_info_list, &codec_info_cnt);
    if (res != PJ_SUCCESS)
    {
        pjsua_perror(THIS_FILE, "Error", res);
    }
    else
    {
        _INFO("There has %d codec\n", codec_info_cnt);
        for (int i = 0; i < codec_info_cnt; i ++)
        {
            _INFO("codec[%d] %s desc:%s priority:%d buf_:%s\n",
            i,
            codec_info_list[i].codec_id.ptr,
            codec_info_list[i].desc.ptr,
            codec_info_list[i].priority,
            codec_info_list[i].buf_);
        }
    }

    // 获取网桥中当前活动端口的数量
    _INFO("There has %d active port\n", pjsua_vid_conf_get_active_ports());

    // 为视频添加端口
    pj_pool_t *tmp_pool = pjsua_pool_create("tmp_pool", 1000, 1000);
    if (tmp_pool == NULL)
    {   
        _ERR("No mem\n");
        exit(1);
    }


    // 获取会议端口
    pjsua_conf_port_id port_id_list[10];
    int port_id_max = _GET_ARRAY_SIZE(port_id_list);
    res = pjsua_vid_conf_enum_ports(port_id_list, &port_id_max);
    if (res != PJ_SUCCESS)
    {
        pjsua_perror(THIS_FILE, "Error", res);
    }
    else
    {
        for (int i = 0; i < port_id_max; i ++)
        {
            pjsua_conf_port_id port_id = port_id_list[i];
            pjsua_vid_conf_port_info port_info;

            pjsua_vid_conf_get_port_info(port_id, &port_info);

            _INFO("port[%d] name:%s", port_id, port_info.name);
        }
    }
#endif

#if 0
    /* video test */
    /* 获取安装的视频设备数量 */
    pjmedia_vid_dev_index vid_dev_index_max;
    vid_dev_index_max = pjsua_vid_dev_count();
    _INFO("Get video num:%d\n", vid_dev_index_max);
    
    // 枚举系统安装的所有设备
    pjmedia_vid_dev_info vid_dev_info_list[5];
    int vid_dev_info_list_cnt = 5;
    res = pjsua_vid_enum_devs(vid_dev_info_list, &vid_dev_info_list_cnt);
    if (res != PJ_SUCCESS)
    {
        pjsua_perror(THIS_FILE, "Error", res);
    }
    else
    {
        _INFO("vid dev info list cnt:%d\n", vid_dev_info_list_cnt);
    }

    // 开启预览
    int dev_id = 0;
    {
        pjsua_vid_preview_param param;
        pjsua_vid_preview_param_default(&param);
        param.wnd_flags = PJMEDIA_VID_DEV_WND_BORDER |
                                    PJMEDIA_VID_DEV_WND_RESIZABLE;
        pjsua_vid_preview_start(dev_id, &param);
        arrange_window(pjsua_vid_preview_get_win(dev_id));
    }

    // 枚举视频窗口
    {
        pjsua_vid_win_id vid_win_list[5];
        int vid_win_id_max = 5;
        res = pjsua_vid_enum_wins(vid_win_list, &vid_win_id_max);
        if (res != PJ_SUCCESS)
        {
            pjsua_perror(THIS_FILE, "Error", res);
        }
        _INFO("vid win max:%d\n", vid_win_id_max);

        {
            pjsua_vid_win_info win_info;
            pjsua_vid_win_get_info(0, &win_info);
            _INFO("vid[%d] is native:%s\n", 0, win_info.is_native?"Yes":"No");
        }
        
    }

    // 调整窗口位置
    {
        pjsua_vid_win_id wid;
        pjmedia_coord coord;
        coord.x = 300;
        coord.y = 300;
        wid = pjsua_vid_preview_get_win(dev_id);
        if (wid != PJSUA_INVALID_ID) {
            pjsua_vid_win_set_pos(wid, &coord);
        }
    }

    // 调整窗口大小
    {
        pjsua_vid_win_id wid;
        pjmedia_rect_size size;
        size.h = 300;
        size.w = 300;
        wid = pjsua_vid_preview_get_win(dev_id);
        if (wid != PJSUA_INVALID_ID) {
            pjsua_vid_win_set_size(wid, &size);
        }
    }

    // 旋转窗口
    {
        pjsua_vid_win_id wid;
        int rotate = 90;
        wid = pjsua_vid_preview_get_win(dev_id);
        if (wid != PJSUA_INVALID_ID) {
            pjsua_vid_win_rotate(wid, rotate);
        }
    }

    // 关闭预览
    {
        while(!exit_flag);
        exit_flag = 0; 

        print_vid();
        _INFO("preview_vid_port:%d\n", pjsua_vid_preview_get_vid_conf_port(dev_id));
        pjsua_vid_win_id wid;
        wid = pjsua_vid_preview_get_win(dev_id);
        if (wid != PJSUA_INVALID_ID) {
            pjsua_vid_win_set_show(wid, PJ_FALSE);  // 显示隐藏窗口
            pjsua_vid_preview_stop(dev_id);         // 停止预览
        }
    }
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

    // /* 自动发送video */
    // res = mf_pjsip_vid_set_autotx(1);
    // _TEST(res, "mf_pjsip_vid_set_autotx\n");

    // /* 自动接受video */
    // res = mf_pjsip_vid_set_autorx(1);
    // _TEST(res, "mf_pjsip_vid_set_autorx\n");
#endif

#if 0
    // while(!exit_flag);
    // exit_flag = 0; 
    /* 拨打一个电话 */
    res = mf_pjsip_make_call("sip:101@192.168.0.115");
    _TEST(res, "mf_pjsip_make_call\n");
#endif

#if 1
    _TEST(0, "Wait call..\n");
    while(!exit_flag)
    {
        mf_pjsip_answer_call(200);
    }
    exit_flag = 0;

    video_call();
    _TEST(0, "video_call\n");
#endif

#if 1
    // while(!exit_flag);
    // exit_flag = 0;
    /* 创建视频流 */
    res = mf_pjsip_vid_add_stream();
    _TEST(res, "mf_pjsip_vid_add_stream\n");
#endif



#if 0
    while(!exit_flag);
    exit_flag = 0;
    res = mf_pjsip_vid_enable_stream(1, 1);
    _TEST(res, "mf_pjsip_vid_enable_stream\n");
#endif

#if 0
    while(!exit_flag);
    exit_flag = 0;
    res = mf_pjsip_vid_set_tx_stream(-1, 1);
    _TEST(res, "mf_pjsip_vid_set_tx_stream\n");
#endif

#if 0
    while(!exit_flag);
    exit_flag = 0;
    res = mf_pjsip_vid_set_rx_stream(-1, 1);
    _TEST(res, "mf_pjsip_vid_set_rx_stream\n");
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
