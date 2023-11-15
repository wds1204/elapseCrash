package com.modi.elapse.elapsecrash

interface CrashHandlerListener {
    fun onCrash(threadName: String, error: Error)
}