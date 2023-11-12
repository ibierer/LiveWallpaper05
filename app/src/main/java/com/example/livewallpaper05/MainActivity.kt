package com.example.livewallpaper05

import android.app.Activity
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity

class MainActivity : Activity() {

    var mView: GLES3JNIView? = null

    override fun onCreate(icicle: Bundle?) {
        /*super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // Example of a call to a native method
        binding.sampleText.text = stringFromJNI()*/

        super.onCreate(icicle)
        mView = GLES3JNIView(application)
        setContentView(mView)
    }

    override fun onPause() {
        super.onPause()
        mView!!.onPause()
    }

    override fun onResume() {
        super.onResume()
        mView!!.onResume()
    }

    /**
     * A native method that is implemented by the 'livewallpaper05' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    companion object {
        // Used to load the 'livewallpaper05' library on application startup.
        init {
            System.loadLibrary("livewallpaper05")
        }

        external fun init()
        external fun resize(width: Int, height: Int)
        external fun step(acc_x: Float, acc_y: Float, acc_z: Float, rot_x: Float, rot_y: Float, rot_z: Float, rot_w: Float)
    }
}