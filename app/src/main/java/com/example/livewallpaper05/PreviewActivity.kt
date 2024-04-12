package com.example.livewallpaper05

import android.graphics.Color
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
import android.widget.CheckBox
import android.widget.EditText
import android.widget.ImageView
import android.widget.LinearLayout
import android.widget.RadioButton
import android.widget.RadioGroup
import android.widget.ScrollView
import android.widget.SeekBar
import android.widget.Spinner
import android.widget.TextView
import androidx.activity.viewModels
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.Observer
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperApplication
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModel
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModelFactory
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import yuku.ambilwarna.AmbilWarnaDialog

class PreviewActivity : AppCompatActivity() {
    var mView: GLES3JNIView? = null

    private val viewModel: ActiveWallpaperViewModel by viewModels {
        ActiveWallpaperViewModelFactory((application as ActiveWallpaperApplication).wallpaperRepo)
    }

    private fun updateSyntaxResult() {
        // update syntax check message
        val equationChecker = EquationChecker()
        val result: String =
            equationChecker.checkEquationSyntax(findViewById<EditText>(R.id.et_equation).text.toString())
        val message: String = if (result == "") {
            "No syntax errors."
        } else {
            result
        }
        findViewById<TextView>(R.id.tv_syntax_check).text = message
    }

    @RequiresApi(Build.VERSION_CODES.R)
    override fun onCreate(icicle: Bundle?) {
        super.onCreate(icicle)
        window.setFlags(
            WindowManager.LayoutParams.FLAG_FULLSCREEN,
            WindowManager.LayoutParams.FLAG_FULLSCREEN
        );
        setContentView(R.layout.activity_preview)
        // get view window from GLES3JNIView
        mView = GLES3JNIView(application, viewModel)

        // grab ui element for preview page
        val layout: LinearLayout = findViewById<LinearLayout>(R.id.render_layout)
        val distanceSeekBar: SeekBar = findViewById<SeekBar>(R.id.distance_seekbar)
        val fieldOfViewSeekBar: SeekBar = findViewById<SeekBar>(R.id.field_of_view_seekbar)
        val gravitySeekBar: SeekBar = findViewById<SeekBar>(R.id.gravity_seekbar)
        val linearAccelerationSeekBar: SeekBar =
            findViewById<SeekBar>(R.id.linear_acceleration_seekbar)
        val efficiencySeekBar: SeekBar = findViewById<SeekBar>(R.id.efficiency_seekbar)
        val visualizationSelectorSpinner: Spinner =
            findViewById<Spinner>(R.id.visualization_type_spinner)
        val environmentMapSelectorSpinner: Spinner =
            findViewById<Spinner>(R.id.image_selection_spinner)
        val graphSelectorSpinner: Spinner =
            findViewById<Spinner>(R.id.graph_selection_spinner)
        val colorButton: Button = findViewById<Button>(R.id.b_color_picker)
        val hideUIButton: Button = findViewById<Button>(R.id.hide_ui_button)
        val saveButton: Button = findViewById<Button>(R.id.save_button)
        val syncButton: Button = findViewById<Button>(R.id.sync_button)
        val equationEditor: EditText = findViewById<EditText>(R.id.et_equation)
        val flipsNormalsCheckBox: CheckBox = findViewById<CheckBox>(R.id.flip_normals_checkbox)
        val linearLayout: LinearLayout = findViewById<LinearLayout>(R.id.settings_linearlayout)
        val doneButton: Button = findViewById<Button>(R.id.done_button)
        val backgroundRadioButton: RadioGroup =
            findViewById<RadioGroup>(R.id.background_radio_group)
        val solidColorRadioButton: RadioButton =
            findViewById<RadioButton>(R.id.solid_color_radio_button)
        val imageRadioButton: RadioButton = findViewById<RadioButton>(R.id.image_radio_button)

        // fill visualization selector box with wallpaper options from native-lib.cpp
        val visualizationSelectorAdapter = ArrayAdapter.createFromResource(
            this,
            R.array.visualization_types,
            android.R.layout.simple_spinner_item
        )
        visualizationSelectorSpinner.adapter = visualizationSelectorAdapter
        // set default to viewmodel visualization type
        visualizationSelectorSpinner.setSelection(viewModel.getVisualization())

        // fill image selector box with image options from native-lib.cpp
        val environmentMapSelectorAdapter = ArrayAdapter.createFromResource(
            this,
            R.array.environment_map_options,
            android.R.layout.simple_spinner_item
        )
        environmentMapSelectorSpinner.adapter = environmentMapSelectorAdapter
        // set default to viewmodel visualization type
        //environmentMapSelectorSpinner.setSelection(viewModel.getEnvironmentMap())
        val graphNames = arrayOf("")
        // fill image selector box with image options from native-lib.cpp
        val graphSelectorAdapter = ArrayAdapter.createFromResource(
            this,
            R.array.graph_names,
            android.R.layout.simple_spinner_item
        )
        graphSelectorSpinner.adapter = graphSelectorAdapter
        graphSelectorSpinner.setSelection(viewModel.getGraph())

        // add gl engine view to viewport
        layout.addView(mView)

        suspend fun loadOrUnloadUIElements() {
            // if repo.visualization is not initialized wait until it is
            val textViewIds: List<Int> = listOf(
                R.id.distance_label,
                R.id.field_of_view_label,
                R.id.gravity_label,
                R.id.linear_acceleration_label,
                R.id.efficiency_label,
                R.id.tv_equation,
                R.id.tv_syntax_check
            )
            val seekBarIds: List<Int> = listOf(
                R.id.distance_seekbar,
                R.id.field_of_view_seekbar,
                R.id.gravity_seekbar,
                R.id.linear_acceleration_seekbar,
                R.id.efficiency_seekbar
            )
            val editTextIds: List<Int> = listOf(
                R.id.et_equation
            )
            val checkBoxIds: List<Int> = listOf(
                R.id.flip_normals_checkbox
            )
            val buttonIds: List<Int> = listOf(
                R.id.done_button
            )
            val spinnerIds: List<Int> = listOf(
                R.id.graph_selection_spinner
            )
            for (id in textViewIds) {
                if (viewModel.repo.visualization.relevantTextViewIds.contains(id) && !viewModel.repo.isCollapsed) {
                    findViewById<TextView>(id).visibility = View.VISIBLE
                } else {
                    findViewById<TextView>(id).visibility = View.GONE
                }
            }
            for (id in seekBarIds) {
                if (viewModel.repo.visualization.relevantSeekBarIds.contains(id) && !viewModel.repo.isCollapsed) {
                    findViewById<SeekBar>(id).visibility = View.VISIBLE
                    findViewById<SeekBar>(id).isEnabled = true
                } else {
                    findViewById<SeekBar>(id).visibility = View.GONE
                    findViewById<SeekBar>(id).isEnabled = false
                }
            }
            for (id in editTextIds) {
                if (viewModel.repo.visualization.relevantEditTextIds.contains(id) && !viewModel.repo.isCollapsed) {
                    findViewById<EditText>(id).visibility = View.VISIBLE
                    findViewById<EditText>(id).isEnabled = true
                } else {
                    findViewById<EditText>(id).visibility = View.GONE
                    findViewById<EditText>(id).isEnabled = false
                }
            }
            for (id in checkBoxIds) {
                if (viewModel.repo.visualization.relevantCheckBoxIds.contains(id) && !viewModel.repo.isCollapsed) {
                    findViewById<CheckBox>(id).visibility = View.VISIBLE
                    findViewById<CheckBox>(id).isEnabled = true
                } else {
                    findViewById<CheckBox>(id).visibility = View.GONE
                    findViewById<CheckBox>(id).isEnabled = false
                }
            }
            for (id in buttonIds) {
                if (viewModel.repo.visualization.relevantButtonIds.contains(id) && !viewModel.repo.isCollapsed) {
                    findViewById<Button>(id).visibility = View.VISIBLE
                    findViewById<Button>(id).isEnabled = true
                } else {
                    findViewById<Button>(id).visibility = View.GONE
                    findViewById<Button>(id).isEnabled = false
                }
            }
            for (id in spinnerIds) {
                if (viewModel.repo.visualization.relevantSpinnerIds.contains(id) && !viewModel.repo.isCollapsed) {
                    findViewById<Spinner>(id).visibility = View.VISIBLE
                    findViewById<Spinner>(id).isEnabled = true
                } else {
                    findViewById<Spinner>(id).visibility = View.GONE
                    findViewById<Spinner>(id).isEnabled = false
                }
            }
        }

        // update orientation in repo
        try {
            viewModel.updateOrientation(this.display!!.rotation)
            CoroutineScope(Dispatchers.Main).launch {
                loadOrUnloadUIElements()
            }
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

        viewModel.repo.distanceFromOrigin.observe(this) { float ->
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

        viewModel.repo.fieldOfView.observe(this) { float ->
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

        viewModel.repo.gravity.observe(this) { float ->
            gravitySeekBar.progress = (float * 100.0f).toInt()
        }

        // register seekbar actions to update linear acceleration in repo
        linearAccelerationSeekBar.setOnSeekBarChangeListener(object :
            SeekBar.OnSeekBarChangeListener {
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

        viewModel.repo.linearAcceleration.observe(this) { float ->
            linearAccelerationSeekBar.progress = (float * 100.0f).toInt()
        }

        // register seekbar actions to update efficiency in repo
        efficiencySeekBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar, progress: Int, fromUser: Boolean) {
                // Do nothing until changes are stopped for smooth ui updates
                viewModel.updateEfficiency(seekBar.progress.toFloat() / 4000.0f)
            }

            override fun onStartTrackingTouch(seekBar: SeekBar) {
                // Do nothing when changes are started
            }

            override fun onStopTrackingTouch(seekBar: SeekBar) {
                viewModel.updateEfficiency(seekBar.progress.toFloat() / 4000.0f)
            }
        })

        viewModel.repo.efficiency.observe(this) { float ->
            efficiencySeekBar.progress = (float * 4000.0f).toInt()
        }

        // register spinner actions to update visualization type in repo
        visualizationSelectorSpinner.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
            override fun onItemSelected(
                parent: AdapterView<*>,
                view: View?,
                pos: Int,
                id: Long
            ) {
                val changed = viewModel.updateVisualizationSelection(pos)
                if (changed) {
                    // update visualization in repo
                    viewModel.updateSimulationType(parent.getItemAtPosition(pos).toString())
                    // tell view it needs to be reloaded
                    mView!!.onPause()
                    mView!!.onResume()
                }
                // Dynamically load or remove UI components
                CoroutineScope(Dispatchers.Main).launch {
                    // Wait until viewModel.visualization is not null
                    while (viewModel.repo.visualization == null) {
                        // Suspend the coroutine for a short duration to avoid blocking the main thread
                        delay(10)
                    }
                    loadOrUnloadUIElements()
                }
            }

            override fun onNothingSelected(parent: AdapterView<*>) {
            // Do nothing
            }
        }

        graphSelectorSpinner.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
            override fun onItemSelected(
                parent: AdapterView<*>,
                view: View?,
                position: Int,
                id: Long
            ) {
                val changed = viewModel.updateGraphSelection(position)
                if (changed) {
                    if (position == 0){
                        viewModel.repo.currentEquation = viewModel.repo.userDefinedEquation
                        // enable the edit text
                        equationEditor.isEnabled = true
                    }
                    else{
                        viewModel.repo.currentEquation = resources.getStringArray(R.array.graph_options)[position]
                        // disable the edit text
                        equationEditor.isEnabled = false
                    }
                    // set text of equation editor
                    equationEditor.setText(viewModel.repo.currentEquation)
                }
                // tell view it needs to be reloaded
                mView!!.onPause()
                mView!!.onResume()
            }

            override fun onNothingSelected(parent: AdapterView<*>?) {
                TODO("Not yet implemented")
            }

        }

        solidColorRadioButton.setOnClickListener {
            viewModel.repo.backgroundIsSolidColor.postValue(true)
            mView!!.onPause()
            mView!!.onResume()
        }

        imageRadioButton.setOnClickListener {
            viewModel.repo.backgroundIsSolidColor.postValue(false)
            mView!!.onPause()
            mView!!.onResume()
        }

        environmentMapSelectorSpinner.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
            override fun onItemSelected(parent: AdapterView<*>, view: View?, pos: Int, id: Long) {
                // get array from arrays
                val environmentMapOptions = resources.getStringArray(R.array.environment_map_options)
                // update environment map in repo
                when (pos){
                    0 -> viewModel.repo.backgroundTexture.value = "ms_paint_colors"
                    1 -> viewModel.repo.backgroundTexture.value = "mandelbrot"
                }
                mView!!.onPause()
                mView!!.onResume()
            }

            override fun onNothingSelected(parent: AdapterView<*>) {
                // Do nothing
            }
        }

        // register spinner actions to update image selection in repo
        /*environmentMapSelectorSpinner.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
            override fun onItemSelected(parent: AdapterView<*>, view: View?, pos: Int, id: Long) {
                val changed = viewModel.updateEnvironmentMapSelection(pos)
                if (changed) {
                    // tell view it needs to be reloaded
                    mView!!.onPause()
                    mView!!.onResume()
                }

                // Dynamically load or remove UI components
                CoroutineScope(Dispatchers.Main).launch {
                    // Wait until viewModel.visualization is not null
                    while (viewModel.visualization == null) {
                        // Suspend the coroutine for a short duration to avoid blocking the main thread
                        delay(10)
                    }
                    loadOrUnloadUIElements()
                }
            }

            override fun onNothingSelected(parent: AdapterView<*>) {
                // Do nothing
            }
        }*/

        // Observe changes to liveDataBitmap in the ViewModel
        viewModel.repo.liveDataBitmap.observe(this, Observer { bitmap ->
            // This code will be executed on the main (UI) thread whenever liveDataBitmap changes
            findViewById<ImageView>(R.id.imageView).setImageBitmap(bitmap)
            viewModel.updatePreviewImg(bitmap)
        })

        // setup color selection dialog
        colorButton.setOnClickListener {
            val colorPickerDialog = AmbilWarnaDialog(
                this,
                viewModel.repo.rememberColorPickerValue,
                object : AmbilWarnaDialog.OnAmbilWarnaListener {
                    override fun onCancel(dialog: AmbilWarnaDialog?) {
                        dialog?.dialog?.dismiss()
                    }

                    override fun onOk(dialog: AmbilWarnaDialog?, color: Int) {
                        viewModel.repo.rememberColorPickerValue = color
                        viewModel.updateColor(Color.valueOf(color))
                        dialog?.dialog?.dismiss()
                        mView!!.onPause()
                        mView!!.onResume()
                    }
                }
            )
            colorPickerDialog.show()
        }

        saveButton.setOnClickListener {
            // Screen buffer capture
            viewModel.repo.getScreenBuffer = 1
        }

        syncButton.setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                viewModel.repo.synchronizeWithServer(viewModel.repo.uid)
                viewModel.repo.synchronizeWithServer(viewModel.repo.uid)
            }
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

        fun doneButton() {
            //val currentFocusView = currentFocus
            //if (currentFocusView != null) {
            //    (getSystemService(Context.INPUT_METHOD_SERVICE) as InputMethodManager).hideSoftInputFromWindow(currentFocusView.windowToken, 0)
            //}
            // update equation in repo if valid
            val equationChecker: EquationChecker = EquationChecker()
            val result: String = equationChecker.checkEquationSyntax(equationEditor.text.toString())
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
        }

        // setup listener for the Done button
        equationEditor.setOnEditorActionListener { v, actionId, event ->
            if (actionId == EditorInfo.IME_ACTION_DONE) {
                doneButton()
                return@setOnEditorActionListener true // Return true to consume the event
            }
            return@setOnEditorActionListener false // Return false if you want to allow further handling of the event
        }

        doneButton.setOnClickListener {
            doneButton()
        }

        // connect fps data to ui fps meter
        val fpsMeter = findViewById<TextView>(R.id.tv_fps_meter)
        viewModel.getFPS().observe(this) {
            fpsMeter.text = String.format("fps: %.2f", it)
        }

        updateSyntaxResult()

        fun showUIComponents() {
            hideUIButton.text = resources.getString(R.string.hideUIButtonText)
            colorButton.visibility = View.VISIBLE
            backgroundRadioButton.visibility = View.VISIBLE
            solidColorRadioButton.visibility = View.VISIBLE
            imageRadioButton.visibility = View.VISIBLE
            findViewById<TextView>(R.id.background_label).visibility = View.VISIBLE
            environmentMapSelectorSpinner.visibility = View.VISIBLE
            syncButton.visibility = View.VISIBLE
            saveButton.visibility = View.VISIBLE
            colorButton.isEnabled = true
            linearLayout.isEnabled = true
            backgroundRadioButton.isEnabled = true
            solidColorRadioButton.isEnabled = true
            imageRadioButton.isEnabled = true
            environmentMapSelectorSpinner.isEnabled = true
            syncButton.isEnabled = true
            saveButton.isEnabled = true
            CoroutineScope(Dispatchers.Main).launch {
                loadOrUnloadUIElements()
            }
        }

        fun hideUIComponents() {
            hideUIButton.text = resources.getString(R.string.showUIButtonText)
            colorButton.visibility = View.GONE
            solidColorRadioButton.visibility = View.GONE
            imageRadioButton.visibility = View.GONE
            backgroundRadioButton.visibility = View.GONE
            findViewById<TextView>(R.id.background_label).visibility = View.GONE
            environmentMapSelectorSpinner.visibility = View.GONE
            syncButton.visibility = View.GONE
            saveButton.visibility = View.GONE
            colorButton.isEnabled = false
            linearLayout.isEnabled = false
            solidColorRadioButton.isEnabled = false
            imageRadioButton.isEnabled = false
            backgroundRadioButton.isEnabled = false
            environmentMapSelectorSpinner.isEnabled = false
            syncButton.isEnabled = false
            saveButton.isEnabled = false
            CoroutineScope(Dispatchers.Main).launch {
                loadOrUnloadUIElements()
            }
        }

        val animationListener = object : Animation.AnimationListener {
            override fun onAnimationStart(animation: Animation?) {
                // Enable the UI elements before restoring the layout
                if (!viewModel.repo.isCollapsed) {
                    showUIComponents()
                }
            }

            override fun onAnimationEnd(animation: Animation?) {
                // Disable the UI elements after collapsing the layout
                if (viewModel.repo.isCollapsed) {
                    hideUIComponents()
                }
            }

            override fun onAnimationRepeat(animation: Animation?) {
                // Not needed, but required to override
            }
        }

        fun setDefaultAnimationParametersAndAnimate(animation: Animation) {
            animation.duration = 500
            animation.fillAfter = true
            animation.setAnimationListener(animationListener)
            linearLayout.startAnimation(animation)
            viewModel.repo.isCollapsed = !viewModel.repo.isCollapsed
        }

        hideUIButton.setOnClickListener {
            val animation: Animation = if (!viewModel.repo.isCollapsed) {
                TranslateAnimation(0f, linearLayout.width.toFloat(), 0f, 0f)
            } else {
                TranslateAnimation(linearLayout.width.toFloat(), 0f, 0f, 0f)
            }
            setDefaultAnimationParametersAndAnimate(animation)
        }

        // Scroll the ScrollView to the bottom when the layout changes (e.g., keyboard shown/hidden)
        findViewById<ScrollView>(R.id.settings_scrollview).viewTreeObserver.addOnGlobalLayoutListener {
            findViewById<ScrollView>(R.id.settings_scrollview).post {
                findViewById<ScrollView>(R.id.settings_scrollview).fullScroll(View.FOCUS_DOWN)
            }
        }

        // Scroll the ScrollView to the bottom when the EditText gains focus
        equationEditor.setOnFocusChangeListener { _, hasFocus ->
            if (hasFocus) {
                findViewById<ScrollView>(R.id.settings_scrollview).post {
                    findViewById<ScrollView>(R.id.settings_scrollview).fullScroll(View.FOCUS_DOWN)
                }
            }
        }

        flipsNormalsCheckBox.setOnCheckedChangeListener { buttonView, isChecked ->
            viewModel.repo.flipNormals.value = isChecked
        }

        if (viewModel.repo.isCollapsed) {
            hideUIComponents()
        } else {
            showUIComponents()
        }
    }

    /* this is run when the app is 'paused'
     * this includes leaving the app, rotating the screen, etc.
     */
    override fun onPause() {
        // save current wallpaper
        val activeConfig = viewModel.getConfig()
        viewModel.saveWallpaper(activeConfig)
        // pause activity
        super.onPause()
        mView!!.onPause()
    }

    // this is run when the activity is 'stopped', like when the app is closed or restarted
    override fun onStop() {
        // save current wallpaper
        val activeConfig = viewModel.getConfig()
        viewModel.saveWallpaper(activeConfig)

        super.onStop()
    }

    // this is run when the activity is 'destroyed', like when the app is shut down or device is restarted
    override fun onDestroy() {
        // save current wallpaper
        val activeConfig = viewModel.getConfig()
        viewModel.saveWallpaper(activeConfig)

        super.onDestroy()
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
            efficiency: Float,
            flip_normals: Boolean
        )

        external fun getScreenBuffer(): ByteArray
    }
}