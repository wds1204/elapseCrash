//
// Created by 吴东生 on 2023/11/16.
//


#include "CrashAnalyser.h"
#include "common/el_util.h"

//互斥锁对象
pthread_cond_t signalCond;
pthread_mutex_t signalLock;
pthread_cond_t exceptionCond;
pthread_mutex_t exceptionLock;

bool isSignalCaught = false;
native_handler_context *handlerContext;

void saveInfo2Context(int code, siginfo_t *info, void *sc);

void initCondition() {
    handlerContext = static_cast<native_handler_context *>(malloc(
            sizeof(native_handler_context)));
    pthread_mutex_init(&signalLock, nullptr);
    pthread_mutex_init(&exceptionLock, nullptr);
    pthread_cond_init(&signalCond, nullptr);
    pthread_cond_init(&exceptionCond, nullptr);

}

void *threadCrashMonitor(void *argv) {
    JNIBridge *jniBridge = static_cast<JNIBridge *>(argv);
    jint estatus;

    JNIEnv *env = nullptr;
    if (jniBridge->javaVm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
        LOGE("AttachCurrentThread faild");
        estatus = 0;
        return &estatus;
    }

    while (true) {
        //等待被唤醒
        waitForSignal();
        //解析异常信息，堆栈
        analysisNativeException();
        //把数据抛给Java层
        jniBridge->throwExceptionToJava(handlerContext);

    }
    if (jniBridge->javaVm->DetachCurrentThread() != JNI_OK) {
        estatus = 0;
        return &estatus;
    }

    return &estatus;

}

void waitForSignal() {
    pthread_mutex_lock(&signalLock);
    LOGD("waitForSignal start.");
    pthread_cond_wait(&signalCond, &signalLock);

    LOGD("waitForSignal Finished.");
    pthread_mutex_unlock(&signalLock);
}


/**
 * 唤醒等待的线程
 */
void notifyCaughtSignal(int code, siginfo_t *info, void *sc) {
    LOGD("notifyCaughtSignal");

    //创建一个结构体保存
    saveInfo2Context(code, info, sc);

    pthread_mutex_lock(&signalLock);

    pthread_cond_signal(&signalCond);
    pthread_mutex_unlock(&signalLock);

}

_Unwind_Reason_Code unwindTraceFn(struct _Unwind_Context *context,
                                  void *args) {
    auto *s = static_cast<native_handler_context *>(args);

    const uintptr_t pc = _Unwind_GetIP(context);
    if (pc != 0x0) {
        s->frames[s->frame_Size++] = pc;
    }
    if (s->frame_Size == BACK_TRACE_FRAME_SIZE) {
        return _URC_END_OF_STACK;
    }
    return _URC_NO_REASON;

}


void saveInfo2Context(int code, siginfo_t *info, void *sc) {
    handlerContext->code = code;
    handlerContext->info = info;
    handlerContext->sc = sc;
    handlerContext->pid = getpid();
    handlerContext->tid = gettid();

    handlerContext->processName = getProcessName(handlerContext->pid);
    if (getpid() == gettid()) {
        handlerContext->threadName = "main";
    } else {
        handlerContext->threadName = getThreadName(handlerContext->tid);
    }

    handlerContext->frame_Size = 0;

    _Unwind_Backtrace(unwindTraceFn, handlerContext);

}

/**
 * 解析
 */
void analysisNativeException() {
    //解析crash的原因
    const char *sigCodename = xcc_util_get_sigcodename(handlerContext->info);
    LOGD("sigCodename ->%s", sigCodename);
    LOGD("address ->%p", handlerContext->info->si_addr);
    LOGD("pid ->%d", handlerContext->pid);
    LOGD("tid ->%d", handlerContext->tid);
    LOGD("processName ->%s", handlerContext->processName);
    LOGD("threadName ->%s", handlerContext->threadName);
    //processName  threadNam



}

