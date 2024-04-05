package com.example.livewallpaper05.activewallpaperdata

import android.app.Application
import android.util.Log
import com.example.livewallpaper05.profiledata.ProfileRoomDatabase
import com.example.livewallpaper05.profiledata.ProfileTable
import com.example.livewallpaper05.savedWallpapers.SavedWallpaperRow
import com.example.livewallpaper05.savedWallpapers.SavedWallpaperRoomDatabase
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext

/**
 * This class manages the active wallpaper data lifecycle and scope for coroutines (not yet fully implemented)
 */
class ActiveWallpaperApplication : Application() {

    // set lifecycle scope for coroutines
    val applicationScope : CoroutineScope = CoroutineScope(SupervisorJob())

    // wallpaper database
    private val savedWallpaperDatabase : SavedWallpaperRoomDatabase by lazy { SavedWallpaperRoomDatabase.getDatabase(this)}
    // profile database
    private val profileDatabase : ProfileRoomDatabase by lazy { ProfileRoomDatabase.getDatabase(this, applicationScope)}
    // wallpaper repo
    val wallpaperRepo : ActiveWallpaperRepo by lazy { ActiveWallpaperRepo.getInstance(this, savedWallpaperDatabase.wallpaperDao(), profileDatabase.profileDao(), applicationScope)}

    fun printProfilesAndWallpapersToLogcat(message : String){
        CoroutineScope(Dispatchers.Main).launch {
            val profileD : ProfileTable? = wallpaperRepo.currentUserProfile.value
            Log.d("WALLPAPERS", "$message\nprofile data: $profileD")
            val wallpapers : List<SavedWallpaperRow> = withContext(Dispatchers.IO) {
                savedWallpaperDatabase.wallpaperDao().getAllWallpapers()
            }
            for (element in wallpapers){
                Log.d("WALLPAPERS", "$message\n$element")
            }
        }
    }
}