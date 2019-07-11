//
// Created by allenxuan on 2019-07-08.
//
#include <jni.h>

#ifndef NDKPRACTICE2_UTIL_H
#define NDKPRACTICE2_UTIL_H
extern struct TaskParams {
    jint threadId;
    jobject javaRunnableCallback;
};

extern void throwJavaRuntimeException(JNIEnv *env, const char *message);

#endif //NDKPRACTICE2_UTIL_H
