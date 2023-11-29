package com.example.livewallpaper05.activewallpaperdata

import android.hardware.SensorManager
import androidx.lifecycle.*
import org.json.JSONObject

/**
 * View Model to keep a reference to the active wallpaper data
 */
class ActiveWallpaperViewModel(private val repo: ActiveWallpaperRepo) : ViewModel() {

    val mRepo: ActiveWallpaperRepo = repo

    fun getRotationRate(): Float {
        return repo.rotationRate
    }

    fun getRotationData(): Array<Float> {
        return repo.rotationData
    }

    fun getAccelerationData(): Array<Float> {
        return repo.accelerationData
    }

    fun updateRotationRate(rate: Float) {
        repo.rotationRate = rate
    }

    fun getRepository(): ActiveWallpaperRepo {
        return mRepo
    }

    fun registerSensorEvents(manager: SensorManager) {
        repo.registerSensors(manager)
    }

    // create a config json string to pass to the wallpaper service
    fun getConfig(): String {
        var config = JSONObject()
        config.put("rotationRate", repo.rotationRate)
        return config.toString()
    }

    fun loadConfig(config: String) {
        val configJson = JSONObject(config)
        repo.rotationRate = configJson.getDouble("rotationRate").toFloat()
    }
}

class ActiveWallpaperViewModelFactory(private val repo: ActiveWallpaperRepo) : ViewModelProvider.Factory {
    override fun <T : ViewModel> create(modelClass: Class<T>): T {
        if (modelClass.isAssignableFrom(ActiveWallpaperViewModel::class.java)) {
            @Suppress("UNCHECKED_CAST")
            return ActiveWallpaperViewModel(repo) as T
        }
        throw IllegalArgumentException("Unknown ViewModel class")
    }
}