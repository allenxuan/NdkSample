package com.allenxuan.xuanyihuang.ndkpractice2;

public class JniFuncEntry {
    static {
        System.loadLibrary("native-lib");
    }

    public final native String getStringFromNative();

    public final native void doHeavyTaskInNative(Runnable runnable);

    public final native void funcDynamicallyRegisteredInNative1(Runnable runnable);

    public final native void funcDynamicallyRegisteredInNative2(Runnable runnable);
}
