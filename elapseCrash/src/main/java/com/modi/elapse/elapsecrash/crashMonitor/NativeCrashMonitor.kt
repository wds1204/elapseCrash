package com.modi.elapse.elapsecrash.crashMonitor

import android.content.Context
import android.os.Looper
import com.modi.elapse.elapsecrash.CrashHandlerListener


internal object NativeCrashMonitor : CrashHandlerListener {
    var CRASH_DOR = "crash_dir"
    // Used to load the 'elapsecrash' library on application startup.
    init {
        System.loadLibrary("elapsecrash")
    }
    private external fun nativeInitCallBack(callback: CrashHandlerListener)
    private external fun nativeInit()

    fun init(context: Context, crashDir: String) {
        this.CRASH_DOR = crashDir
        nativeInitCallBack(this)
        nativeInit()
        context::class.java.name

    }

    override fun onCrash(threadName: String, error: Error) {
        //监听native层文件

        println("threadName=$threadName  error=${error.message}")

    }


    fun getThreadStackTraces(threadName: String): String {

        return buildString {
            getThreadByName(threadName)?.stackTrace?.forEach {
                append("${it.toString()} \n")
            }
        }

    }

    private fun getThreadByName(threadName: String): Thread? {
        if (threadName == "main") {
            return Looper.getMainLooper().thread;
        }
        val allThreads = Thread.getAllStackTraces()
        allThreads.forEach {
            if (it.key.name == threadName) {
                return it.key
            }
        }
        return null
    }
}