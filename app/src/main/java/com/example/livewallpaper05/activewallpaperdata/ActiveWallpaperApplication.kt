package com.example.livewallpaper05.activewallpaperdata

import android.app.Application
import com.example.livewallpaper05.profiledata.ProfileRepo
import com.example.livewallpaper05.profiledata.ProfileRoomDatabase
import com.example.livewallpaper05.savedWallpapers.SavedWallpaperRepo
import com.example.livewallpaper05.savedWallpapers.SavedWallpaperRoomDatabase
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.SupervisorJob

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
}