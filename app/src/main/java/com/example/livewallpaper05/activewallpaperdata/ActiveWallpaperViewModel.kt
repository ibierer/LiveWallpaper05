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
    var width: Int = 0
    var height: Int = 0
    private var mBitmap : Bitmap = Bitmap.createBitmap(500, 500, Bitmap.Config.ARGB_8888) as Bitmap
    var liveDataBitmap : MutableLiveData<Bitmap> = MutableLiveData<Bitmap>(mBitmap)
    var getScreenBuffer: Int = 0

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

    // return rotation rate value from repo
    fun getRotationRate(): Float {
        return repo.rotationRate
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
    fun getVisualizationType(): Int {
        return repo.simulationType
    }

    // update orientation in repo
    fun updateOrientation(orient: Int) {
        repo.updateOrientation(orient)
    }

    // return orientation from repo
    fun getOrientation(): Int {
        return repo.orientation
    }

    // update rotation rate in repo
    fun updateRotationRate(rate: Float) {
        repo.rotationRate = rate
    }

    // update simulation type in repo, return true if value changed
    fun updateSimulationType(type: Int): Boolean {
        if (type != repo.simulationType) {
            repo.simulationType = type
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
        config.put("type", repo.simulationType)
        config.put("background_color", JSONObject("{\"r\": 51, \"g\": 51, \"b\": 77, \"a\": 255}"))
        config.put("settings", "1/((sqrt(x^2 + y^2) - 2 + 1.25cos(t))^2 + (z - 1.5sin(t))^2) + 1/((sqrt(x^2 + y^2) - 2 - 1.25cos(t))^2 + (z + 1.5sin(t))^2) = 1.9")

        return config.toString()
    }

    // load config table into repo
    fun loadConfig(table: SavedWallpaperTable) {
        try {
            repo.wid = table.wid
            val configJson = JSONObject(table.config)
            repo.simulationType = configJson.getInt("type")
            val red = configJson.getJSONObject("background_color").getInt("r").toFloat()
            val green = configJson.getJSONObject("background_color").getInt("g").toFloat()
            val blue = configJson.getJSONObject("background_color").getInt("b").toFloat()
            val alpha = configJson.getJSONObject("background_color").getInt("a").toFloat()

            repo.color = Color.valueOf(red, green, blue, alpha)
            repo.equation = configJson.getString("settings")
            //repo.rotationRate = configJson.getDouble("rotationRate").toFloat()
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

    fun getSpeed(): Float {
        return repo.speed
    }

    fun updateSpeed(speed: Float) {
        repo.speed = speed
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