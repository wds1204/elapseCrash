package com.modi.elapse.crash

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.modi.elapse.crash.databinding.ActivityMainBinding
import kotlin.concurrent.thread

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
            divisionTest(10,0)

        }
    }

    /**
     * A native method that is implemented by the 'crash' native library,
     * which is packaged with this application.
     */
    private external fun stringFromJNI(): String

    companion object {
        // Used to load the 'crash' library on application startup.
        init {
            System.loadLibrary("crash")
        }
    }
}