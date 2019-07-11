package com.allenxuan.xuanyihuang.ndkpractice2

import android.nfc.Tag
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.os.Looper
import android.util.Log
import android.view.View
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {
    var jniFuncEntry: JniFuncEntry? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        jniFuncEntry = JniFuncEntry()
        //初始化JniFuncEntry
        jniFuncEntry?.init()

        sample_text.text = jniFuncEntry?.stringFromNative
        jniFuncEntry?.doHeavyTaskInNative(object : Runnable {
            override fun run() {
                Log.d(TAG, "doHeavyTaskInNative callback in java is invoked")
            }

        })
        jniFuncEntry?.funcDynamicallyRegisteredInNative1(object : Runnable {
            override fun run() {
                Log.d(TAG, "funcDynamicallyRegisteredInNative1 callback in java is invoked")
            }

        })
        jniFuncEntry?.funcDynamicallyRegisteredInNative2(object : Runnable {
            override fun run() {
                Log.d(TAG, "funcDynamicallyRegisteredInNative2  callback in java is invoked")
            }

        })

        findViewById<View>(R.id.btn_start_native_thread)?.setOnClickListener {
            jniFuncEntry?.nativeMultiThreadTask(5, object : Runnable {
                override fun run() {
                    Log.d(
                        TAG,
                        String.format(
                            "nativeMultiThreadTask callback in java, thread id = %d, thread name = %s",
                            Thread.currentThread().id,
                            Thread.currentThread().name
                        )
                    )
                }

            })
        }

        Log.d(TAG, String.format("UI Thread id = %d", Looper.getMainLooper().thread.id))
    }


    override fun onDestroy() {
        super.onDestroy()

        //销毁JniFuncEntry
        jniFuncEntry?.release()
    }

    companion object {
        const val TAG = "MainActivity"
    }
}
