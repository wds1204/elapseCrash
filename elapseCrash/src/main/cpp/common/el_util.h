//
// Created by 吴东生 on 2023/11/16.
//

#ifndef ELAPSECRASH_EL_UTIL_H
#define ELAPSECRASH_EL_UTIL_H
#include <jni.h>
#include <string>

# define EL_CLASS_NAME                   "java/lang/Class"
# define EL_GET_NAME_METHOD_NAME         "getName"
# define EL_CET_NAME_METHOD_SIGNATURE    "()Ljava/lang/String;"

extern   char* getClassName(JNIEnv *env,  jclass anyClass);


#endif //ELAPSECRASH_EL_UTIL_H
