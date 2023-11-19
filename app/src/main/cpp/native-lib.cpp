#include <jni.h>
#include <string>

extern "C" JNIEXPORT jstring JNICALL
Java_com_modi_elapse_crash_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";


    return env->NewStringUTF(hello.c_str());
}

void crash() {
    int *num = nullptr;
    *num = 100;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_modi_elapse_crash_MainActivity_nativeCrash(JNIEnv *env, jobject thiz) {
    crash();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_modi_elapse_crash_MainActivity_setCallBack(JNIEnv *env, jobject thiz, jobject call_back) {


    jclass callBackClass = env->GetObjectClass(call_back);
    jmethodID callMethodId = env->GetMethodID(callBackClass, "call",
                                              "(Lcom/modi/elapse/crash/UserManager;)V");
    if (callMethodId == nullptr)return;
    jclass u_class = env->FindClass("com/modi/elapse/crash/User");
    if (u_class == nullptr) {
        return;
    }
    jmethodID jmethodId = env->GetStaticMethodID(u_class, "getUser",
                                                 "()Lcom/modi/elapse/crash/User;");
    if (jmethodId == nullptr) {
        return;
    }
    jobject user_object = env->CallStaticObjectMethod(u_class, jmethodId);

    jclass managerClass = env->FindClass("com/modi/elapse/crash/UserManager");
    jmethodID initId = env->GetMethodID(managerClass, "<init>", "()V");
    jobject managerObj = env->NewObject(managerClass, initId);
    jmethodID setUserId = env->GetMethodID(managerClass, "setUser",
                                           "(Lcom/modi/elapse/crash/User;)V");

    env->CallVoidMethod(managerObj, setUserId, user_object);


    env->CallVoidMethod(call_back, callMethodId, managerObj);


}