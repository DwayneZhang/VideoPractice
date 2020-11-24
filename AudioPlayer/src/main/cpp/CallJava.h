//
// Created by Dwayne on 20/11/24.
//

#ifndef AUDIO_PRACTICE_CALLJAVA_H
#define AUDIO_PRACTICE_CALLJAVA_H

#include "jni.h"
#include "androidlog.h"

#define MAIN_THREAD 0
#define CHILD_THREAD 1


class CallJava {

public:
    _JavaVM *javaVM = NULL;
    JNIEnv *jniEnv = NULL;
    jobject jobj;

    jmethodID  jmid_prepared;

public:
    CallJava(JavaVM *vm, JNIEnv *env, jobject *obj);
    ~CallJava();

    void onCallPrepared(int threadType);

};


#endif //AUDIO_PRACTICE_CALLJAVA_H
