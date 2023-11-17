//
// Created by 吴东生 on 2023/11/16.
//

#include "JNIBridge.h"

JNIBridge::JNIBridge(JavaVM *javaVm, jobject callbackObj, jclass nativeCrashMonitorClass) {
    this->javaVm = javaVm;
    this->callbackObj = callbackObj;
    this->nativeCrashMonitorClass = nativeCrashMonitorClass;

}


const char *JNIBridge::getJavaThreadStackTraces(const char *threadName) {
    // 1. 在非Java线程中获取JNIEnv
    JNIEnv *env = nullptr;
    JavaVM *jvm = this->javaVm;

    if (jvm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
        LOGE("javaVm AttachCurrentThread error");
        return nullptr;
    }
    jfieldID staticFieldID = env->GetStaticFieldID(this->nativeCrashMonitorClass,
                                                   INSTANCE_FIELD_NAME,
                                                   INSTANCE_FIELD_NAME_SIGNATURE);
    if (staticFieldID == nullptr) {
        LOGE("staticFieldID INSTANCE error");
        return nullptr;
    }

    jobject kotlinObject = env->GetStaticObjectField(this->nativeCrashMonitorClass,
                                                     staticFieldID);
    if (kotlinObject == nullptr) {
        LOGE("GetStaticObjectField kotlinObject error");
        return nullptr;
    }
    jmethodID getThreadTracesMethodId = env->GetMethodID(this->nativeCrashMonitorClass,
                                                         GET_THREAD_STACK_TRACES_METHOD_NAME,
                                                         GET_THREAD_STACK_TRACES_METHOD_NAME_SIGNATURE);
    if (getThreadTracesMethodId == nullptr) {
        LOGE("getThreadStackTraces methodId error");
        return nullptr;
    }
    jstring jThreadName = env->NewStringUTF(threadName);

    auto jTraces = (jstring) env->CallObjectMethod(kotlinObject,
                                                   getThreadTracesMethodId, jThreadName);
    const char *javaTraces = env->GetStringUTFChars(jTraces, nullptr);

    LOGD("JavaTraces--%s", javaTraces);

    // 释放引用
    env->DeleteLocalRef(kotlinObject);
    // 3. 将线程与JVM分离
    jvm->DetachCurrentThread();
    return javaTraces;
}

void JNIBridge::callJavaCrashMethod(const char *threadName, const char *errorMsg) {
    JNIEnv *env = nullptr;
    JavaVM *jvm = this->javaVm;
    if (jvm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
        LOGE("callJavaCrashMethod javaVm AttachCurrentThread error");
        return;
    }

    jclass crashHandlerListenerClass = env->GetObjectClass(this->callbackObj);
    if (crashHandlerListenerClass == nullptr) {
        const char *className = getClassName(env, crashHandlerListenerClass);
        LOGE("Class %s not found", className);
    }
    jmethodID onCrash_methodId = env->GetMethodID(crashHandlerListenerClass,
                                                  EL_ON_CRASH_METHOD_NAME,
                                                  EL_ON_CRASH_METHOD_SIGNATURE);
    if (onCrash_methodId == nullptr) {
        LOGE("ERROR  onCrash not found");
    }

    jclass errorClass = (*env).FindClass(EL_ERROR_CLASS);
    if (errorClass == nullptr) {
        LOGE("java/lang/Error not found");
        return;
    }
    jmethodID constructor = env->GetMethodID(errorClass, EL_CONSTRUCTOR_METHOD,
                                             EL_CONSTRUCTOR_METHOD_SIGNATURE);
    if (constructor == nullptr) {
        const char *className = getClassName(env, errorClass);
        LOGE("Class %s constructor not found", className);
        return;
    }
    jstring javaErrorMsg = env->NewStringUTF(errorMsg);
    jstring thread_Name = env->NewStringUTF(threadName);

    if (javaErrorMsg == nullptr) {
        return;
    }
    //create an instance of Error with the custom error message
    jobject errorObject = env->NewObject(errorClass, constructor, javaErrorMsg);

    env->CallVoidMethod(this->callbackObj, onCrash_methodId, thread_Name, errorObject);

    env->DeleteLocalRef(javaErrorMsg);
    env->DeleteLocalRef(thread_Name);
    env->DeleteLocalRef(crashHandlerListenerClass);

    // 3. 将线程与JVM分离
    jvm->DetachCurrentThread();

}

/**
 *
 * @param handlerContext
 */
void JNIBridge::throwExceptionToJava(native_handler_context *handlerContext) {
    //java的线程名
    const char *javaTraces = getJavaThreadStackTraces(handlerContext->threadName);
    LOGD("throwExceptionToJava JavaTraces--%s", javaTraces);
    callJavaCrashMethod(handlerContext->threadName, javaTraces);

}


