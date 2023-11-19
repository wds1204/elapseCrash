//
// Created by 吴东生 on 2023/11/16.
//

#ifndef ELAPSECRASH_EL_UTIL_H
#define ELAPSECRASH_EL_UTIL_H

#include <jni.h>
#include <string>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/system_properties.h>


# define EL_CLASS_NAME                   "java/lang/Class"
# define EL_GET_NAME_METHOD_NAME         "getName"
# define EL_CET_NAME_METHOD_SIGNATURE    "()Ljava/lang/String;"

extern char *getClassName(JNIEnv *env, jclass anyClass);

extern const char *xcc_util_get_signame(const siginfo_t *si);

extern const char *xcc_util_get_sigcodename(const siginfo_t *si);

extern int xcc_util_signal_has_si_addr(const siginfo_t *si);

extern int xcc_util_signal_has_sender(const siginfo_t *si, pid_t caller_pid);

extern const char *getProcessName(pid_t pid);

extern const char *getThreadName(pid_t pid);

extern bool is_dll(const char *dll_name);

#endif //ELAPSECRASH_EL_UTIL_H
