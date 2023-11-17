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


//            setCallBack(object :CallBack{
//                override fun call(manager: UserManager) {
//                    println("UserManager---$manager")
//                }
//
//            })


        }
        binding.button2.setOnClickListener {
            nativeCrash()
        }

    }

    /**
     * A native method that is implemented by the 'crash' native library,
     * which is packaged with this application.
     */
    private external fun stringFromJNI(): String

    private external fun nativeCrash()

    private external fun setCallBack(callBack: CallBack);

    companion object {
        // Used to load the 'crash' library on application startup.
        init {
            System.loadLibrary("crash")
        }
    }
}