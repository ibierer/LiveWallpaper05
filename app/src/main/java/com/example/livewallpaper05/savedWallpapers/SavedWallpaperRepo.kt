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
    private var lastId: Int = 1

    // create new wallpaper table
    fun createWallpaperTable(id: Int) : SavedWallpaperTable {
        // create unique id
        if (id > 0){
            lastId = id
        } else {
            lastId += 1
        }
        var wid = lastId

        // create new wallpaper table with default config
        val wallpaper = SavedWallpaperTable(
            wid,
            "{\n" +
                    "\"name\": \"New Wallpaper\",\n" +
                    "\"type\": \"0\",\n" +
                    "\"background_color\": {\"r\": 51, \"g\": 51, \"b\": 77, \"a\": 255},\n" +
                    "\"settings\": \"x^2+y^2+z^2=1\"\n" +
                    "}"
        )

        return wallpaper
    }

    // save wallpaper
    fun setWallpaper(wallpaper: SavedWallpaperTable) {
        if (wallpaper.config != "") {
            mScope.launch(Dispatchers.IO) {
                if(wallpapers.value != null) {
                    // if wallpaper not in list, add it
                    val list = wallpapers.value!!.toMutableList()
                    if (!containsWallpaper(wallpaper)) {
                        list.add(wallpaper)
                        wallpapers.postValue(list)
                    } else {
                        // replace wallpaper in list
                        for (i in 0 until list.size) {
                            if (list[i].wid == wallpaper.wid) {
                                list[i] = wallpaper
                                break
                            }
                        }
                        wallpapers.postValue(list)
                    }
                    syncWallpaperDao(wallpaper, list)
                } else {
                    wallpapers.postValue(listOf(wallpaper))
                    syncWallpaperDao(wallpaper, listOf(wallpaper).toMutableList())
                }
            }
        }
    }

    private fun containsWallpaper(wallpaper: SavedWallpaperTable) : Boolean {
        if (wallpapers.value != null) {
            for (w in wallpapers.value!!) {
                if (w.wid == wallpaper.wid) {
                    return true
                }
            }
        }
        return false
    }

    // set active wallpaper live data to wallpaper given by id
    fun setLiveWallpaperData(wid: Int) {
        mScope.launch(Dispatchers.IO) {
            var wallpaper = mWallpaperDao.getWallpaperData(wid)
            if (wallpaper != null) {
                activeWallpaper.postValue(wallpaper)
            } else {
                // try again until wallpaper is found
                var allWallpapers = mWallpaperDao.getAllWallpapers()
                activeWallpaper.postValue(allWallpapers[0])
            }
        }
    }

    private fun syncWallpaperDao(newWallpaper: SavedWallpaperTable, newWallpapers: MutableList<SavedWallpaperTable>){
        // sync wallpapers with database, removing dead values
        var allWallpapers = mWallpaperDao.getAllWallpapers()
        // if list is not the same size as wallpapers clear and add all wallpapers
        if (newWallpapers != null) {
            mWallpaperDao.deleteAll()
            for (wallpaper in newWallpapers) {
                mWallpaperDao.saveWallpaper(wallpaper)
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

    fun saveSwitchWallpaper(activeWid: Int, activeConfig: String) {
        mScope.launch(Dispatchers.IO) {
            val wallpaper = SavedWallpaperTable(
                activeWid,
                activeConfig
            )

            if(wallpapers.value != null) {
                // if wallpaper not in list, add it
                val list = wallpapers.value!!.toMutableList()
                if (!containsWallpaper(wallpaper)) {
                    list.add(wallpaper)
                    wallpapers.postValue(list)
                } else {
                    // replace wallpaper in list
                    for (i in 0 until list.size) {
                        if (list[i].wid == wallpaper.wid) {
                            list[i] = wallpaper
                            break
                        }
                    }
                    wallpapers.postValue(list)
                }
                syncWallpaperDao(wallpaper, list)
            } else {
                wallpapers.postValue(listOf(wallpaper))
                syncWallpaperDao(wallpaper, listOf(wallpaper).toMutableList())
            }

            // update activeWallpaper live data
            activeWallpaper.postValue(wallpaper)
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