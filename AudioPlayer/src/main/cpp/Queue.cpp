//
// Created by Dwayne on 20/11/24.
//

#include "Queue.h"

Queue::Queue(PlayStatus *playStatus) {
    this->playStatus = playStatus;
    pthread_mutex_init(&mutexPacket, NULL);
    pthread_cond_init(&condPacket, NULL);
}

Queue::~Queue() {
    clearAVPacket();
    pthread_mutex_destroy(&mutexPacket);
    pthread_cond_destroy(&condPacket);
}

int Queue::putAvPacket(AVPacket *packet) {

    pthread_mutex_lock(&mutexPacket);

    queuePacket.push(packet);
    if(LOG_DEBUG) {
//        LOGD("put a AVPacket to queue, now the size is %d", queuePacket.size());
    }

    pthread_cond_signal(&condPacket);

    pthread_mutex_unlock(&mutexPacket);
    return 0;
}

int Queue::getAvPacket(AVPacket *packet) {

    pthread_mutex_lock(&mutexPacket);

    while (playStatus != NULL && !playStatus->exit) {
        if(queuePacket.size() > 0) {
            AVPacket  *avPacket = queuePacket.front();
            if(av_packet_ref(packet, avPacket) == 0) {
                queuePacket.pop();
            }
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            if(LOG_DEBUG) {
//                LOGD("pop a AVPacket from queue, now the size is %d", queuePacket.size());
//                LOGD("packet->size is %d", packet->size);
            }
            break;
        } else {
            pthread_cond_wait(&condPacket, &mutexPacket);
        }
    }

    pthread_mutex_unlock(&mutexPacket);
    return 0;
}

int Queue::getQueueSize() {
    int size = 0;
    pthread_mutex_lock(&mutexPacket);

    size = queuePacket.size();

    pthread_mutex_unlock(&mutexPacket);

    return size;
}

void Queue::clearAVPacket() {
    pthread_cond_signal(&condPacket);
    pthread_mutex_lock(&mutexPacket);

    while (!queuePacket.empty()) {
        AVPacket *avPacket = queuePacket.front();
        queuePacket.pop();
        av_packet_free(&avPacket);
        av_free(avPacket);
        avPacket = NULL;
    }

    pthread_mutex_unlock(&mutexPacket);
}
