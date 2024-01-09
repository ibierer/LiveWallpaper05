package com.example.livewallpaper05.activewallpaperdata

import android.app.Activity
import android.hardware.SensorManager
import android.util.Log
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

    fun getSimulationType(): Int {
        return repo.simulationType
    }

    fun updateOrientation(orient: Int) {
        repo.updateOrientation(orient)
    }

    fun getOrientation(): Int {
        Log.d("Livewallpaper", "orientation home: ${repo.orientation}")

        return repo.orientation
    }

    fun updateRotationRate(rate: Float) {
        repo.rotationRate = rate
    }

    fun updateSimulationType(type: Int): Boolean {
        if (type != repo.simulationType) {
            repo.simulationType = type
            return true
        }
        return false
    }

    fun updateFPS(fps: Float) {
        // update fps in repo
        repo.fps.postValue(fps)
    }

    fun getFPS(): MutableLiveData<Float> {
        return repo.fps
    }

    fun getRepository(): ActiveWallpaperRepo {
        return mRepo
    }

    fun registerSensorEvents(manager: SensorManager) {
        repo.registerSensors(manager)
    }

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