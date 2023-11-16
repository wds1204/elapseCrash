//
// Created by 吴东生 on 2023/11/16.
//

#include <cstdlib>
#include <unistd.h>
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

    while (true) {
        //等待被唤醒
        waitForSignal();
        //解析异常信息，堆栈
        analysisNativeException();
        //把数据抛给Java层
        jniBridge->throwExceptionToJava(handlerContext);

    }

}

void waitForSignal() {
    pthread_mutex_lock(&signalLock);
    LOGD("waitForSignal start.");
//     while (!isSignalCaught){
    pthread_cond_wait(&signalCond, &signalLock);
    //}
    //isSignalCaught= false;

    LOGD("waitForSignal Finished.");
    pthread_mutex_unlock(&signalLock);
}


/**
 * 唤醒等待的线程
 */
void notifyCaughtSignal(int code, siginfo_t *info, void *sc) {
    //创建一个结构体保存
    saveInfo2Context(code, info, sc);

    pthread_mutex_lock(&signalLock);
    LOGD("notifyCaughtSignal");

    pthread_cond_signal(&signalCond);
    pthread_mutex_unlock(&signalLock);

}

void saveInfo2Context(int code, siginfo_t *info, void *sc) {
    handlerContext->code = code;
    handlerContext->info = info;
    handlerContext->sc = sc;
    handlerContext->pid=getpid();
    handlerContext->tid=gettid();

    handlerContext->processName=getProcessName( handlerContext->pid);
    if (getpid()==gettid()){
        handlerContext->threadName="main";
    }else{
        handlerContext->threadName=getThreadName( handlerContext->tid);

    }
}

/**
 * 解析
 */
void analysisNativeException() {
    //解析crash的原因
    const char *sigCodename = xcc_util_get_sigcodename(handlerContext->info);
    LOGD("sigCodename ->%s", sigCodename);
    LOGD("address ->%p",handlerContext->info->si_addr);
    LOGD("pid ->%d",handlerContext->pid);
    LOGD("tid ->%d",handlerContext->tid);
    LOGD("processName ->%s",handlerContext->processName);
    LOGD("threadName ->%s",handlerContext->threadName);
    //processName  threadNam



}

