package com.modi.elapse.crash

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.modi.elapse.crash.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // Example of a call to a native method
        binding.sampleText.text = stringFromJNI()

        binding.button.setOnClickListener {
//            thread {
//                divisionTest(10,0)
//            }
            divisionTest(10, 0)


        }
        binding.button2.setOnClickListener {
            nativeCrash()
        }
        val error = Error(
            "JavaTraces--com.modi.elapse.crash.MainActivity.nativeCrash(Native Method) \n" +
                    "                                                                                                    com.modi.elapse.crash.MainActivity.onCreate\$lambda\$1(MainActivity.kt:31) \n" +
                    "                                                                                                    com.modi.elapse.crash.MainActivity.\$r8\$lambda\$Ycj6g-OrBu3LU20J0DEk7b7h5Pk(Unknown Source:0) \n" +
                    "                                                                                                    com.modi.elapse.crash.MainActivity\$\$ExternalSyntheticLambda1.onClick(Unknown Source:2) \n" +
                    "                                                                                                    android.view.View.performClick(View.java:7564) \n" +
                    "                                                                                                    com.google.android.material.button.MaterialButton.performClick(MaterialButton.java:1202) \n" +
                    "                                                                                                    android.view.View.performClickInternal(View.java:7537) \n" +
                    "                                                                                                    android.view.View.-\$\$Nest\$mperformClickInternal(Unknown Source:0) \n" +
                    "                                                                                                    android.view.View\$PerformClick.run(View.java:29761) \n" +
                    "                                                                                                    android.os.Handler.handleCallback(Handler.java:942) \n" +
                    "                                                                                                    android.os.Handler.dispatchMessage(Handler.java:99) \n" +
                    "                                                                                                    android.os.Looper.loopOnce(Looper.java:210) \n" +
                    "                                                                                                    android.os.Looper.loop(Looper.java:299) \n" +
                    "                                                                                                    android.app.ActivityThread.main(ActivityThread.java:8136) \n" +
                    "                                                                                                    java.lang.reflect.Method.invoke(Native Method) \n" +
                    "                                                                                                    com.android.internal.os.RuntimeInit\$MethodAndArgsCaller.run(RuntimeInit.java:580"
        )
        error.stackTraceToString()

        println(
            "error---${
                error.stackTraceToString()
            }"
        )
    }

    /**
     * A native method that is implemented by the 'crash' native library,
     * which is packaged with this application.
     */
    private external fun stringFromJNI(): String

    private external fun nativeCrash()

    companion object {
        // Used to load the 'crash' library on application startup.
        init {
            System.loadLibrary("crash")
        }
    }
}