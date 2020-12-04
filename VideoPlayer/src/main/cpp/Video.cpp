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

        if (video->playStatus->seek) {
            av_usleep(1000*100);
            continue;
        }
        if (video->queue->getQueueSize() == 0) {
            if(!video->playStatus->load) {
                video->playStatus->load = true;
                video->callJava->onCallLoad(CHILD_THREAD, true);
            }
            av_usleep(1000*100);
            continue;
        } else {
            if(video->playStatus->load) {
                video->playStatus->load = false;
                video->callJava->onCallLoad(CHILD_THREAD, false);
            }
        }

        AVPacket  *avPacket = av_packet_alloc();
        if (video->queue->getAvPacket(avPacket) != 0) {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }
        if (avcodec_send_packet(video->avCodecContext, avPacket) != 0) {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }
        AVFrame  *avFrame = av_frame_alloc();
        if (avcodec_receive_frame(video->avCodecContext, avFrame) != 0) {

            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;

            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }
        LOGD("decoded avFrame success");

        av_frame_free(&avFrame);
        av_free(avFrame);
        avFrame = NULL;

        av_packet_free(&avPacket);
        av_free(avPacket);
        avPacket = NULL;
    }
    pthread_exit(&video->thread_play);
}

void Video::play() {
    pthread_create(&thread_play, NULL, playVideoCallBack, this);
}

void Video::release() {

    if (queue != NULL) {
        delete queue;
        queue = NULL;
    }

    if (avCodecContext != NULL) {
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
        avCodecContext = NULL;
    }

    if (playStatus != NULL) {
        playStatus = NULL;
    }

    if (callJava != NULL) {
        callJava = NULL;
    }
}
