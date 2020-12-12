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
    Video *video = (Video *) data;
    while (video->playStatus != NULL && !video->playStatus->exit) {

        if (video->playStatus->seek) {
            av_usleep(1000 * 100);
            continue;
        }
        if (video->queue->getQueueSize() == 0) {
            if (!video->playStatus->load) {
                video->playStatus->load = true;
                video->callJava->onCallLoad(CHILD_THREAD, true);
            }
            av_usleep(1000 * 100);
            continue;
        } else {
            if (video->playStatus->load) {
                video->playStatus->load = false;
                video->callJava->onCallLoad(CHILD_THREAD, false);
            }
        }

        AVPacket *avPacket = av_packet_alloc();
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
        AVFrame *avFrame = av_frame_alloc();
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
        if (avFrame->format == AV_PIX_FMT_YUV420P) {
            video->callJava->onCallRenderYUV(CHILD_THREAD, video->avCodecContext->width,
                                             video->avCodecContext->height,
                                             avFrame->data[0], avFrame->data[1],
                                             avFrame->data[2]);
        } else {
            AVFrame *pFrameYUV420P = av_frame_alloc();
            int size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,
                                                video->avCodecContext->width,
                                                video->avCodecContext->height, 1);
            uint8_t *buffer = (uint8_t *) av_malloc(size * sizeof(uint8_t));
            av_image_fill_arrays(pFrameYUV420P->data, pFrameYUV420P->linesize, buffer,
                                 AV_PIX_FMT_YUV420P, video->avCodecContext->width,
                                 video->avCodecContext->height, 1);
            SwsContext *swsContext = sws_getContext(video->avCodecContext->width,
                                                    video->avCodecContext->height,
                                                    video->avCodecContext->pix_fmt,
                                                    video->avCodecContext->width,
                                                    video->avCodecContext->height,
                                                    AV_PIX_FMT_YUV420P,
                                                    SWS_BICUBIC, NULL,
                                                    NULL, NULL);
            if (!swsContext) {
                av_frame_free(&avFrame);
                av_free(pFrameYUV420P);
                av_free(buffer);
                continue;
            }

            sws_scale(swsContext, avFrame->data, avFrame->linesize, 0, avFrame->height,
                      pFrameYUV420P->data, pFrameYUV420P->linesize);

            video->callJava->onCallRenderYUV(CHILD_THREAD, video->avCodecContext->width,
                                             video->avCodecContext->height,
                                             pFrameYUV420P->data[0], pFrameYUV420P->data[1],
                                             pFrameYUV420P->data[2]);
            av_frame_free(&avFrame);
            av_free(pFrameYUV420P);
            av_free(buffer);
            sws_freeContext(swsContext);

        }

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
