package com.modi.elapse.elapsecrash

import android.content.Context
import com.modi.elapse.elapsecrash.javaCrash.CrashHandler
import com.modi.elapse.elapsecrash.util.AppGlobals
import java.io.File

class CrashManager {

    /**
     * A native method that is implemented by the 'elapsecrash' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    companion object {

        private const val CRASH_DIR_JAVA = "java_crash"
        private const val CRASH_DIR_NATIVE = "native_crash"

        // Used to load the 'elapsecrash' library on application startup.
        init {
            System.loadLibrary("elapsecrash")
        }

        fun initElapseCrash(context: Context) {
            val javaCrashDir = getJavaCrashDir().absolutePath
            CrashHandler.init(context, javaCrashDir)
        }


        private fun getJavaCrashDir(): File {
            val javaCrashFile = File(AppGlobals.get()!!.cacheDir, CRASH_DIR_JAVA)
            if (!javaCrashFile.exists()) {
                javaCrashFile.mkdirs()
            }
            return javaCrashFile
        }


        private fun getNativeCrashDir(): File {
            val nativeCrashFile = File(AppGlobals.get()!!.cacheDir, CRASH_DIR_NATIVE)
            if (!nativeCrashFile.exists()) {
                nativeCrashFile.mkdirs()
            }
            return nativeCrashFile
        }

        fun crashFiles(): Array<File> {
            return getJavaCrashDir().listFiles() + getNativeCrashDir().listFiles()
        }

    }


}
