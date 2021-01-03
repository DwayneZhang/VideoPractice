//
// Created by Dwayne on 20/11/24.
//

#ifndef AUDIO_PRACTICE_CALLJAVA_H
#define AUDIO_PRACTICE_CALLJAVA_H

#include "jni.h"
#include <linux/stddef.h>
#include "androidlog.h"

#define MAIN_THREAD 0
#define CHILD_THREAD 1


class CallJava {

public:
    _JavaVM *javaVM = NULL;
    JNIEnv *jniEnv = NULL;
    jobject jobj;

    jmethodID jmid_prepared;
    jmethodID jmid_load;
    jmethodID jmid_timeinfo;
    jmethodID jmid_error;
    jmethodID jmid_complete;
    jmethodID jmid_renderyuv;
    jmethodID jmid_supportvideo;
    jmethodID jmid_initmediacodec;
    jmethodID jmid_decodeavpacket;

public:
    CallJava(JavaVM *vm, JNIEnv *env, jobject *obj);

    ~CallJava();

    void onCallPrepared(int threadType);

    void onCallLoad(int threadType, bool load);

    void onCallTimeInfo(int threadType, int curr, int total);

    void onCallError(int threadType, int code, char *msg);

    void onCallComplete(int threadType);

    void onCallRenderYUV(int threadType, int width, int height, uint8_t *fy,
                         uint8_t *fu, uint8_t *fv);

    bool onCallIsSupportVideo(int threadType, const char *ffcodecname);

    void onCallInitMediaCodec(const char *mime, int width, int height, int csd0_size, int csd1_size,
                        uint8_t *csd_0, uint8_t *csd_1);

    void onCallDecodeAVPacket(int datasize, uint8_t *data);

};


#endif //AUDIO_PRACTICE_CALLJAVA_H
