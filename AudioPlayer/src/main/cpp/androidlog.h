//
// Created by Dwayne on 20/11/22.
//

#ifndef JNI_THREAD_ANDROIDLOG_H
#define JNI_THREAD_ANDROIDLOG_H

#endif //JNI_THREAD_ANDROIDLOG_H

#include "android/log.h"

#define LOG_DEBUG true

#define LOGD(FORMAT,...) __android_log_print(ANDROID_LOG_DEBUG, "TAG", FORMAT, ##__VA_ARGS__);

#define LOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR, "TAG", FORMAT, ##__VA_ARGS__);
