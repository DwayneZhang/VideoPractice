#include <jni.h>
#include <string>
#include "androidlog.h"
#include "CallJava.h"
#include "FFmpeg.h"

extern "C"{
#include "libavformat/avformat.h"
}

JavaVM *javaVM = NULL;
CallJava *callJava = NULL;
FFmpeg *ffmpeg = NULL;
PlayStatus *playStatus = NULL;

extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    jint result = -1;
    javaVM = vm;
    JNIEnv *env;
    if(vm->GetEnv((void **)(&env), JNI_VERSION_1_4) != JNI_OK) {
        return result;
    }
    return JNI_VERSION_1_4;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_dwayne_com_audioplayer_player_AudioPlayer_n_1prepare(JNIEnv *env, jobject thiz, jstring source) {

    const char *url = env->GetStringUTFChars(source, 0);

    if(ffmpeg == NULL) {
        if(callJava == NULL) {
            callJava = new CallJava(javaVM, env, &thiz);
        }
        playStatus = new PlayStatus;
        ffmpeg = new FFmpeg(playStatus, callJava, url);
        ffmpeg->perpare();
    }

//    env->ReleaseStringUTFChars(source, url);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_dwayne_com_audioplayer_player_AudioPlayer_n_1start(JNIEnv *env, jobject thiz) {
    if(ffmpeg != NULL) {
        ffmpeg->start();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_dwayne_com_audioplayer_player_AudioPlayer_n_1pause(JNIEnv *env, jobject thiz) {
    if(ffmpeg != NULL) {
        ffmpeg->pause();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_dwayne_com_audioplayer_player_AudioPlayer_n_1resume(JNIEnv *env, jobject thiz) {
    if(ffmpeg != NULL) {
        ffmpeg->resume();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_dwayne_com_audioplayer_player_AudioPlayer_n_1stop(JNIEnv *env, jobject thiz) {
    if(ffmpeg != NULL) {
        ffmpeg->release();
        delete (ffmpeg);
        ffmpeg = NULL;
        if (callJava != NULL) {
            delete (callJava);
            callJava = NULL;
        }
        if (playStatus != NULL) {
            delete (playStatus);
            playStatus = NULL;
        }
    }
}