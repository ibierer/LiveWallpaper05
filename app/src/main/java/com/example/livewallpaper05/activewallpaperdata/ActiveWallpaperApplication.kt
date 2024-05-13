package com.example.livewallpaper05.activewallpaperdata

import android.app.Application
import com.example.livewallpaper05.GLES3JNIView

class ActiveWallpaperApplication : Application() {

    companion object {
        var mView: GLES3JNIView? = null
    }

}