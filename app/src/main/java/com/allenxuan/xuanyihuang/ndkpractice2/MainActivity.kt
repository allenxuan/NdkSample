package com.allenxuan.xuanyihuang.ndkpractice2

import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.widget.Toast
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // Example of a call to a native method
        val jniFuncEntry = JniFuncEntry()
        sample_text.text = jniFuncEntry.stringFromNative
        jniFuncEntry.doHeavyTaskInNative(object : Runnable {
            override fun run() {
                Log.d("MainActivity", "doHeavyTaskInNative callback in java is invoked")
            }

        })
        jniFuncEntry.funcDynamicallyRegisteredInNative1(object : Runnable {
            override fun run() {
                Log.d("MainActivity", "funcDynamicallyRegisteredInNative1 callback in java is invoked")
            }

        })
        jniFuncEntry.funcDynamicallyRegisteredInNative2(object : Runnable {
            override fun run() {
                Log.d("MainActivity", "funcDynamicallyRegisteredInNative2  callback in java is invoked")
            }

        })
    }
}
