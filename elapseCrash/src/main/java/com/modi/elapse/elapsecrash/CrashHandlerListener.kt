package com.modi.elapse.elapsecrash

import androidx.annotation.Keep

interface CrashHandlerListener {
    @Keep
    fun onCrash(threadName: String, error: Error)
}