package com.vizbox4d.activewallpaperdata

import android.annotation.SuppressLint
import android.content.Context
import android.content.SharedPreferences
import android.graphics.Color
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager
import androidx.lifecycle.MutableLiveData
import com.vizbox4d.R
import kotlinx.coroutines.CoroutineScope
import org.json.JSONArray
import org.json.JSONObject

class ActiveWallpaperRepo private constructor(val context: Context) : SensorEventListener {

    private val sharedPreferences: SharedPreferences = getPreferences()
    private val sharedPreferencesEditor: SharedPreferences.Editor = sharedPreferences.edit()
    var visualization: Visualization = visualizationIntToVisualizationObject(getVisualizationSelection())
    val backgroundTexture: MutableLiveData<String> = MutableLiveData<String>(visualization.toJsonObject().getString("background_texture"))
    val fluidSurface: MutableLiveData<Boolean> = MutableLiveData(visualization.toJsonObject().optBoolean("fluid_surface", true))
    var backgroundIsSolidColor: MutableLiveData<Boolean> = MutableLiveData<Boolean>(visualization.toJsonObject().optBoolean("background_is_solid_color", false))
    var referenceFrameRotates: MutableLiveData<Boolean> = MutableLiveData<Boolean>(visualization.toJsonObject().optBoolean("reference_frame_rotates", false))
    var smoothSphereSurface: MutableLiveData<Boolean> = MutableLiveData<Boolean>(visualization.toJsonObject().optBoolean("smooth_sphere_surface", true))
    var color: MutableLiveData<Color> = MutableLiveData<Color>(Color.valueOf(0.0f, 0.0f, 0.0f, 0.0f))
    var orientation: Int = 0
    var fps: MutableLiveData<Float> = MutableLiveData<Float>(0.0f)
    var lastFrame: Long = 0
    var distanceFromOrigin: MutableLiveData<Float> = MutableLiveData<Float>(0.5f)
    var fieldOfView: MutableLiveData<Float> = MutableLiveData<Float>(60.0f)
    var gravity: MutableLiveData<Float> = MutableLiveData<Float>(visualization.toJsonObject().optDouble("gravity", 2.0).toFloat())
    var linearAcceleration: MutableLiveData<Float> = MutableLiveData<Float>(1.0f)
    var efficiency: MutableLiveData<Float> = MutableLiveData<Float>(1.0f)
    var particleCount: MutableLiveData<Int> = MutableLiveData<Int>(1000)
    val flipNormals: MutableLiveData<Boolean> = MutableLiveData<Boolean>(visualization.toJsonObject().optBoolean("vector_points_positive", false))
    var rotationData: Array<Float> = arrayOf(0.0f, 0.0f, 0.0f, 0.0f)
    var accelerationData: Array<Float> = arrayOf(0.0f, 0.0f, 0.0f)
    var linearAccelerationData: Array<Float> = arrayOf(0.0f, 0.0f, 0.0f)
    var equationsJSONArray: JSONArray = JSONArray(sharedPreferences.getString(
            "savedEquations",
            JSONArray("""[{"name": "", "value": ""}]""").toString()
    ))
    private var preferredGraphListValue: Int = -1
    private var defaultEquationSelectionValue: Int = -1
    private var savedEquationSelectionValue: Int = -1
    var preferredGraphList: Int
        get() { if(preferredGraphListValue == -1) preferredGraphListValue = sharedPreferences.getInt("preferredGraphList", 0); return preferredGraphListValue }
        set(value) { preferredGraphListValue = value; sharedPreferencesEditor.putInt("preferredGraphList", value); sharedPreferencesEditor.apply() }
    var defaultEquationSelection: Int
        get() { if(defaultEquationSelectionValue == -1) defaultEquationSelectionValue = sharedPreferences.getInt("preferredDefaultEquation", 0); return defaultEquationSelectionValue }
        set(value) { defaultEquationSelectionValue = value; sharedPreferencesEditor.putInt("preferredDefaultEquation", value); sharedPreferencesEditor.apply() }
    var savedEquationSelection: Int
        get() { if(savedEquationSelectionValue == -1) savedEquationSelectionValue = sharedPreferences.getInt("preferredSavedEquation", 0); return savedEquationSelectionValue }
        set(value) { savedEquationSelectionValue = value; sharedPreferencesEditor.putInt("preferredSavedEquation", value); sharedPreferencesEditor.apply() }
    var currentEquation: Equation
        get() {
            return when (preferredGraphList) {
                0 -> {
                    Equation(
                        context.resources.getStringArray(R.array.graph_names)[defaultEquationSelection],
                        context.resources.getStringArray(R.array.graph_options)[defaultEquationSelection]
                    )
                }
                1 -> {
                    getEquation(savedEquationSelection)
                }
                else -> throw IllegalArgumentException("Invalid graph list")
            }
        }
        set(value) {}

    var rememberColorPickerValue: Int = Color.RED

    private lateinit var mSensorManager: SensorManager
    var isCollapsed = false

    // setup companion object so repo can be created from any thread and still be a singleton
    companion object {
        @SuppressLint("StaticFieldLeak")
        @Volatile
        private var instance: ActiveWallpaperRepo? = null
        private lateinit var mScope: CoroutineScope
        //private val syncMutex: Mutex = Mutex()

        @Synchronized
        fun getInstance(
            context: Context,
            scope: CoroutineScope
        ): ActiveWallpaperRepo {
            return instance ?: ActiveWallpaperRepo(context).also {
                instance = it
                mScope = scope
            }
        }
    }

    // register sensor events to repo sensor manager
    fun registerSensors(mSensorManager: SensorManager) {
        this.mSensorManager = mSensorManager
        mSensorManager.registerListener(
            this,
            mSensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR),
            SensorManager.SENSOR_DELAY_GAME
        )
        mSensorManager.registerListener(
            this,
            mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER),
            SensorManager.SENSOR_DELAY_GAME
        )
        mSensorManager.registerListener(
            this,
            mSensorManager.getDefaultSensor(Sensor.TYPE_LINEAR_ACCELERATION),
            SensorManager.SENSOR_DELAY_GAME
        )
    }

    // update sensor data variables when sensor values change
    override fun onSensorChanged(p0: SensorEvent?) {
        if (p0 != null) {
            if (p0.sensor.type == Sensor.TYPE_ROTATION_VECTOR) {
                rotationData = p0.values.toTypedArray()
            }
            if (p0.sensor.type == Sensor.TYPE_ACCELEROMETER) {
                accelerationData = p0.values.toTypedArray()
            }
            if (p0.sensor.type == Sensor.TYPE_LINEAR_ACCELERATION) {
                linearAccelerationData = p0.values.toTypedArray()
            }
        }
    }

    // do nothing when sensor accuracy changes
    override fun onAccuracyChanged(p0: Sensor?, p1: Int) {
        // Do nothing
    }

    // update orientation value
    fun updateOrientation(orient: Int) {
        this.orientation = orient
    }

    private fun getPreferences(): SharedPreferences {
        val preferences: SharedPreferences = context.getSharedPreferences(context.packageName, Context.MODE_PRIVATE)
        if(preferences.getBoolean("firstTimeStartup", true)){
            val sharedPreferencesEditor = preferences.edit()
            with(sharedPreferencesEditor){
                putInt("preferredVisualization", 0)
                putInt("preferredGraphList", 0)
                putInt("preferredDefaultEquation", 0)
                putInt("preferredSavedEquation", 0)
                putString("savedEquations", JSONArray("""[{"name": "", "value": ""}]""").toString())
                putString("0", NBodyVisualization().toJsonObject().toString())
                putString("1", NaiveFluidVisualization().toJsonObject().toString())
                putString("2", PicFlipVisualization().toJsonObject().toString())
                putString("3", TriangleVisualization().toJsonObject().toString())
                putString("4", GraphVisualization().toJsonObject().toString())
                putBoolean("firstTimeStartup", false)
                apply()
            }
        }
        return preferences
    }

    fun getVisualizationSelection(): Int {
        return sharedPreferences.getInt("preferredVisualization", 0)
    }

    fun getEnvironmentMapSelection(): Int {
        return if (backgroundTexture.value!! == "ms_paint_colors") {
            0
        } else if (backgroundTexture.value!! == "mandelbrot") {
            1
        } else if (backgroundTexture.value!! == "rgb_cube") {
            2
        } else {
            0
        }
    }

    fun visualizationIntToVisualizationObject(selection: Int): Visualization {
        sharedPreferencesEditor.putInt("preferredVisualization", selection)
        sharedPreferencesEditor.apply()
        val config: JSONObject = sharedPreferences.getString(selection.toString(), "")?.let { JSONObject(it) }!!
        return when (selection){
            0 -> NBodyVisualization(config)
            1 -> NaiveFluidVisualization(config)
            2 -> PicFlipVisualization(config)
            3 -> TriangleVisualization(config)
            4 -> GraphVisualization(config)
            else -> throw IllegalArgumentException("Invalid visualization type")
        }
    }

    fun saveVisualizationState() {
        val selection: Int = getVisualizationSelection()
        visualization = when (selection) {
            0 -> NBodyVisualization(this)
            1 -> NaiveFluidVisualization(this)
            2 -> PicFlipVisualization(this)
            3 -> TriangleVisualization(this)
            4 -> GraphVisualization(this)
            else -> throw IllegalArgumentException("Invalid visualization type")
        }
        sharedPreferencesEditor.putString(
            selection.toString(),
            visualization.toJsonObject().toString()
        )
        sharedPreferencesEditor.apply()
    }

    data class Equation(val name: String, val value: String)

    fun insertEquation(name: String, value: String) {
        // Add equation to equationsJSONArray
        val jsonObject = JSONObject()
        jsonObject.put("name", name)
        jsonObject.put("value", value)
        equationsJSONArray.put(jsonObject)

        // Push update to shared preferences
        updateSavedEquations()
    }

    fun updateEquation(index: Int, name: String, value: String) {
        // Update equation in equationJSONArray at given index
        val jsonObject = equationsJSONArray.getJSONObject(index)
        jsonObject.put("name", name)
        jsonObject.put("value", value)

        // Push update to shared preferences
        updateSavedEquations()
    }

    fun getEquation(index: Int) : Equation {
        // Return equation from equationJSONArray at given index
        val jsonObject = equationsJSONArray.getJSONObject(index)
        return Equation(jsonObject.getString("name"), jsonObject.getString("value"))
    }

    fun deleteEquation(index: Int) {
        // Delete equation from equationJSONArray at given index
        equationsJSONArray.remove(index)

        // Ensure index stays within bounds
        if(savedEquationSelection > equationsJSONArray.length() - 1) {
            savedEquationSelection = equationsJSONArray.length() - 1
        }

        // Push update to shared preferences
        updateSavedEquations()
    }

    fun updateSavedEquations() {
        sharedPreferencesEditor.putString("savedEquations", equationsJSONArray.toString())
        sharedPreferencesEditor.apply()
    }
}