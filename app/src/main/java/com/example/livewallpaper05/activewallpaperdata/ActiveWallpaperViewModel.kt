package com.example.livewallpaper05.activewallpaperdata

import android.app.Activity
import android.graphics.Color
import android.hardware.SensorManager
import android.util.Log
import androidx.lifecycle.*
import org.json.JSONObject
import java.math.RoundingMode

/**
 * View Model to keep a reference to the active wallpaper data
 */
class ActiveWallpaperViewModel(private val repo: ActiveWallpaperRepo) : ViewModel() {
    // reference repo from constructor value
    val mRepo: ActiveWallpaperRepo = repo

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

    // return simulation type from repo
    fun getSimulationType(): Int {
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
        var rounded = fps.toBigDecimal().setScale(2, RoundingMode.UP).toDouble()
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
        var config = JSONObject()
        config.put("rotationRate", repo.rotationRate)
        return config.toString()
    }

    // load config string into repo
    fun loadConfig(config: String) {
        val configJson = JSONObject(config)
        repo.rotationRate = configJson.getDouble("rotationRate").toFloat()
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