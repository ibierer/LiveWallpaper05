package com.example.livewallpaper05.activewallpaperdata

import android.content.res.Resources
import android.graphics.Bitmap
import android.graphics.Color
import android.hardware.SensorManager
import android.util.Log
import androidx.lifecycle.*
import com.example.livewallpaper05.savedWallpapers.SavedWallpaperTable
import org.json.JSONObject
import java.math.RoundingMode
import java.util.Random

/**
 * View Model to keep a reference to the active wallpaper data
 */
class ActiveWallpaperViewModel(private val repo: ActiveWallpaperRepo) : ViewModel() {
    open class Visualization() {
        open fun toJsonObject(): JSONObject {
            return JSONObject()
        }
    }
    data class NBodyVisualization (
        val visualizationType: String = "simulation",
        val simulationType: String = "nbody",
        var distance: Float = 0.5f,
        var fieldOfView: Float = 60.0f,
        var backgroundIsSolidColor: Boolean = true,
        var backgroundTexture: String = "ms_paint_colors",
        var backgroundColor: Color = Color.valueOf(0.0f, 0.0f, 0.0f, 0.0f)
    ) : Visualization() {
        override fun toJsonObject() : JSONObject {
            val jsonObject = JSONObject()
            jsonObject.put("visualization_type", visualizationType)
            jsonObject.put("simulation_type", simulationType)
            jsonObject.put("distance", distance)
            jsonObject.put("field_of_view", fieldOfView)
            jsonObject.put("background_is_solid_color", backgroundIsSolidColor)
            jsonObject.put("background_texture", backgroundTexture)

            val backgroundColorObject = JSONObject()
            backgroundColorObject.put("r", backgroundColor.red())
            backgroundColorObject.put("g", backgroundColor.green())
            backgroundColorObject.put("b", backgroundColor.blue())
            backgroundColorObject.put("a", backgroundColor.alpha())

            jsonObject.put("background_color", backgroundColorObject)

            return jsonObject
        }
    }
    data class NaiveFluidVisualization (
        val visualizationType: String = "simulation",
        val simulationType: String = "naive",
        var fluidSurface: Boolean = true,
        var particleCount: Int = 1000,
        var smoothSphereSurface: Boolean = true,
        var distance: Float = 0.5f,
        var fieldOfView: Float = 60.0f,
        var gravity: Float = 0.0f,
        var linearAcceleration: Float = 1.0f,
        var efficiency: Float = 1.0f,
        var referenceFrameRotates: Boolean = false,
        var backgroundIsSolidColor: Boolean = false,
        var backgroundTexture: String = "ms_paint_colors",
        var backgroundColor: Color = Color.valueOf(0.0f, 0.0f, 0.0f, 0.0f)
    ) : Visualization() {
        override fun toJsonObject() : JSONObject {
            val jsonObject = JSONObject()
            jsonObject.put("visualization_type", visualizationType)
            jsonObject.put("simulation_type", simulationType)
            jsonObject.put("fluid_surface", fluidSurface)
            jsonObject.put("particle_count", particleCount)
            jsonObject.put("smooth_sphere_surface", smoothSphereSurface)
            jsonObject.put("distance", distance)
            jsonObject.put("field_of_view", fieldOfView)
            jsonObject.put("gravity", gravity)
            jsonObject.put("linear_acceleration", linearAcceleration)
            jsonObject.put("efficiency", efficiency)
            jsonObject.put("reference_frame_rotates", referenceFrameRotates)
            jsonObject.put("background_is_solid_color", backgroundIsSolidColor)
            jsonObject.put("background_texture", backgroundTexture)

            val backgroundColorObject = JSONObject()
            backgroundColorObject.put("r", backgroundColor.red())
            backgroundColorObject.put("g", backgroundColor.green())
            backgroundColorObject.put("b", backgroundColor.blue())
            backgroundColorObject.put("a", backgroundColor.alpha())

            jsonObject.put("background_color", backgroundColorObject)

            Log.d("VISUALIZATION = ", jsonObject.toString())

            return jsonObject
        }
    }
    data class PicFlipVisualization (
        val visualizationType: String = "simulation",
        val simulationType: String = "picflip",
        var distance: Float = 0.5f,
        var fieldOfView: Float = 60.0f,
        var gravity: Float = 0.0f,
        var linearAcceleration: Float = 1.0f,
        var referenceFrameRotates: Boolean = true,
        var backgroundIsSolidColor: Boolean = false,
        var backgroundTexture: String = "ms_paint_colors",
        var backgroundColor: Color = Color.valueOf(0.0f, 0.0f, 0.0f, 0.0f)
    ) : Visualization() {
        override fun toJsonObject() : JSONObject {
            val jsonObject = JSONObject()
            jsonObject.put("visualization_type", visualizationType)
            jsonObject.put("simulation_type", simulationType)
            jsonObject.put("distance", distance)
            jsonObject.put("field_of_view", fieldOfView)
            jsonObject.put("gravity", gravity)
            jsonObject.put("linear_acceleration", linearAcceleration)
            jsonObject.put("reference_frame_rotates", referenceFrameRotates)
            jsonObject.put("background_is_solid_color", backgroundIsSolidColor)
            jsonObject.put("background_texture", backgroundTexture)

            val backgroundColorObject = JSONObject()
            backgroundColorObject.put("r", backgroundColor.red())
            backgroundColorObject.put("g", backgroundColor.green())
            backgroundColorObject.put("b", backgroundColor.blue())
            backgroundColorObject.put("a", backgroundColor.alpha())

            jsonObject.put("background_color", backgroundColorObject)

            return jsonObject
        }
    }
    data class TriangleVisualization (
        val visualizationType: String = "other",
        var distance: Float = 0.5f,
        var fieldOfView: Float = 60.0f,
        var backgroundIsSolidColor: Boolean = false,
        var backgroundTexture: String = "mandelbrot",
        var backgroundColor: Color = Color.valueOf(0.0f, 0.0f, 0.0f, 0.0f)
    ) : Visualization() {
        override fun toJsonObject() : JSONObject {
            val jsonObject = JSONObject()
            jsonObject.put("visualization_type", visualizationType)
            jsonObject.put("distance", distance)
            jsonObject.put("field_of_view", fieldOfView)
            jsonObject.put("background_is_solid_color", backgroundIsSolidColor)
            jsonObject.put("background_texture", backgroundTexture)

            val backgroundColorObject = JSONObject()
            backgroundColorObject.put("r", backgroundColor.red())
            backgroundColorObject.put("g", backgroundColor.green())
            backgroundColorObject.put("b", backgroundColor.blue())
            backgroundColorObject.put("a", backgroundColor.alpha())

            jsonObject.put("background_color", backgroundColorObject)

            return jsonObject
        }
    }
    data class GraphVisualization (
        val visualizationType: String = "graph",
        var distance: Float = 0.5f,
        var fieldOfView: Float = 60.0f,
        var referenceFrameRotates: Boolean = false,
        var backgroundIsSolidColor: Boolean = false,
        var backgroundTexture: String = "ms_paint_colors",
        var vectorPointsPositive: Boolean = false,
        var backgroundColor: Color = Color.valueOf(0.0f, 0.0f, 0.0f, 0.0f),
        var equation: String = "1/((sqrt(x^2 + y^2) - 1.5 + sin(t))^2 + (z + cos(t))^2) + 1/((sqrt(x^2 + y^2) - 1.5 + sin(t + 2π/3))^2 + (z + cos(t + 2π/3))^2) + 1/((sqrt(x^2 + y^2) - 1.5 + sin(t + 4π/3))^2 + (z + cos(t + 4π/3))^2) = 5"
    ) : Visualization() {
        override fun toJsonObject() : JSONObject {
            val jsonObject = JSONObject()
            jsonObject.put("visualization_type", visualizationType)
            jsonObject.put("distance", distance)
            jsonObject.put("field_of_view", fieldOfView)
            jsonObject.put("reference_frame_rotates", referenceFrameRotates)
            jsonObject.put("background_is_solid_color", backgroundIsSolidColor)
            jsonObject.put("background_texture", backgroundTexture)
            jsonObject.put("vector_points_positive", vectorPointsPositive)
            jsonObject.put("equation", equation)

            val backgroundColorObject = JSONObject()
            backgroundColorObject.put("r", backgroundColor.red())
            backgroundColorObject.put("g", backgroundColor.green())
            backgroundColorObject.put("b", backgroundColor.blue())
            backgroundColorObject.put("a", backgroundColor.alpha())

            jsonObject.put("background_color", backgroundColorObject)

            return jsonObject
        }
    }
    lateinit var visualization : Visualization
    var width: Int = 0
    var height: Int = 0
    private var mBitmap : Bitmap = Bitmap.createBitmap(500, 500, Bitmap.Config.ARGB_8888) as Bitmap
    var liveDataBitmap : MutableLiveData<Bitmap> = MutableLiveData<Bitmap>(mBitmap)
    var getScreenBuffer: Int = 0
    var saveAsNew: Int = 0
    // reference repo from constructor value
    val mRepo: ActiveWallpaperRepo = repo


    fun getPreviewImg(seed: Int): Bitmap {
        if (repo.preview != null && false) // This condition is always false... hmm...
            return repo.preview!!
        else { // This branch is never executed!
            val rng = Random()
            rng.setSeed(seed.toLong())
            val color = Color.argb(255,
                rng.nextInt(256),
                rng.nextInt(256),
                rng.nextInt(256))

            val screenWidth = Resources.getSystem().getDisplayMetrics().widthPixels
            val screenHeight = Resources.getSystem().getDisplayMetrics().heightPixels
            val imgWidth = screenWidth/4
            val imgHeight = screenHeight/4

            val tmp = Bitmap.createBitmap(imgWidth, imgHeight, Bitmap.Config.ARGB_8888)
            tmp.eraseColor(color)

            return tmp
        }
    }

    // return distance value from repo
    fun getDistanceFromOrigin(): Float {
        return repo.distanceFromOrigin.value!!
    }

    // return field of view value from repo
    fun getFieldOfView(): Float {
        return repo.fieldOfView.value!!
    }

    // return rotation data from repo
    fun getRotationData(): Array<Float> {
        return repo.rotationData
    }

    // return acceleration data from repo
    fun getAccelerationData(): Array<Float> {
        return repo.accelerationData
    }

    // return linear acceleration data from repo
    fun getLinearAccelerationData(): Array<Float> {
        return repo.linearAccelerationData
    }

    // return simulation type from repo
    fun getVisualization(): Int {
        return repo.visualizationSelection
    }

    // update orientation in repo
    fun updateOrientation(orient: Int) {
        repo.updateOrientation(orient)
    }

    // return orientation from repo
    fun getOrientation(): Int {
        return repo.orientation
    }

    // update distance from origin in repo
    fun updateDistanceFromCenter(distance: Float) {
        repo.distanceFromOrigin.value = distance
    }

    // update field of view in repo
    fun updateFieldOfView(angle: Float) {
        repo.fieldOfView.value = angle
    }
    // update simulation type in repo, return true if value changed
    fun updateVisualizationSelection(selection: Int): Boolean {
        if (selection != repo.visualizationSelection) {
            repo.visualizationSelection = selection
            return true
        }
        return false
    }

    // update color in repo
    fun updateColor(color: Color) {
        repo.color = color
    }

    // return color from repo
    fun getColor(): Color {
        return repo.color
    }

    // update fps in repo (scale float to 2 decimal places)
    fun updateFPS(fps: Float) {
        // update fps in repo
        val rounded = fps.toBigDecimal().setScale(2, RoundingMode.UP).toDouble()
        repo.fps.postValue(rounded.toFloat())
    }

    // return fps from repo
    fun getFPS(): MutableLiveData<Float> {
        return repo.fps
    }

    // update last frame time value in repo
    fun updateLastFrame(new: Long){
        repo.lastFrame = new
    }

    // return last frame time value from repo
    fun getLastFrame(): Long {
        return repo.lastFrame
    }

    // register sensor events to repo sensor manager
    fun registerSensorEvents(manager: SensorManager) {
        repo.registerSensors(manager)
    }

    // return config string from repo
    fun getConfig(): String {
        val config = JSONObject()
        // store simulation type, background color, and settings (with default values for now
        config.put("name", "New Wallpaper")
        config.put("type", repo.visualizationSelection)
        val color = getColor()
        config.put("background_color", JSONObject("{\"r\": ${color.red()}, \"g\": ${color.green()}, \"b\": ${color.blue()}, \"a\": ${color.alpha()} }"))
        val eq = if (repo.equation != "") repo.equation else "1/((sqrt(x^2 + y^2) - 2 + 1.25cos(t))^2 + (z - 1.5sin(t))^2) + 1/((sqrt(x^2 + y^2) - 2 - 1.25cos(t))^2 + (z + 1.5sin(t))^2) = 1.9"
        config.put("settings", eq)

        return config.toString()
    }

    // load config table into repo
    fun loadConfig(table: SavedWallpaperTable) {
        // prevent reloading same config
        if (repo.wid == table.wid && repo.savedConfig == table.config) {
            return
        }
        try {
            repo.wid = table.wid
            val configJson = JSONObject(table.config)
            repo.visualizationSelection = configJson.getInt("type")
            val red = configJson.getJSONObject("background_color").getInt("r").toFloat()
            val green = configJson.getJSONObject("background_color").getInt("g").toFloat()
            val blue = configJson.getJSONObject("background_color").getInt("b").toFloat()
            val alpha = configJson.getJSONObject("background_color").getInt("a").toFloat()

            updateColor(Color.valueOf(red, green, blue, alpha))
            repo.equation = configJson.getString("settings")

            // store config string in repo
            repo.savedConfig = table.config
        } catch (e: Exception) {
            Log.d("LiveWallpaperLoad", "Error loading config: $e")
            Log.d("LiveWallpaperLoad", "Config: ${table.config}")
            return
        }
    }

    fun getWid(): Int {
        return repo.wid
    }

    fun setWid(wid: Int) {
        repo.wid = wid
    }

    fun updatePreviewImg(preview: Bitmap?) {
        if (preview != null)
            repo.preview = preview
    }

    fun updateEquation(toString: String) {
        repo.equation = toString
    }

    fun getEquation(): String {
        return repo.equation
    }

    fun updateGravity(value: Float) {
        repo.gravity.value = value
    }

    fun getGravity(): Float {
        return repo.gravity.value!!
    }

    fun updateLinearAcceleration(value: Float) {
        repo.linearAcceleration.value = value
    }

    fun getLinearAcceleration(): Float {
        return repo.linearAcceleration.value!!
    }

    fun updateEfficiency(value: Float) {
        repo.efficiency.value = value
    }

    fun getEfficiency(): Float {
        return repo.efficiency.value!!
    }

    fun constructJSONObjectFromViewModel() : JSONObject {
        var jsonConfig : JSONObject = JSONObject()
        when(getVisualization()){
            0 -> {
                jsonConfig = (visualization as NBodyVisualization).toJsonObject()
            }
            1 -> {
                jsonConfig = (visualization as NaiveFluidVisualization).toJsonObject()
            }
            2 -> {
                jsonConfig = (visualization as PicFlipVisualization).toJsonObject()
            }
            3 -> {
                jsonConfig = (visualization as TriangleVisualization).toJsonObject()
            }
            4 -> {
                jsonConfig = (visualization as GraphVisualization).toJsonObject()
            }
        }
        return jsonConfig
    }
}

/**
 * Factory for constructing ActiveWallpaperViewModel with repository parameter.
 *
 * This ensures that when a view model is created, it will be created with the same instance/reference no matter
 * what activity or fragment it is created from.
 */
class ActiveWallpaperViewModelFactory(private val repo: ActiveWallpaperRepo) : ViewModelProvider.Factory {
    override fun <T : ViewModel> create(modelClass: Class<T>): T {
        if (modelClass.isAssignableFrom(ActiveWallpaperViewModel::class.java)) {
            @Suppress("UNCHECKED_CAST")
            return ActiveWallpaperViewModel(repo) as T
        }
        throw IllegalArgumentException("Unknown ViewModel class")
    }
}