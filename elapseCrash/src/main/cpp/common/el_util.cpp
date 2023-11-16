//
// Created by 吴东生 on 2023/11/16.
//

#include "el_util.h"

  char* getClassName(JNIEnv *env,  jclass anyClass)
{
    // Get the JNIEnv class
    jclass classClass = env->FindClass( EL_CLASS_NAME);
    // Get the getName method ID
    jmethodID getNameMethod = env->GetMethodID(classClass, EL_GET_NAME_METHOD_NAME, EL_CET_NAME_METHOD_SIGNATURE);
    // Call the getName method to get the class name
    auto className = (jstring)env->CallObjectMethod( anyClass, getNameMethod);
    // Convert the Java string to a C string
    const char *classNameStr = env->GetStringUTFChars( className, nullptr);
    // Print or use the class name as needed
    printf("Class name: %s\n", classNameStr);
    // Release the C string
    env->ReleaseStringUTFChars( className, classNameStr);

    return const_cast<char *>(classNameStr);
}