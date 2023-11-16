//
// Created by 吴东生 on 2023/11/16.
//

#include "JNIBridge.h"

JNIBridge::JNIBridge(JavaVM *javaVm, jobject callbackObj) {
    this->javaVm=javaVm;
    this->callbackObj=callbackObj;

}

void JNIBridge::throwExceptionToJava(native_handler_context *handlerContext) {
    //java的线程名

}

