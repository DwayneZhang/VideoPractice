//
// Created by Dwayne on 20/11/24.
//

#include "CallJava.h"

CallJava::CallJava(JavaVM *vm, JNIEnv *env, jobject *obj) {
    this->javaVM = vm;
    this->jniEnv = env;
    this->jobj = env->NewGlobalRef(*obj);

    jclass jclz = jniEnv->GetObjectClass(jobj);
    if (!jclz) {
        if (LOG_DEBUG) {
            LOGE("get jclass wrong");
        }
        return;
    }

    jmid_prepared = env->GetMethodID(jclz, "onCallPrepare", "()V");
    jmid_load = env->GetMethodID(jclz, "onCallLoad", "(Z)V");
    jmid_timeinfo = env->GetMethodID(jclz, "onCallTimeInfo", "(II)V");
    jmid_error = env->GetMethodID(jclz, "onCallError", "(ILjava/lang/String;)V");
    jmid_complete = env->GetMethodID(jclz, "onCallComplete", "()V");
    jmid_renderyuv = env->GetMethodID(jclz, "onCallRenderYUV", "(II[B[B[B)V");
    jmid_supportvideo = env->GetMethodID(jclz, "onCallSupportMediaCodec",
                                         "(Ljava/lang/String;)Z");
}

CallJava::~CallJava() {
}

void CallJava::onCallPrepared(int threadType) {

    if (threadType == MAIN_THREAD) {
        jniEnv->CallVoidMethod(jobj, jmid_prepared);
    } else if (threadType == CHILD_THREAD) {
        JNIEnv *jniEnv;
        if (javaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv wrong");
            }
            return;
        }
        jniEnv->CallVoidMethod(jobj, jmid_prepared);
        javaVM->DetachCurrentThread();
    }
}

void CallJava::onCallLoad(int threadType, bool load) {
    if (threadType == MAIN_THREAD) {
        jniEnv->CallVoidMethod(jobj, jmid_load, load);
    } else if (threadType == CHILD_THREAD) {
        JNIEnv *jniEnv;
        if (javaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv wrong");
            }
            return;
        }
        jniEnv->CallVoidMethod(jobj, jmid_load, load);
        javaVM->DetachCurrentThread();
    }
}

void CallJava::onCallTimeInfo(int threadType, int curr, int total) {
    if (threadType == MAIN_THREAD) {
        jniEnv->CallVoidMethod(jobj, jmid_timeinfo, curr, total);
    } else if (threadType == CHILD_THREAD) {
        JNIEnv *jniEnv;
        if (javaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv wrong");
            }
            return;
        }
        jniEnv->CallVoidMethod(jobj, jmid_timeinfo, curr, total);
        javaVM->DetachCurrentThread();
    }
}

void CallJava::onCallError(int threadType, int code, char *msg) {
    if (threadType == MAIN_THREAD) {
        jstring jmsg = jniEnv->NewStringUTF(msg);
        jniEnv->CallVoidMethod(jobj, jmid_error, code, jmsg);
        jniEnv->DeleteLocalRef(jmsg);
    } else if (threadType == CHILD_THREAD) {
        JNIEnv *jniEnv;
        if (javaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv wrong");
            }
            return;
        }
        jstring jmsg = jniEnv->NewStringUTF(msg);
        jniEnv->CallVoidMethod(jobj, jmid_error, code, jmsg);
        jniEnv->DeleteLocalRef(jmsg);
        javaVM->DetachCurrentThread();
    }
}

void CallJava::onCallComplete(int threadType) {
    if (threadType == MAIN_THREAD) {
        jniEnv->CallVoidMethod(jobj, jmid_complete);
    } else if (threadType == CHILD_THREAD) {
        JNIEnv *jniEnv;
        if (javaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv wrong");
            }
            return;
        }
        jniEnv->CallVoidMethod(jobj, jmid_complete);
        javaVM->DetachCurrentThread();
    }
}

void
CallJava::onCallRenderYUV(int threadType, int width, int height, uint8_t *fy, uint8_t *fu,
                          uint8_t *fv) {
    if (threadType == MAIN_THREAD) {
        jbyteArray y = jniEnv->NewByteArray(width * height);
        jniEnv->SetByteArrayRegion(y, 0, width * height, (jbyte *) fy);

        jbyteArray u = jniEnv->NewByteArray(width * height / 4);
        jniEnv->SetByteArrayRegion(u, 0, width * height / 4, (jbyte *) fu);

        jbyteArray v = jniEnv->NewByteArray(width * height / 4);
        jniEnv->SetByteArrayRegion(v, 0, width * height / 4, (jbyte *) fv);

        jniEnv->CallVoidMethod(jobj, jmid_renderyuv, width, height, y, u, v);
    } else if (threadType == CHILD_THREAD) {
        JNIEnv *jniEnv;
        if (javaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv wrong");
            }
            return;
        }
        jbyteArray y = jniEnv->NewByteArray(width * height);
        jniEnv->SetByteArrayRegion(y, 0, width * height, (jbyte *) fy);

        jbyteArray u = jniEnv->NewByteArray(width * height / 4);
        jniEnv->SetByteArrayRegion(u, 0, width * height / 4, (jbyte *) fu);

        jbyteArray v = jniEnv->NewByteArray(width * height / 4);
        jniEnv->SetByteArrayRegion(v, 0, width * height / 4, (jbyte *) fv);

        jniEnv->CallVoidMethod(jobj, jmid_renderyuv, width, height, y, u, v);
        jniEnv->DeleteLocalRef(y);
        jniEnv->DeleteLocalRef(u);
        jniEnv->DeleteLocalRef(v);
        javaVM->DetachCurrentThread();
    }
}

bool CallJava::onCallIsSupportVideo(int threadType, const char *ffcodecname) {
    bool support = false;
    if (threadType == MAIN_THREAD) {
        jstring type = jniEnv->NewStringUTF(ffcodecname);
        support = jniEnv->CallBooleanMethod(jobj, jmid_supportvideo, type);
        jniEnv->DeleteLocalRef(type);
        javaVM->DetachCurrentThread();
    } else if (threadType == CHILD_THREAD) {
        JNIEnv *jniEnv;
        if (javaVM->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv wrong");
            }
            return support;
        }
        jstring type = jniEnv->NewStringUTF(ffcodecname);
        support = jniEnv->CallBooleanMethod(jobj, jmid_supportvideo, type);
        jniEnv->DeleteLocalRef(type);
        javaVM->DetachCurrentThread();
    }
    return support;
}


