//
// Created by Dwayne on 20/11/24.
//

#ifndef AUDIO_PRACTICE_AUDIO_H
#define AUDIO_PRACTICE_AUDIO_H


#include "Queue.h"
#include "PlayStatus.h"

extern "C" {
#include <libavcodec/avcodec.h>
};

class Audio {

public:
    int streamIndex = -1;
    AVCodecParameters *codecpar = NULL;
    AVCodecContext *avCodecContext = NULL;
    Queue *queue = NULL;
    PlayStatus *playStatus = NULL;

public:
    Audio(PlayStatus *playStatus);
    ~Audio();
};


#endif //AUDIO_PRACTICE_AUDIO_H
