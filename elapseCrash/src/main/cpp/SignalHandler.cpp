//
// Created by modi on 2023/11/16.
//

#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include "SignalHandler.h"

void signalPass(int code, siginfo_t *info, void *sc) {
    LOGE("监听到Native崩溃...");
    signal(code, SIG_DFL);
    signal(SIGALRM, SIG_DFL);
    (void) alarm(8);

    //给系统原来的处理
    oldHandlers[code].sa_sigaction(code, info, sc);
}
/**
 * 当进程的用户栈已满或被损坏是，操作系统无法在用户栈上执行信号处理函数。
 * 为了解决这个问题，可以使用备用信号栈
 * 备用信号栈是一个独立于用户栈的栈，用于执行信号处理函数。
 */
void installAlternateStack() {
    stack_t current_stack;
    stack_t new_stack;
    memset(&current_stack,0, sizeof(current_stack));
    memset(&new_stack,0, sizeof(new_stack));
    static const unsigned sigaltstackSize = std::max(16384, SIGSTKSZ);
    if (sigaltstack(nullptr,&current_stack)!=1){
        // 输出当前信号栈信息
        LOGD("Current signal stack:\n ss_sp = %p\nss_size = %zu\n", current_stack.ss_sp,current_stack.ss_size);
    }

    if (sigaltstack(nullptr,&current_stack)==-1||current_stack.ss_size<sigaltstackSize){
        new_stack.ss_sp=malloc(sigaltstackSize);//分配备用信号栈的内存
        new_stack.ss_size=SIGSTKSZ;
        new_stack.ss_flags=0;
        if (sigaltstack(&new_stack, nullptr)==-1){
            perror("sigaltstack");
            return;
        }
        LOGD("New signal stack set:\n ss_sp = %p\nss_size = %zu\n", new_stack.ss_sp,new_stack.ss_size);
    }

    free(new_stack.ss_sp);//释放备用信号栈的内存

}

bool installSignalHandlers() {

    LOGD("installSignalHandlers");
    //取出系统或则第三方设置信号处理
    for (int i = 0; i < exceptionSignalsNumber; ++i) {
        if (sigaction(exceptionSignals[i], nullptr, &oldHandlers[exceptionSignals[i]]) != -1) {
            LOGE("sigaction error");
        }
    }

    //初始化赋值
    struct sigaction sa{};
    memset(&sa, 0, sizeof(sa));
    sigemptyset(&sa.sa_mask);

    //指定信号处理的回调函数
    sa.sa_sigaction = signalPass;
    sa.sa_flags = SA_ONSTACK | SA_SIGINFO;

    //1.调用sigaction 来处理信号回调
    for (int i = 0; i < exceptionSignalsNumber; ++i) {
        if (sigaction(exceptionSignals[i], &sa, nullptr) == -1) {
            //可以输出一个警告
        }

    }
    return true;
};