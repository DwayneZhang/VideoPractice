//
// Created by admin on 2020/12/3.
//

#ifndef VIDEO_PRACTICE_VIDEO_H
#define VIDEO_PRACTICE_VIDEO_H

#include "Queue.h"
#include "CallJava.h"
#include "Audio.h"

#define CODEC_YUV 0
#define CODEC_MEDIACODEC 1

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/time.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
}

class Video {

public:
    int streamIndex = -1;
    AVCodecContext *avCodecContext = NULL;
    AVCodecParameters *avCodecPar = NULL;
    Queue *queue = NULL;
    PlayStatus *playStatus = NULL;
    CallJava *callJava = NULL;
    AVRational time_base;

    pthread_t thread_play;

    Audio *audio = NULL;
    double clock = 0;
    double delayTime = 0;
    double defaultDelayTime = 0.04;
    pthread_mutex_t codecMutex;

    int codectype = CODEC_YUV;

    AVBSFContext *abs_ctx = NULL;

public:
    Video(PlayStatus *playStatus, CallJava *callJava);

    ~Video();

    void play();

    void release();

    double getFrameDiffTime(AVFrame *avFrame);

    double getDelayTime(double diff);
};


#endif //VIDEO_PRACTICE_VIDEO_H
