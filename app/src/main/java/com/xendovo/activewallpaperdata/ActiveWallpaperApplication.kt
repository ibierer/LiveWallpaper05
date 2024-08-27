package com.xendovo.activewallpaperdata

import android.app.Application
import android.content.Context
import android.hardware.SensorManager
import android.util.Log
import com.google.android.gms.ads.MobileAds
import com.xendovo.R
//import com.example.livewallpaper05.GLES3JNIView
//import android.util.Log
//import com.example.livewallpaper05.profiledata.ProfileRoomDatabase
//import com.example.livewallpaper05.savedWallpapers.SavedWallpaperRoomDatabase
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.SupervisorJob
import yuku.ambilwarna.BuildConfig

class ActiveWallpaperApplication : Application() {

    // wallpaper repo
    val wallpaperRepo: ActiveWallpaperRepo by lazy {
        ActiveWallpaperRepo.getInstance(
            this,
            applicationScope
        )
    }

    override fun onCreate() {
        super.onCreate()
        wallpaperRepo.registerSensors(getSystemService(Context.SENSOR_SERVICE) as SensorManager)
        //if (BuildConfig.FLAVOR == "free") {
        if (getString(R.string.flavor) == "free") {
            MobileAds.initialize(this)
        }
    }

    companion object {
        // set lifecycle scope for coroutines
        val applicationScope : CoroutineScope = CoroutineScope(SupervisorJob())
        init {
            // Used to load the 'livewallpaper05' library on application startup.
            System.loadLibrary("livewallpaper05")
        }
    }

}