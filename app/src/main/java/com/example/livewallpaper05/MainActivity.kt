package com.example.livewallpaper05

import android.app.Activity
import android.content.Context
import android.content.Intent
import android.hardware.SensorManager
import android.os.Bundle
import android.widget.LinearLayout
import android.util.Log
import android.view.View
import android.widget.AdapterView
import android.widget.ArrayAdapter
import android.widget.SeekBar
import android.widget.Spinner
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

    override fun onCreate(icicle: Bundle?) {
        super.onCreate(icicle)
        setContentView(R.layout.activity_main)

        mView = GLES3JNIView(application, viewModel)

        val layout = findViewById<LinearLayout>(R.id.render_layout)
        val rotationScrollBar = findViewById<SeekBar>(R.id.rotation_rate_seekbar)
        val simSelectorSpinner = findViewById<Spinner>(R.id.simulation_type_spinner)

        // fill ui elements with default data
        // fill sim selector with wallpaper options from native-lib.cpp
        // ie box, naive, picflip, triangle, graph
        val simSelectorAdapter = ArrayAdapter.createFromResource(
            this,
            R.array.simulation_types,
            android.R.layout.simple_spinner_item
        )
        simSelectorSpinner.adapter = simSelectorAdapter

        // register senser event listeners
        viewModel.registerSensorEvents(getSystemService(Context.SENSOR_SERVICE) as SensorManager)
        // add gl engine to viewport
        layout.addView(mView)

        // setup ui element actions
        rotationScrollBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
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

        simSelectorSpinner.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
            override fun onItemSelected(parent: AdapterView<*>, view: View?, pos: Int, id: Long) {
                viewModel.updateSimulationType(pos)
            }
            override fun onNothingSelected(parent: AdapterView<*>) {
                // Do nothing
            }
        }

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

        external fun init(visualization: Int)
        external fun resize(width: Int, height: Int)
        external fun step(acc_x: Float, acc_y: Float, acc_z: Float, rot_x: Float, rot_y: Float, rot_z: Float, rot_w: Float)
        external fun sendData(value: Float)
    }
}