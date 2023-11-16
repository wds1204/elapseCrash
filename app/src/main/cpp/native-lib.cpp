#include <jni.h>
#include <string>

extern "C" JNIEXPORT jstring JNICALL
Java_com_modi_elapse_crash_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_modi_elapse_crash_MainActivity_nativeCrash(JNIEnv *env, jobject thiz) {
    int *num= nullptr;
    *num=100;
}