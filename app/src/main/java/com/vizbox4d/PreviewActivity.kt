package com.vizbox4d

import android.content.Context
import android.graphics.Color
import android.os.Build
import android.os.Bundle
import android.os.Handler
import android.text.TextWatcher
import android.util.Log
import android.view.KeyEvent
import android.view.View
import android.view.ViewTreeObserver
import android.view.WindowManager
import android.view.animation.Animation
import android.view.animation.TranslateAnimation
import android.view.inputmethod.EditorInfo
import android.view.inputmethod.InputMethodManager
import android.widget.AdapterView
import android.widget.ArrayAdapter
import android.widget.Button
import android.widget.CheckBox
import android.widget.EditText
import android.widget.LinearLayout
import android.widget.RadioButton
import android.widget.RadioGroup
import android.widget.SeekBar
import android.widget.Spinner
import android.widget.TextView
import androidx.activity.viewModels
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import com.vizbox4d.activewallpaperdata.ActiveWallpaperApplication
import com.vizbox4d.activewallpaperdata.ActiveWallpaperRepo
import com.vizbox4d.activewallpaperdata.ActiveWallpaperViewModel
import com.vizbox4d.activewallpaperdata.ActiveWallpaperViewModelFactory
import yuku.ambilwarna.AmbilWarnaDialog

class PreviewActivity : AppCompatActivity() {
    private var mView: GLES3JNIView? = null

    private val viewModel: ActiveWallpaperViewModel by viewModels {
        ActiveWallpaperViewModelFactory((application as ActiveWallpaperApplication).wallpaperRepo)
    }

    private val layout: LinearLayout by lazy { findViewById<LinearLayout>(R.id.render_layout) }
    private val fpsMeter: TextView by lazy { findViewById<TextView>(R.id.tv_fps_meter) }
    private val syntaxCheck: TextView by lazy { findViewById<TextView>(R.id.tv_syntax_check) }
    private val distanceSeekBar: SeekBar by lazy { findViewById<SeekBar>(R.id.distance_seekbar) }
    private val fieldOfViewSeekBar: SeekBar by lazy { findViewById<SeekBar>(R.id.field_of_view_seekbar) }
    private val gravitySeekBar: SeekBar by lazy { findViewById<SeekBar>(R.id.gravity_seekbar) }
    private val linearAccelerationSeekBar: SeekBar by lazy { findViewById<SeekBar>(R.id.linear_acceleration_seekbar) }
    private val efficiencySeekBar: SeekBar by lazy { findViewById<SeekBar>(R.id.efficiency_seekbar) }
    private val visualizationSelectorSpinner: Spinner by lazy { findViewById<Spinner>(R.id.visualization_type_spinner) }
    private val environmentMapSelectorSpinner: Spinner by lazy { findViewById<Spinner>(R.id.image_selection_spinner) }
    private val colorButton: Button by lazy { findViewById<Button>(R.id.b_color_picker) }
    private val hideUIButton: Button by lazy { findViewById<Button>(R.id.hide_ui_button) }
    private val newButton: Button by lazy { findViewById<Button>(R.id.new_button) }
    private val copyButton: Button by lazy { findViewById<Button>(R.id.copy_button) }
    private val deleteButton: Button by lazy { findViewById<Button>(R.id.delete_button) }
    private val equationNameEditText: EditText by lazy { findViewById<EditText>(R.id.et_equation_name) }
    private val equationValueEditText: EditText by lazy { findViewById<EditText>(R.id.et_equation_value) }
    private val flipsNormalsCheckBox: CheckBox by lazy { findViewById<CheckBox>(R.id.flip_normals_checkbox) }
    private val fluidSurfaceCheckBox: CheckBox by lazy { findViewById<CheckBox>(R.id.fluid_surface_checkbox) }
    private val gyroscopeCompensationCheckBox: CheckBox by lazy { findViewById<CheckBox>(R.id.gyroscope_compensation_checkbox) }
    private val smoothSphereSurfaceCheckBox: CheckBox by lazy { findViewById<CheckBox>(R.id.smooth_sphere_surface_checkbox) }
    private val linearLayout: LinearLayout by lazy { findViewById<LinearLayout>(R.id.settings_linearlayout) }
    private val backgroundRadioGroup: RadioGroup by lazy { findViewById<RadioGroup>(R.id.background_radio_group) }
    private val backgroundLabelTextView: TextView by lazy { findViewById<TextView>(R.id.background_label) }
    private val preferredGraphListRadioGroup: RadioGroup by lazy { findViewById<RadioGroup>(R.id.equation_radio_group) }
    private val solidColorRadioButton: RadioButton by lazy { findViewById<RadioButton>(R.id.solid_color_radio_button) }
    private val imageRadioButton: RadioButton by lazy { findViewById<RadioButton>(R.id.image_radio_button) }
    private val defaultEquationsRadioButton: RadioButton by lazy { findViewById<RadioButton>(R.id.default_equations_radio_button) }
    private val savedEquationsRadioButton: RadioButton by lazy { findViewById<RadioButton>(R.id.saved_equations_radio_button) }
    private val defaultGraphsSpinner: Spinner by lazy { findViewById<Spinner>(R.id.default_graph_selection_spinner) }
    private val savedGraphsSpinner: Spinner by lazy { findViewById<Spinner>(R.id.saved_graph_selection_spinner) }

    private var editTextsChangingViaKeyboard = true

    private fun updateSyntaxResult() {
        // update syntax check message
        val equationChecker = EquationChecker()
        val result: String = equationChecker.checkEquationSyntax(equationValueEditText.text.toString())
        val message: String = if (result == "") {
            "No syntax errors."
        } else {
            result
        }
        syntaxCheck.text = message
    }

    private fun loadOrUnloadUIElements() {
        val textViewIds: List<Int> = listOf(
            R.id.distance_label,
            R.id.field_of_view_label,
            R.id.gravity_label,
            R.id.linear_acceleration_label,
            R.id.efficiency_label,
            R.id.equation_selection_label,
            R.id.tv_equation_name,
            R.id.tv_equation_value,
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
            R.id.et_equation_name,
            R.id.et_equation_value
        )
        val checkBoxIds: List<Int> = listOf(
            R.id.flip_normals_checkbox,
            R.id.fluid_surface_checkbox,
            R.id.gyroscope_compensation_checkbox,
            R.id.smooth_sphere_surface_checkbox
        )
        val spinnerIds: List<Int> = listOf(
            R.id.default_graph_selection_spinner,
            R.id.saved_graph_selection_spinner,
            R.id.image_selection_spinner
        )
        val buttonIds: List<Int> = listOf(
            R.id.new_button,
            R.id.copy_button,
            R.id.delete_button
        )
        val radioButtonIds: List<Int> = listOf(
            R.id.solid_color_radio_button,
            R.id.image_radio_button,
            R.id.default_equations_radio_button,
            R.id.saved_equations_radio_button
        )
        val radioGroupIds: List<Int> = listOf(
            R.id.background_radio_group,
            R.id.equation_radio_group
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
                if((id != R.id.et_equation_name && id != R.id.et_equation_value) || viewModel.repo.preferredGraphList != 0) {
                    findViewById<EditText>(id).isEnabled = true
                }
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
        for (id in spinnerIds) {
            if (viewModel.repo.visualization.relevantSpinnerIds.contains(id) && !viewModel.repo.isCollapsed) {
                findViewById<Spinner>(id).visibility = View.VISIBLE
                if(
                    (id == R.id.default_graph_selection_spinner && viewModel.repo.preferredGraphList == 0) ||
                    (id == R.id.saved_graph_selection_spinner && viewModel.repo.preferredGraphList == 1) ||
                    id == R.id.image_selection_spinner
                ){
                    findViewById<Spinner>(id).isEnabled = true
                }
            } else {
                findViewById<Spinner>(id).visibility = View.GONE
                findViewById<Spinner>(id).isEnabled = false
            }
        }
        for (id in buttonIds) {
            if (viewModel.repo.visualization.relevantButtonIds.contains(id) && !viewModel.repo.isCollapsed) {
                findViewById<Button>(id).visibility = View.VISIBLE
                if(id != R.id.delete_button || viewModel.repo.preferredGraphList != 0) {
                    findViewById<Button>(id).isEnabled = true
                }
            } else {
                findViewById<Button>(id).visibility = View.GONE
                findViewById<Button>(id).isEnabled = false
            }
        }
        for (id in radioButtonIds) {
            if (viewModel.repo.visualization.relevantRadioButtonIds.contains(id) && !viewModel.repo.isCollapsed) {
                findViewById<RadioButton>(id).visibility = View.VISIBLE
                findViewById<RadioButton>(id).isEnabled = true
            } else {
                findViewById<RadioButton>(id).visibility = View.GONE
                findViewById<RadioButton>(id).isEnabled = false
            }
        }
        for (id in radioGroupIds) {
            if (viewModel.repo.visualization.relevantRadioGroupIds.contains(id) && !viewModel.repo.isCollapsed) {
                findViewById<RadioGroup>(id).visibility = View.VISIBLE
                findViewById<RadioGroup>(id).isEnabled = true
            } else {
                findViewById<RadioGroup>(id).visibility = View.GONE
                findViewById<RadioGroup>(id).isEnabled = false
            }
        }
    }

    private fun doneButton(): Runnable {
        return Runnable {
            viewModel.repo.updateEquation(viewModel.repo.savedEquationSelection, equationNameEditText.text.toString(), equationValueEditText.text.toString())
            viewModel.repo.updateSavedEquations()
            Log.d("onPause,onResume", "doneButton()")
            mView!!.onPause()
            mView!!.onResume()
        }
    }

    private fun showUIComponents() {
        hideUIButton.text = resources.getString(R.string.hideUIButtonText)
        colorButton.visibility = View.VISIBLE
        newButton.visibility = View.VISIBLE
        copyButton.visibility = View.VISIBLE
        deleteButton.visibility = View.VISIBLE
        backgroundRadioGroup.visibility = View.VISIBLE
        solidColorRadioButton.visibility = View.VISIBLE
        imageRadioButton.visibility = View.VISIBLE
        backgroundLabelTextView.visibility = View.VISIBLE
        environmentMapSelectorSpinner.visibility = View.VISIBLE
        defaultEquationsRadioButton.visibility = View.VISIBLE
        savedEquationsRadioButton.visibility = View.VISIBLE
        preferredGraphListRadioGroup.visibility = View.VISIBLE
        flipsNormalsCheckBox.visibility = View.VISIBLE
        fluidSurfaceCheckBox.visibility = View.VISIBLE
        gyroscopeCompensationCheckBox.visibility = View.VISIBLE
        smoothSphereSurfaceCheckBox.visibility = View.VISIBLE
        colorButton.isEnabled = true
        newButton.isEnabled = true
        copyButton.isEnabled = true
        if(viewModel.repo.preferredGraphList == 1) {
            deleteButton.isEnabled = true
        }
        linearLayout.isEnabled = true
        backgroundRadioGroup.isEnabled = true
        solidColorRadioButton.isEnabled = true
        imageRadioButton.isEnabled = true
        environmentMapSelectorSpinner.isEnabled = true
        defaultEquationsRadioButton.isEnabled = true
        savedEquationsRadioButton.isEnabled = true
        preferredGraphListRadioGroup.isEnabled = true
        flipsNormalsCheckBox.isEnabled = true
        fluidSurfaceCheckBox.isEnabled = true
        gyroscopeCompensationCheckBox.isEnabled = true
        smoothSphereSurfaceCheckBox.isEnabled = true
        loadOrUnloadUIElements()
    }

    private fun hideUIComponents() {
        hideUIButton.text = resources.getString(R.string.showUIButtonText)
        colorButton.visibility = View.GONE
        newButton.visibility = View.GONE
        copyButton.visibility = View.GONE
        deleteButton.visibility = View.GONE
        solidColorRadioButton.visibility = View.GONE
        imageRadioButton.visibility = View.GONE
        backgroundRadioGroup.visibility = View.GONE
        backgroundLabelTextView.visibility = View.GONE
        environmentMapSelectorSpinner.visibility = View.GONE
        defaultEquationsRadioButton.visibility = View.GONE
        savedEquationsRadioButton.visibility = View.GONE
        preferredGraphListRadioGroup.visibility = View.GONE
        flipsNormalsCheckBox.visibility = View.GONE
        fluidSurfaceCheckBox.visibility = View.GONE
        gyroscopeCompensationCheckBox.visibility = View.GONE
        smoothSphereSurfaceCheckBox.visibility = View.GONE
        colorButton.isEnabled = false
        newButton.isEnabled = false
        copyButton.isEnabled = false
        deleteButton.isEnabled = false
        linearLayout.isEnabled = false
        solidColorRadioButton.isEnabled = false
        imageRadioButton.isEnabled = false
        backgroundRadioGroup.isEnabled = false
        environmentMapSelectorSpinner.isEnabled = false
        defaultEquationsRadioButton.isEnabled = false
        savedEquationsRadioButton.isEnabled = false
        preferredGraphListRadioGroup.isEnabled = false
        flipsNormalsCheckBox.isEnabled = false
        fluidSurfaceCheckBox.isEnabled = false
        gyroscopeCompensationCheckBox.isEnabled = false
        smoothSphereSurfaceCheckBox.isEnabled = false
        loadOrUnloadUIElements()
    }

    private val animationListener = object : Animation.AnimationListener {
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

    private fun setDefaultAnimationParametersAndAnimate(animation: Animation) {
        animation.duration = 500
        animation.fillAfter = true
        animation.setAnimationListener(animationListener)
        linearLayout.startAnimation(animation)
        viewModel.repo.isCollapsed = !viewModel.repo.isCollapsed
        viewModel.repo.sharedPreferencesEditor.putBoolean("isCollapsed", viewModel.repo.isCollapsed)
        viewModel.repo.sharedPreferencesEditor.apply()
    }

    private fun populateSavedEquationNamesSpinner() {
        val savedEquationNames: MutableList<String> = mutableListOf()
        for (i in 0 until viewModel.repo.equationsJSONArray.length()) {
            savedEquationNames.add(viewModel.repo.getEquation(i).name)
        }
        savedGraphsSpinner.adapter = ArrayAdapter(
            this,
            android.R.layout.simple_spinner_dropdown_item,
            savedEquationNames
        )
    }

    @RequiresApi(Build.VERSION_CODES.R)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        @Suppress("DEPRECATION")
        window.setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN)

        setContentView(R.layout.activity_preview)

        // get view window from GLES3JNIView
        mView = GLES3JNIView(this@PreviewActivity, viewModel)

        // add gl engine view to viewport
        layout.addView(mView)
    }

    @RequiresApi(Build.VERSION_CODES.R)
    override fun onPostCreate(savedInstanceState: Bundle?) {
        super.onPostCreate(savedInstanceState)

        loadOrUnloadUIElements()

        // set default to viewmodel visualization type
        visualizationSelectorSpinner.setSelection(viewModel.getVisualization())

        environmentMapSelectorSpinner.setSelection(viewModel.repo.getEnvironmentMapSelection())

        viewModel.updateOrientation(this.display!!.rotation)

        // setup name editor
        equationNameEditText.setText(viewModel.repo.currentEquation.name)

        // setup equation editor
        equationValueEditText.setText(viewModel.repo.currentEquation.value)

        updateSyntaxResult()

        if (viewModel.repo.isCollapsed) {
            hideUIComponents()
        } else {
            showUIComponents()
        }

        populateSavedEquationNamesSpinner()

        defaultGraphsSpinner.setSelection(viewModel.repo.defaultEquationSelection)

        savedGraphsSpinner.setSelection(viewModel.repo.savedEquationSelection)

        when(viewModel.repo.preferredGraphList){
            0 -> {
                preferredGraphListRadioGroup.check(defaultEquationsRadioButton.id)
                defaultGraphsSpinner.isEnabled = true
                savedGraphsSpinner.isEnabled = false
                equationNameEditText.isEnabled = false
                equationValueEditText.isEnabled = false
                deleteButton.isEnabled = false
            }
            1 -> {
                preferredGraphListRadioGroup.check(savedEquationsRadioButton.id)
                savedGraphsSpinner.isEnabled = true
                defaultGraphsSpinner.isEnabled = false
                equationNameEditText.isEnabled = true
                equationValueEditText.isEnabled = true
                deleteButton.isEnabled = true
            }
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
                viewModel.updateDistanceFromCenter(seekBar.progress.toFloat() / 100.0f)
                viewModel.saveVisualizationState()
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
                viewModel.updateFieldOfView(seekBar.progress.toFloat() / 100.0f)
                viewModel.saveVisualizationState()
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
                viewModel.saveVisualizationState()
            }
        })

        viewModel.repo.gravity.observe(this) { float ->
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
                viewModel.saveVisualizationState()
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
                viewModel.saveVisualizationState()
            }
        })

        viewModel.repo.efficiency.observe(this) { float ->
            efficiencySeekBar.progress = (float * 4000.0f).toInt()
        }

        // register spinner actions to update visualization type in repo
        visualizationSelectorSpinner.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
            override fun onItemSelected(parent: AdapterView<*>, view: View?, pos: Int, id: Long) {
                val changed = viewModel.updateVisualizationSelection(pos)
                if (changed) {
                    // tell view it needs to be reloaded
                    Log.d("onPause,onResume", "visualizationSelectorSpinner.onItemSelectedListener")
                    mView!!.onPause()
                    mView!!.onResume()
                    loadOrUnloadUIElements()
                }
            }

            override fun onNothingSelected(parent: AdapterView<*>) {

            }
        }

        defaultGraphsSpinner.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
            override fun onItemSelected(parent: AdapterView<*>, view: View?, position: Int, id: Long) {
                if(position != viewModel.repo.defaultEquationSelection){
                    viewModel.repo.defaultEquationSelection = position
                    equationNameEditText.setText(resources.getStringArray(R.array.graph_names)[position])
                    equationValueEditText.setText(resources.getStringArray(R.array.graph_options)[position])
                    equationNameEditText.isEnabled = false
                    equationValueEditText.isEnabled = false
                    Log.d("onPause,onResume", "defaultGraphsSpinner.onItemSelectedListener")
                    mView!!.onPause()
                    mView!!.onResume()
                }
            }

            override fun onNothingSelected(parent: AdapterView<*>?) {

            }

        }

        savedGraphsSpinner.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
            override fun onItemSelected(parent: AdapterView<*>, view: View?, position: Int, id: Long) {
                if(position != viewModel.repo.savedEquationSelection){
                    val initialEquation: String = equationValueEditText.text.toString()
                    viewModel.repo.savedEquationSelection = position
                    equationNameEditText.setText(viewModel.repo.getEquation(position).name)
                    equationValueEditText.setText(viewModel.repo.getEquation(position).value)
                    equationNameEditText.isEnabled = true
                    equationValueEditText.isEnabled = true
                    if(viewModel.repo.currentEquation.value != initialEquation) {
                        Log.d("onPause,onResume", "savedGraphsSpinner.onItemSelectedListener")
                        mView!!.onPause()
                        mView!!.onResume()
                    }
                }
            }

            override fun onNothingSelected(parent: AdapterView<*>?) {

            }

        }

        solidColorRadioButton.setOnClickListener {
            viewModel.repo.backgroundIsSolidColor.value = true
            viewModel.saveVisualizationState()
            Log.d("onPause,onResume", "solidColorRadioButton.setOnClickListener")
            mView!!.onPause()
            mView!!.onResume()
        }

        imageRadioButton.setOnClickListener {
            viewModel.repo.backgroundIsSolidColor.value = false
            viewModel.saveVisualizationState()
            Log.d("onPause,onResume", "imageRadioButton.setOnClickListener")
            mView!!.onPause()
            mView!!.onResume()
        }

        viewModel.repo.backgroundIsSolidColor.observe(this) { isSolidColor ->
            if (isSolidColor) {
                backgroundRadioGroup.check(solidColorRadioButton.id)
            } else {
                backgroundRadioGroup.check(imageRadioButton.id)
            }
        }

        // register spinner actions to update image selection in repo
        environmentMapSelectorSpinner.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
            override fun onItemSelected(parent: AdapterView<*>, view: View?, pos: Int, id: Long) {
                val currentValue: Int = viewModel.repo.getEnvironmentMapSelection()
                if(pos != currentValue){
                    viewModel.repo.backgroundTexture.value = when(pos) {
                        0 -> "ms_paint_colors"
                        1 -> "mandelbrot"
                        2 -> "rgb_cube"
                        else -> throw IllegalArgumentException("Invalid environment map")
                    }
                    viewModel.saveVisualizationState()
                    Log.d("onPause,onResume", "environmentMapSelectorSpinner.onItemSelectedListener")
                    mView!!.onPause()
                    mView!!.onResume()
                }
            }

            override fun onNothingSelected(parent: AdapterView<*>) {
                // Do nothing
            }
        }

        // setup color selection dialog
        colorButton.setOnClickListener {
            val colorPickerDialog = AmbilWarnaDialog(
                this,
                viewModel.repo.rememberColorPickerValue,
                object : AmbilWarnaDialog.OnAmbilWarnaListener {
                    override fun onCancel(dialog: AmbilWarnaDialog?) {

                    }

                    override fun onOk(dialog: AmbilWarnaDialog?, color: Int) {
                        if(Color.valueOf(color) != viewModel.repo.color.value) {
                            viewModel.repo.rememberColorPickerValue = color
                            viewModel.repo.color.value = Color.valueOf(color)
                            viewModel.saveVisualizationState()
                            if(viewModel.repo.backgroundIsSolidColor.value!!) {
                                Log.d("onPause,onResume", "colorButton.setOnClickListener")
                                mView!!.onPause()
                                mView!!.onResume()
                            }
                        }
                    }
                }
            )
            colorPickerDialog.show()
        }

        defaultEquationsRadioButton.setOnClickListener {
            val initialEquation: String = equationValueEditText.text.toString()
            defaultGraphsSpinner.isEnabled = true
            savedGraphsSpinner.isEnabled = false
            editTextsChangingViaKeyboard = false
            equationNameEditText.setText(resources.getStringArray(R.array.graph_names)[viewModel.repo.defaultEquationSelection])
            equationValueEditText.setText(resources.getStringArray(R.array.graph_options)[viewModel.repo.defaultEquationSelection])
            editTextsChangingViaKeyboard = true
            equationNameEditText.isEnabled = false
            equationValueEditText.isEnabled = false
            deleteButton.isEnabled = false
            if(viewModel.repo.preferredGraphList != 0) {
                viewModel.repo.preferredGraphList = 0
                if(viewModel.repo.currentEquation.value != initialEquation) {
                    Log.d("onPause,onResume", "defaultEquationsRadioButton.setOnClickListener")
                    mView!!.onPause()
                    mView!!.onResume()
                }
            }
        }

        savedEquationsRadioButton.setOnClickListener {
            val initialEquation: String = equationValueEditText.text.toString()
            savedGraphsSpinner.isEnabled = true
            defaultGraphsSpinner.isEnabled = false
            editTextsChangingViaKeyboard = false
            equationNameEditText.setText(viewModel.repo.getEquation(viewModel.repo.savedEquationSelection).name)
            equationValueEditText.setText(viewModel.repo.getEquation(viewModel.repo.savedEquationSelection).value)
            editTextsChangingViaKeyboard = true
            equationNameEditText.isEnabled = true
            equationValueEditText.isEnabled = true
            deleteButton.isEnabled = true
            if(viewModel.repo.preferredGraphList != 1) {
                viewModel.repo.preferredGraphList = 1
                if(viewModel.repo.currentEquation.value != initialEquation) {
                    Log.d("onPause,onResume", "savedEquationsRadioButton.setOnClickListener")
                    mView!!.onPause()
                    mView!!.onResume()
                }
            }
        }

        newButton.setOnClickListener {
            viewModel.repo.insertEquation("", "")
            if(viewModel.repo.preferredGraphList == 0) {
                viewModel.repo.preferredGraphList = 1
                preferredGraphListRadioGroup.check(savedEquationsRadioButton.id)
                savedGraphsSpinner.isEnabled = true
                defaultGraphsSpinner.isEnabled = false
                deleteButton.isEnabled = true
            }
            populateSavedEquationNamesSpinner()
            savedGraphsSpinner.setSelection(viewModel.repo.equationsJSONArray.length() - 1)
        }

        copyButton.setOnClickListener {
            val equation: ActiveWallpaperRepo.Equation = viewModel.repo.currentEquation
            viewModel.repo.insertEquation(equation.name, equation.value)
            if(viewModel.repo.preferredGraphList == 0) {
                viewModel.repo.preferredGraphList = 1
                preferredGraphListRadioGroup.check(savedEquationsRadioButton.id)
                savedGraphsSpinner.isEnabled = true
                defaultGraphsSpinner.isEnabled = false
                deleteButton.isEnabled = true
            }
            populateSavedEquationNamesSpinner()
            savedGraphsSpinner.setSelection(viewModel.repo.equationsJSONArray.length() - 1)
        }

        deleteButton.setOnClickListener {
            val initialEquation: String = equationValueEditText.text.toString()
            if(viewModel.repo.equationsJSONArray.length() > 1) {
                viewModel.repo.deleteEquation(viewModel.repo.savedEquationSelection)
                equationNameEditText.setText(viewModel.repo.currentEquation.name)
                equationValueEditText.setText(viewModel.repo.currentEquation.value)
            } else {
                viewModel.repo.updateEquation(viewModel.repo.savedEquationSelection, "", "")
                equationNameEditText.setText("")
                equationValueEditText.setText("")
            }
            populateSavedEquationNamesSpinner()
            savedGraphsSpinner.setSelection(viewModel.repo.savedEquationSelection)
            if(viewModel.repo.currentEquation.value != initialEquation) {
                Log.d("onPause,onResume", "deleteButton.setOnClickListener")
                mView!!.onPause()
                mView!!.onResume()
            }
        }

        equationNameEditText.addTextChangedListener(object : TextWatcher {
            override fun beforeTextChanged(s: CharSequence?, start: Int, count: Int, after: Int) {
                // Do nothing
            }

            override fun onTextChanged(s: CharSequence?, start: Int, before: Int, count: Int) {
                if(viewModel.repo.preferredGraphList == 1 && editTextsChangingViaKeyboard) {
                    val equation: ActiveWallpaperRepo.Equation = viewModel.repo.getEquation(viewModel.repo.savedEquationSelection)
                    viewModel.repo.updateEquation(viewModel.repo.savedEquationSelection, s.toString(), equation.value)
                    populateSavedEquationNamesSpinner()
                    savedGraphsSpinner.setSelection(viewModel.repo.savedEquationSelection)
                }
            }

            override fun afterTextChanged(s: android.text.Editable?) {
                // Do nothing
            }
        })

        equationValueEditText.addTextChangedListener(object : TextWatcher {
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

        // Setup listener for the Return button
        equationValueEditText.setOnEditorActionListener { _, actionId, event ->
            if (actionId == EditorInfo.IME_ACTION_DONE || (event != null && event.keyCode == KeyEvent.KEYCODE_ENTER && event.action == KeyEvent.ACTION_DOWN)) {
                val view = currentFocus
                if (view != null) {
                    val imm = getSystemService(Context.INPUT_METHOD_SERVICE) as InputMethodManager
                    imm.hideSoftInputFromWindow(view.windowToken, 0)

                    // Add a global layout listener to wait until the keyboard is hidden
                    view.viewTreeObserver.addOnGlobalLayoutListener(object : ViewTreeObserver.OnGlobalLayoutListener {
                        override fun onGlobalLayout() {
                            if (findViewById<View>(android.R.id.content).rootView.height - findViewById<View>(android.R.id.content).height <= 200) {
                                // Keyboard is hidden, run the action
                                view.viewTreeObserver.removeOnGlobalLayoutListener(this)
                                Handler().post(doneButton())
                            }
                        }
                    })
                } else {
                    // If no view is focused, run the action immediately
                    Handler().post(doneButton())
                }
                return@setOnEditorActionListener true
            } else {
                return@setOnEditorActionListener false
            }
        }

        // connect fps data to ui fps meter
        viewModel.getFPS().observe(this) {
            fpsMeter.text = String.format("fps: %.2f", it)
        }

        hideUIButton.setOnClickListener {
            val animation: Animation = if (viewModel.repo.isCollapsed) {
                TranslateAnimation(linearLayout.width.toFloat(), 0f, 0f, 0f)
            } else {
                TranslateAnimation(0f, linearLayout.width.toFloat(), 0f, 0f)
            }
            setDefaultAnimationParametersAndAnimate(animation)
        }

        flipsNormalsCheckBox.setOnCheckedChangeListener { _, isChecked ->
            if(viewModel.repo.flipNormals.value != isChecked) {
                viewModel.repo.flipNormals.value = isChecked
                viewModel.saveVisualizationState()
                Log.d("onPause,onResume", "flipsNormalsCheckBox.setOnCheckedChangeListener")
                mView!!.onPause()
                mView!!.onResume()
            }
        }

        fluidSurfaceCheckBox.setOnCheckedChangeListener { _, isChecked ->
            if(viewModel.repo.fluidSurface.value != isChecked) {
                viewModel.repo.fluidSurface.value = isChecked
                viewModel.saveVisualizationState()
                Log.d("onPause,onResume", "fluidSurfaceCheckBox.setOnCheckedChangeListener")
                mView!!.onPause()
                mView!!.onResume()
            }
        }

        gyroscopeCompensationCheckBox.setOnCheckedChangeListener { _, isChecked ->
            if(viewModel.repo.referenceFrameRotates.value == isChecked) {
                viewModel.repo.referenceFrameRotates.value = !isChecked
                viewModel.saveVisualizationState()
                Log.d("onPause,onResume", "gyroscopeCompensationCheckBox.setOnCheckedChangeListener")
                mView!!.onPause()
                mView!!.onResume()
            }
        }

        smoothSphereSurfaceCheckBox.setOnCheckedChangeListener { _, isChecked ->
            if(viewModel.repo.smoothSphereSurface.value != isChecked) {
                viewModel.repo.smoothSphereSurface.value = isChecked
                viewModel.saveVisualizationState()
                Log.d("onPause,onResume", "smoothSphereSurfaceCheckBox.setOnCheckedChangeListener")
                mView!!.onPause()
                mView!!.onResume()
            }
        }

        viewModel.repo.flipNormals.observe(this){ flip ->
            flipsNormalsCheckBox.isChecked = flip
        }

        viewModel.repo.fluidSurface.observe(this){ flip ->
            fluidSurfaceCheckBox.isChecked = flip
        }

        viewModel.repo.referenceFrameRotates.observe(this){ flip ->
            gyroscopeCompensationCheckBox.isChecked = !flip
        }

        viewModel.repo.smoothSphereSurface.observe(this){ flip ->
            smoothSphereSurfaceCheckBox.isChecked = flip
        }
    }

    companion object {
        // Used to load the 'livewallpaper05' library on application startup.
        init {
            System.loadLibrary("livewallpaper05")
        }

        // register simulation functions as external and belonging to the livewallpaper05 library
        external fun init(visualization: String, mode: Int)
        external fun resize(width: Int, height: Int, mode: Int)
        external fun step(
            accX: Float,
            accY: Float,
            accZ: Float,
            rotX: Float,
            rotY: Float,
            rotZ: Float,
            rotW: Float,
            linearAccX: Float,
            linearAccY: Float,
            linearAccZ: Float,
            distance: Float,
            fieldOfView: Float,
            gravity: Float,
            efficiency: Float,
            flipNormals: Boolean,
            orientation: Int,
            mode: Int
        )

        //external fun getScreenBuffer(mode: Int): ByteArray
    }
}