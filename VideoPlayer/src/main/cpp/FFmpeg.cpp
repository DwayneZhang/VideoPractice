//
// Created by Dwayne on 20/11/24.
//

#include "FFmpeg.h"

FFmpeg::FFmpeg(PlayStatus *playStatus, CallJava *callJava, const char *url) {
    this->callJava = callJava;
    this->url = url;
    this->playStatus = playStatus;
    pthread_mutex_init(&init_mutex, NULL);
    pthread_mutex_init(&seek_mutex, NULL);
}

FFmpeg::~FFmpeg() {
    pthread_mutex_destroy(&init_mutex);
    pthread_mutex_destroy(&seek_mutex);
}

void *decodeFFmpeg(void *data) {
    FFmpeg *ffmpeg = (FFmpeg *) data;
    ffmpeg->decodeFFmpegThread();
    pthread_exit(&ffmpeg->decodeThread);
}

void FFmpeg::perpare() {
    //创建子线程执行
    pthread_create(&decodeThread, NULL, decodeFFmpeg, this);
}

int avformat_callback(void *ctx) {
    FFmpeg *ffmpeg = (FFmpeg *) ctx;
    if (ffmpeg->playStatus->exit) {
        return AVERROR_EOF;
    }
    return 0;
}

/**
 * 解码
 */
void FFmpeg::decodeFFmpegThread() {
    pthread_mutex_lock(&init_mutex);
    //注册解码器并初始化网络
    av_register_all();
    avformat_network_init();

    //打开文件或网络流
    pFormatCtx = avformat_alloc_context();
    pFormatCtx->interrupt_callback.callback = avformat_callback;
    pFormatCtx->interrupt_callback.opaque = this;
    if (avformat_open_input(&pFormatCtx, url, NULL, NULL) != 0) {
        if (LOG_DEBUG) {
            LOGE("can not open url");
        }
        callJava->onCallError(CHILD_THREAD, 1001, "can not open url");
        exit = true;
        pthread_mutex_unlock(&init_mutex);
        return;
    }

    //获取信息流
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        if (LOG_DEBUG) {
            LOGE("can not find streams form url");
        }
        callJava->onCallError(CHILD_THREAD, 1002, "can not find streams form url");
        exit = true;
        pthread_mutex_unlock(&init_mutex);
        return;
    }
    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        //获取音频流
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (audio == NULL) {
                audio = new Audio(playStatus,
                                  pFormatCtx->streams[i]->codecpar->sample_rate,
                                  callJava);
                audio->streamIndex = i;
                audio->codecpar = pFormatCtx->streams[i]->codecpar;
                audio->duration = pFormatCtx->duration / AV_TIME_BASE;
                audio->time_base = pFormatCtx->streams[i]->time_base;
                duration = audio->duration;
            }
        } else if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            if (video == NULL) {
                video = new Video(playStatus, callJava);
                video->streamIndex = i;
                video->avCodecPar = pFormatCtx->streams[i]->codecpar;
                video->time_base = pFormatCtx->streams[i]->time_base;

                int num = pFormatCtx->streams[i]->avg_frame_rate.num;
                int den = pFormatCtx->streams[i]->avg_frame_rate.den;
                if (num != 0 && den != 0) {
                    int fps = num / den;
                    video->defaultDelayTime = 1.0 / fps;
                }
            }
        }
    }

    if (audio != NULL) {
        getCodecContext(audio->codecpar, &audio->avCodecContext);
    }
    if (video != NULL) {
        getCodecContext(video->avCodecPar, &video->avCodecContext);
    }

    if (callJava != NULL) {
        if (playStatus != NULL && !playStatus->exit) {
            callJava->onCallPrepared(CHILD_THREAD);
        } else {
            exit = true;
        }
    }
    pthread_mutex_unlock(&init_mutex);

}

void FFmpeg::start() {

    if (audio == NULL) {
        if (LOG_DEBUG) {
            LOGE("audio is null");
        }
        return;
    }

    if (video == NULL) {
        if (LOG_DEBUG) {
            LOGE("video is null");
        }
        return;
    }
    video->audio = audio;

    const char *codecName = ((const AVCodec*)video->avCodecContext->codec)->name;
    supportMediaCodec = callJava->onCallIsSupportVideo(CHILD_THREAD, codecName);
    if (supportMediaCodec) {
        video->codectype = CODEC_MEDIACODEC;
    } else {
        video->codectype = CODEC_YUV;
    }

    audio->play();
    video->play();

    //解码音视频流
    while (playStatus != NULL && !playStatus->exit) {
        if (playStatus->seek) {
            av_usleep(1000 * 100);
            continue;
        }
        if (audio->queue->getQueueSize() > 40) {
            av_usleep(1000 * 100);
            continue;
        }

        if (video->queue->getQueueSize() > 40) {
            av_usleep(1000 * 100);
            continue;
        }
        AVPacket *avPacket = av_packet_alloc();
        pthread_mutex_lock(&seek_mutex);
        int ret = av_read_frame(pFormatCtx, avPacket);
        pthread_mutex_unlock(&seek_mutex);
        if (ret == 0) {
            if (avPacket->stream_index == audio->streamIndex) {
                audio->queue->putAvPacket(avPacket);
            } else if (avPacket->stream_index == video->streamIndex) {
                video->queue->putAvPacket(avPacket);
            } else {
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;
            }
        } else {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            while (playStatus != NULL && !playStatus->exit) {
                if (audio->queue->getQueueSize() > 0) {
                    av_usleep(1000 * 100);
                    continue;
                } else {
                    if (!playStatus->seek) {
                        av_usleep(1000 * 500);
                        playStatus->exit = true;
                    }
                    break;
                }
            }
            break;
        }
    }

    if (callJava != NULL) {
        callJava->onCallComplete(CHILD_THREAD);
    }
    exit = true;
}

void FFmpeg::pause() {
    if (audio != NULL) {
        audio->pause();
    }
}

void FFmpeg::resume() {
    if (audio != NULL) {
        audio->resume();
    }
}

void FFmpeg::release() {
    playStatus->exit = true;
    pthread_mutex_lock(&init_mutex);
    int sleepCount = 0;
    while (!exit) {
        if (sleepCount > 1000) {
            exit = true;
        }
        if (LOG_DEBUG) {
            LOGE("wait ffmpeg exit %d", sleepCount);
        }
        sleepCount++;
        av_usleep(1000 * 10);
    }

    if (audio != NULL) {
        audio->release();
        delete audio;
        audio = NULL;
    }

    if (video != NULL) {
        video->release();
        delete video;
        video = NULL;
    }

    if (pFormatCtx != NULL) {
        avformat_close_input(&pFormatCtx);
        avformat_free_context(pFormatCtx);
        pFormatCtx = NULL;
    }

    if (playStatus != NULL) {
        playStatus = NULL;
    }

    if (callJava != NULL) {
        callJava = NULL;
    }

    pthread_mutex_unlock(&init_mutex);
}

void FFmpeg::seek(int64_t secds) {

    if (duration <= 0) {
        return;
    }
    if (secds >= 0 && secds <= duration) {
        playStatus->seek = true;
        pthread_mutex_lock(&seek_mutex);
        int64_t rel = secds * AV_TIME_BASE;
        avformat_seek_file(pFormatCtx, -1, INT64_MIN, rel, INT64_MAX, 0);
        if (audio != NULL) {
            audio->queue->clearAVPacket();
            audio->clock = 0;
            audio->last_time = 0;
            pthread_mutex_lock(&audio->codecMutex);
            avcodec_flush_buffers(audio->avCodecContext);
            pthread_mutex_unlock(&audio->codecMutex);
        }

        if (video != NULL) {
            video->queue->clearAVPacket();
            video->clock = 0;
            pthread_mutex_lock(&video->codecMutex);
            avcodec_flush_buffers(video->avCodecContext);
            pthread_mutex_unlock(&video->codecMutex);
        }
        pthread_mutex_unlock(&seek_mutex);
        playStatus->seek = false;
    }
}

int
FFmpeg::getCodecContext(AVCodecParameters *codecpar, AVCodecContext **avCodecContext) {
    //获取解码器
    AVCodec *dec = avcodec_find_decoder(codecpar->codec_id);
    if (!dec) {
        if (LOG_DEBUG) {
            LOGE("can not find decoder");
        }
        callJava->onCallError(CHILD_THREAD, 1003, "can not find decoder");
        exit = true;
        pthread_mutex_unlock(&init_mutex);
        return -1;
    }

    //利用解码器创建解码器上下文
    *avCodecContext = avcodec_alloc_context3(dec);
    if (!audio->avCodecContext) {
        if (LOG_DEBUG) {
            LOGE("can not find decoderCtx");
        }
        callJava->onCallError(CHILD_THREAD, 1004, "can not find decoderCtx");
        exit = true;
        pthread_mutex_unlock(&init_mutex);
        return -1;
    }
    if (avcodec_parameters_to_context(*avCodecContext, codecpar) < 0) {
        if (LOG_DEBUG) {
            LOGE("can not fill decoderCtx");
        }
        callJava->onCallError(CHILD_THREAD, 1005, "can not fill decoderCtx");
        exit = true;
        pthread_mutex_unlock(&init_mutex);
        return -1;
    }

    //打开解码器
    if (avcodec_open2(*avCodecContext, dec, 0) != 0) {
        if (LOG_DEBUG) {
            LOGE("can not open stream");
        }
        callJava->onCallError(CHILD_THREAD, 1006, "can not open stream");
        exit = true;
        pthread_mutex_unlock(&init_mutex);
        return -1;
    }
    return 0;
}
