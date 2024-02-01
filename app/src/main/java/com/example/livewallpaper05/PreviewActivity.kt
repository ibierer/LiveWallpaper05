package com.example.livewallpaper05

import android.content.Context
import android.content.Intent
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
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperApplication
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModel
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModelFactory
import com.example.livewallpaper05.helpful_fragments.ColorActivity

class PreviewActivity : AppCompatActivity() {

    var mView: GLES3JNIView? = null

    private val viewModel: ActiveWallpaperViewModel by viewModels {
        ActiveWallpaperViewModelFactory((application as ActiveWallpaperApplication).repository)
    }

    @RequiresApi(Build.VERSION_CODES.R)
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
        val equationEditor = findViewById<EditText>(R.id.et_equation)

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
        try{
            viewModel.updateOrientation(this.display!!.rotation)
        } catch (e: Exception){
            Log.d("Livewallpaper", "api level too low!")
        }

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
        colorButton.setOnClickListener {
            val colorIntent = Intent(this, ColorActivity::class.java)
            colorActivity.launch(colorIntent)
        }

        // setup equation editor
        viewModel.updateEquation(equationEditor.text.toString())
        equationEditor.addTextChangedListener(object : TextWatcher {
            override fun afterTextChanged(s: android.text.Editable?) {
                // update equation in repo if valid
                val result: String = checkEquationSyntax(equationEditor.text.toString())
                //val result2: String = checkEquationSyntax2(equationEditor.text.toString())
                Log.d("LiveWallpaper05", "result is: " + result)
                //Log.d("LiveWallpaper05", "result2 is: " + result2)
                if (result == "") {
                    viewModel.updateEquation(equationEditor.text.toString())
                    mView!!.onPause()
                    mView!!.onResume()
                }
            }
            override fun beforeTextChanged(s: CharSequence?, start: Int, count: Int, after: Int) {
                // Do nothing
            }
            override fun onTextChanged(s: CharSequence?, start: Int, before: Int, count: Int) {
                // Do nothing
            }
        })

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

        viewModel.updatePreviewImg(updatePreviewImage())
    }

    /* this is run when the app is 'resumed'
     * this includes returning to the app, rotating the screen, etc.
     */
    override fun onResume() {
        super.onResume()
        mView!!.onResume()
    }

    private val colorActivity = registerForActivityResult(ActivityResultContracts.StartActivityForResult()) { result ->
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

    fun updatePreviewImage() : Bitmap{
        // store view as preview image
        /**
        var preview = Bitmap.createBitmap(mView!!.width, mView!!.height, Bitmap.Config.ARGB_8888)
        var canvas = Canvas(preview)
        mView!!.draw(canvas)*/
        var preview = Bitmap.createBitmap(mView!!.width, mView!!.height, Bitmap.Config.ARGB_8888)
        var canvas = Canvas(preview)
        var background = mView!!.background
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
        external fun step(acc_x: Float, acc_y: Float, acc_z: Float, rot_x: Float, rot_y: Float, rot_z: Float, rot_w: Float, linear_acc_x: Float, linear_acc_y: Float, linear_acc_z: Float, value: Float)
        external fun sendData(value: Float)
    }

    /* NOTE: Used chatgpt for equation conversion from C++ to Kotlin*/
    fun checkEquationSyntax(editable: String): String {
        var syntaxError = ""
        var containsEqualSign = false
        var parenthesisCountLeft = 0
        var parenthesisCountRight = 0
        var improperParenthesisLeft = false
        var improperParenthesisRight = false

        for (i in editable.indices) {
            when (editable[i]) {
                '(' -> if (containsEqualSign) parenthesisCountRight++ else parenthesisCountLeft++
                ')' -> if (containsEqualSign) parenthesisCountRight-- else parenthesisCountLeft--
                '=' -> if (containsEqualSign) {
                    syntaxError = "Syntax Error: contains more than one equal sign."
                    return syntaxError
                } else {
                    containsEqualSign = true
                }
            }

            if (parenthesisCountLeft < 0) {
                improperParenthesisLeft = true
            } else if (parenthesisCountRight < 0) {
                improperParenthesisRight = true
            }
        }

        if (!containsEqualSign) {
            syntaxError = "Syntax Error: lacks an equal sign."
            return syntaxError
        }

        if (improperParenthesisLeft || improperParenthesisRight) {
            syntaxError = "Syntax Error: improper parenthesis on "
            syntaxError += if (!improperParenthesisLeft) {
                "right."
            } else if (improperParenthesisRight) {
                "both sides."
            } else {
                "left."
            }
            return syntaxError
        }

        if (parenthesisCountLeft != 0 || parenthesisCountRight != 0) {
            syntaxError = "Syntax Error: invalid parenthesis count on "
            syntaxError += if (parenthesisCountLeft == 0) {
                "right."
            } else if (parenthesisCountRight != 0) {
                "both sides."
            } else {
                "left."
            }
            return syntaxError
        }

        // Check for expressions
        var termOnLeft = false
        var termOnRight = false
        var passedEqualSign = false
        for (i in editable.indices) {
            if (aDigit(editable[i]) || aCharacter(editable[i])) {
                if (passedEqualSign) {
                    termOnRight = true
                } else {
                    termOnLeft = true
                }
            } else if (editable[i] == '=') {
                passedEqualSign = true
            }
        }

        if (!termOnLeft || !termOnRight) {
            syntaxError = "Syntax Error: no expression on "
            syntaxError += if (termOnLeft) {
                "right side."
            } else if (!termOnRight) {
                "either side."
            } else {
                "left side."
            }
            return syntaxError
        }

        // Improper term I
        var badTermOnLeft = false
        var badTermOnRight = false
        passedEqualSign = false
        for (i in 0 until editable.length - 1) {
            if (aCharacter(editable[i]) &&
                (editable[i + 1] == '.' || aDigit(editable[i + 1]))
            ) {
                if (!passedEqualSign) {
                    badTermOnLeft = true
                } else {
                    badTermOnRight = true
                }
            } else if (editable[i] == '=') {
                passedEqualSign = true
            }
        }

        if (badTermOnLeft || badTermOnRight) {
            syntaxError = "Syntax Error: improper term on "
            syntaxError += if (!badTermOnLeft) {
                "right side."
            } else if (badTermOnRight) {
                "each side."
            } else {
                "left side."
            }
            return syntaxError
        }

        // Convert to noSpaces
        val noSpaces = editable.filter { it != ' ' }

        // Improper term II
        passedEqualSign = false
        for (i in 0 until noSpaces.length - 1) {
            if (noSpaces[i] == '=') {
                passedEqualSign = true
            }
            if ((noSpaces[i] == '(' &&
                        (noSpaces[i + 1] == ')' ||
                                (anOperator(noSpaces[i + 1]) && noSpaces[i + 1] != '-')
                                )
                        ) || (
                        (noSpaces[i] == '(' || anOperator(noSpaces[i])) &&
                                noSpaces[i + 1] == ')'
                        )
            ) {
                if (!passedEqualSign) {
                    badTermOnLeft = true
                } else {
                    badTermOnRight = true
                }
            }
        }

        if (badTermOnLeft || badTermOnRight) {
            syntaxError = "Syntax Error: improper term on "
            syntaxError += if (!badTermOnLeft) {
                "right side."
            } else if (badTermOnRight) {
                "each side."
            } else {
                "left side."
            }
            return syntaxError
        }

        // Improper Decimal
        passedEqualSign = false
        for (i in 0 until noSpaces.length - 1) {
            if (noSpaces[i] == '=') {
                passedEqualSign = true
            }
            if ((noSpaces[i] == '.' && !aDigit(noSpaces[i + 1])) ||
                (i == noSpaces.length - 2 && noSpaces[noSpaces.length - 1] == '.')
            ) {
                if (!passedEqualSign) {
                    badTermOnLeft = true
                } else {
                    badTermOnRight = true
                }
            }
        }

        if (badTermOnLeft || badTermOnRight) {
            syntaxError = "Syntax Error: improper decimal on "
            syntaxError += if (!badTermOnLeft) {
                "right side."
            } else if (badTermOnRight) {
                "each side."
            } else {
                "left side."
            }
            return syntaxError
        }

        // Invalid Operator
        passedEqualSign = false
        for (i in 0 until noSpaces.length - 1) {
            if (noSpaces[i] == '=') {
                passedEqualSign = true
            }
            if ((i == noSpaces.length - 2 && anOperator(noSpaces[noSpaces.length - 1])) ||
                (anOperator(noSpaces[i]) &&
                        (noSpaces[i + 1] == ')' || noSpaces[i + 1] == '=' || (i == 0 && noSpaces[i] != '-'))
                        ) || (
                        (noSpaces[i] == '(' || noSpaces[i] == '=') &&
                                (anOperator(noSpaces[i + 1]) && noSpaces[i + 1] != '-')
                        )
            ) {
                if (!passedEqualSign) {
                    badTermOnLeft = true
                } else {
                    badTermOnRight = true
                }
            }
        }
        if (badTermOnLeft || badTermOnRight) {
            syntaxError = "Syntax Error: invalid operator on "
            syntaxError += if (!badTermOnLeft) {
                "right side."
            } else if (badTermOnRight) {
                "each side."
            }
            else {
                syntaxError += "left side."
            }
            return syntaxError
        }
        syntaxError = ""
        return syntaxError
    }

    /*fun checkEquationSyntax2(editable: String): String {
        // Check for the presence of any equal sign
        if (!"=".toRegex().containsMatchIn(editable)) {
            return "Syntax Error: lacks an equal sign."
        }

        // Check for more than one equal sign
        if (editable.split("=").size > 2) {
            return "Syntax Error: contains more than one equal sign."
        }

        // Check for proper parenthesis count
        val parenthesisRegex = Regex("[()]")
        val leftParenthesisCount = parenthesisRegex.findAll(editable)
            .filter { it.value == "(" }
            .count()
        val rightParenthesisCount = parenthesisRegex.findAll(editable)
            .filter { it.value == ")" }
            .count()

        if (leftParenthesisCount != rightParenthesisCount) {
            val imbalanceSide = if (leftParenthesisCount < rightParenthesisCount) "left" else "right"
            return "Syntax Error: improper parenthesis count on $imbalanceSide side."
        }

        // Check for expressions on both sides
        val expressionsRegex = Regex("[a-zA-Z0-9]+")
        val leftSideExpression = expressionsRegex.containsMatchIn(editable.substringBefore("="))
        val rightSideExpression = expressionsRegex.containsMatchIn(editable.substringAfter("="))

        if (!leftSideExpression || !rightSideExpression) {
            return "Syntax Error: no expression on ${if (!leftSideExpression) "left" else "right"} side."
        }

        // Additional regex checks can be added for specific cases (e.g., invalid operators, decimals)

        return "" // No syntax errors found
    }*/

    fun aDigit(character: Char): Boolean {
        return character in '0'..'9'
    }
    fun aCharacter(character: Char): Boolean {
        return character == 'x' ||
                character == 'y' ||
                character == 'z' ||
                character == 't' ||
                character == 'e' ||
                character == pi
    }

    fun anOperator(character: Char): Boolean {
        return character == '^' ||
                character == '/' ||
                character == '*' ||
                character == '-' ||
                character == '+'
    }

    val pi: Char = 5.toChar()
}