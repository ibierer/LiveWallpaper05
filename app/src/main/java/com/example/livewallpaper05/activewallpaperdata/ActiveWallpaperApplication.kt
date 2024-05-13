package com.example.livewallpaper05.activewallpaperdata

import android.app.Application
import android.app.Activity
import android.os.Bundle
import com.example.livewallpaper05.GLES3JNIView

class ActiveWallpaperApplication : Application() {

    companion object {
        var mView: GLES3JNIView? = null
    }

}