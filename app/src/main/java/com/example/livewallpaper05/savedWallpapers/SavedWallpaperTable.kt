package com.example.livewallpaper05.savedWallpapers

import androidx.room.ColumnInfo
import androidx.room.Entity
import androidx.room.PrimaryKey

@Entity(tableName = "wallpaper_table")
data class SavedWallpaperRow (
    @field:PrimaryKey(autoGenerate = true)
    @field:ColumnInfo(name = "wid")
    var wid: Int = 0,
    @field:ColumnInfo(name = "config")
    var config: String
)