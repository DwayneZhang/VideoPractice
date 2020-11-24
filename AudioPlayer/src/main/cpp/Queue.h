//
// Created by Dwayne on 20/11/24.
//

#ifndef AUDIO_PRACTICE_QUEUE_H
#define AUDIO_PRACTICE_QUEUE_H

#include "queue"
#include "pthread.h"
#include "androidlog.h"
#include "PlayStatus.h"

extern "C" {
#include "libavcodec/avcodec.h"
};

class Queue {

public:
    std::queue<AVPacket *> queuePacket;
    pthread_mutex_t mutexPacket;
    pthread_cond_t condPacket;
    PlayStatus *playStatus = NULL;

public:
    Queue(PlayStatus *playStatus);
    ~Queue();

    int putAvPacket(AVPacket *packet);
    int getAvPacket(AVPacket *packet);
    int getQueueSize();

};


#endif //AUDIO_PRACTICE_QUEUE_H
