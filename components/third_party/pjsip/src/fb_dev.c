/* $Id$ */
/*
 * Copyright (C) 2008-2011 Teluu Inc. (http://www.teluu.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <pjmedia-videodev/videodev_imp.h>
#include <pjmedia/event.h>
#include <pj/assert.h>
#include <pj/log.h>
#include <pj/os.h>

#define _LINE()     printf("%s  line:%d\n", THIS_FILE, __LINE__)
#define PJMEDIA_VIDEO_DEV_HAS_FRAMEBUFFER 1
#if defined(PJMEDIA_HAS_VIDEO) && PJMEDIA_HAS_VIDEO != 0 && \
    defined(PJMEDIA_VIDEO_DEV_HAS_FRAMEBUFFER) && PJMEDIA_VIDEO_DEV_HAS_FRAMEBUFFER != 0

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#define THIS_FILE		    "fb_dev.c"
#define DEFAULT_CLOCK_RATE	90000
#define DEFAULT_WIDTH		640
#define DEFAULT_HEIGHT		480
#define DEFAULT_FPS		    25
#define SCREEN_WIDTH        DEFAULT_WIDTH 
#define SCREEN_HEIGHT       DEFAULT_HEIGHT

/* 定义颜色格式映射表 */
typedef struct fb_fmt_info
{
    pjmedia_format_id   fmt_id;
    Uint32              fb_format;
    Uint32              Rmask;
    Uint32              Gmask;
    Uint32              Bmask;
    Uint32              Amask;
} fb_fmt_info;

static fb_fmt_info fb_fmts[] =
{
#if PJ_IS_BIG_ENDIAN
    {PJMEDIA_FORMAT_RGBA,  (Uint32)SDL_PIXELFORMAT_RGBA8888,
     0xFF000000, 0xFF0000, 0xFF00, 0xFF} ,
    {PJMEDIA_FORMAT_RGB24, (Uint32)SDL_PIXELFORMAT_RGB24,
     0xFF0000, 0xFF00, 0xFF, 0} ,
    {PJMEDIA_FORMAT_BGRA,  (Uint32)SDL_PIXELFORMAT_BGRA8888,
     0xFF00, 0xFF0000, 0xFF000000, 0xFF} ,
#else /* PJ_IS_BIG_ENDIAN */
    {PJMEDIA_FORMAT_RGBA,  (Uint32)SDL_PIXELFORMAT_ABGR8888,
     0xFF, 0xFF00, 0xFF0000, 0xFF000000} ,
    {PJMEDIA_FORMAT_RGB24, (Uint32)SDL_PIXELFORMAT_BGR24,
     0xFF, 0xFF00, 0xFF0000, 0} ,
    {PJMEDIA_FORMAT_BGRA,  (Uint32)SDL_PIXELFORMAT_ARGB8888,
     0xFF0000, 0xFF00, 0xFF, 0xFF000000} ,

    {PJMEDIA_FORMAT_YUY2, SDL_PIXELFORMAT_YUY2, 0, 0, 0, 0} ,
    {PJMEDIA_FORMAT_UYVY, SDL_PIXELFORMAT_UYVY, 0, 0, 0, 0} ,
    {PJMEDIA_FORMAT_YVYU, SDL_PIXELFORMAT_YVYU, 0, 0, 0, 0} ,
    {PJMEDIA_FORMAT_I420, SDL_PIXELFORMAT_IYUV, 0, 0, 0, 0} ,
    {PJMEDIA_FORMAT_YV12, SDL_PIXELFORMAT_YV12, 0, 0, 0, 0} ,
#endif /* PJ_IS_BIG_ENDIAN */
};

/* 定义设备信息 */
struct fb_dev_info
{
    pjmedia_vid_dev_info	 info;
};

/* 定义数据流的链表 */
struct fb_stream_list
{
    PJ_DECL_LIST_MEMBER(struct fb_stream_list);
    struct fb_stream	*stream;
};

/*sdl的定义，看情况删除*/
#define INITIAL_MAX_JOBS 64
#define JOB_QUEUE_INC_FACTOR 2

typedef pj_status_t (*job_func_ptr)(void *data);

typedef struct job {
    job_func_ptr    func;
    void           *data;
    unsigned        flags;
    pj_status_t     retval;
} job;

typedef struct job_queue {
    pj_pool_t      *pool;
    job           **jobs;
    pj_sem_t      **job_sem;
    pj_sem_t      **old_sem;
    pj_mutex_t     *mutex;
    pj_thread_t    *thread;
    pj_sem_t       *sem;

    unsigned        size;
    unsigned        head, tail;
    pj_bool_t	    is_full;
    pj_bool_t       is_quitting;
} job_queue;

/* 工厂模式定义 */
struct fb_factory
{
    pjmedia_vid_dev_factory base;      /* 必须在第一位，因为会直接通过fb_factory*指针来指向base */
    pj_pool_t			    *pool;
    pj_pool_factory		    *pf;

    unsigned			    dev_count;
    struct fb_dev_info      *dev_info;
    job_queue               *jq;

    pj_thread_t			    *fb_thread;        /**< fb thread.        */
    pj_sem_t                *sem;
    pj_mutex_t			    *mutex;
    struct fb_stream_list   streams;
    pj_bool_t               is_quitting;
    pj_thread_desc 		    thread_desc;
    pj_thread_t 		    *ev_thread;
};

/* 视频数据流结构 */
struct fb_stream
{
    pjmedia_vid_dev_stream	 base;		        /**< Base stream	    */
    pjmedia_vid_dev_param	 param;		        /**< Settings	    */
    pj_pool_t			    *pool;              /**< Memory pool.       */

    pjmedia_vid_dev_cb		 vid_cb;            /**< Stream callback.   */
    void			        *user_data;         /**< Application data.  */

    struct fb_factory       *ff;
    const pjmedia_frame     *frame;
    pj_bool_t			    is_running;
    pj_timestamp		    last_ts;
    struct fb_stream_list   list_entry;

    SDL_Window              *window;            /**< Display window.    */
    SDL_Renderer            *renderer;          /**< Display renderer.  */
    SDL_Texture             *scr_tex;           /**< Screen texture.    */
    int                     pitch;              /**< Pitch value.       */
    SDL_Rect                rect;               /**< Frame rectangle.   */
    SDL_Rect                dstrect;            /**< Display rectangle. */

    pjmedia_video_apply_fmt_param vafp;
};

/* 函数声明 */
/* Prototypes */
static pj_status_t fb_factory_init(pjmedia_vid_dev_factory *f);
static pj_status_t fb_factory_destroy(pjmedia_vid_dev_factory *f);
static pj_status_t fb_factory_refresh(pjmedia_vid_dev_factory *f);
static unsigned    fb_factory_get_dev_count(pjmedia_vid_dev_factory *f);
static pj_status_t fb_factory_get_dev_info(pjmedia_vid_dev_factory *f,
					    unsigned index,
					    pjmedia_vid_dev_info *info);
static pj_status_t fb_factory_default_param(pj_pool_t *pool,
                                             pjmedia_vid_dev_factory *f,
					     unsigned index,
					     pjmedia_vid_dev_param *param);
static pj_status_t fb_factory_create_stream(
					pjmedia_vid_dev_factory *f,
					pjmedia_vid_dev_param *param,
					const pjmedia_vid_dev_cb *cb,
					void *user_data,
					pjmedia_vid_dev_stream **p_vid_strm);

static pj_status_t fb_stream_get_param(pjmedia_vid_dev_stream *strm,
					pjmedia_vid_dev_param *param);
static pj_status_t fb_stream_get_cap(pjmedia_vid_dev_stream *strm,
				      pjmedia_vid_dev_cap cap,
				      void *value);
static pj_status_t fb_stream_set_cap(pjmedia_vid_dev_stream *strm,
				      pjmedia_vid_dev_cap cap,
				      const void *value);
static pj_status_t fb_stream_put_frame(pjmedia_vid_dev_stream *strm,
                                        const pjmedia_frame *frame);
static pj_status_t fb_stream_start(pjmedia_vid_dev_stream *strm);
static pj_status_t fb_stream_stop(pjmedia_vid_dev_stream *strm);
static pj_status_t fb_stream_destroy(pjmedia_vid_dev_stream *strm);

static pj_status_t resize_disp(struct fb_stream *strm,
                               pjmedia_rect_size *new_disp_size);
static pj_status_t fb_destroy_all(void *data);

/* Job queue prototypes */
// static pj_status_t job_queue_create(pj_pool_t *pool, job_queue **pjq);
// static pj_status_t job_queue_post_job(job_queue *jq, job_func_ptr func,
// 				      void *data, unsigned flags,
// 				      pj_status_t *retval);
// static pj_status_t job_queue_destroy(job_queue *jq);

/* 工厂应用操作 */
static pjmedia_vid_dev_factory_op factory_op =
{
    &fb_factory_init,
    &fb_factory_destroy,
    &fb_factory_get_dev_count,
    &fb_factory_get_dev_info,
    &fb_factory_default_param,
    &fb_factory_create_stream,
    &fb_factory_refresh
};

/* 视频流操作 */
static pjmedia_vid_dev_stream_op stream_op =
{
    &fb_stream_get_param,
    &fb_stream_get_cap,
    &fb_stream_set_cap,
    &fb_stream_start,
    NULL,
    &fb_stream_put_frame,
    &fb_stream_stop,
    &fb_stream_destroy
};

/* 其他 */
static void sdl_log_err(const char *op)
{
    PJ_LOG(1,(THIS_FILE, "%s error: %s", op, SDL_GetError()));
}

/**************************** 工厂操作实现 *********************************/
/*************************************************************************/

/* 创建工厂，这个API是外部(videodev.c)调用这个文件操作的接口 */
pjmedia_vid_dev_factory* pjmedia_fb_factory(pj_pool_factory *pf)
{
    struct fb_factory *f;
    pj_pool_t *pool;

    pool = pj_pool_create(pf, "fb video", 1000, 1000, NULL);
    f = PJ_POOL_ZALLOC_T(pool, struct fb_factory);  // 为f分配内存
    f->pf = pf;
    f->pool = pool;
    f->base.op = &factory_op;
}

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;

/* 初始化工厂 */
static pj_status_t fb_factory_init(pjmedia_vid_dev_factory *f)
{
    struct fb_factory *ff = (struct fb_factory *)f;
    struct fb_dev_info *fdi;

    pj_status_t status = PJ_SUCCESS;

    /**/
    SDL_Init(SDL_INIT_EVERYTHING);

    // 创建一个窗口
    window = SDL_CreateWindow("show win", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    // 渲染层
    renderer = SDL_CreateRenderer(window, -1, 0);

    // 载入图片来生成材质到渲染器
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV, 
            SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

    /* 初始化数据流链表 */
    pj_list_init(&ff->streams);
    if (status != PJ_SUCCESS)
        return PJMEDIA_EVID_INIT;

    /* 创建一个递归互斥锁 */
    status = pj_mutex_create_recursive(ff->pool, "fb_factory",
				       &ff->mutex);
    if (status != PJ_SUCCESS)
	return status;

    /* 创建一个信号量 */           
    status = pj_sem_create(ff->pool, NULL, 0, 1, &ff->sem);
    if (status != PJ_SUCCESS)
	return status; 

    /* 为设备信息分配内存 */
    ff->dev_count = 1;      
    ff->dev_info = (struct fb_dev_info *)pj_pool_calloc(ff->pool, ff->dev_count, sizeof(struct fb_dev_info));
    if (ff->dev_info == NULL)
    return PJMEDIA_EVID_INIT;

    /* 初始化设备名称 */
    fdi = &ff->dev_info[0];   
    pj_bzero(fdi, sizeof(*fdi));
    strncpy(fdi->info.name, "Framebuffer", sizeof(fdi->info.name));
    fdi->info.name[sizeof(fdi->info.name) - 1] = '\0';

    /* 初始化设备能力 */
    fdi->info.fmt_cnt = PJ_ARRAY_SIZE(fb_fmts);
    for (int i = 0; i < ff->dev_count; i ++)
    {
        fdi = &ff->dev_info[i];
        strncpy(fdi->info.driver, "framebuffer", sizeof(fdi->info.driver));
        fdi->info.driver[sizeof(fdi->info.driver) - 1] = '\0';
        fdi->info.dir = PJMEDIA_DIR_RENDER;
        fdi->info.has_callback = PJ_FALSE;
        fdi->info.caps = PJMEDIA_VID_DEV_CAP_FORMAT | 
                        PJMEDIA_VID_DEV_CAP_OUTPUT_RESIZE;
        fdi->info.caps |= PJMEDIA_VID_DEV_CAP_OUTPUT_WINDOW_FLAGS;
        fdi->info.caps |= PJMEDIA_VID_DEV_CAP_OUTPUT_WINDOW;
        
        // 注册图像映射格式
        for (int j = 0; j < fdi->info.fmt_cnt; j ++)
        {
            pjmedia_format *fmt = &fdi->info.fmt[j];
            pjmedia_format_init_video(fmt, fb_fmts[j].fmt_id, 
                DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_FPS, 1);
        }
    }

    PJ_LOG(4, (THIS_FILE, "Framebuffer dev initialized"));

    return PJ_SUCCESS;
}

/* 销毁工厂 */
static pj_status_t fb_factory_destroy(pjmedia_vid_dev_factory *f)
{
    struct fb_factory *ff = (struct fb_factory *)f;
    pj_pool_t *pool = ff->pool;
    pj_status_t status;
    PJ_UNUSED_ARG(status);

    /* 清空链表 */
    pj_assert(pj_list_empty(&ff->streams));

    /* 正在退出 */
    ff->is_quitting = PJ_TRUE;

    /* 销毁递归互斥锁 */
    if (ff->mutex) {
	pj_mutex_destroy(ff->mutex);
	ff->mutex = NULL;
    }

    /* 销毁信号量 */
    if (ff->sem) {
        pj_sem_destroy(ff->sem);
        ff->sem = NULL;
    }

    /* 释放内存池 */
    ff->pool = NULL;
    pj_pool_release(pool);

    return PJ_SUCCESS;
}

/* 刷新设备列表 */
static pj_status_t fb_factory_refresh(pjmedia_vid_dev_factory *f)
{
    PJ_UNUSED_ARG(f);
    return PJ_SUCCESS;
}

/* 获取设备数 */
static unsigned fb_factory_get_dev_count(pjmedia_vid_dev_factory *f)
{
    struct fb_factory *sf = (struct fb_factory*)f;
    return sf->dev_count;
}

/* 获取设备信息 */
static pj_status_t fb_factory_get_dev_info(pjmedia_vid_dev_factory *f,
                                            unsigned index,
                                            pjmedia_vid_dev_info *info)
{
    struct fb_factory *ff = (struct fb_factory*)f;

    PJ_ASSERT_RETURN(index < ff->dev_count, PJMEDIA_EVID_INVDEV);
    pj_memcpy(info, &ff->dev_info[index].info, sizeof(*info));

    return PJ_SUCCESS;
}

/* 创建默认的设备参数 */
static pj_status_t fb_factory_default_param(pj_pool_t *pool,
                                             pjmedia_vid_dev_factory *f,
                                            unsigned index,
                                            pjmedia_vid_dev_param *param)
{
    struct fb_factory *ff = (struct fb_factory*)f;
    struct fb_dev_info *fdi = &ff->dev_info[index];

    PJ_ASSERT_RETURN(index < ff->dev_count, PJMEDIA_EVID_INVDEV);
    
    PJ_UNUSED_ARG(pool);

    pj_bzero(param, sizeof(*param));
    param->dir = PJMEDIA_DIR_RENDER;
    param->rend_id = index;
    param->cap_id = PJMEDIA_VID_INVALID_DEV;

    /* Set the device capabilities here */
    param->flags = PJMEDIA_VID_DEV_CAP_FORMAT;
    param->fmt.type = PJMEDIA_TYPE_VIDEO;
    param->clock_rate = DEFAULT_CLOCK_RATE;
    pj_memcpy(&param->fmt, &fdi->info.fmt[0], sizeof(param->fmt));

    return PJ_SUCCESS;  
}

/**************************** 数据流操作实现 *********************************/
/*************************************************************************/
/* 发送流 */
static pj_status_t put_frame(void *data)
{
    struct fb_stream *stream = (struct fb_stream *)data;
    const pjmedia_frame *frame = stream->frame;

    /* 本地显示？ */
    if (stream->scr_tex) {
        SDL_UpdateTexture(stream->scr_tex, NULL, frame->buf, stream->pitch);
        SDL_RenderClear(stream->renderer);
        SDL_RenderCopy(stream->renderer, stream->scr_tex,
		       &stream->rect, &stream->dstrect);
        SDL_RenderPresent(stream->renderer);
    }

    return PJ_SUCCESS;
}

static pj_status_t fb_stream_put_frame(pjmedia_vid_dev_stream *strm,
					const pjmedia_frame *frame)
{
    struct fb_stream *stream = (struct fb_stream*)strm;
    pj_status_t status;

    stream->last_ts.u64 = frame->timestamp.u64;

    /* Video conference just trying to send heart beat for updating timestamp
     * or keep-alive, this port doesn't need any, just ignore.
     */
    if (frame->size==0 || frame->buf==NULL)
	return PJ_SUCCESS;

    if (frame->size < stream->vafp.framebytes)
	return PJ_ETOOSMALL;

    if (!stream->is_running)
	return PJ_EINVALIDOP;

    stream->frame = frame;

    return put_frame(strm);
}

/* 创建流 */
static fb_fmt_info* get_fb_format_info(pjmedia_format_id id)
{
    unsigned i;

    for (i = 0; i < sizeof(fb_fmts)/sizeof(fb_fmts[0]); i++) {
        if (fb_fmts[i].fmt_id == id)
            return &fb_fmts[i];
    }

    return NULL;
}

static pj_status_t fb_destroy(void *data)
{
    struct fb_stream *strm = (struct fb_stream *)data;

    if (strm->scr_tex) {
        SDL_DestroyTexture(strm->scr_tex);
        strm->scr_tex = NULL;
    }
    if (strm->renderer) {
        SDL_DestroyRenderer(strm->renderer);
        strm->renderer = NULL;
    } 

    return PJ_SUCCESS;
}

static pj_status_t sdl_create_window(struct fb_stream *strm, 
				     pj_bool_t use_app_win,
				     Uint32 sdl_format,
				     pjmedia_vid_dev_hwnd *hwnd)
{
    if (!strm->window) {
        Uint32 flags = 0;
        
        if (strm->param.flags & PJMEDIA_VID_DEV_CAP_OUTPUT_WINDOW_FLAGS) {
            if (!(strm->param.window_flags & PJMEDIA_VID_DEV_WND_BORDER))
                flags |= SDL_WINDOW_BORDERLESS;
            if (strm->param.window_flags & PJMEDIA_VID_DEV_WND_RESIZABLE)
                flags |= SDL_WINDOW_RESIZABLE;
        } else {
            flags |= SDL_WINDOW_BORDERLESS;
        }

        if (!((strm->param.flags & PJMEDIA_VID_DEV_CAP_OUTPUT_HIDE) &&
            strm->param.window_hide))
        {
            flags |= SDL_WINDOW_SHOWN;
        } else {
            flags &= ~SDL_WINDOW_SHOWN;
            flags |= SDL_WINDOW_HIDDEN;
        }

        if ((strm->param.flags & PJMEDIA_VID_DEV_CAP_OUTPUT_FULLSCREEN) &&
            strm->param.window_fullscreen)
        {
            flags |= SDL_WINDOW_FULLSCREEN;
        }

        {
            int x, y;

            x = y = SDL_WINDOWPOS_CENTERED;
            if (strm->param.flags & PJMEDIA_VID_DEV_CAP_OUTPUT_POSITION) {
                x = strm->param.window_pos.x;
                y = strm->param.window_pos.y;
            }

            /* Create the window where we will draw. */

            window = SDL_CreateWindow("pjmedia-SDL video",
                                            x, y,
                                            strm->param.disp_size.w,
                                            strm->param.disp_size.h,
                                            flags);
            strm->window = window;
            if (!strm->window) {
            sdl_log_err("SDL_CreateWindow()");
            return PJMEDIA_EVID_SYSERR;
            }
        }
    }

    /**
      * We must call SDL_CreateRenderer in order for draw calls to
      * affect this window.
      */
    renderer = SDL_CreateRenderer(strm->window, -1, 0);
    strm->renderer = renderer;
    if (!strm->renderer) {
	sdl_log_err("SDL_CreateRenderer()");
        return PJMEDIA_EVID_SYSERR;
    }

    {    
        texture = SDL_CreateTexture(strm->renderer, sdl_format,
                                          SDL_TEXTUREACCESS_STREAMING,
                                          strm->rect.w, strm->rect.h);
        strm->scr_tex = texture;
        if (strm->scr_tex == NULL) {
            sdl_log_err("SDL_CreateTexture()");
            return PJMEDIA_EVID_SYSERR;
        }
    
        strm->pitch = strm->rect.w * SDL_BYTESPERPIXEL(sdl_format);
    }

    return PJ_SUCCESS;
}

static pj_status_t fb_create(void *data)
{
    struct fb_stream *strm = (struct fb_stream *)data;
    pjmedia_format *fmt = &strm->param.fmt;
    fb_fmt_info *fb_info;
    const pjmedia_video_format_info *vfi;
    pjmedia_video_format_detail *vfd;

    /* 获取图像格式信息 */
    fb_info = get_fb_format_info(fmt->id);
    vfi = pjmedia_get_video_format_info(pjmedia_video_format_mgr_instance(),
                                        fmt->id);

    if (!vfi || !fb_info)
        return PJMEDIA_EVID_BADFORMAT;

    /* 获取图像细节信息 */
    vfd = pjmedia_format_get_video_format_detail(fmt, PJ_TRUE);
    strm->rect.x = strm->rect.y = 0;
    strm->rect.w = (Uint16)vfd->size.w;
    strm->rect.h = (Uint16)vfd->size.h;
    if (strm->param.disp_size.w == 0)
        strm->param.disp_size.w = strm->rect.w;
    if (strm->param.disp_size.h == 0)
        strm->param.disp_size.h = strm->rect.h;
    strm->dstrect.x = strm->dstrect.y = 0;
    strm->dstrect.w = (Uint16)strm->param.disp_size.w;
    strm->dstrect.h = (Uint16)strm->param.disp_size.h;

    // ???这里不销毁可能导致花屏
    fb_destroy(strm);

    return sdl_create_window(strm, 
			 (strm->param.flags & PJMEDIA_VID_DEV_CAP_OUTPUT_WINDOW),
			 fb_info->fb_format,
			 &strm->param.window);
}

static pj_status_t fb_factory_create_stream(
					pjmedia_vid_dev_factory *f,
					pjmedia_vid_dev_param *param,
					const pjmedia_vid_dev_cb *cb,
					void *user_data,
					pjmedia_vid_dev_stream **p_vid_strm)
{
    struct fb_factory *ff = (struct fb_factory*)f;
    pj_pool_t *pool;
    struct fb_stream *strm;
    pj_status_t status;

    PJ_ASSERT_RETURN(param->dir == PJMEDIA_DIR_RENDER, PJ_EINVAL);

    /* 创建并初始化流描述符 */
    pool = pj_pool_create(ff->pf, "fb-dev", 1000, 1000, NULL);
    PJ_ASSERT_RETURN(pool != NULL, PJ_ENOMEM);
    strm = PJ_POOL_ZALLOC_T(pool, struct fb_stream);

    /* 为流添加参数 */
    pj_memcpy(&strm->param, param, sizeof(*param));
    strm->pool = pool;
    strm->ff = ff;
    pj_memcpy(&strm->vid_cb, cb, sizeof(*cb));

    /* 初始化链表 */
    pj_list_init(&strm->list_entry);
    strm->list_entry.stream = strm;
    strm->user_data = user_data;

    /* 创建流 */
    status = fb_create(strm);
    if (status != PJ_SUCCESS) {
        goto on_error;
    }

    /* 加锁 */


    /* Done */
    strm->base.op = &stream_op;
    *p_vid_strm = &strm->base;
    
on_error:
    fb_stream_destroy(&strm->base);
    return status;
}

/* 获取数据流的信息 */
static pj_status_t fb_stream_get_param(pjmedia_vid_dev_stream *s,
					pjmedia_vid_dev_param *pi)
{
    struct fb_stream *strm = (struct fb_stream*)s;
    PJ_ASSERT_RETURN(strm && pi, PJ_EINVAL);

    pj_memcpy(pi, &strm->param, sizeof(*pi));

    if (fb_stream_get_cap(s, PJMEDIA_VID_DEV_CAP_OUTPUT_WINDOW,
			   &pi->window) == PJ_SUCCESS)
    {
	pi->flags |= PJMEDIA_VID_DEV_CAP_OUTPUT_WINDOW;
    }
    if (fb_stream_get_cap(s, PJMEDIA_VID_DEV_CAP_OUTPUT_POSITION,
			   &pi->window_pos) == PJ_SUCCESS)
    {
	pi->flags |= PJMEDIA_VID_DEV_CAP_OUTPUT_POSITION;
    }
    if (fb_stream_get_cap(s, PJMEDIA_VID_DEV_CAP_OUTPUT_RESIZE,
			   &pi->disp_size) == PJ_SUCCESS)
    {
	pi->flags |= PJMEDIA_VID_DEV_CAP_OUTPUT_RESIZE;
    }
    if (fb_stream_get_cap(s, PJMEDIA_VID_DEV_CAP_OUTPUT_HIDE,
			   &pi->window_hide) == PJ_SUCCESS)
    {
	pi->flags |= PJMEDIA_VID_DEV_CAP_OUTPUT_HIDE;
    }
    if (fb_stream_get_cap(s, PJMEDIA_VID_DEV_CAP_OUTPUT_WINDOW_FLAGS,
			   &pi->window_flags) == PJ_SUCCESS)
    {
	pi->flags |= PJMEDIA_VID_DEV_CAP_OUTPUT_WINDOW_FLAGS;
    }
    if (fb_stream_get_cap(s, PJMEDIA_VID_DEV_CAP_OUTPUT_FULLSCREEN,
			   &pi->window_fullscreen) == PJ_SUCCESS)
    {
	pi->flags |= PJMEDIA_VID_DEV_CAP_OUTPUT_FULLSCREEN;
    }

    return PJ_SUCCESS;
}

/* 获取数据流已支持的能力 */
struct strm_cap {
    struct fb_stream   *strm;
    pjmedia_vid_dev_cap  cap;
    union {
        void            *pval;
        const void      *cpval;
    } pval;
};

static pj_status_t get_cap(void *data)
{
    struct strm_cap *scap = (struct strm_cap *)data;
    struct fb_stream *strm = scap->strm;
    pjmedia_vid_dev_cap cap = scap->cap;
    void *pval = scap->pval.pval;

    if (cap == PJMEDIA_VID_DEV_CAP_OUTPUT_WINDOW)
    {
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);

	if (SDL_GetWindowWMInfo(strm->window, &info)) {
	    pjmedia_vid_dev_hwnd *wnd = (pjmedia_vid_dev_hwnd *)pval;
	    if (0) { }
#if defined(SDL_VIDEO_DRIVER_WINDOWS)
	    else if (info.subsystem == SDL_SYSWM_WINDOWS) {
		wnd->type = PJMEDIA_VID_DEV_HWND_TYPE_WINDOWS;
		wnd->info.win.hwnd = (void *)info.info.win.window;
	    }
#endif
#if defined(SDL_VIDEO_DRIVER_X11)
	    else if (info.subsystem == SDL_SYSWM_X11) {
		wnd->info.x11.window = (void *)info.info.x11.window;
		wnd->info.x11.display = (void *)info.info.x11.display;
	    }
#endif
#if defined(SDL_VIDEO_DRIVER_COCOA)
	    else if (info.subsystem == SDL_SYSWM_COCOA) {
		wnd->info.cocoa.window = (void *)info.info.cocoa.window;
	    }
#endif
#if defined(SDL_VIDEO_DRIVER_UIKIT)
	    else if (info.subsystem == SDL_SYSWM_UIKIT) {
		wnd->info.ios.window = (void *)info.info.uikit.window;
	    }
#endif
	    else {
		return PJMEDIA_EVID_INVCAP;
	    }
	    return PJ_SUCCESS;
	} else
	    return PJMEDIA_EVID_INVCAP;
    } else if (cap == PJMEDIA_VID_DEV_CAP_OUTPUT_POSITION) {
        SDL_GetWindowPosition(strm->window, &((pjmedia_coord *)pval)->x,
                              &((pjmedia_coord *)pval)->y);
	return PJ_SUCCESS;
    } else if (cap == PJMEDIA_VID_DEV_CAP_OUTPUT_RESIZE) {
        SDL_GetWindowSize(strm->window, (int *)&((pjmedia_rect_size *)pval)->w,
                          (int *)&((pjmedia_rect_size *)pval)->h);
	return PJ_SUCCESS;
    } else if (cap == PJMEDIA_VID_DEV_CAP_OUTPUT_HIDE) {
	Uint32 flag = SDL_GetWindowFlags(strm->window);
	*((pj_bool_t *)pval) = (flag & SDL_WINDOW_HIDDEN)? PJ_TRUE: PJ_FALSE;
	return PJ_SUCCESS;
    } else if (cap == PJMEDIA_VID_DEV_CAP_OUTPUT_WINDOW_FLAGS) {
	Uint32 flag = SDL_GetWindowFlags(strm->window);
        unsigned *wnd_flags = (unsigned *)pval;
        if (!(flag & SDL_WINDOW_BORDERLESS))
            *wnd_flags |= PJMEDIA_VID_DEV_WND_BORDER;
        if (flag & SDL_WINDOW_RESIZABLE)
            *wnd_flags |= PJMEDIA_VID_DEV_WND_RESIZABLE;
	return PJ_SUCCESS;
    } else if (cap == PJMEDIA_VID_DEV_CAP_OUTPUT_FULLSCREEN) {
	Uint32 flag = SDL_GetWindowFlags(strm->window);
	*((pj_bool_t *)pval) = (flag & SDL_WINDOW_FULLSCREEN)? PJ_TRUE: PJ_FALSE;
	return PJ_SUCCESS;
    }

    return PJMEDIA_EVID_INVCAP;
}

static pj_status_t fb_stream_get_cap(pjmedia_vid_dev_stream *s,
				      pjmedia_vid_dev_cap cap,
				      void *pval)
{
    struct fb_stream *strm = (struct fb_stream*)s;
    struct strm_cap scap;
    pj_status_t status;

    PJ_ASSERT_RETURN(s && pval, PJ_EINVAL);

    scap.strm = strm;
    scap.cap = cap;
    scap.pval.pval = pval;

    status = get_cap(&scap);

    return status;
}

/* 设置数据流支持的能力 */
static pj_status_t resize_disp(struct fb_stream *strm,
                               pjmedia_rect_size *new_disp_size)
{
    pj_memcpy(&strm->param.disp_size, new_disp_size,
              sizeof(strm->param.disp_size));
    
    if (strm->scr_tex) {
        strm->dstrect.x = strm->dstrect.y = 0;
        strm->dstrect.w = (Uint16)strm->param.disp_size.w;
	strm->dstrect.h = (Uint16)strm->param.disp_size.h;
	SDL_RenderSetViewport(strm->renderer, &strm->dstrect);
    }
#if PJMEDIA_VIDEO_DEV_SDL_HAS_OPENGL
    else if (strm->param.rend_id == OPENGL_DEV_IDX) {
	sdl_create_rend(strm, &strm->param.fmt);
    }
#endif /* PJMEDIA_VIDEO_DEV_SDL_HAS_OPENGL */

    return PJ_SUCCESS;
}

static pj_status_t change_format(struct fb_stream *strm,
                                 pjmedia_format *new_fmt)
{
    pj_status_t status;

    /* Recreate SDL renderer */
    // status = fb_create_rend(strm, (new_fmt? new_fmt :
	// 			   &strm->param.fmt));
    // if (status == PJ_SUCCESS && new_fmt)
    //     pjmedia_format_copy(&strm->param.fmt, new_fmt);

    return status;
}

static pj_status_t set_cap(void *data)
{
    struct strm_cap *scap = (struct strm_cap *)data;
    struct fb_stream *strm = scap->strm;
    pjmedia_vid_dev_cap cap = scap->cap;
    const void *pval = scap->pval.cpval;

    if (cap == PJMEDIA_VID_DEV_CAP_OUTPUT_POSITION) {
        /**
         * Setting window's position when the window is hidden also sets
         * the window's flag to shown (while the window is, actually,
         * still hidden). This causes problems later when setting/querying
         * the window's visibility.
         * See ticket #1429 (http://trac.pjsip.org/repos/ticket/1429)
         */
	Uint32 flag = SDL_GetWindowFlags(strm->window);
	if (flag & SDL_WINDOW_HIDDEN)
            SDL_ShowWindow(strm->window);
        SDL_SetWindowPosition(strm->window, ((pjmedia_coord *)pval)->x,
                              ((pjmedia_coord *)pval)->y);
	if (flag & SDL_WINDOW_HIDDEN)
            SDL_HideWindow(strm->window);
	return PJ_SUCCESS;
    } else if (cap == PJMEDIA_VID_DEV_CAP_OUTPUT_HIDE) {
        if (*(pj_bool_t *)pval)
            SDL_HideWindow(strm->window);
        else
            SDL_ShowWindow(strm->window);
	return PJ_SUCCESS;
    } else if (cap == PJMEDIA_VID_DEV_CAP_FORMAT) {
        pj_status_t status;

        status = change_format(strm, (pjmedia_format *)pval);
	if (status != PJ_SUCCESS) {
	    pj_status_t status_;
	    
	    /**
	     * Failed to change the output format. Try to revert
	     * to its original format.
	     */
            status_ = change_format(strm, &strm->param.fmt);
	    if (status_ != PJ_SUCCESS) {
		/**
		 * This means that we failed to revert to our
		 * original state!
		 */
		status = PJMEDIA_EVID_ERR;
	    }
	}
	
	return status;
    } else if (cap == PJMEDIA_VID_DEV_CAP_OUTPUT_RESIZE) {
	pjmedia_rect_size *new_size = (pjmedia_rect_size *)pval;

	SDL_SetWindowSize(strm->window, new_size->w, new_size->h);
        return resize_disp(strm, new_size);
    } else if (cap == PJMEDIA_VID_DEV_CAP_OUTPUT_WINDOW) {
	pjmedia_vid_dev_hwnd *hwnd = (pjmedia_vid_dev_hwnd*)pval;
	pj_status_t status = PJ_SUCCESS;
	fb_fmt_info *fb_info = get_fb_format_info(strm->param.fmt.id);
	/* Re-init SDL */
	status = fb_destroy_all(strm);
	if (status != PJ_SUCCESS)
	    return status;	

	status = sdl_create_window(strm, PJ_TRUE, fb_info->fb_format, hwnd);
        PJ_PERROR(4, (THIS_FILE, status,
		      "Re-initializing SDL with native window %d",
		      hwnd->info.window));
	return status;	
    } else if (cap == PJMEDIA_VID_DEV_CAP_OUTPUT_FULLSCREEN) {
        Uint32 flag;

	flag = SDL_GetWindowFlags(strm->window);
        if (*(pj_bool_t *)pval)
            flag |= SDL_WINDOW_FULLSCREEN;
        else
            flag &= (~SDL_WINDOW_FULLSCREEN);

        SDL_SetWindowFullscreen(strm->window, flag);

	/* Trying to restore the border after returning from fullscreen,
	 * unfortunately not sure how to put back the resizable flag.
	 */
	if ((flag & SDL_WINDOW_FULLSCREEN)==0 &&
	    (flag & SDL_WINDOW_BORDERLESS)==0)
	{
	    SDL_SetWindowBordered(strm->window, SDL_FALSE);
	    SDL_SetWindowBordered(strm->window, SDL_TRUE);
	}

	return PJ_SUCCESS;
    }

    return PJMEDIA_EVID_INVCAP;
}

static pj_status_t fb_stream_set_cap(pjmedia_vid_dev_stream *s,
				      pjmedia_vid_dev_cap cap,
				      const void *pval)
{
    struct sdl_stream *strm = (struct sdl_stream*)s;
    struct strm_cap scap;
    pj_status_t status;

    PJ_ASSERT_RETURN(s && pval, PJ_EINVAL);

    scap.strm = strm;
    scap.cap = cap;
    scap.pval.cpval = pval;

    status = set_cap(&scap);

    return status;
}

/* 开启数据流 */
static pj_status_t fb_stream_start(pjmedia_vid_dev_stream *strm)
{
    struct fb_stream *stream = (struct fb_stream*)strm;

    PJ_LOG(4, (THIS_FILE, "Starting framebuffer video stream"));

    stream->is_running = PJ_TRUE;

    return PJ_SUCCESS;
}

/* 停止数据流 */
static pj_status_t fb_stream_stop(pjmedia_vid_dev_stream *strm)
{
    struct fb_stream *stream = (struct fb_stream*)strm;

    PJ_LOG(4, (THIS_FILE, "Stopping framebuffer video stream"));

    stream->is_running = PJ_FALSE;

    return PJ_SUCCESS;
}

/* 销毁流 */
static pj_status_t fb_destroy_all(void *data)
{
    struct fb_stream *strm = (struct fb_stream *)data;  

    fb_destroy(data);
#if !defined(TARGET_OS_IPHONE) || TARGET_OS_IPHONE == 0
    if (strm->window &&
        !(strm->param.flags & PJMEDIA_VID_DEV_CAP_OUTPUT_WINDOW))
    {
        SDL_DestroyWindow(strm->window);
    }
    strm->window = NULL;
#endif /* TARGET_OS_IPHONE */
    return PJ_SUCCESS;
}

static pj_status_t fb_stream_destroy(pjmedia_vid_dev_stream *strm)
{
    struct fb_stream *stream = (struct fb_stream*)strm;
    pj_status_t status;

    PJ_ASSERT_RETURN(stream != NULL, PJ_EINVAL);

    fb_stream_stop(strm);

    fb_destroy_all(strm);

    pj_mutex_lock(stream->ff->mutex);
    if (!pj_list_empty(&stream->list_entry))
	pj_list_erase(&stream->list_entry);
    pj_mutex_unlock(stream->ff->mutex);

    pj_pool_release(stream->pool);

    return PJ_SUCCESS;
}
#endif	/* PJMEDIA_VIDEO_DEV_HAS_FRAMEBUFFER */
