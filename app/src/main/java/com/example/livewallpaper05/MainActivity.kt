package com.example.livewallpaper05

import android.app.Activity
import android.os.Bundle
import android.widget.LinearLayout
import android.util.Log
import android.widget.SeekBar
import com.example.livewallpaper05.WallpaperRepo

class MainActivity : Activity() {

    var mView: GLES3JNIView? = null
    private var mRepo: WallpaperRepo? = null

    override fun onCreate(icicle: Bundle?) {
        /*super.onCreate(savedInstanceState)
        // Example of a call to a native method
        binding.sampleText.text = stringFromJNI()*/
        super.onCreate(icicle)
        setContentView(R.layout.activity_main)

        mRepo = WallpaperRepo()
        mView = GLES3JNIView(application, mRepo!!)

        val layout = findViewById<LinearLayout>(R.id.render_layout)
        val scroller = findViewById<SeekBar>(R.id.rotation_rate_seekbar)

        layout.addView(mView)

        Log.d("Livewallpaper", "rotationRate: " + mRepo!!.rotationRate.toString())

        scroller.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar, progress: Int, fromUser: Boolean){
                mRepo!!.rotationRate = progress.toFloat()
                Log.d("Livewallpaper", "rotationRate: " + mRepo!!.rotationRate.toString())
            }
            override fun onStartTrackingTouch(seekBar: SeekBar) {
                // Do nothing
            }
            override fun onStopTrackingTouch(seekBar: SeekBar) {
                // Do nothing
                mRepo!!.rotationRate = seekBar.progress.toFloat()
                Log.d("Livewallpaper", "rotationRate: " + mRepo!!.rotationRate.toString())
            }
        })

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