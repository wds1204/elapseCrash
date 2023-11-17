#include <jni.h>
#include <string>
#include <android/log.h>
#include "SignalHandler.h"
#include "elf.h"
#include "common/el_util.h"
#include "common/JNIBridge.h"
#include "CrashAnalyser.h"

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




extern "C"
JNIEXPORT void JNICALL
Java_com_modi_elapse_elapsecrash_crashMonitor_NativeCrashMonitor_nativeInitCallBack(
        JNIEnv *env,
        jobject nativeCrashMonitor,
        jobject callback) {

    //保持callback回调方法， 通过callback把监听到异常时回调给 java 层
    j_obj = env->NewGlobalRef(callback);
    JavaVM *javaVm;
    env->GetJavaVM(&javaVm);
    jclass nativeCrashMonitorClass = env->FindClass(
            "com/modi/elapse/elapsecrash/crashMonitor/NativeCrashMonitor");
    nativeCrashMonitorClass = (jclass) (env->NewGlobalRef(nativeCrashMonitorClass));
    JNIBridge *jniBridge = new JNIBridge(javaVm, j_obj, nativeCrashMonitorClass);

    //创建一个线程取监听是否有异常
    initCondition();

    pthread_t pthread;
    int ret = pthread_create(&pthread, nullptr, threadCrashMonitor, jniBridge);
    if (ret == -1) {
        LOGD("pthread_create error,ret:%d", ret);
    }

}



extern "C"
JNIEXPORT void JNICALL
Java_com_modi_elapse_elapsecrash_crashMonitor_NativeCrashMonitor_nativeInit(JNIEnv *env,
                                                                            jobject thiz) {
    installSignalHandlers();
    installAlternateStack();


}