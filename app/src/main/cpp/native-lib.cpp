#include <jni.h>
#include <string>
//ndk提供的android log库, 在CMakeList中find_library找到log库并与native-lib进行连接
#include <android/log.h>
#include <pthread.h>
#include "util.h"
//定义TAG宏, 在android log中使用
#define TAG "NATIVE-LIB"
//定义用于输出android log的方法宏, 有两种写法
//写法1，其中__VA_ARGS__代表variable arguments
#define LogI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
//写法2
//#define LogI(...) ((void)__android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__))
#define LogE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

JavaVM *javaVM = nullptr;
jobject javaThiz = nullptr;


void init(JNIEnv *env, jobject thiz) {
    LogI("init()");

    //缓存native方法所属的java对象
    javaThiz = env->NewGlobalRef(thiz);
    if (javaThiz == nullptr) {
        throwJavaRuntimeException(env, "init() cannot create GlobalRef of java \'THIS\'");
        return;
    }
}

void release(JNIEnv *env, jobject thiz) {
    LogI("release()");

    //释放javaThiz
    if (javaThiz != nullptr) {
        env->DeleteGlobalRef(javaThiz);
        javaThiz = nullptr;
    }
}

/**
 * 静态注册, 若是c++ compiler, 则方法必须带上extern "C"将方法标识为C Style
 * 动态注册无需extern "C"
 */
void funcDynamicallyRegisteredInNative1(JNIEnv *env, jobject thiz, jobject javaRunnable) {
    LogI("funcDynamicallyRegisteredInNative1() in c++ is invoked");
    jclass runnableClass = env->FindClass("java/lang/Runnable");
    if (runnableClass == nullptr) {
        LogE("funcDynamicallyRegisteredInNative1() in c++ cannot find Runnable class");
        return;
    }
    //寻找run()方法的method id
    jmethodID runMethodId = env->GetMethodID(runnableClass, "run", "()V");
    if (runMethodId == nullptr) {
        LogE("funcDynamicallyRegisteredInNative1() in c++ cannot find run() method id");
        return;
    }
    //调用Runnable对象的run方法
    env->CallVoidMethod(javaRunnable, runMethodId);
    //释放jclass局部变量
    env->DeleteLocalRef(runnableClass);
}

void funcDynamicallyRegisteredInNative2(JNIEnv *env, jobject thiz, jobject javaRunnable) {
    LogI("funcDynamicallyRegisteredInNative2() in c++ is invoked");
    jclass runnableClass = env->FindClass("java/lang/Runnable");
    if (runnableClass == nullptr) {
        LogE("funcDynamicallyRegisteredInNative2() in c++ cannot find Runnable class");
        return;
    }
    //寻找run()方法的method id
    jmethodID runMethodId = env->GetMethodID(runnableClass, "run", "()V");
    if (runMethodId == nullptr) {
        LogE("funcDynamicallyRegisteredInNative2() in c++ cannot find run() method id");
        return;
    }
    //调用Runnable对象的run方法
    env->CallVoidMethod(javaRunnable, runMethodId);
    //释放jclass局部变量
    env->DeleteLocalRef(runnableClass);
}

void *nativeThreadTask(void *params) {
    JNIEnv *env = nullptr;
    //将当前POSIX线程attach到JVM
    if (javaVM->AttachCurrentThread(&env, nullptr) == 0) {
        TaskParams *taskParams = (TaskParams *) (params);

        jclass runnableClass = env->FindClass("java/lang/Runnable");
        if (runnableClass == nullptr) {
            LogE("nativeThreadTask() in c++ cannot find Runnable class");
        }
        //寻找run()方法的method id
        jmethodID runMethodId = env->GetMethodID(runnableClass, "run", "()V");
        if (runMethodId == nullptr) {
            LogE("nativeThreadTask() in c++ cannot find run() method id");
        }
        //调用Runnable对象的run方法
        if (taskParams->javaRunnableCallback != nullptr && runnableClass != nullptr && runMethodId != nullptr) {
            LogI("nativeThreadTask native thread id %d", taskParams->threadId);
            env->CallVoidMethod(taskParams->javaRunnableCallback, runMethodId);
        }
        env->DeleteGlobalRef(runnableClass);
        delete taskParams;
        //将当前POSIX线程从JVM detach
        javaVM->DetachCurrentThread();
    }

    pthread_exit(nullptr);
}

void nativeMultiThreadTask(JNIEnv *env, jobject thiz, jint threadsCount, jobject javaRunnable) {
    LogI("nativeMultiThreadTask() in c++ is invoked");
    jclass runnableClass = env->FindClass("java/lang/Runnable");
    if (runnableClass == nullptr) {
        LogE("nativeMultiThreadTask() in c++ cannot find Runnable class");
        return;
    }
    //寻找run()方法的method id
    jmethodID runMethodId = env->GetMethodID(runnableClass, "run", "()V");
    if (runMethodId == nullptr) {
        LogE("nativeMultiThreadTask() in c++ cannot find run() method id");
        return;
    }

    //如果设置为 PTHREAD_CREATE_JOINABLE，就继续用 pthread_join() 来等待和释放资源，否则会内存泄露。
//    pthread_attr_t pthreadAttr;

//    // 初始化并设置线程为可连接的（joinable）
//    pthread_attr_init(&pthreadAttr);
//    pthread_attr_setdetachstate(&pthreadAttr, PTHREAD_CREATE_JOINABLE);
    for (int i = 0; i < 1; i++) {
        pthread_t tid;
        TaskParams* taskParams = new TaskParams();
        taskParams->threadId = i;
        taskParams->javaRunnableCallback = javaRunnable;
        int result = pthread_create(&tid, nullptr, nativeThreadTask, (void *) taskParams);
        if (result != 0) {
            LogE("failed to create pthread:%d", i);
        }
    }
//    pthread_attr_destroy(&pthreadAttr);


    //释放jclass局部变量
    env->DeleteLocalRef(runnableClass);
}

/**
 * 静态注册, 若是c++ compiler, 则方法必须带上extern "C"将方法标识为C Style
 * 动态注册无需extern "C"
 */
extern "C" JNIEXPORT jstring JNICALL
Java_com_allenxuan_xuanyihuang_ndkpractice2_JniFuncEntry_getStringFromNative(JNIEnv *env, jobject thiz) {
    LogI("getStringFromNative() in c++ is invoked");
//    std::string hello = "hello from c++";
//    return env->NewStringUTF(hello.c_str());
    return env->NewStringUTF("hello from c++");
}

extern "C" JNIEXPORT void JNICALL
Java_com_allenxuan_xuanyihuang_ndkpractice2_JniFuncEntry_doHeavyTaskInNative(JNIEnv *env, jobject thiz,
                                                                             jobject javaRunnable) {
    LogI("doHeavyTaskInNative() in c++ is invoked");
    //寻找Runnable类
    jclass runnableClass = env->FindClass("java/lang/Runnable");
    if (runnableClass == nullptr) {
        LogE("doHeavyTaskInNative() in c++ cannot find Runnable class");
        return;
    }
    //寻找run()方法的method id
    jmethodID runMethodId = env->GetMethodID(runnableClass, "run", "()V");
    if (runMethodId == nullptr) {
        LogE("doHeavyTaskInNative() in c++ cannot find run() method id");
        return;
    }
    //调用Runnable对象的run方法
    env->CallVoidMethod(javaRunnable, runMethodId);
    //释放jclass局部变量
    env->DeleteLocalRef(runnableClass);
}


//用于动态注册的方法数组
//{"Java方法名",       "Java方法签名",     "native方法指针"}
static const char *jniFunEntryClassName = "com/allenxuan/xuanyihuang/ndkpractice2/JniFuncEntry";
static const JNINativeMethod jniFuncEntryMethods[] = {
        {"init",                               "()V",                      (void *) init},
        {"release",                            "()V",                      (void *) release},
        {"funcDynamicallyRegisteredInNative1", "(Ljava/lang/Runnable;)V",  (void *) funcDynamicallyRegisteredInNative1},
        {"funcDynamicallyRegisteredInNative2", "(Ljava/lang/Runnable;)V",  (void *) funcDynamicallyRegisteredInNative2},
        {"nativeMultiThreadTask",              "(ILjava/lang/Runnable;)V", (void *) nativeMultiThreadTask}
};


int dynamicallyRegisterNatives(
        const char *javaClassName, const JNINativeMethod *methods, int methodsCount, JNIEnv *env) {
    //注意JNINativeMethod数组长度需要从外部传入, 通过methods指针是无法获取的
    jclass javaClass = env->FindClass(javaClassName);
    if (javaClass == nullptr) {
        return JNI_FALSE;
    }
    if (env->RegisterNatives(javaClass, methods, methodsCount) < 0) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

//Java层的System.loadLibrary()会回调JNI_OnLoad(), 在这里实现native方法动态注册的逻辑
//JNI_OnLoad()定义在jni.h中, 已经处理了extern "C"
JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void * /*reserved*/) {
    //缓存Java虚拟机指针
    javaVM = vm;

    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    // Get jclass with env->FindClass.
    // Register methods with env->RegisterNatives.
    if (dynamicallyRegisterNatives(jniFunEntryClassName, jniFuncEntryMethods,
                                   sizeof(jniFuncEntryMethods) / sizeof(jniFuncEntryMethods[0]), env)
        != JNI_TRUE) {
        LogE("dynamicallyRegisterNatives for JniFuncEntry error");
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}