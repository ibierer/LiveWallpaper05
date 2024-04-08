package com.example.livewallpaper05.activewallpaperdata

import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider

class ExplorerViewModel(val repo: ActiveWallpaperRepo) : ViewModel() {

}

class ExplorerViewModelFactory(private val repo: ActiveWallpaperRepo) : ViewModelProvider.Factory {
    override fun <T : ViewModel> create(modelClass: Class<T>): T {
        if (modelClass.isAssignableFrom(ExplorerViewModelFactory::class.java)) {
            @Suppress("UNCHECKED_CAST")
            return ExplorerViewModel(repo) as T
        }
        throw IllegalArgumentException("Unknown ViewModel class")
    }
}