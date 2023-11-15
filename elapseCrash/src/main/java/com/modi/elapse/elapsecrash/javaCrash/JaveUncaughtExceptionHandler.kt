package com.modi.elapse.elapsecrash.javaCrash

import android.app.ActivityManager
import android.content.Context
import android.os.Build
import android.os.Process
import android.util.Log
import java.io.File
import java.io.FileOutputStream
import java.text.SimpleDateFormat
import java.util.Arrays
import java.util.Date
import java.util.Locale
import kotlin.system.exitProcess

internal object CrashHandler {

    var CRASH_DOR = "crash_dir"
    fun init(context: Context, crashDir: String) {
        CRASH_DOR = crashDir
        Thread.setDefaultUncaughtExceptionHandler(JavaUncaughtExceptionHandler(context))
    }

    private class JavaUncaughtExceptionHandler(var context: Context) :
        Thread.UncaughtExceptionHandler {

        val sdf = SimpleDateFormat("yyyy-M-dd hh:mm:ss", Locale.CANADA)
        val launchTime: String = sdf.format(Date())
        private val defaultException = Thread.getDefaultUncaughtExceptionHandler()

        override fun uncaughtException(thread: Thread, throwable: Throwable) {

            if (!handleCrashInfo(throwable) && defaultException != null) {
                defaultException.uncaughtException(thread, throwable)
            }

        }


        private fun handleCrashInfo(e: Throwable): Boolean {
            return try {
                val content = collectInfo(e)
                Log.e("crashInfo", content)
                saveCrashInfoToFile(content)
                true
            } finally {
                // Try everything to make sure this process goes away.
                Process.killProcess(Process.myPid())
                exitProcess(10)
            }
        }

        fun collectInfo(e: Throwable): String {
            val content = """FATAL EXCEPTION: ${Thread.currentThread().name}
                |Process: ${context.applicationInfo.processName},PID:${Process.myPid()}
                |${e.stackTraceToString()}""".trimMargin()
            val info = buildString {
                append("brand=${Build.BRAND}\n")// huawei,xiaomi
                append("rom=${Build.MODEL}\n") //sm-G9550
                append("os=${Build.VERSION.RELEASE}\n")//9.0
                append("sdk=${Build.VERSION.SDK_INT}\n")//28
                append("crash_time=${sdf.format(Date())}\n")//crash发生的时间
//                 append("forground=${ActivityManager.instance.front}\n")//应用处于前后台
                append("thread=${Thread.currentThread().name}\n")//异常线程名
                append("cpu_arch=${Build.CPU_ABI}\n")//armv7 armv8

                //app 信息
                val packageInfo = context.packageManager.getPackageInfo(context.packageName, 0)
                append("version_code=${packageInfo.versionCode}\n")
                append("version_name=${packageInfo.versionName}\n")
                append("package_name=${packageInfo.packageName}\n")
                append("requested_permission=${Arrays.toString(packageInfo.requestedPermissions)}\n")//已申请到那些权限
            }
            return "$content\n$info"
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
}



