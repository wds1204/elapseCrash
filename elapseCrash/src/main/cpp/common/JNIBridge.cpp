//
// Created by 吴东生 on 2023/11/16.
//

#include <unwind.h>
#include <dlfcn.h>
#include "JNIBridge.h"
#include <execinfo.h>
#include <cxxabi.h>
#include <dlfcn.h>
#include <unwind.h>
#include <cstdio>

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
    //1.计算Java岑的堆栈信息
    setJavaThreadStackTraces(handlerContext->threadName);
    //2.获取native层的堆栈信息
    setNativeStackTraces(handlerContext);

    //3.调用CrashHandlerListener的onCrash方法
    callJavaCrashMethod(handlerContext->threadName);

}

void JNIBridge::setJavaThreadStackTraces(const char *threadName) {
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

void JNIBridge::setNativeStackTraces(native_handler_context *handlerContext) {
    LOGD("java.long.Error: signal %d (%s) at address ", handlerContext->code,
         xcc_util_get_sigcodename(handlerContext->info));
    int frameSize = handlerContext->frame_Size;
    for (size_t index = 0; index < frameSize; ++index) {
        uintptr_t pc = handlerContext->frames[index];
        Dl_info info;
        const void *addr = (void *const) (pc);
        if (dladdr(addr, &info) != 0 && info.dli_fname != nullptr) {

            // 解码 C++ 函数名
            int status;
            char *demangled = abi::__cxa_demangle(info.dli_sname, 0, 0, &status);
            const auto near = (uintptr_t) (info.dli_saddr);
            const uintptr_t offs = pc - near;
            //偏移量=当前堆栈帧的地址(pc)-共享库的基地址(dli_fbase)，这个偏移量可以用于定位在共享库中的具体位置
            uintptr_t addr_rel = pc - (uintptr_t) info.dli_fbase;
            const uintptr_t addr_to_use = is_dll(info.dli_fname) ? addr_rel : pc;
            if (status == 0) {
                LOGD("native crash #%02zx: pc 0x%016x %s (%s)\n", index, addr_to_use, demangled,
                     info.dli_fname);
                free(demangled);
            } else {
                LOGD("native crash #%02zx: pc 0x%016x %s (%s)\n", index, addr_to_use,
                     info.dli_sname, info.dli_fname);
            }
//
//            LOGD("native crash #%02lx pc 0x%016lx %s (%s+0x%lx)", index, addr_to_use,
//                 info.dli_fname, info.dli_sname, offs);


        }

    }


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




