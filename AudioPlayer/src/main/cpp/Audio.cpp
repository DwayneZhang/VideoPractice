//
// Created by Dwayne on 20/11/24.
//

#include "Audio.h"

Audio::Audio(PlayStatus *playStatus) {
    this->playStatus = playStatus;
    queue = new Queue(playStatus);
    buffer = (uint8_t *) av_malloc(44100 * 2 * 2);
}

Audio::~Audio() {

}

void *decodPlay(void *data) {
    Audio *audio = (Audio *) data;
    audio->resampleAudio();
    pthread_exit(&audio->thread_play);
}

void Audio::play() {
    pthread_create(&thread_play, NULL, decodPlay, this);
}

int Audio::resampleAudio() {

    while (playStatus != NULL && !playStatus->exit) {
        avPacket = av_packet_alloc();
        if (queue->getAvPacket(avPacket) != 0) {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }
        ret = avcodec_send_packet(avCodecContext, avPacket);
        if (ret != 0) {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }
        avFrame = av_frame_alloc();
        ret = avcodec_receive_frame(avCodecContext, avFrame);
        if (ret == 0) {
            if (avFrame->channels > 0 && avFrame->channel_layout == 0) {
                avFrame->channel_layout = av_get_default_channel_layout(
                        avFrame->channels);
            } else if (avFrame->channels == 0 && avFrame->channel_layout > 0) {
                avFrame->channels = av_get_channel_layout_nb_channels(
                        avFrame->channel_layout);
            }
            SwrContext *swr_ctx;
            LOGE("avFrame->channel_layout :%d", avFrame->channel_layout);
            LOGE("avFrame->channels :%d", avFrame->channels);
            LOGE("avFrame->sample_rate :%d", avFrame->sample_rate);
            swr_ctx = swr_alloc_set_opts(
                    NULL,
                    AV_CH_LAYOUT_STEREO,
                    AV_SAMPLE_FMT_S16,
                    avFrame->sample_rate,
                    avFrame->channel_layout,
                    (AVSampleFormat) avFrame->format,
                    avFrame->sample_rate,
                    NULL, NULL);
            ret = swr_init(swr_ctx);
            LOGE("swr_init error code :%d", ret);
            if (!swr_ctx || ret < 0) {
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;

                av_frame_free(&avFrame);
                av_free(avFrame);
                avFrame = NULL;
                if (swr_ctx != NULL) {
                    swr_free(&swr_ctx);
                    swr_ctx = NULL;
                }
                continue;
            }

            int nb = swr_convert(
                    swr_ctx,
                    &buffer,
                    avFrame->nb_samples,
                    (const uint8_t **)avFrame->data,
                    avFrame->nb_samples);

            int out_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
            data_size = nb * out_channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
            LOGE("data size is %d", data_size);

            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;

            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;
            swr_free(&swr_ctx);
            swr_ctx = NULL;

        } else {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;

            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;
            continue;
        }
    }

    return 0;
}
