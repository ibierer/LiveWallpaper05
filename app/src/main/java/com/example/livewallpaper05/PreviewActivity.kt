package com.example.livewallpaper05

import android.content.Context
import android.hardware.SensorManager
import android.os.Bundle
import android.widget.LinearLayout
import android.view.View
import android.view.WindowManager
import android.widget.AdapterView
import android.widget.ArrayAdapter
import android.widget.SeekBar
import android.widget.Spinner
import androidx.activity.viewModels
import androidx.appcompat.app.AppCompatActivity
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperApplication
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModel
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModelFactory

class PreviewActivity : AppCompatActivity() {

    var mView: GLES3JNIView? = null

    private val viewModel: ActiveWallpaperViewModel by viewModels {
        ActiveWallpaperViewModelFactory((application as ActiveWallpaperApplication).repository)
    }

    override fun onCreate(icicle: Bundle?) {
        super.onCreate(icicle)
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_preview)

        mView = GLES3JNIView(application, viewModel)

        val layout = findViewById<LinearLayout>(R.id.render_layout)
        val rotationScrollBar = findViewById<SeekBar>(R.id.rotation_rate_seekbar)
        val simSelectorSpinner = findViewById<Spinner>(R.id.simulation_type_spinner)

        // fill ui elements with default data
        // fill sim selector with wallpaper options from native-lib.cpp
        // ie box, naive, picflip, triangle, implicitGrapher
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
                viewModel.updateRotationRate(seekBar.progress.toFloat() / 100.0f)
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
                val changed = viewModel.updateSimulationType(pos)
                if (changed){
                    // tell view it needs to be reloaded
                    mView!!.onPause()
                    mView!!.onResume()
                }
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

    companion object {
        // Used to load the 'livewallpaper05' library on application startup.
        init {
            System.loadLibrary("livewallpaper05")
        }

        external fun init(visualization: String)
        external fun resize(width: Int, height: Int)
        external fun step(acc_x: Float, acc_y: Float, acc_z: Float, rot_x: Float, rot_y: Float, rot_z: Float, rot_w: Float, value: Float)
        external fun sendData(value: Float)
    }
}