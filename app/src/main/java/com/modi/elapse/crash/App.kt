package com.modi.elapse.crash

import android.app.Application
import com.modi.elapse.elapsecrash.CrashManager

class App : Application() {
    override fun onCreate() {
        super.onCreate()
        CrashManager.Companion.initElapseCrash(this)
    }
}