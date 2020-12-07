//
// Created by admin on 2020/12/3.
//

#ifndef VIDEO_PRACTICE_VIDEO_H
#define VIDEO_PRACTICE_VIDEO_H

#include "Queue.h"
#include "CallJava.h"

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

public:
    Video(PlayStatus *playStatus, CallJava *callJava);
    ~Video();

    void play();

    void release();
};


#endif //VIDEO_PRACTICE_VIDEO_H
