//
// Created by 吴东生 on 2023/11/16.
//

#ifndef ELAPSECRASH_JNIBRIDGE_H
#define ELAPSECRASH_JNIBRIDGE_H
#include <jni.h>
#include <android/log.h>
#include "HandlerContext.h"


#define TAG "JNI_TAG"

# define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
# define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

class JNIBridge {
private:
    JavaVM *javaVm;
    jobject callbackObj;

public:
    JNIBridge( JavaVM *javaVm,jobject callbackObj);

public:
    void throwExceptionToJava(native_handler_context *handlerContext);

};


#endif //ELAPSECRASH_JNIBRIDGE_H
