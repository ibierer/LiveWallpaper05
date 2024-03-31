package com.example.livewallpaper05.activewallpaperdata

import android.app.Application
import android.util.Log
import androidx.lifecycle.lifecycleScope
import com.example.livewallpaper05.ProfileActivity
import com.example.livewallpaper05.profiledata.ProfileRepo
import com.example.livewallpaper05.profiledata.ProfileRoomDatabase
import com.example.livewallpaper05.profiledata.ProfileTable
import com.example.livewallpaper05.savedWallpapers.SavedWallpaperRepo
import com.example.livewallpaper05.savedWallpapers.SavedWallpaperRoomDatabase
import com.example.livewallpaper05.savedWallpapers.SavedWallpaperTable
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
    // init repo
    //val repository by lazy { ActiveWallpaperRepo.getInstance(applicationScope)}
    private val savedWallpaperDatabase : SavedWallpaperRoomDatabase by lazy { SavedWallpaperRoomDatabase.getDatabase(this, applicationScope)}
    val wallpaperRepo : ActiveWallpaperRepo by lazy { ActiveWallpaperRepo.getInstance(savedWallpaperDatabase.wallpaperDao(), applicationScope)}

    // profile data
    private val profileDatabase : ProfileRoomDatabase by lazy { ProfileRoomDatabase.getDatabase(this, applicationScope)}
    val profileRepo : ProfileRepo by lazy { ProfileRepo.getInstance(profileDatabase.profileDao(), applicationScope)}

    // saved wallpaper data
    //val savedWallpaperRepo by lazy { SavedWallpaperRepo.getInstance(savedWallpaperDatabase.wallpaperDao(), applicationScope)}

    override fun onCreate() {
        super.onCreate()
        printProfilesAndWallpapersToLogcat("ActiveWallpaperApplication.onCreate()")
    }

    fun printProfilesAndWallpapersToLogcat(message : String){
        CoroutineScope(Dispatchers.Main).launch {
            val profileD : ProfileTable? = profileRepo.data.value
            Log.d("WALLPAPERS", "profile data: $profileD")
            val wallpapers : List<SavedWallpaperTable> = withContext(Dispatchers.IO) {
                savedWallpaperDatabase.wallpaperDao().getAllWallpapers()
            }
            for (element in wallpapers){
                Log.d("WALLPAPERS", "$message $element")
            }
        }
    }
}