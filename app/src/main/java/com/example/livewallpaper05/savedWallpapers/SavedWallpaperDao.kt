package com.example.livewallpaper05.savedWallpapers

import androidx.room.Dao
import androidx.room.Insert
import androidx.room.Query
import androidx.room.OnConflictStrategy

@Dao
interface SavedWallpaperDao {

    // get saved wallpaper given id
    @Query("SELECT * FROM wallpaper_table WHERE wid = :wid")
    fun getWallpaperData(wid: Int): SavedWallpaperTable

    // get all saved wallpapers
    @Query("SELECT * FROM wallpaper_table")
    fun getAllWallpapers(): List<SavedWallpaperTable>

    // save wallpaper
    @Insert(onConflict = OnConflictStrategy.REPLACE)
    fun saveWallpaper(wallpaper: SavedWallpaperTable)

    // delete wallpaper
    @Query("DELETE FROM wallpaper_table WHERE wid = :wid")
    fun deleteWallpaper(wid: Int)

    // delete all wallpapers
    @Query("DELETE FROM wallpaper_table")
    fun deleteAll()
}