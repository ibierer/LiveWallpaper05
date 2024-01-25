package com.example.livewallpaper05.savedWallpapers

import androidx.lifecycle.LiveData
import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import java.io.ByteArrayOutputStream

class SavedWallpaperViewModel(repo: SavedWallpaperRepo) : ViewModel() {

    val activeWallpaper: LiveData<SavedWallpaperTable> = repo.activeWallpaper
    val savedWallpapers: LiveData<List<SavedWallpaperTable>> = repo.wallpapers
    private var mRepo = repo

    // switch wallpaper
    fun switchWallpaper(wid: Int) {
        mRepo.getWallpaper(wid)
    }

    // save wallpaper from config string
    fun saveWallpaper(config: String) {
        // create new wallpaper table with given data
        var wallpaper = SavedWallpaperTable(
            0,
            ""
        )
        try {
            wallpaper = SavedWallpaperTable(
                activeWallpaper.value!!.wid,
                activeWallpaper.value!!.config
            )
        } catch (e: Exception) {}
        // update profile table
        mRepo.setWallpaper(wallpaper)
    }

    // delete wallpaper
    fun deleteWallpaper(wid: Int) {
        mRepo.deleteWallpaper(wid)
    }

    // delete all wallpapers
    fun deleteAll() {
        mRepo.deleteAll()
    }

    // get saved wallpaper given id
    fun getWallpaper(wid: Int) {
        mRepo.getWallpaper(wid)
    }

    // create new wallpaper table
    fun createWallpaperTable() {
        var new_wallpaper = mRepo.createWallpaperTable()
        mRepo.setWallpaper(new_wallpaper)
    }

    // update active wallpaper config
    fun updateActiveWallpaperConfig(config: String) {
        var wallpaper = SavedWallpaperTable(
            0,
            ""
        )
        try {
            wallpaper = SavedWallpaperTable(
                activeWallpaper.value!!.wid,
                config
            )
        } catch (e: Exception) {}
        // update profile table
        mRepo.setWallpaper(wallpaper)
    }

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