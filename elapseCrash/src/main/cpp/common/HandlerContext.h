//
// Created by 吴东生 on 2023/11/17.
//

#ifndef ELAPSECRASH_HANDLERCONTEXT_H
#define ELAPSECRASH_HANDLERCONTEXT_H
# include <signal.h>
typedef struct Native_handler_context_struct{
    int code;
    siginfo_t *info;
    void *sc;
    pid_t pid;
    pid_t tid;
    const char *processName;
    const char *threadName;
} native_handler_context;


#endif //ELAPSECRASH_HANDLERCONTEXT_H
