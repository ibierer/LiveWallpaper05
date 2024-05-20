package com.example.livewallpaper05.savedWallpapers

//import androidx.lifecycle.LiveData
import androidx.room.Dao
import androidx.room.Insert
//import androidx.room.Query
import androidx.room.OnConflictStrategy
//import com.example.livewallpaper05.activewallpaperdata.Tuple

@Dao
interface SavedWallpaperDao {

    // get saved wallpaper given id
    //@Query("SELECT * FROM wallpaper_table WHERE wid = :wid")
    //fun getWallpaperData(wid: Int): LiveData<SavedWallpaperTable>
    //
    //// get all saved wallpapers
    //@Query("SELECT * FROM wallpaper_table")
    //fun getAllWallpapers(): List<SavedWallpaperTable>
    //
    //
    //@Query("SELECT * FROM wallpaper_table WHERE uid = :uid and wid = :wid")
    //fun getWallpaperByUidAndWid(uid: Int, wid: Int): SavedWallpaperTable
    //
    //// get wallpapers by uid
    //@Query("SELECT wid, lastModified FROM wallpaper_table WHERE uid = :uid")
    //fun getAllWallpapersByUID(uid: Int): List<Tuple>

    // save wallpaper
    @Insert(onConflict = OnConflictStrategy.REPLACE)
    fun saveWallpaper(wallpaper: SavedWallpaperRow)

    //// delete wallpaper
    //@Query("DELETE FROM wallpaper_table WHERE wid = :wid")
    //fun deleteWallpaper(wid: Int)
    //
    //// delete all wallpapers
    //@Query("DELETE FROM wallpaper_table")
    //fun deleteAll()
}

data class Tuple(val wid: Int, val lastModified: Long)