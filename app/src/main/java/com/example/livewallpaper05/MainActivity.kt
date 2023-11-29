package com.example.livewallpaper05

import android.app.Activity
import android.content.Context
import android.content.Intent
import android.hardware.SensorManager
import android.os.Bundle
import android.widget.LinearLayout
import android.util.Log
import android.widget.SeekBar
import android.widget.TextView
import androidx.activity.viewModels
import androidx.appcompat.app.AppCompatActivity
import androidx.core.content.getSystemService
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.ViewModelProviders
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperApplication
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperRepo
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModel
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModelFactory

class MainActivity : AppCompatActivity() {

    var mView: GLES3JNIView? = null

    private val viewModel: ActiveWallpaperViewModel by viewModels {
        ActiveWallpaperViewModelFactory((application as ActiveWallpaperApplication).repository)
    }

    //private val mRepo = (application as ActiveWallpaperApplication).repository
    //private val mRepo = viewModel.getRepository()

    private var mRotLabel: TextView? = null

    override fun onCreate(icicle: Bundle?) {
        super.onCreate(icicle)
        setContentView(R.layout.activity_main)

        mView = GLES3JNIView(application, viewModel)

        val layout = findViewById<LinearLayout>(R.id.render_layout)
        val scroller = findViewById<SeekBar>(R.id.rotation_rate_seekbar)

        // register senser event listeners
        viewModel.registerSensorEvents(getSystemService(Context.SENSOR_SERVICE) as SensorManager)

        layout.addView(mView)

        scroller.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar, progress: Int, fromUser: Boolean){
                // Do nothing until changes are stopped for smooth ui updates
            }
            override fun onStartTrackingTouch(seekBar: SeekBar) {
                // Do nothing when changes are started
            }
            override fun onStopTrackingTouch(seekBar: SeekBar) {
                //mRepo!!.rotationRate = seekBar.progress.toFloat() / 100.0f
                viewModel.updateRotationRate(seekBar.progress.toFloat() / 100.0f)
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