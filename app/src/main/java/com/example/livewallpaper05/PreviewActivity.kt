package com.example.livewallpaper05

import android.content.Context
import android.graphics.Color
import android.hardware.SensorManager
import android.os.Bundle
import android.util.Log
import android.widget.LinearLayout
import android.view.View
import android.view.View.OnClickListener
import android.view.WindowManager
import android.widget.AdapterView
import android.widget.ArrayAdapter
import android.widget.Button
import android.widget.SeekBar
import android.widget.Spinner
import android.widget.TextView
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

        // get view window from GLES3JNIView
        mView = GLES3JNIView(application, viewModel)

        // grab ui element for preview page
        val layout = findViewById<LinearLayout>(R.id.render_layout)
        val rotationScrollBar = findViewById<SeekBar>(R.id.rotation_rate_seekbar)
        val simSelectorSpinner = findViewById<Spinner>(R.id.simulation_type_spinner)
        val colorButton = findViewById<Button>(R.id.b_color_picker)

        // fill sim selector box with wallpaper options from native-lib.cpp
        val simSelectorAdapter = ArrayAdapter.createFromResource(
            this,
            R.array.simulation_types,
            android.R.layout.simple_spinner_item
        )
        simSelectorSpinner.adapter = simSelectorAdapter

        // register senser event listeners
        viewModel.registerSensorEvents(getSystemService(Context.SENSOR_SERVICE) as SensorManager)
        // add gl engine view to viewport
        layout.addView(mView)

        // update orientation in repo
        Log.d("Livewallpaper", "orientation preview: ${this.display!!.rotation}")
        viewModel.updateOrientation(this.display!!.rotation)

        // register scrollbar actions to update rotation rate in repo
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

        // register spinner actions to update simulation type in repo
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

        // setup color selection dialog
        /*colorButton.setOnClickListener {
            object : OnClickListener() {
                fun onClick(v: View?) {
                    Builder(this@PreviewActivity).initialColor(Color.RED)

                }
            }
        }*/

        // connect fps data to ui fps meter
        var fpsMeter = findViewById<TextView>(R.id.tv_fps_meter)
        viewModel.getFPS().observe(this) {
            fpsMeter.text = it.toString()
        }

    }

    /* this is run when the app is 'paused'
     * this includes leaving the app, rotating the screen, etc.
     */
    override fun onPause() {
        super.onPause()
        mView!!.onPause()
    }

    /* this is run when the app is 'resumed'
     * this includes returning to the app, rotating the screen, etc.
     */
    override fun onResume() {
        super.onResume()
        mView!!.onResume()
    }

    companion object {
        // Used to load the 'livewallpaper05' library on application startup.
        init {
            System.loadLibrary("livewallpaper05")
        }

        // register simulation functions as external and belonging to the livewallpaper05 library
        external fun init(visualization: String)
        external fun resize(width: Int, height: Int, orientation: Int)
        external fun step(acc_x: Float, acc_y: Float, acc_z: Float, rot_x: Float, rot_y: Float, rot_z: Float, rot_w: Float, linear_acc_x: Float, linear_acc_y: Float, linear_acc_z: Float, value: Float)
        external fun sendData(value: Float)
    }
}