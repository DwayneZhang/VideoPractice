//
// Created by Dwayne on 20/11/24.
//

#ifndef AUDIO_PRACTICE_AUDIO_H
#define AUDIO_PRACTICE_AUDIO_H


#include "Queue.h"
#include "PlayStatus.h"

extern "C" {
#include "libavcodec/avcodec.h"
#include <libswresample/swresample.h>
}

class Audio {

public:
    int streamIndex = -1;
    AVCodecParameters *codecpar = NULL;
    AVCodecContext *avCodecContext = NULL;
    Queue *queue = NULL;
    PlayStatus *playStatus = NULL;
    pthread_t thread_play;
    AVPacket *avPacket = NULL;
    AVFrame *avFrame = NULL;
    int ret = 0;
    uint8_t *buffer = NULL;
    int data_size = 0;

public:
    Audio(PlayStatus *playStatus);
    ~Audio();

    void play();

    int resampleAudio();
};


#endif //AUDIO_PRACTICE_AUDIO_H
