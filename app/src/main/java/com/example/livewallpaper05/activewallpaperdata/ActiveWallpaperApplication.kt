package com.example.livewallpaper05.activewallpaperdata

import android.app.Application
import com.example.livewallpaper05.GLES3JNIView
import android.util.Log
import com.example.livewallpaper05.profiledata.ProfileRoomDatabase
import com.example.livewallpaper05.savedWallpapers.SavedWallpaperRoomDatabase
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.SupervisorJob

class ActiveWallpaperApplication : Application() {

    // set lifecycle scope for coroutines
    val applicationScope : CoroutineScope = CoroutineScope(SupervisorJob())

    // wallpaper database
    private val savedWallpaperDatabase : SavedWallpaperRoomDatabase by lazy { SavedWallpaperRoomDatabase.getDatabase(this)}
    // profile database
    private val profileDatabase : ProfileRoomDatabase by lazy { ProfileRoomDatabase.getDatabase(this, applicationScope)}
    // wallpaper repo
    val wallpaperRepo : ActiveWallpaperRepo by lazy { ActiveWallpaperRepo.getInstance(this, savedWallpaperDatabase.wallpaperDao(), profileDatabase.profileDao(), applicationScope)}

    companion object {
        var mView: GLES3JNIView? = null
        init {
            // Used to load the 'livewallpaper05' library on application startup.
            System.loadLibrary("livewallpaper05")
        }
    }

}