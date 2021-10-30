#include <pj/config_site_sample.h>
#define PJMEDIA_AUDIO_DEV_HAS_ALSA	    1

#define PJMEDIA_HAS_VIDEO		        1
#define PJMEDIA_HAS_FFMPEG		        1
#define PJMEDIA_HAS_FFMPEG_CODEC_H264   1
#define PJMEDIA_VIDEO_DEV_HAS_SDL	    1           // 开启本地显示
#define PJMEDIA_VIDEO_DEV_HAS_V4L2	    1           // 开启摄像头