package com.example.livewallpaper05.activewallpaperdata

import android.app.Application
import com.example.livewallpaper05.GLES3JNIView
import android.util.Log

class ActiveWallpaperApplication : Application() {

    companion object {
        var mView: GLES3JNIView? = null
        init {
            // Used to load the 'livewallpaper05' library on application startup.
            System.loadLibrary("livewallpaper05")
        }
    }

}