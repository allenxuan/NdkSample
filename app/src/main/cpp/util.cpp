//
// Created by allenxuan on 2019-07-08.
//
#include <jni.h>
#include "util.h"

void throwJavaRuntimeException(JNIEnv* env, const char* message){
    jclass javaRuntimeExceptionClass = env->FindClass("java/lang/RuntimeException");
    if(javaRuntimeExceptionClass != nullptr){
        env->ThrowNew(javaRuntimeExceptionClass, message);
    }
}


