package com.modi.elapse.elapsecrash.util

import android.annotation.SuppressLint
import android.app.Application

object AppGlobals {
    private var application: Application? = null

    @SuppressLint("PrivateApi")
    fun get(): Application? {
        if (application == null) {
            try {
                application =
                    Class.forName("android.app.ActivityThread").getMethod("currentApplication")
                        .invoke(null) as Application

            } catch (e: Exception) {
                e.printStackTrace()
            }
        }
        return application
    }
}