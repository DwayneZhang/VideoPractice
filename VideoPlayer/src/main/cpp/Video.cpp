//
// Created by admin on 2020/12/3.
//

#include "Video.h"

Video::Video(PlayStatus *playStatus, CallJava *callJava) {

    this->playStatus = playStatus;
    this->callJava = callJava;
    queue = new Queue(playStatus);
}

Video::~Video() {

}

void *playVideoCallBack(void *data) {
    Video *video = (Video *)data;
    while (video->playStatus != NULL && !video->playStatus->exit) {
        AVPacket  *avPacket = av_packet_alloc();
        if (video->queue->getAvPacket(avPacket) == 0) {
            //解码渲染操作
            LOGD("decoded playVideoCallBack");
        }
        av_packet_free(&avPacket);
        av_free(avPacket);
        avPacket = NULL;
    }

    pthread_exit(&video->thread_play);
}

void Video::play() {
    pthread_create(&thread_play, NULL, playVideoCallBack, this);
}
