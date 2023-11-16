//
// Created by modi on 2023/11/16.
//

#include <cstring>
#include "SignalHandler.h"

void signalPass(int code, siginfo_t *info, void *sc) {
    LOGE("监听到Native崩溃");
};

void installAlternateStack(){

}
bool installSignalHandlers() {

    LOGD("installSignalHandlers");

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));//初始化
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = signalPass;//指定信号处理的回调函数
    sa.sa_flags=SA_ONSTACK|SA_SIGINFO;

    //1.调用sigaction 来处理信号回调
    for (int i = 0; i < exceptionSignalsNumber; ++i) {
        if (sigaction(exceptionSignals[i], &sa, nullptr)== -1) {
            //可以输出一个警告

        }

    }

    return true;
};