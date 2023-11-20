//
// Created by 吴东生 on 2023/11/16.
//

#ifndef ELAPSECRASH_JNIBRIDGE_H
#define ELAPSECRASH_JNIBRIDGE_H

#include <jni.h>
#include <android/log.h>
#include "HandlerContext.h"
#include "el_util.h"
# include <cstdint>
#include <iostream>


#define TAG "JNI_TAG"

# define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
# define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)



class JNIBridge {
private:

    jobject callbackObj;
    jclass nativeCrashMonitorClass;

public:
    JNIBridge(JavaVM *javaVm, jobject callbackObj, jclass nativeCrashMonitorClass);

    JavaVM *javaVm;
public:
    void throwExceptionToJava(native_handler_context *handlerContext);

    void setJavaThreadStackTraces(native_handler_context *handlerContext);

    void callJavaCrashMethod(native_handler_context *handlerContext);

    void setNativeStackTraces(native_handler_context *handlerContext);
};


#endif //ELAPSECRASH_JNIBRIDGE_H
