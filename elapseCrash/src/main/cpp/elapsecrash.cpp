#include <jni.h>
#include <string>
#include <android/log.h>
#include "SignalHandler.h"
#include "elf.h"
#include "common/el_util.h"

#define TAG "JNI_TAG"

# define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
# define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

# define EL_ON_CRASH_METHOD_NAME         "onCrash"
# define EL_ON_CRASH_METHOD_SIGNATURE    "(Ljava/lang/String;Ljava/lang/Error;)V"


# define EL_ERROR_CLASS                  "java/lang/Error"
# define EL_CONSTRUCTOR_METHOD           "<init>"
# define EL_CONSTRUCTOR_METHOD_SIGNATURE "(Ljava/lang/String;)V"


jobject j_obj = nullptr;
jmethodID onCrash_methodId = nullptr;



/**
 * 触发Java层的 CrashHandlerListener#onCrash方法
 * @param env
 * @param threadName
 * @param errorMsg
 */
static void callJavaCrashMethod(JNIEnv *env,const char* threadName,const char* errorMsg){
    if (j_obj== nullptr||onCrash_methodId== nullptr){
        return;
    }
    jclass errorClass = (*env).FindClass( EL_ERROR_CLASS);
    if (errorClass==nullptr){
        LOGE("java/lang/Error not found");
        return;
    }
    jmethodID constructor=env->GetMethodID(errorClass,EL_CONSTRUCTOR_METHOD,EL_CONSTRUCTOR_METHOD_SIGNATURE);
    if (constructor== nullptr){
        const char *className=getClassName(env,errorClass);
        LOGE("Class %s constructor not found",className);
        return;
    }
    jstring javaErrorMsg=env->NewStringUTF(errorMsg);
    jstring thread_Name=env->NewStringUTF(threadName);

    if (javaErrorMsg== nullptr){
        return;
    }
    //create an instance of Error with the custom error message
    jobject  errorObject=env->NewObject(errorClass,constructor,javaErrorMsg);

    env->CallVoidMethod(j_obj,onCrash_methodId,thread_Name,errorObject);

    env->DeleteLocalRef(javaErrorMsg);
    env->DeleteLocalRef(thread_Name);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_modi_elapse_elapsecrash_crashMonitor_NativeCrashMonitor_nativeInitCallBack(
        JNIEnv *env,
        jobject thiz,
        jobject callback) {
    //保持callback回调方法， 通过callback把监听到异常时回调给 java 层
    j_obj = env->NewGlobalRef(callback);

    jclass clz = env->GetObjectClass(callback);
    if (clz == nullptr) {
        const char *className= getClassName(env,clz );
        LOGE("Class %s not found",className);
    }
    onCrash_methodId = env->GetMethodID(clz, EL_ON_CRASH_METHOD_NAME,
                                          EL_ON_CRASH_METHOD_SIGNATURE);

    if (onCrash_methodId == nullptr) {
        LOGE("ERROR  onCrash not found");
    }


}
extern "C"
JNIEXPORT void JNICALL
Java_com_modi_elapse_elapsecrash_crashMonitor_NativeCrashMonitor_nativeInit(JNIEnv *env,
                                                                            jobject thiz) {
    installSignalHandlers();
    installAlternateStack();


}