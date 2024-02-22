package com.example.livewallpaper05.savedWallpapers

import androidx.lifecycle.MutableLiveData
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch

class SavedWallpaperRepo private constructor(wallpaperDao: SavedWallpaperDao) {

    val activeWallpaper: MutableLiveData<SavedWallpaperTable> = MutableLiveData()
    val wallpapers = MutableLiveData<List<SavedWallpaperTable>>()
    val wallpaperFragIds: MutableList<WallpaperRef> = mutableListOf()

    private var mWallpaperDao: SavedWallpaperDao = wallpaperDao
    private var lastId: Int = 0

    // create new wallpaper table
    fun createWallpaperTable(id: Int) : SavedWallpaperTable {
        // create unique id
        if (id >= 0){
            lastId = id
        } else {
            lastId += 1
        }
        var wid = lastId

        // create new wallpaper table with default config
        val wallpaper = SavedWallpaperTable(
            wid,
            "{\n" +
                    "    \"type\": \"box\",\n" +
                    "    \"background_color\": {\"r\": 51, \"g\": 51, \"b\": 77, \"a\": 255}\n" +
                    "}"
        )

        return wallpaper
    }

    // save wallpaper
    fun setWallpaper(wallpaper: SavedWallpaperTable) {
        mScope.launch(Dispatchers.IO) {
            if (wallpaper != null) {
                // if wallpapers list is empty, create new list
                if (wallpapers.value == null) {
                    wallpapers.postValue(listOf(wallpaper))
                } else {
                    // add new wallpaper to list
                    val list = wallpapers.value!!.toMutableList()
                    // if wallpaper is already in list, remove it
                    for (i in 0 until list.size) {
                        if (list[i].wid == wallpaper.wid) {
                            list.removeAt(i)
                            break
                        }
                    }
                    // add new wallpaper to list
                    list.add(wallpaper)
                    wallpapers.postValue(list)
                }
            }
        }
    }

    // get saved wallpaper given id
    fun getWallpaper(wid: Int) {
        mScope.launch(Dispatchers.IO) {
            val wallpaper = mWallpaperDao.getWallpaperData(wid)
            if (wallpaper != null) {
                activeWallpaper.postValue(wallpaper)
            }
        }
    }

    // get all saved wallpapers
    fun getAllWallpapers() {
        mScope.launch(Dispatchers.IO) {
            val list = mWallpaperDao.getAllWallpapers()
            if (list != null) {
                wallpapers.postValue(list)
            }
        }
    }

    // delete wallpaper
    fun deleteWallpaper(wid: Int) {
        mScope.launch(Dispatchers.IO) {
            mWallpaperDao.deleteWallpaper(wid)
            // remove wallpaper from list
            val list = wallpapers.value!!.toMutableList()
            for (i in 0 until list.size) {
                if (list[i].wid == wid) {
                    list.removeAt(i)
                    break
                }
            }
            wallpapers.postValue(list)
        }
    }

    // delete all wallpapers
    fun deleteAll() {
        mScope.launch(Dispatchers.IO) {
            mWallpaperDao.deleteAll()
            wallpapers.postValue(listOf())
        }
    }

    companion object {
        @Volatile
        private var instance: SavedWallpaperRepo? = null
        private lateinit var mScope: CoroutineScope

        @Synchronized
        fun getInstance(
            savedWallpaperDao: SavedWallpaperDao,
            scope: CoroutineScope
        ): SavedWallpaperRepo {
            return instance ?: SavedWallpaperRepo(savedWallpaperDao).also {
                instance = it
                mScope = scope
            }
        }
    }

    class WallpaperRef {
        var wallpaperId: Int = 0
        var fragmentId: Int = 0
        var fragmentTag: String = ""
    }

}