//
// Created by 吴东生 on 2023/11/16.
//

#include "JNIBridge.h"

StackTraces *stackTraces;


JNIBridge::JNIBridge(JavaVM *javaVm, jobject callbackObj, jclass nativeCrashMonitorClass) {
    this->javaVm = javaVm;
    this->callbackObj = callbackObj;
    this->nativeCrashMonitorClass = nativeCrashMonitorClass;

}


/**
 *
 * @param handlerContext
 */
void JNIBridge::throwExceptionToJava(native_handler_context *handlerContext) {
    //1.获取Java岑的堆栈信息
    getJavaThreadStackTraces(handlerContext->threadName);
    //2.获取native层的堆栈信息

    //3.调用CrashHandlerListener的onCrash方法
    callJavaCrashMethod(handlerContext->threadName);

}


void JNIBridge::getJavaThreadStackTraces(const char *threadName) {
    // 1. 在非Java线程中获取JNIEnv
    JNIEnv *env = nullptr;
    JavaVM *jvm = this->javaVm;
    stackTraces = static_cast<StackTraces *>(malloc(
            sizeof(StackTraces)));

    if (jvm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
        LOGE("javaVm AttachCurrentThread error");
        return;
    }
    jfieldID staticFieldID = env->GetStaticFieldID(this->nativeCrashMonitorClass,
                                                   INSTANCE_FIELD_NAME,
                                                   INSTANCE_FIELD_NAME_SIGNATURE);
    if (staticFieldID == nullptr) {
        LOGE("staticFieldID INSTANCE error");
        return;
    }
    jobject kotlinObject = env->GetStaticObjectField(this->nativeCrashMonitorClass,
                                                     staticFieldID);
    if (kotlinObject == nullptr) {
        LOGE("GetStaticObjectField kotlinObject error");
        return;
    }
    jmethodID getThreadMethodId = env->GetMethodID(this->nativeCrashMonitorClass,
                                                   GET_THREAD_BY_NAME_METHOD_NAME,
                                                   GET_THREAD_BY_NAME_METHOD_SIGNATURE);
    if (getThreadMethodId == nullptr) {
        LOGE("getThreadByName methodId error");
        return;
    }
    jstring jThreadName = env->NewStringUTF(threadName);

    jobject threadObj = env->CallObjectMethod(kotlinObject,
                                              getThreadMethodId, jThreadName);
    if (threadObj == nullptr) {
        LOGE("getThreadByName obj error");
        return;
    }
    jclass threadClass = env->GetObjectClass(threadObj);
    jmethodID getStackTraceId = env->GetMethodID(threadClass, GET_STACK_TRACE_METHOD_NAME,
                                                 GET_STACK_TRACE_METHOD_SIGNATURE);
    if (getStackTraceId == nullptr) {
        LOGE("%s find error", GET_STACK_TRACE_METHOD_NAME);
        return;

    }
    auto tracesArrayObj = (jobjectArray) env->CallObjectMethod(threadObj,
                                                               getStackTraceId);
    tracesArrayObj = (jobjectArray) env->NewGlobalRef(tracesArrayObj);
    stackTraces->javaTraceObj = tracesArrayObj;


    // 释放引用
    env->DeleteLocalRef(kotlinObject);
    env->DeleteLocalRef(threadClass);
    env->DeleteLocalRef(threadObj);
    env->DeleteLocalRef(jThreadName);
    // 3. 将线程与JVM分离
    jvm->DetachCurrentThread();

}

void JNIBridge::callJavaCrashMethod(const char *threadName) {

    JNIEnv *env = nullptr;
    JavaVM *jvm = this->javaVm;
    if (jvm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
        LOGE("callJavaCrashMethod javaVm AttachCurrentThread error");
        return;
    }
    if (stackTraces == nullptr || stackTraces->javaTraceObj == nullptr) {
        LOGE("jStackTraces error");
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
    jstring thread_Name = env->NewStringUTF(threadName);

    jobject errorObject = env->NewObject(errorClass, constructor);
    jmethodID setTracesId = env->GetMethodID(errorClass, SET_STACK_TRACE_METHOD_NAME,
                                             SET_STACK_TRACE_METHOD_SIGNATURE);
    if (setTracesId == nullptr) {
        LOGE("%s fond error", SET_STACK_TRACE_METHOD_NAME);
        return;
    }
    // 调用 setStackTrace 方法
    env->CallVoidMethod(errorObject, setTracesId, stackTraces->javaTraceObj);

    env->CallVoidMethod(this->callbackObj, onCrash_methodId, thread_Name, errorObject);

//    env->DeleteLocalRef(javaErrorMsg);
    env->DeleteLocalRef(errorClass);
    env->DeleteLocalRef(thread_Name);
    env->DeleteLocalRef(errorObject);
    env->DeleteLocalRef(crashHandlerListenerClass);
    delete stackTraces;
    // 3. 将线程与JVM分离
    jvm->DetachCurrentThread();

}




