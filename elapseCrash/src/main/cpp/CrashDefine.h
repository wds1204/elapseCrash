//
// Created by 吴东生 on 2023/11/16.
//

#ifndef ELAPSECRASH_CRASHDEFINE_H
#define ELAPSECRASH_CRASHDEFINE_H

#include <android/log.h>
#include <signal.h>

#define TAG "JNI_TAG"
# define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
# define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
// 异常信号量
const int exceptionSignals[] = {SIGSEGV, SIGABRT, SIGFPE, SIGILL, SIGBUS, SIGTRAP};
const int exceptionSignalsNumber = sizeof(exceptionSignals)/ sizeof(exceptionSignals[0]);
static struct sigaction oldHandlers[NSIG];

#endif //ELAPSECRASH_CRASHDEFINE_H
