//
// Created by 吴东生 on 2023/11/17.
//

#ifndef ELAPSECRASH_HANDLERCONTEXT_H
#define ELAPSECRASH_HANDLERCONTEXT_H

#define BACK_TRACE_FRAME_SIZE 32

#define MAX_BUFFER_SIZE 256

# include <signal.h>

typedef struct Native_handler_context_struct {
    int code;
    siginfo_t *info;
    void *sc;
    pid_t pid;
    pid_t tid;
    const char *processName;
    const char *threadName;
    int frame_Size;
    uintptr_t frames[BACK_TRACE_FRAME_SIZE];
} native_handler_context;


typedef struct Stack_Traces {
    jobjectArray javaTraceObj;
    int buffer_size;
    char buffers[BACK_TRACE_FRAME_SIZE][MAX_BUFFER_SIZE];
} StackTraces;

#define INSTANCE_FIELD_NAME                              "INSTANCE"
#define INSTANCE_FIELD_NAME_SIGNATURE                    "Lcom/modi/elapse/elapsecrash/crashMonitor/NativeCrashMonitor;"
#define GET_THREAD_BY_NAME_METHOD_NAME                   "getThreadByName"
#define GET_THREAD_BY_NAME_METHOD_SIGNATURE              "(Ljava/lang/String;)Ljava/lang/Thread;"


# define EL_ON_CRASH_METHOD_NAME                          "onCrash"
# define EL_ON_CRASH_METHOD_SIGNATURE                     "(Ljava/lang/String;Ljava/lang/Error;)V"


# define EL_ERROR_CLASS                                    "java/lang/Error"
# define EL_CONSTRUCTOR_METHOD                             "<init>"
# define EL_CONSTRUCTOR_METHOD_SIGNATURE                   "(Ljava/lang/String;)V"

# define GET_STACK_TRACE_METHOD_NAME                       "getStackTrace"
# define GET_STACK_TRACE_METHOD_SIGNATURE                  "()[Ljava/lang/StackTraceElement;"

# define SET_STACK_TRACE_METHOD_NAME                       "setStackTrace"
# define SET_STACK_TRACE_METHOD_SIGNATURE                  "([Ljava/lang/StackTraceElement;)V"


#endif //ELAPSECRASH_HANDLERCONTEXT_H
