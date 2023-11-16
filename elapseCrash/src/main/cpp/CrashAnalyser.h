//
// Created by 吴东生 on 2023/11/16.
//

#ifndef ELAPSECRASH_CRASHANALYSER_H
#define ELAPSECRASH_CRASHANALYSER_H

#include "common/JNIBridge.h"
#include <pthread.h>
#include <stdio.h>
#include "common/HandlerContext.h"



extern void initCondition();

void *threadCrashMonitor(void *argv);

extern void waitForSignal();

extern void notifyCaughtSignal(int code, siginfo_t *info, void *sc);

extern void analysisNativeException();

#endif //ELAPSECRASH_CRASHANALYSER_H
