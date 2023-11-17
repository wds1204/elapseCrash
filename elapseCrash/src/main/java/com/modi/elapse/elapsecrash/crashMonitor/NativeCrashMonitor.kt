package com.modi.elapse.elapsecrash.crashMonitor

import android.content.Context
import android.os.Looper
import androidx.annotation.Keep
import com.modi.elapse.elapsecrash.CrashHandlerListener
import java.io.File
import java.io.FileOutputStream
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale


internal object NativeCrashMonitor : CrashHandlerListener {
    var CRASH_DOR = "crash_dir"

    // Used to load the 'elapsecrash' library on application startup.
    init {
        System.loadLibrary("elapsecrash")
    }

    val sdf = SimpleDateFormat("yyyy-M-dd hh:mm:ss", Locale.CANADA)
    val launchTime: String = sdf.format(Date())
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
        val a = error.stackTrace
        val sb = StringBuffer()
        a.forEach {
            println("onCrash forEach=${it}")
            sb.append(it.toString()).append("\n")
        }

        println("onCrash=$sb")
        buildString {
            error.stackTrace.forEach {
                sb.append(it.toString()).append("\n")
            }
        }
        saveCrashInfoToFile(sb.toString())

    }


    @Keep
    fun getThreadByName(threadName: String): Thread? {
        if (threadName == "main") {
            return Looper.getMainLooper().thread
        }
        val allThreads = Thread.getAllStackTraces()
        allThreads.forEach {
            if (it.key.name == threadName) {
                return it.key
            }
        }
        return null
    }


    private fun saveCrashInfoToFile(log: String) {
        val crashDir = File(CRASH_DOR)
        if (!crashDir.exists()) {
            crashDir.mkdirs()
        }
        val crashFile = File(crashDir, "$launchTime-crash.txt")
        crashFile.createNewFile()
        val fos = FileOutputStream(crashFile)

        try {
            fos.write(log.toByteArray())
            fos.flush()
        } catch (ex: Exception) {
            ex.printStackTrace()
        } finally {
            fos.close()
        }
    }
}