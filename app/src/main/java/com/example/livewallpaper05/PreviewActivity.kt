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
import android.view.inputmethod.EditorInfo
import android.widget.AdapterView
import android.widget.ArrayAdapter
import android.widget.Button
import android.widget.EditText
import android.widget.ImageView
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
import com.example.livewallpaper05.savedWallpapers.SavedWallpaperViewModel
import yuku.ambilwarna.AmbilWarnaDialog

class PreviewActivity : AppCompatActivity() {

    var mView: GLES3JNIView? = null

    private val viewModel: ActiveWallpaperViewModel by viewModels {
        ActiveWallpaperViewModelFactory((application as ActiveWallpaperApplication).repository)
    }

    // saved wallpaper data
    private val mSavedWallpaperViewModel: SavedWallpaperViewModel by viewModels {
        SavedWallpaperViewModel.SavedWallpaperViewModelFactory((application as ActiveWallpaperApplication).savedWallpaperRepo)
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

        // load saved wallpaper data
        // get config string from saved wallpaper
        val configTable = mSavedWallpaperViewModel.activeWallpaper.value ?: null
        // load saved string if param is empty
        if (configTable != null) {
            viewModel.loadConfig(configTable)
        }

        // get view window from GLES3JNIView
        mView = GLES3JNIView(application, viewModel)

        // grab ui element for preview page
        val layout = findViewById<LinearLayout>(R.id.render_layout)
        val distanceSeekBar = findViewById<SeekBar>(R.id.distance_seekbar)
        val fieldOfViewSeekBar = findViewById<SeekBar>(R.id.field_of_view_seekbar)
        val visualizationSelectorSpinner = findViewById<Spinner>(R.id.simulation_type_spinner)
        val colorButton = findViewById<Button>(R.id.b_color_picker)
        val equationEditor = findViewById<EditText>(R.id.et_equation)

        // fill sim selector box with wallpaper options from native-lib.cpp
        val simSelectorAdapter = ArrayAdapter.createFromResource(
            this,
            R.array.simulation_types,
            android.R.layout.simple_spinner_item
        )
        visualizationSelectorSpinner.adapter = simSelectorAdapter
        // set default to viewmodel simulation type
        visualizationSelectorSpinner.setSelection(viewModel.getVisualizationType())

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

        // register spinner actions to update simulation type in repo
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
            findViewById<ImageView>(R.id.imageView).setImageBitmap(bitmap)
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

            // Temporary signal to trigger screen buffer capture
            viewModel.getScreenBuffer = 1
        }

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
            fpsMeter.text = "fps = " + it.toString()
        }

        updateSyntaxResult()
    }

    /* this is run when the app is 'paused'
     * this includes leaving the app, rotating the screen, etc.
     */
    override fun onPause() {
        super.onPause()
        mView!!.onPause()

        viewModel.updatePreviewImg(updatePreviewImage())
        viewModel.saveConfig()
    }

    /* this is run when the app is 'resumed'
     * this includes returning to the app, rotating the screen, etc.
     */
    override fun onResume() {
        super.onResume()
        mView!!.onResume()
    }

    private val colorActivity =
        registerForActivityResult(ActivityResultContracts.StartActivityForResult()) { result ->
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
        /**
        var preview = Bitmap.createBitmap(mView!!.width, mView!!.height, Bitmap.Config.ARGB_8888)
        var canvas = Canvas(preview)
        mView!!.draw(canvas)*/
        val preview = Bitmap.createBitmap(mView!!.width, mView!!.height, Bitmap.Config.ARGB_8888)
        val canvas = Canvas(preview)
        val background = mView!!.background
        if (background != null) {
            background.draw(canvas)
        }
        mView!!.draw(canvas)

        return preview
    }

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
            field_of_view: Float
        )

        external fun getScreenBuffer(): ByteArray
    }
}