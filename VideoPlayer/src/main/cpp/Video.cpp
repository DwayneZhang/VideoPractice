//
// Created by admin on 2020/12/3.
//

#include "Video.h"

Video::Video(PlayStatus *playStatus, CallJava *callJava) {

    this->playStatus = playStatus;
    this->callJava = callJava;
    queue = new Queue(playStatus);
    pthread_mutex_init(&codecMutex, NULL);
}

Video::~Video() {
    pthread_mutex_destroy(&codecMutex);
}

void *playVideoCallBack(void *data) {
    Video *video = (Video *) data;
    while (video->playStatus != NULL && !video->playStatus->exit) {

        if (video->playStatus->seek) {
            av_usleep(1000 * 100);
            continue;
        }
        if (video->playStatus->pause) {
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
        if (video->codectype == CODEC_MEDIACODEC) {

            if (av_bsf_send_packet(video->abs_ctx, avPacket) != 0) {
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;
                continue;
            }
            while (av_bsf_receive_packet(video->abs_ctx, avPacket) == 0) {

                double diff = video->getFrameDiffTime(NULL, avPacket);
                av_usleep(video->getDelayTime(diff) * 1000000);

                video->callJava->onCallDecodeAVPacket(avPacket->size, avPacket->data);

                av_packet_free(&avPacket);
                av_free(avPacket);
                continue;
            }
            avPacket = NULL;
        } else if (video->codectype == CODEC_YUV) {
            pthread_mutex_lock(&video->codecMutex);
            if (avcodec_send_packet(video->avCodecContext, avPacket) != 0) {
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;
                pthread_mutex_unlock(&video->codecMutex);
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
                pthread_mutex_unlock(&video->codecMutex);
                continue;
            }
            if (avFrame->format == AV_PIX_FMT_YUV420P) {
                double diff = video->getFrameDiffTime(avFrame, NULL);
                av_usleep(video->getDelayTime(diff) * 1000000);
//            LOGD("diff is :%f", diff);
//            LOGD("delay time is :%f", video->getDelayTime(diff));
                video->callJava->onCallRenderYUV(CHILD_THREAD,
                                                 video->avCodecContext->width,
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
                    pthread_mutex_unlock(&video->codecMutex);
                    continue;
                }

                sws_scale(swsContext, avFrame->data, avFrame->linesize, 0,
                          avFrame->height,
                          pFrameYUV420P->data, pFrameYUV420P->linesize);

                double diff = video->getFrameDiffTime(avFrame, NULL);
                av_usleep(video->getDelayTime(diff) * 1000000);
//            LOGD("diff is :%f", diff);
//            LOGD("delay time is :%f", video->getDelayTime(diff));
                video->callJava->onCallRenderYUV(CHILD_THREAD,
                                                 video->avCodecContext->width,
                                                 video->avCodecContext->height,
                                                 pFrameYUV420P->data[0],
                                                 pFrameYUV420P->data[1],
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
            pthread_mutex_unlock(&video->codecMutex);
        }
    }
    return 0;
}

void Video::play() {
    if (playStatus != NULL && !playStatus->exit) {
        pthread_create(&thread_play, NULL, playVideoCallBack, this);
    }
}

void Video::release() {

    if (queue != NULL) {
        queue->noticeQueue();
    }
    pthread_join(thread_play, NULL);

    if (queue != NULL) {
        delete queue;
        queue = NULL;
    }

    if (abs_ctx != NULL) {
        av_bsf_free(&abs_ctx);
        abs_ctx = NULL;
    }

    if (avCodecContext != NULL) {
        pthread_mutex_lock(&codecMutex);
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
        avCodecContext = NULL;
        pthread_mutex_unlock(&codecMutex);
    }

    if (playStatus != NULL) {
        playStatus = NULL;
    }

    if (callJava != NULL) {
        callJava = NULL;
    }
}

double Video::getFrameDiffTime(AVFrame *avFrame, AVPacket *avPacket) {
    double pts = 0;
    if (avFrame != NULL) {
        pts = av_frame_get_best_effort_timestamp(avFrame);
    }

    if (avPacket != NULL) {
        pts = avPacket->pts;
    }


    if (pts == AV_NOPTS_VALUE) {
        pts = 0;
    }
    pts *= av_q2d(time_base);
    if (pts > 0) {
        clock = pts;
    }
    double diff = audio->clock - clock;

    return diff;
}

double Video::getDelayTime(double diff) {

    if (diff > 0.003) {
        delayTime = delayTime * 2 / 3;
        if (delayTime < defaultDelayTime / 2) {
            delayTime = defaultDelayTime * 2 / 3;
        } else if (delayTime > defaultDelayTime * 2) {
            delayTime = defaultDelayTime * 2;
        }
    } else if (diff < -0.003) {
        delayTime = delayTime * 3 / 2;
        if (delayTime < defaultDelayTime / 2) {
            delayTime = defaultDelayTime * 2 / 3;
        } else if (delayTime > defaultDelayTime * 2) {
            delayTime = defaultDelayTime * 2;
        }
    }
    if (diff >= 0.03) {
        delayTime = 0;
    } else if (diff <= -0.03) {
        delayTime = defaultDelayTime * 2;
    }

    if (fabs(diff) >= 5) {
        delayTime = defaultDelayTime;
    }
    return delayTime;
}
