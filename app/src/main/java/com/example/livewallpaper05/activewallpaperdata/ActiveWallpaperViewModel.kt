package com.example.livewallpaper05.activewallpaperdata

import android.hardware.SensorManager
import androidx.lifecycle.*

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