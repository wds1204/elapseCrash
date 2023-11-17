//
// Created by 吴东生 on 2023/11/16.
//

#ifndef ELAPSECRASH_JNIBRIDGE_H
#define ELAPSECRASH_JNIBRIDGE_H

#include <jni.h>
#include <android/log.h>
#include "HandlerContext.h"
#include "el_util.h"


#define TAG "JNI_TAG"

# define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
# define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)


typedef struct Stack_Traces {
    jobjectArray javaTraceObj;
} StackTraces;

class JNIBridge {
private:

    JavaVM *javaVm;
    jobject callbackObj;
    jclass nativeCrashMonitorClass;

public:
    JNIBridge(JavaVM *javaVm, jobject callbackObj, jclass nativeCrashMonitorClass);

public:
    void throwExceptionToJava(native_handler_context *handlerContext);

    void getJavaThreadStackTraces(const char *threadName);

    void callJavaCrashMethod(const char *threadName);

};


#endif //ELAPSECRASH_JNIBRIDGE_H
