package com.example.livewallpaper05.activewallpaperdata

import android.app.Application
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.SupervisorJob

/**
 * This class manages the active wallpaper data lifecycle and scope for coroutines (not yet fully implemented)
 */
class ActiveWallpaperApplication : Application() {
    // set lifecycle scope for coroutines
    val applicationScope = CoroutineScope(SupervisorJob())
    // init repo
    val repository by lazy { ActiveWallpaperRepo.getInstance(applicationScope)}
}