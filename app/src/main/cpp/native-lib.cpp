#include <jni.h>
#include <string>
//ndk提供的android log库, 在CMakeList中find_library找到log库并与native-lib进行连接
#include <android/log.h>
//定义TAG宏, 在android log中使用
#define TAG "NATIVE-LIB"
//定义用于输出android log的方法宏, 有两种写法
//写法1，其中__VA_ARGS__代表variable arguments
#define LogI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
//写法2
//#define LogI(...) ((void)__android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__))
#define LogE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

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


//用于动态注册的方法数组
//{"Java方法名",       "Java方法签名",     "native方法指针"}
static const char *jniFunEntryClassName = "com/allenxuan/xuanyihuang/ndkpractice2/JniFuncEntry";
static const JNINativeMethod jniFuncEntryMethods[] = {
        {"funcDynamicallyRegisteredInNative1", "(Ljava/lang/Runnable;)V", (void *) funcDynamicallyRegisteredInNative1},
        {"funcDynamicallyRegisteredInNative2", "(Ljava/lang/Runnable;)V", (void *) funcDynamicallyRegisteredInNative2}
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
jint JNI_OnLoad(JavaVM *vm, void * /*reserved*/) {
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