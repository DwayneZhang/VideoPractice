//
// Created by Dwayne on 20/11/24.
//

#include "CallJava.h"

CallJava::CallJava(JavaVM *vm, JNIEnv *env, jobject *obj) {
    this->javaVM = vm;
    this->jniEnv = env;
    this->jobj = env->NewGlobalRef(*obj);

    jclass jclz = jniEnv->GetObjectClass(jobj);
    if(!jclz) {
        if(LOG_DEBUG) {
            LOGE("get jclass wrong");
        }
        return;
    }

    jmid_prepared = env->GetMethodID(jclz, "onCallPrepare", "()V");
}

CallJava::~CallJava() {
    LOGD("CallJava is destroyed")
}

void CallJava::onCallPrepared(int threadType) {

    if(threadType == MAIN_THREAD) {
        jniEnv->CallVoidMethod(jobj, jmid_prepared);
    } else if(threadType == CHILD_THREAD) {
        JNIEnv *jniEnv;
        if(javaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            if(LOG_DEBUG) {
                LOGE("get child thread jnienv wrong");
            }
            return;
        }
        jniEnv->CallVoidMethod(jobj, jmid_prepared);
        javaVM->DetachCurrentThread();
    }
}
