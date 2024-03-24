package com.example.livewallpaper05

import android.content.Context
import android.graphics.Bitmap
import android.graphics.Canvas
import android.graphics.Color
import android.hardware.SensorManager
import android.os.Build
import android.os.Bundle
import android.text.TextWatcher
import android.util.Log
import android.view.View
import android.view.WindowManager
import android.view.animation.Animation
import android.view.animation.TranslateAnimation
import android.view.inputmethod.EditorInfo
import android.widget.AdapterView
import android.widget.ArrayAdapter
import android.widget.Button
import android.widget.EditText
import android.widget.LinearLayout
import android.widget.SeekBar
import android.widget.Spinner
import android.widget.TextView
import androidx.activity.result.contract.ActivityResultContracts
import androidx.activity.viewModels
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import androidx.core.graphics.toColor
import androidx.lifecycle.Observer
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperApplication
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModel
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModelFactory
import yuku.ambilwarna.AmbilWarnaDialog

class PreviewActivity : AppCompatActivity() {

    var mView: GLES3JNIView? = null

    private val viewModel: ActiveWallpaperViewModel by viewModels {
        ActiveWallpaperViewModelFactory((application as ActiveWallpaperApplication).repository)
    }

    private fun updateSyntaxResult() {
        // update syntax check message
        val equationChecker = EquationChecker()
        val result: String = equationChecker.checkEquationSyntax(findViewById<EditText>(R.id.et_equation).text.toString())
        val message: String
        message = if (result == "") {
            "No syntax errors."
        } else {
            result
        }
        findViewById<TextView>(R.id.tv_syntax_check).text = message
    }

    @RequiresApi(Build.VERSION_CODES.R)
    override fun onCreate(icicle: Bundle?) {
        super.onCreate(icicle)
        getWindow().setFlags(
            WindowManager.LayoutParams.FLAG_FULLSCREEN,
            WindowManager.LayoutParams.FLAG_FULLSCREEN
        );
        setContentView(R.layout.activity_preview)

        // get view window from GLES3JNIView
        mView = GLES3JNIView(application, viewModel)

        // grab ui element for preview page
        val layout = findViewById<LinearLayout>(R.id.render_layout)
        val distanceSeekBar = findViewById<SeekBar>(R.id.distance_seekbar)
        val fieldOfViewSeekBar = findViewById<SeekBar>(R.id.field_of_view_seekbar)
        val gravitySeekBar = findViewById<SeekBar>(R.id.gravity_seekbar)
        val linearAccelerationSeekBar = findViewById<SeekBar>(R.id.linear_acceleration_seekbar)
        val efficiencySeekBar = findViewById<SeekBar>(R.id.efficiency_seekbar)
        val visualizationSelectorSpinner = findViewById<Spinner>(R.id.visualization_type_spinner)
        val colorButton = findViewById<Button>(R.id.b_color_picker)
        val hideUIButton = findViewById<Button>(R.id.hide_ui_button)
        //val saveButton = findViewById<Button>(R.id.save_button)
        //val saveAsButton = findViewById<Button>(R.id.save_as_new_button)
        val equationEditor = findViewById<EditText>(R.id.et_equation)
        val linearLayout = findViewById<LinearLayout>(R.id.settings_linearlayout)

        // fill sim selector box with wallpaper options from native-lib.cpp
        val simSelectorAdapter = ArrayAdapter.createFromResource(
            this,
            R.array.simulation_types,
            android.R.layout.simple_spinner_item
        )
        visualizationSelectorSpinner.adapter = simSelectorAdapter
        // set default to viewmodel simulation type
        visualizationSelectorSpinner.setSelection(viewModel.getVisualization())

        // register senser event listeners
        viewModel.registerSensorEvents(getSystemService(Context.SENSOR_SERVICE) as SensorManager)
        // add gl engine view to viewport
        layout.addView(mView)

        // update orientation in repo
        try {
            viewModel.updateOrientation(this.display!!.rotation)
        } catch (e: Exception) {
            Log.d("Livewallpaper", "api level too low!")
        }

        // register seekbar actions to update distance in repo
        distanceSeekBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar, progress: Int, fromUser: Boolean) {
                // Do nothing until changes are stopped for smooth ui updates
                viewModel.updateDistanceFromCenter(seekBar.progress.toFloat() / 100.0f)
            }

            override fun onStartTrackingTouch(seekBar: SeekBar) {
                // Do nothing when changes are started
            }

            override fun onStopTrackingTouch(seekBar: SeekBar) {
                //mRepo!!.distanceFromCenter = seekBar.progress.toFloat() / 100.0f
                viewModel.updateDistanceFromCenter(seekBar.progress.toFloat() / 100.0f)
            }
        })

        viewModel.mRepo.distanceFromOrigin.observe(this) { float ->
            distanceSeekBar.progress = (float * 100.0f).toInt()
        }

        // register seekbar actions to update speed in repo
        fieldOfViewSeekBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar, progress: Int, fromUser: Boolean) {
                // Do nothing until changes are stopped for smooth ui updates
                viewModel.updateFieldOfView(seekBar.progress.toFloat() / 100.0f)
            }

            override fun onStartTrackingTouch(seekBar: SeekBar) {
                // Do nothing when changes are started
            }

            override fun onStopTrackingTouch(seekBar: SeekBar) {
                //mRepo!!.fieldOfView = seekBar.progress.toFloat() / 100.0f
                viewModel.updateFieldOfView(seekBar.progress.toFloat() / 100.0f)
            }
        })

        viewModel.mRepo.fieldOfView.observe(this) { float ->
            fieldOfViewSeekBar.progress = (float * 100.0f).toInt()
        }

        // register seekbar actions to update gravity in repo
        gravitySeekBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar, progress: Int, fromUser: Boolean) {
                // Do nothing until changes are stopped for smooth ui updates
                viewModel.updateGravity(seekBar.progress.toFloat() / 100.0f)
            }

            override fun onStartTrackingTouch(seekBar: SeekBar) {
                // Do nothing when changes are started
            }

            override fun onStopTrackingTouch(seekBar: SeekBar) {
                viewModel.updateGravity(seekBar.progress.toFloat() / 100.0f)
            }
        })

        viewModel.mRepo.gravity.observe(this) { float ->
            gravitySeekBar.progress = (float * 100.0f).toInt()
        }

        // register seekbar actions to update linear acceleration in repo
        linearAccelerationSeekBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar, progress: Int, fromUser: Boolean) {
                // Do nothing until changes are stopped for smooth ui updates
                viewModel.updateLinearAcceleration(seekBar.progress.toFloat() / 100.0f)
            }

            override fun onStartTrackingTouch(seekBar: SeekBar) {
                // Do nothing when changes are started
            }

            override fun onStopTrackingTouch(seekBar: SeekBar) {
                viewModel.updateLinearAcceleration(seekBar.progress.toFloat() / 100.0f)
            }
        })

        viewModel.mRepo.linearAcceleration.observe(this) { float ->
            linearAccelerationSeekBar.progress = (float * 100.0f).toInt()
        }

        // register seekbar actions to update efficiency in repo
        efficiencySeekBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar, progress: Int, fromUser: Boolean) {
                // Do nothing until changes are stopped for smooth ui updates
                viewModel.updateEfficiency(seekBar.progress.toFloat() / 10000.0f)
            }

            override fun onStartTrackingTouch(seekBar: SeekBar) {
                // Do nothing when changes are started
            }

            override fun onStopTrackingTouch(seekBar: SeekBar) {
                viewModel.updateEfficiency(seekBar.progress.toFloat() / 10000.0f)
            }
        })

        viewModel.mRepo.efficiency.observe(this) { float ->
            efficiencySeekBar.progress = (float * 10000.0f).toInt()
        }

        // register spinner actions to update visualization type in repo
        visualizationSelectorSpinner.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
            override fun onItemSelected(parent: AdapterView<*>, view: View?, pos: Int, id: Long) {
                val changed = viewModel.updateVisualizationSelection(pos)
                if (changed) {
                    // tell view it needs to be reloaded
                    mView!!.onPause()
                    mView!!.onResume()
                }
            }

            override fun onNothingSelected(parent: AdapterView<*>) {
                // Do nothing
            }
        }

        // Observe changes to liveDataBitmap in the ViewModel
        viewModel.liveDataBitmap.observe(this, Observer { bitmap ->
            // This code will be executed on the main (UI) thread whenever liveDataBitmap changes
            //findViewById<ImageView>(R.id.imageView).setImageBitmap(bitmap)
            viewModel.updatePreviewImg(bitmap)
        })

        // setup color selection dialog
        colorButton.setOnClickListener {
            val initColor = 0
            val colorPickerDialog = AmbilWarnaDialog(
                this,
                initColor,
                object : AmbilWarnaDialog.OnAmbilWarnaListener {
                    override fun onCancel(dialog: AmbilWarnaDialog?) {
                        dialog?.dialog?.dismiss()
                    }

                    override fun onOk(dialog: AmbilWarnaDialog?, color: Int) {
                        Log.d("colorPicker", "color selected is: $color")
                        viewModel.updateColor(Color.valueOf(color))
                        dialog?.dialog?.dismiss()
                        mView!!.onPause()
                        mView!!.onResume()
                    }
                }
            )
            colorPickerDialog.show()
        }

        /*saveButton.setOnClickListener {
            // Screen buffer capture
            viewModel.getScreenBuffer = 1
        }

        saveAsButton.setOnClickListener {
            // Screen buffer capture
            viewModel.getScreenBuffer = 1
            viewModel.saveAsNew = 1
        }*/

        // setup equation editor
        viewModel.updateEquation(equationEditor.text.toString())
        equationEditor.addTextChangedListener(object : TextWatcher {
            override fun beforeTextChanged(s: CharSequence?, start: Int, count: Int, after: Int) {
                // Do nothing
            }

            override fun onTextChanged(s: CharSequence?, start: Int, before: Int, count: Int) {
                updateSyntaxResult()
            }

            override fun afterTextChanged(s: android.text.Editable?) {
                // Do nothing
            }
        })

        // setup listener for the Done button
        equationEditor.setOnEditorActionListener { v, actionId, event ->
            if (actionId == EditorInfo.IME_ACTION_DONE) {
                // update equation in repo if valid
                val equationChecker: EquationChecker = EquationChecker()
                val result: String =
                    equationChecker.checkEquationSyntax(equationEditor.text.toString())
                //val result2: String = checkEquationSyntax2(equationEditor.text.toString())
                Log.d("LiveWallpaper05", "result is: $result")
                //Log.d("LiveWallpaper05", "result2 is: " + result2)
                if (result == "") {
                    viewModel.updateEquation(equationEditor.text.toString())
                    Log.d("LiveWallpaper05", "Syntax check passed.")
                } else {
                    viewModel.updateEquation(getString(R.string.default_equation))
                    Log.d("LiveWallpaper05", "Syntax check failed.")
                }
                mView!!.onPause()
                mView!!.onResume()
                return@setOnEditorActionListener true // Return true to consume the event
            }
            return@setOnEditorActionListener false // Return false if you want to allow further handling of the event
        }

        // connect fps data to ui fps meter
        val fpsMeter = findViewById<TextView>(R.id.tv_fps_meter)
        viewModel.getFPS().observe(this) {
            fpsMeter.text = String.format("fps: %.2f", it)
        }

        updateSyntaxResult()

        fun showUIComponents(){
            findViewById<Button>(R.id.hide_ui_button).text = "Hide UI"
            distanceSeekBar.visibility = View.VISIBLE
            fieldOfViewSeekBar.visibility = View.VISIBLE
            gravitySeekBar.visibility = View.VISIBLE
            linearAccelerationSeekBar.visibility = View.VISIBLE
            efficiencySeekBar.visibility = View.VISIBLE
            colorButton.visibility = View.VISIBLE
            //saveButton.visibility = View.VISIBLE
            //saveAsButton.visibility = View.VISIBLE
            equationEditor.visibility = View.VISIBLE
            linearLayout.isEnabled = true
            distanceSeekBar.isEnabled = true
            fieldOfViewSeekBar.isEnabled = true
            gravitySeekBar.isEnabled = true
            linearAccelerationSeekBar.isEnabled = true
            efficiencySeekBar.isEnabled = true
            colorButton.isEnabled = true
            //saveButton.isEnabled = true
            //saveAsButton.isEnabled = true
            equationEditor.isEnabled = true
            linearLayout.isEnabled = true
            findViewById<TextView>(R.id.distance_label).visibility = View.VISIBLE
            findViewById<TextView>(R.id.field_of_view_label).visibility = View.VISIBLE
            findViewById<TextView>(R.id.gravity_label).visibility = View.VISIBLE
            findViewById<TextView>(R.id.linear_acceleration_label).visibility = View.VISIBLE
            findViewById<TextView>(R.id.efficiency_label).visibility = View.VISIBLE
            findViewById<TextView>(R.id.tv_equation).visibility = View.VISIBLE
            findViewById<TextView>(R.id.tv_syntax_check).visibility = View.VISIBLE
        }

        fun hideUIComponents(){
            findViewById<Button>(R.id.hide_ui_button).text = "Show UI"
            distanceSeekBar.visibility = View.INVISIBLE
            fieldOfViewSeekBar.visibility = View.INVISIBLE
            gravitySeekBar.visibility = View.INVISIBLE
            linearAccelerationSeekBar.visibility = View.INVISIBLE
            efficiencySeekBar.visibility = View.INVISIBLE
            colorButton.visibility = View.INVISIBLE
            //saveButton.visibility = View.INVISIBLE
            //saveAsButton.visibility = View.INVISIBLE
            equationEditor.visibility = View.INVISIBLE
            distanceSeekBar.isEnabled = false
            fieldOfViewSeekBar.isEnabled = false
            gravitySeekBar.isEnabled = false
            linearAccelerationSeekBar.isEnabled = false
            efficiencySeekBar.isEnabled = false
            colorButton.isEnabled = false
            //saveButton.isEnabled = false
            //saveAsButton.isEnabled = false
            equationEditor.isEnabled = false
            findViewById<TextView>(R.id.distance_label).visibility = View.INVISIBLE
            findViewById<TextView>(R.id.field_of_view_label).visibility = View.INVISIBLE
            findViewById<TextView>(R.id.gravity_label).visibility = View.INVISIBLE
            findViewById<TextView>(R.id.linear_acceleration_label).visibility = View.INVISIBLE
            findViewById<TextView>(R.id.efficiency_label).visibility = View.INVISIBLE
            findViewById<TextView>(R.id.tv_equation).visibility = View.INVISIBLE
            findViewById<TextView>(R.id.tv_syntax_check).visibility = View.INVISIBLE
        }

        val animationListener = object : Animation.AnimationListener {
            override fun onAnimationStart(animation: Animation?) {
                // Enable the UI elements before restoring the layout
                if(!viewModel.isCollapsed){
                    showUIComponents()
                }
            }

            override fun onAnimationEnd(animation: Animation?) {
                // Disable the UI elements after collapsing the layout
                if (viewModel.isCollapsed) {
                    hideUIComponents()
                }
            }

            override fun onAnimationRepeat(animation: Animation?) {
                // Not needed, but required to override
            }
        }

        fun setDefaultAnimationParametersAndAnimate(animation: Animation){
            animation.duration = 500
            animation.fillAfter = true
            animation.setAnimationListener(animationListener)
            linearLayout.startAnimation(animation)
            viewModel.isCollapsed = !viewModel.isCollapsed
        }

        hideUIButton.setOnClickListener {
            val animation: Animation = if(!viewModel.isCollapsed){
                TranslateAnimation(0f, linearLayout.width.toFloat(), 0f, 0f)
            } else {
                TranslateAnimation(linearLayout.width.toFloat(), 0f, 0f, 0f)
            }
            setDefaultAnimationParametersAndAnimate(animation)
        }

        linearLayout.setOnClickListener {
            if(viewModel.isCollapsed){
                val animation: Animation = TranslateAnimation(linearLayout.width.toFloat(), 0f, 0f, 0f)
                setDefaultAnimationParametersAndAnimate(animation)
            }
        }

        if(viewModel.isCollapsed){
            hideUIComponents()
        }else{
            showUIComponents()
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

    /*private val colorActivity = registerForActivityResult(ActivityResultContracts.StartActivityForResult()) { result ->
        if (result.resultCode == RESULT_OK) {
            val data = result.data
            //val color = data?.extras?.get("data")
            val color = data?.getIntExtra("color", Color.WHITE)

            if (color == null) {
                return@registerForActivityResult
            }
            val trueColor = color.toColor()
            viewModel.updateColor(trueColor)
            mView!!.onPause()
            mView!!.onResume()
        }
    }

    fun updatePreviewImage(): Bitmap {
        // store view as preview image
        *//**
        var preview = Bitmap.createBitmap(mView!!.width, mView!!.height, Bitmap.Config.ARGB_8888)
        var canvas = Canvas(preview)
        mView!!.draw(canvas)*//*
        val preview = Bitmap.createBitmap(mView!!.width, mView!!.height, Bitmap.Config.ARGB_8888)
        val canvas = Canvas(preview)
        val background = mView!!.background
        if (background != null) {
            background.draw(canvas)
        }
        mView!!.draw(canvas)

        return preview
    }*/

    companion object {
        // Used to load the 'livewallpaper05' library on application startup.
        init {
            System.loadLibrary("livewallpaper05")
        }

        // register simulation functions as external and belonging to the livewallpaper05 library
        external fun init(visualization: String)
        external fun resize(width: Int, height: Int, orientation: Int)
        external fun step(
            acc_x: Float,
            acc_y: Float,
            acc_z: Float,
            rot_x: Float,
            rot_y: Float,
            rot_z: Float,
            rot_w: Float,
            linear_acc_x: Float,
            linear_acc_y: Float,
            linear_acc_z: Float,
            distance: Float,
            field_of_view: Float,
            gravity: Float,
            efficiency: Float
        )

        external fun getScreenBuffer(): ByteArray
    }
}