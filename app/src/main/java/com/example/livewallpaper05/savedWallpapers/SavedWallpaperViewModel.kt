package com.example.livewallpaper05.savedWallpapers

import androidx.lifecycle.LiveData
import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider

class SavedWallpaperViewModel(repo: SavedWallpaperRepo) : ViewModel() {

    val activeWallpaper: LiveData<SavedWallpaperRow> = repo.activeWallpaper
    val savedWallpapers: LiveData<List<SavedWallpaperRow>> = repo.wallpapers
    private var mRepo = repo





    class SavedWallpaperViewModelFactory(private val repo: SavedWallpaperRepo) : ViewModelProvider.Factory {
        override fun <T : ViewModel> create(modelClass: Class<T>): T {
            if (modelClass.isAssignableFrom(SavedWallpaperViewModel::class.java)) {
                @Suppress("UNCHECKED_CAST")
                return SavedWallpaperViewModel(repo) as T
            }
            throw IllegalArgumentException("Unknown ViewModel class")
        }
    }

}
