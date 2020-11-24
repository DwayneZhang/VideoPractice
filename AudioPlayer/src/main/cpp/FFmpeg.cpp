//
// Created by Dwayne on 20/11/24.
//

#include "FFmpeg.h"

FFmpeg::FFmpeg(CallJava *callJava, const char *url) {
    this->callJava = callJava;
    this->url = url;
}

FFmpeg::~FFmpeg() {

}

void *decodeFFmpeg(void *data) {
    FFmpeg *ffmpeg = (FFmpeg *) (data);
    ffmpeg->decodeFFmpegThread();
    pthread_exit(&ffmpeg->decodeThread);
}

void FFmpeg::perpare() {
    //创建子线程执行
    pthread_create(&decodeThread, NULL, decodeFFmpeg, this);
}

/**
 * 解码音频
 */
void FFmpeg::decodeFFmpegThread() {

    //注册解码器并初始化网络
    av_register_all();
    avformat_network_init();

    //打开文件或网络流
    pFormatCtx = avformat_alloc_context();
    if(avformat_open_input(&pFormatCtx, url, NULL, NULL) != 0) {
        if(LOG_DEBUG) {
            LOGE("can not open url");
        }
        return;
    }

    //获取信息流
    if(avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        if(LOG_DEBUG) {
            LOGE("can not find streams form url");
        }
        return;
    }
    for(int i = 0; i< pFormatCtx->nb_streams; i++) {
        //获取音频流
        if(pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            if(audio == NULL) {
                audio = new Audio();
                audio->streamIndex = i;
                audio->codecpar =  pFormatCtx->streams[i]->codecpar;
            }
        }
    }

    //获取解码器
    AVCodec  *dec = avcodec_find_decoder(audio->codecpar->codec_id);
    if(!dec) {
        if(LOG_DEBUG) {
            LOGE("can not find decoder");
        }
        return;
    }

    //利用解码器创建解码器上下文
    audio->avCodecContext = avcodec_alloc_context3(dec);
    if(!audio->avCodecContext) {
        if(LOG_DEBUG) {
            LOGE("can not find decoderCtx");
        }
        return;
    }
    if(avcodec_parameters_to_context(audio->avCodecContext, audio->codecpar) < 0) {
        if(LOG_DEBUG) {
            LOGE("can not fill decoderCtx");
        }
        return;
    }

    //打开解码器
    if(avcodec_open2(audio->avCodecContext, dec, 0) != 0) {
        if(LOG_DEBUG) {
            LOGE("can not open stream");
        }
        return;
    }

    callJava->onCallPrepared(CHILD_THREAD);


}
