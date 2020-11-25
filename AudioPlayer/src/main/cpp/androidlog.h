//
// Created by Dwayne on 20/11/22.
//

#ifndef AUDIO_PRACTICE_ANDROIDLOG_H
#define AUDIO_PRACTICE_ANDROIDLOG_H

#endif //AUDIO_PRACTICE_ANDROIDLOG_H

#include "android/log.h"

#define LOG_DEBUG true

#define LOGD(FORMAT,...) __android_log_print(ANDROID_LOG_DEBUG, "TAG", FORMAT, ##__VA_ARGS__);

#define LOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR, "TAG", FORMAT, ##__VA_ARGS__);
