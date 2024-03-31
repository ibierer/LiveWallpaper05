package com.example.livewallpaper05.activewallpaperdata

import android.app.Activity
import android.content.res.Resources
import android.graphics.Bitmap
import android.graphics.Color
import android.hardware.SensorManager
import android.util.Log
import androidx.lifecycle.*
import com.example.livewallpaper05.R
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperRepo.WallpaperRef
import com.example.livewallpaper05.savedWallpapers.SavedWallpaperTable
import kotlinx.coroutines.Job
import org.json.JSONObject
import java.math.RoundingMode
import java.util.Random

/**
 * View Model to keep a reference to the active wallpaper data
 */
class ActiveWallpaperViewModel(private val repo: ActiveWallpaperRepo) : ViewModel() {
    abstract class Visualization {
        var viewModel: ActiveWallpaperViewModel? = null

        abstract val relevantTextViewIds: List<Int>
        abstract val relevantSeekBarIds: List<Int>
        abstract val relevantEditTextIds: List<Int>
        abstract val relevantCheckBoxIds: List<Int>
        abstract fun toJsonObject(): JSONObject
        fun colorToJSONObject(color: Color): JSONObject {
            val colorJSONObject = JSONObject()
            colorJSONObject.put("r", (color.red() * 255).toInt())
            colorJSONObject.put("g", (color.green() * 255).toInt())
            colorJSONObject.put("b", (color.blue() * 255).toInt())
            colorJSONObject.put("a", (color.alpha() * 255).toInt())
            return colorJSONObject
        }
        fun jsonObjectToColor(colorJSONObject: JSONObject): Color {
            val r = colorJSONObject.getInt("r").toFloat()
            val g = colorJSONObject.getInt("g").toFloat()
            val b = colorJSONObject.getInt("b").toFloat()
            val a = colorJSONObject.getInt("a").toFloat()
            return Color.valueOf(a, r, g, b)
        }

        // set unique values for each visualization type
        abstract fun updateValues()

        companion object {
            fun createColorFromInts(r: Int, g: Int, b: Int, a: Int): Color {
                val rf = r.toFloat() / 255
                val gf = g.toFloat() / 255
                val bf = b.toFloat() / 255
                val af = a.toFloat() / 255
                return Color.valueOf(rf, gf, bf, af)
            }
            fun jsonObjectToColor(colorJSONObject: JSONObject): Color {
                val r = colorJSONObject.getInt("r").toFloat()
                val g = colorJSONObject.getInt("g").toFloat()
                val b = colorJSONObject.getInt("b").toFloat()
                val a = colorJSONObject.getInt("a").toFloat()
                return Color.valueOf(a, r, g, b)
            }
        }
    }
    data class NBodyVisualization (
        private var job: Job? = null,
        val visualizationType: String = "simulation",
        val simulationType: String = "nbody",
        var distance: Float = 0.5f,
        var fieldOfView: Float = 60.0f,
        var backgroundColor: Color = createColorFromInts(0, 0, 0, 0),
        var backgroundIsSolidColor: Boolean = true,
        var backgroundTexture: String = "ms_paint_colors"
    ) : Visualization() {

        constructor(jsonObject: JSONObject) : this(
            visualizationType = jsonObject.optString("visualization_type", "simulation"),
            simulationType = jsonObject.optString("simulation_type", "nbody"),
            distance = jsonObject.optDouble("distance", 0.5).toFloat(),
            fieldOfView = jsonObject.optDouble("field_of_view", 60.0).toFloat(),
            backgroundColor = jsonObject.optJSONObject("background_color")?.let { Visualization.jsonObjectToColor(it) }?: createColorFromInts(0, 0, 0, 0),
            backgroundIsSolidColor = jsonObject.optBoolean("background_is_solid_color", true),
            backgroundTexture = jsonObject.optString("background_texture", "ms_paint_colors")
        )

        override val relevantTextViewIds : List<Int> = listOf(
            R.id.distance_label,
            R.id.field_of_view_label
        )

        override fun updateValues() {
            if (viewModel == null) {
                return
            }
            // change this to observe the live data from the repo
            distance = viewModel!!.getDistanceFromOrigin()
            fieldOfView = viewModel!!.getFieldOfView()
            backgroundColor = viewModel!!.getColor()

        }

        override val relevantSeekBarIds : List<Int> = listOf(
            R.id.distance_seekbar,
            R.id.field_of_view_seekbar
        )
        override val relevantEditTextIds: List<Int> = listOf(

        )
        override val relevantCheckBoxIds: List<Int> = listOf(

        )

        override fun toJsonObject() : JSONObject {
            val jsonObject = JSONObject()
            jsonObject.put("visualization_type", visualizationType)
            jsonObject.put("simulation_type", simulationType)
            jsonObject.put("distance", distance)
            jsonObject.put("field_of_view", fieldOfView)
            jsonObject.put("background_color", colorToJSONObject(backgroundColor))
            jsonObject.put("background_is_solid_color", backgroundIsSolidColor)
            jsonObject.put("background_texture", backgroundTexture)
            return jsonObject
        }
    }
    data class NaiveFluidVisualization (
        val visualizationType: String = "simulation",
        val simulationType: String = "naive",
        var distance: Float = 0.5f,
        var fieldOfView: Float = 60.0f,
        var backgroundColor: Color = createColorFromInts(0, 0, 0, 0),
        var fluidSurface: Boolean = true,
        var particleCount: Int = 1000,
        var smoothSphereSurface: Boolean = true,
        var gravity: Float = 0.0f,
        var linearAcceleration: Float = 1.0f,
        var efficiency: Float = 1.0f,
        var referenceFrameRotates: Boolean = false,
        var backgroundIsSolidColor: Boolean = false,
        var backgroundTexture: String = "ms_paint_colors"
    ) : Visualization() {

        constructor(jsonObject: JSONObject) : this(
            visualizationType = jsonObject.optString("visualization_type", "simulation"),
            simulationType = jsonObject.optString("simulation_type", "naive"),
            distance = jsonObject.optDouble("distance", 0.5).toFloat(),
            fieldOfView = jsonObject.optDouble("field_of_view", 60.0).toFloat(),
            backgroundColor = jsonObject.optJSONObject("background_color")?.let { Visualization.jsonObjectToColor(it) }?: createColorFromInts(0, 0, 0, 0),
            fluidSurface = jsonObject.optBoolean("fluid_surface", true),
            particleCount = jsonObject.optInt("particle_count", 1000),
            smoothSphereSurface = jsonObject.optBoolean("smooth_sphere_surface", true),
            gravity = jsonObject.optDouble("gravity", 0.0).toFloat(),
            linearAcceleration = jsonObject.optDouble("linear_acceleration", 1.0).toFloat(),
            efficiency = jsonObject.optDouble("efficiency", 1.0).toFloat(),
            referenceFrameRotates = jsonObject.optBoolean("reference_frame_rotates", false),
            backgroundIsSolidColor = jsonObject.optBoolean("background_is_solid_color", false),
            backgroundTexture = jsonObject.optString("background_texture", "ms_paint_colors")
        )

        override val relevantTextViewIds : List<Int> = listOf(
            R.id.distance_label,
            R.id.field_of_view_label,
            R.id.gravity_label,
            R.id.linear_acceleration_label,
            R.id.efficiency_label
        )
        override val relevantSeekBarIds : List<Int> = listOf(
            R.id.distance_seekbar,
            R.id.field_of_view_seekbar,
            R.id.gravity_seekbar,
            R.id.linear_acceleration_seekbar,
            R.id.efficiency_seekbar
        )
        override val relevantEditTextIds: List<Int> = listOf(

        )
        override val relevantCheckBoxIds: List<Int> = listOf(

        )

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
            jsonObject.put("background_color", colorToJSONObject(backgroundColor))
            jsonObject.put("efficiency", efficiency)
            jsonObject.put("reference_frame_rotates", referenceFrameRotates)
            jsonObject.put("background_is_solid_color", backgroundIsSolidColor)
            jsonObject.put("background_texture", backgroundTexture)
            return jsonObject
        }

        override fun updateValues() {
            if (viewModel == null) {
                return
            }
            // get values from repo
            distance = viewModel!!.getDistanceFromOrigin()
            fieldOfView = viewModel!!.getFieldOfView()
            backgroundColor = viewModel!!.getColor()
            gravity = viewModel!!.getGravity()
            linearAcceleration = viewModel!!.getLinearAcceleration()
            efficiency = viewModel!!.getEfficiency()
        }
    }
    data class PicFlipVisualization (
        val visualizationType: String = "simulation",
        val simulationType: String = "picflip",
        var distance: Float = 0.5f,
        var fieldOfView: Float = 60.0f,
        var backgroundColor: Color = createColorFromInts(0, 0, 0, 0),
        var gravity: Float = 0.0f,
        var linearAcceleration: Float = 1.0f,
        var referenceFrameRotates: Boolean = true,
        var backgroundIsSolidColor: Boolean = false,
        var backgroundTexture: String = "ms_paint_colors"
    ) : Visualization() {
        constructor(jsonObject: JSONObject) : this(
            visualizationType = jsonObject.optString("visualization_type", "simulation"),
            simulationType = jsonObject.optString("simulation_type", "picflip"),
            distance = jsonObject.optDouble("distance", 0.5).toFloat(),
            fieldOfView = jsonObject.optDouble("field_of_view", 60.0).toFloat(),
            gravity = jsonObject.optDouble("gravity", 0.0).toFloat(),
            linearAcceleration = jsonObject.optDouble("linear_acceleration", 1.0).toFloat(),
            backgroundColor = jsonObject.optJSONObject("background_color")?.let { Visualization.jsonObjectToColor(it) }?: createColorFromInts(0, 0, 0, 0),
            referenceFrameRotates = jsonObject.optBoolean("reference_frame_rotates", true),
            backgroundIsSolidColor = jsonObject.optBoolean("background_is_solid_color", false),
            backgroundTexture = jsonObject.optString("background_texture", "ms_paint_colors")
        )

        override val relevantTextViewIds : List<Int> = listOf(
            R.id.distance_label,
            R.id.field_of_view_label,
            R.id.gravity_label,
            R.id.linear_acceleration_label
        )
        override val relevantSeekBarIds : List<Int> = listOf(
            R.id.distance_seekbar,
            R.id.field_of_view_seekbar,
            R.id.gravity_seekbar,
            R.id.linear_acceleration_seekbar
        )
        override val relevantEditTextIds: List<Int> = listOf(

        )
        override val relevantCheckBoxIds: List<Int> = listOf(

        )

        override fun toJsonObject() : JSONObject {
            val jsonObject = JSONObject()
            jsonObject.put("visualization_type", visualizationType)
            jsonObject.put("simulation_type", simulationType)
            jsonObject.put("distance", distance)
            jsonObject.put("field_of_view", fieldOfView)
            jsonObject.put("gravity", gravity)
            jsonObject.put("linear_acceleration", linearAcceleration)
            jsonObject.put("background_color", colorToJSONObject(backgroundColor))
            jsonObject.put("reference_frame_rotates", referenceFrameRotates)
            jsonObject.put("background_is_solid_color", backgroundIsSolidColor)
            jsonObject.put("background_texture", backgroundTexture)
            return jsonObject
        }

        override fun updateValues() {
            if (viewModel == null) {
                return
            }
            // get values from repo
            distance = viewModel!!.getDistanceFromOrigin()
            fieldOfView = viewModel!!.getFieldOfView()
            backgroundColor = viewModel!!.getColor()
            gravity = viewModel!!.getGravity()
            linearAcceleration = viewModel!!.getLinearAcceleration()

        }

    }
    data class TriangleVisualization (
        val visualizationType: String = "other",
        var distance: Float = 0.5f,
        var fieldOfView: Float = 60.0f,
        var backgroundColor: Color = createColorFromInts(0, 0, 0, 0),
        var backgroundIsSolidColor: Boolean = false,
        var backgroundTexture: String = "mandelbrot"
    ) : Visualization() {

        constructor(jsonObject: JSONObject) : this(
            visualizationType = jsonObject.optString("visualization_type", "other"),
            distance = jsonObject.optDouble("distance", 0.5).toFloat(),
            fieldOfView = jsonObject.optDouble("field_of_view", 60.0).toFloat(),
            backgroundColor = jsonObject.optJSONObject("background_color")?.let { Visualization.jsonObjectToColor(it) }?: createColorFromInts(0, 0, 0, 0),
            backgroundIsSolidColor = jsonObject.optBoolean("background_is_solid_color", false),
            backgroundTexture = jsonObject.optString("background_texture", "mandelbrot")
        )

        override val relevantTextViewIds : List<Int> = listOf(
            R.id.distance_label,
            R.id.field_of_view_label
        )
        override val relevantSeekBarIds : List<Int> = listOf(
            R.id.distance_seekbar,
            R.id.field_of_view_seekbar
        )
        override val relevantEditTextIds: List<Int> = listOf(

        )
        override val relevantCheckBoxIds: List<Int> = listOf(

        )

        override fun toJsonObject() : JSONObject {
            val jsonObject = JSONObject()
            jsonObject.put("visualization_type", visualizationType)
            jsonObject.put("distance", distance)
            jsonObject.put("field_of_view", fieldOfView)
            jsonObject.put("background_color", colorToJSONObject(backgroundColor))
            jsonObject.put("background_is_solid_color", backgroundIsSolidColor)
            jsonObject.put("background_texture", backgroundTexture)
            return jsonObject
        }

        override fun updateValues() {
            if (viewModel == null) {
                return
            }
            // get values from repo
            distance = viewModel!!.getDistanceFromOrigin()
            fieldOfView = viewModel!!.getFieldOfView()
            backgroundColor = viewModel!!.getColor()
        }

    }
    data class GraphVisualization (
        val visualizationType: String = "graph",
        var distance: Float = 0.5f,
        var fieldOfView: Float = 60.0f,
        var backgroundColor: Color = createColorFromInts(0, 0, 0, 0),
        var referenceFrameRotates: Boolean = false,
        var backgroundIsSolidColor: Boolean = false,
        var backgroundTexture: String = "ms_paint_colors",
        var vectorPointsPositive: Boolean = false,
        var equation: String = "1/((sqrt(x^2 + y^2) - 1.5 + sin(t))^2 + (z + cos(t))^2) + 1/((sqrt(x^2 + y^2) - 1.5 + sin(t + 2π/3))^2 + (z + cos(t + 2π/3))^2) + 1/((sqrt(x^2 + y^2) - 1.5 + sin(t + 4π/3))^2 + (z + cos(t + 4π/3))^2) = 5"
    ) : Visualization() {

        constructor(jsonObject: JSONObject) : this(
            visualizationType = jsonObject.optString("visualization_type", "graph"),
            distance = jsonObject.optDouble("distance", 0.5).toFloat(),
            fieldOfView = jsonObject.optDouble("field_of_view", 60.0).toFloat(),
            backgroundColor = jsonObject.optJSONObject("background_color")?.let { Visualization.jsonObjectToColor(it) }?: createColorFromInts(0, 0, 0, 0),
            referenceFrameRotates = jsonObject.optBoolean("reference_frame_rotates", false),
            backgroundIsSolidColor = jsonObject.optBoolean("background_is_solid_color", false),
            backgroundTexture = jsonObject.optString("background_texture", "ms_paint_colors"),
            vectorPointsPositive = jsonObject.optBoolean("vector_points_positive", false),
            equation = jsonObject.optString("equation", "1/((sqrt(x^2 + y^2) - 1.5 + sin(t))^2 + (z + cos(t))^2) + 1/((sqrt(x^2 + y^2) - 1.5 + sin(t + 2π/3))^2 + (z + cos(t + 2π/3))^2) + 1/((sqrt(x^2 + y^2) - 1.5 + sin(t + 4π/3))^2 + (z + cos(t + 4π/3))^2) = 5")
        )

        override val relevantTextViewIds : List<Int> = listOf(
            R.id.distance_label,
            R.id.field_of_view_label,
            R.id.tv_equation,
            R.id.tv_syntax_check
        )
        override val relevantSeekBarIds : List<Int> = listOf(
            R.id.distance_seekbar,
            R.id.field_of_view_seekbar
        )
        override val relevantEditTextIds: List<Int> = listOf(
            R.id.et_equation
        )
        override val relevantCheckBoxIds: List<Int> = listOf(
            R.id.flip_normals_checkbox
        )

        override fun toJsonObject() : JSONObject {
            val jsonObject = JSONObject()
            jsonObject.put("visualization_type", visualizationType)
            jsonObject.put("distance", distance)
            jsonObject.put("field_of_view", fieldOfView)
            jsonObject.put("background_color", colorToJSONObject(backgroundColor))
            jsonObject.put("reference_frame_rotates", referenceFrameRotates)
            jsonObject.put("background_is_solid_color", backgroundIsSolidColor)
            jsonObject.put("background_texture", backgroundTexture)
            jsonObject.put("vector_points_positive", vectorPointsPositive)
            jsonObject.put("equation", equation)
            return jsonObject
        }

        override fun updateValues() {
            if (viewModel == null) {
                return
            }
            // get values from repo
            distance = viewModel!!.getDistanceFromOrigin()
            fieldOfView = viewModel!!.getFieldOfView()
            backgroundColor = viewModel!!.getColor()
            equation = viewModel!!.getEquation()
        }

    }

    // initialize local variables for active wallpaper
    lateinit var visualization : Visualization
    var width: Int = 0
    var height: Int = 0
    private var mBitmap : Bitmap = Bitmap.createBitmap(500, 500, Bitmap.Config.ARGB_8888) as Bitmap
    var liveDataBitmap : MutableLiveData<Bitmap> = MutableLiveData<Bitmap>(mBitmap)
    var getScreenBuffer: Int = 0
    var saveAsNew: Int = 0

    // reference repo from constructor value
    val mRepo: ActiveWallpaperRepo = repo
    var isCollapsed = false

    // initialize local variables for saved wallpapers
    val activeWallpaper: LiveData<SavedWallpaperTable> = repo.activeWallpaper
    val savedWallpapers: LiveData<List<SavedWallpaperTable>> = repo.wallpapers

    // GETTERS - here are all the methods for getting data from the repo -------------------

    // grab bitmap from repo
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

    // return orientation from repo
    fun getOrientation(): Int {
        return repo.orientation
    }

    // return color from repo
    fun getColor(): Color {
        return repo.color.value!!
    }

    // return fps from repo
    fun getFPS(): MutableLiveData<Float> {
        return repo.fps
    }

    // return last frame time value from repo
    fun getLastFrame(): Long {
        return repo.lastFrame
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

        // [TODO] temp solution
        when (repo.visualizationSelection){
            0 -> {
                //var sim = NBodyVisualization()
                config.put("distance", getDistanceFromOrigin())
                config.put("field_of_view", getFieldOfView())
            }
            1 -> {
                var sim = NaiveFluidVisualization()
                config.put("distance", getDistanceFromOrigin())
                config.put("field_of_view", getFieldOfView())
                config.put("background_is_solid_color", sim.backgroundIsSolidColor)
                config.put("background_texture", sim.backgroundTexture)
                config.put("gravity", getGravity())
                config.put("linear_acceleration", getLinearAcceleration())
                config.put("efficiency", getEfficiency())
                config.put("particle_count", sim.particleCount)
            }
            2 -> {
                var sim = PicFlipVisualization()
                config.put("distance", sim.distance)
                config.put("field_of_view", sim.fieldOfView)
                config.put("background_is_solid_color", sim.backgroundIsSolidColor)
                config.put("background_texture", sim.backgroundTexture)
                config.put("gravity", sim.gravity)
                config.put("linear_acceleration", sim.linearAcceleration)
            }
            3 -> {
                var sim = TriangleVisualization()
                config.put("distance", sim.distance)
                config.put("field_of_view", sim.fieldOfView)
                config.put("background_is_solid_color", sim.backgroundIsSolidColor)
                config.put("background_texture", sim.backgroundTexture)
            }
            4 -> {
                var sim = GraphVisualization()
                config.put("distance", sim.distance)
                config.put("field_of_view", sim.fieldOfView)
                config.put("background_is_solid_color", sim.backgroundIsSolidColor)
                config.put("background_texture", sim.backgroundTexture)
                config.put("equation", sim.equation)
            }
        }

        return config.toString()

    }

    fun getWid(): Int {
        return repo.wid
    }

    // get equation string from repo
    fun getEquation(): String {
        return repo.equation
    }

    // get efficiency value from repo
    fun getEfficiency(): Float {
        return repo.efficiency.value!!
    }

    // get vector direction from repo
    fun getVectorDirection(): Boolean {
        return repo.flipNormals.value!!
    }

    // get linear acceleration value from repo
    fun getLinearAcceleration(): Float {
        return repo.linearAcceleration.value!!
    }

    // get gravity value from repo
    fun getGravity(): Float {
        return repo.gravity.value!!
    }

    // get the wallpaper id list for the wallpaper preview fragment(s)
    fun getWallpaperFragIds() : MutableList<WallpaperRef> {
        return mRepo.wallpaperFragIds
    }

    // SETTERS - here are all the methods for updating data in the repo --------------------

    // update visualization type in repo
    fun updateSimulationType(type: String) {
        repo.visualizationName.postValue(type)
    }

    // update orientation in repo
    fun updateOrientation(orient: Int) {
        repo.updateOrientation(orient)
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
        repo.color.value = color
    }

    // update fps in repo (scale float to 2 decimal places)
    fun updateFPS(fps: Float) {
        // update fps in repo
        val rounded = fps.toBigDecimal().setScale(2, RoundingMode.UP).toDouble()
        repo.fps.postValue(rounded.toFloat())
    }

    // update last frame time value in repo
    fun updateLastFrame(new: Long){
        repo.lastFrame = new
    }

    // update wallpaper id in repo
    fun setWid(wid: Int) {
        repo.wid = wid
    }

    // update preview image in repo
    fun updatePreviewImg(preview: Bitmap?) {
        if (preview != null)
            repo.preview = preview
    }

    // update equation string in repo
    fun updateEquation(toString: String) {
        repo.equation = toString
    }

    // update gravity value in repo
    fun updateGravity(value: Float) {
        repo.gravity.value = value
    }

    // update efficiency value in repo
    fun updateEfficiency(value: Float) {
        repo.efficiency.value = value
    }

    // update particle count value in repo
    fun updateParticleCount(value: Int) {
        repo.particleCount.value = value
    }

    // update linear acceleration value in repo
    fun updateLinearAcceleration(value: Float) {
        repo.linearAcceleration.value = value
    }

    // update wallpaper preview fragments list in repo
    fun updateWallpaperFragIds(wallpaperRef: WallpaperRef) {
        // if wallpaperRef is already in list ignore
        var data = mRepo.wallpaperFragIds
        if (data != null) {
            for (r in data){
                if (r.wallpaperId == wallpaperRef.wallpaperId) {
                    return
                }
            }
            // add new wallpaperRef to list
            data.add(wallpaperRef)
        }
    }

    // update viz name in repo
    private fun updateVizualizationName(s: String) {
        repo.visualizationName.postValue(s)
    }

    // OTHER - other helper methods --------------------------------------------------------

    // register sensor events to repo sensor manager
    fun registerSensorEvents(manager: SensorManager) {
        repo.registerSensors(manager)
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

            // update visualization specific parameters
            // [TODO] - add all viz parameters here and in repo (requires getter and setter functions as well)
            when (repo.visualizationSelection) {
                0 -> {
                    var sim = NBodyVisualization(configJson)
                    updateDistanceFromCenter(sim.distance)
                    updateFieldOfView(sim.fieldOfView)
                    updateColor(sim.backgroundColor)
                    updateVizualizationName("N-Body Simulation")
                }
                1 -> {
                    var sim = NaiveFluidVisualization(configJson)
                    updateDistanceFromCenter(sim.distance)
                    updateFieldOfView(sim.fieldOfView)
                    updateColor(sim.backgroundColor)
                    updateGravity(sim.gravity)
                    updateLinearAcceleration(sim.linearAcceleration)
                    updateEfficiency(sim.efficiency)
                    updateParticleCount(sim.particleCount)
                    updateVizualizationName("Naive Fluid Simulation")
                }
                2 -> {
                    var sim = PicFlipVisualization(configJson)
                    updateDistanceFromCenter(sim.distance)
                    updateFieldOfView(sim.fieldOfView)
                    updateColor(sim.backgroundColor)
                    updateGravity(sim.gravity)
                    updateLinearAcceleration(sim.linearAcceleration)
                    updateVizualizationName("PicFlip Simulation")
                }
                3 -> {
                    var sim = TriangleVisualization(configJson)
                    updateDistanceFromCenter(sim.distance)
                    updateFieldOfView(sim.fieldOfView)
                    updateColor(sim.backgroundColor)
                    updateVizualizationName("Triangle Visualization")
                }
                4 -> {
                    var sim = GraphVisualization(configJson)
                    updateDistanceFromCenter(sim.distance)
                    updateFieldOfView(sim.fieldOfView)
                    updateColor(sim.backgroundColor)
                    updateEquation(sim.equation)
                    updateVizualizationName("Graph Visualization")
                }
            }

            // store config string in repo
            repo.savedConfig = table.config

        } catch (e: Exception) {
            Log.d("LiveWallpaperLoad", "Error loading config: $e")
            Log.d("LiveWallpaperLoad", "Config: ${table.config}")
            return
        }
    }

    // switch active wallpaper
    fun switchWallpaper(wid: Int) {
        mRepo.setLiveWallpaperData(wid)
    }

    fun saveWids(activity: Activity){
        // get wids from repo
        val wids = mRepo.getSavedWids()
        // save wids to shared prefs
        activity.getSharedPreferences("livewallpaper05", 0).edit()
            .putString("savedWids", wids).apply()
    }

    fun removeWid(wid: Int){
        mRepo.removeWid(wid)
    }

    fun loadWidsFromMem(activity: Activity){
        try {
            // load wids from shared prefs
            val wids =
                activity.getSharedPreferences("livewallpaper05", 0).getString("savedWids", "")
            if (wids == null || wids == "") {
                return
            }
            // set wids in repo
            mRepo.setSavedWids(wids!!)
        } catch (e: Exception) {
            Log.d("LiveWallpaper05", "Error loading wids: $e")
        }
    }

    // save wallpaper from config string
    fun saveWallpaper(config: String) : Int {
        // create new wallpaper table with given data
        var wallpaper = SavedWallpaperTable(
            1,
            config
        )
        try {
            wallpaper = SavedWallpaperTable(
                activeWallpaper.value!!.wid,
                config
            )
        } catch (e: Exception) {}
        // update profile table
        mRepo.setWallpaper(wallpaper)

        return wallpaper.wid
    }

    // delete wallpaper
    fun deleteWallpaper(wid: Int) {
        mRepo.deleteWallpaper(wid)
    }

    // create new wallpaper table
    fun createWallpaperTable(id: Int) {
        var new_wallpaper = mRepo.createWallpaperTable(id)
        mRepo.setWallpaper(new_wallpaper)
    }

    // create wallpaper save with default values
    fun createDefaultWallpaperTable(wid: Int, config: String) {
        val wallpaper = SavedWallpaperTable(
            wid,
            config
        )
        mRepo.setWallpaper(wallpaper)
    }

    // remove specified wallpapers from fragment list via wallpaper reference
    fun removeWallpaperFragId(wallpaperRef: WallpaperRef) {
        // remove wallpaperRef from list
        var data = mRepo.wallpaperFragIds
        var toRemove = listOf<WallpaperRef>()
        if (data != null) {
            for (r in data){
                if (r.wallpaperId == wallpaperRef.wallpaperId) {
                    toRemove = toRemove.plus(r)
                }
            }
            data.removeAll(toRemove)
        }
    }

    // save active wallpaper and switch to new wallpaper. This allows us to save
    // the wallpaper before changing it, ensuring no changes are forgotten
    fun saveSwitchWallpaper(activeWid: Int, activeConfig: String) {
        mRepo.saveSwitchWallpaper(activeWid, activeConfig)
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
            val vm = ActiveWallpaperViewModel(repo) as T
            return vm
            //return ActiveWallpaperViewModel(repo) as T
        }
        throw IllegalArgumentException("Unknown ViewModel class")
    }
}