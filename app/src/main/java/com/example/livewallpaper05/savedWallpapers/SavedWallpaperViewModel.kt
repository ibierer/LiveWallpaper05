package com.example.livewallpaper05.savedWallpapers

import androidx.lifecycle.LiveData
import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider

class SavedWallpaperViewModel(repo: SavedWallpaperRepo) : ViewModel() {

    val activeWallpaper: LiveData<SavedWallpaperTable> = repo.activeWallpaper
    val savedWallpapers: LiveData<List<SavedWallpaperTable>> = repo.wallpapers
    private var mRepo = repo

    // switch wallpaper
    fun switchWallpaper(wid: Int) {
        mRepo.getWallpaper(wid)
    }

    // save wallpaper from config string
    fun saveWallpaper(config: String) : Int {
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

        return wallpaper.wid
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
    fun createWallpaperTable(id: Int) {
        var new_wallpaper = mRepo.createWallpaperTable(id)
        mRepo.setWallpaper(new_wallpaper)
    }

    fun getWallpaperFragIds() : MutableList<SavedWallpaperRepo.WallpaperRef> {
        return mRepo.wallpaperFragIds
    }

    fun updateWallpaperFragIds(wallpaperRef: SavedWallpaperRepo.WallpaperRef) {
        // if wallpaperRef is already in list ignore
        var data = mRepo.wallpaperFragIds
        if (data != null) {
            for (r in data){
                if (r.wallpaperId == wallpaperRef.wallpaperId) {
                    return
                }
            }
            // add new wallpaperRef to list
            data.add(wallpaperRef)
        }
    }

    fun removeWallpaperFragId(wallpaperRef: SavedWallpaperRepo.WallpaperRef) {
        // remove wallpaperRef from list
        var data = mRepo.wallpaperFragIds
        var toRemove = listOf<SavedWallpaperRepo.WallpaperRef>()
        if (data != null) {
            for (r in data){
                if (r.wallpaperId == wallpaperRef.wallpaperId) {
                    toRemove = toRemove.plus(r)
                }
            }
            data.removeAll(toRemove)
        }
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