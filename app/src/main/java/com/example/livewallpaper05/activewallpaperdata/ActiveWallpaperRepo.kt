package com.example.livewallpaper05.activewallpaperdata

import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager
import android.util.Log
import androidx.lifecycle.MutableLiveData
import kotlinx.coroutines.CoroutineScope

class ActiveWallpaperRepo private constructor () : SensorEventListener {
    var orientation: Int = 0
    var fps: MutableLiveData<Float> = MutableLiveData<Float>(0.0f)
    var lastFrame: Long = 0
    var rotationRate: Float = 0.0f
    var rotationData: Array<Float> = arrayOf(0.0f,0.0f,0.0f,0.0f)
    var accelerationData: Array<Float> = arrayOf(0.0f,0.0f,0.0f)
    var simulationType: Int = 0

    private lateinit var mSensorManager: SensorManager

    companion object {
        @Volatile
        private var instance: ActiveWallpaperRepo? = null
        private lateinit var mScope: CoroutineScope

        @Synchronized
        fun getInstance(
            scope: CoroutineScope
        ): ActiveWallpaperRepo {
            mScope = scope
            return instance ?: synchronized(this) {
                val inst = ActiveWallpaperRepo()
                instance = inst
                inst
            }
        }
    }

    fun registerSensors(mSensorManager: SensorManager){
        this.mSensorManager = mSensorManager
        mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR), SensorManager.SENSOR_DELAY_GAME)
        mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER), SensorManager.SENSOR_DELAY_GAME)
    }

    override fun onSensorChanged(p0: SensorEvent?) {
        if (p0 != null) {
            if (p0.sensor.type == Sensor.TYPE_ROTATION_VECTOR) {
                rotationData = p0.values.toTypedArray()
            }
            if (p0.sensor.type == Sensor.TYPE_ACCELEROMETER) {
                accelerationData = p0.values.toTypedArray()
            }
        }
    }

    override fun onAccuracyChanged(p0: Sensor?, p1: Int) {
        // Do nothing
    }

    fun updateOrientation(orient: Int) {
        this.orientation = orient
    }

}