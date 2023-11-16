#include <jni.h>
#include <string>
#include <android/log.h>
#include "SignalHandler.h"

#define TAG "JNI_TAG"

# define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
# define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

jobject j_obj = nullptr;
jmethodID onCrash_methodId = nullptr;


static  char* getClassName(JNIEnv *env,  jclass anyClass)
{
    // Get the JNIEnv class
    jclass classClass = env->FindClass( "java/lang/Class");
    // Get the getName method ID
    jmethodID getNameMethod = env->GetMethodID(classClass, "getName", "()Ljava/lang/String;");
    // Call the getName method to get the class name
    auto className = (jstring)env->CallObjectMethod( anyClass, getNameMethod);
    // Convert the Java string to a C string
    const char *classNameStr = env->GetStringUTFChars( className, nullptr);
    // Print or use the class name as needed
    printf("Class name: %s\n", classNameStr);
    // Release the C string
    env->ReleaseStringUTFChars( className, classNameStr);

    return const_cast<char *>(classNameStr);
}
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
    jclass errorClass = (*env).FindClass( "java/lang/Error");
    if (errorClass==nullptr){
        LOGE("java/lang/Error not found");
        return;
    }
    jmethodID constructor=env->GetMethodID(errorClass,"<init>","(Ljava/lang/String;)V");
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
    onCrash_methodId = env->GetMethodID(clz, "onCrash",
                                          "(Ljava/lang/String;Ljava/lang/Error;)V");

    if (onCrash_methodId == nullptr) {
        LOGE("ERROR  onCrash not found");
    }


}
extern "C"
JNIEXPORT void JNICALL
Java_com_modi_elapse_elapsecrash_crashMonitor_NativeCrashMonitor_nativeInit(JNIEnv *env,
                                                                            jobject thiz) {
    installSignalHandlers();


}