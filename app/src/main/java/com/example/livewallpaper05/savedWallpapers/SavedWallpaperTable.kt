package com.example.livewallpaper05.savedWallpapers

import androidx.room.ColumnInfo
import androidx.room.Entity

@Entity(tableName = "wallpaper_table", primaryKeys = ["uid", "wid"])
data class SavedWallpaperRow(
    @ColumnInfo(name = "uid")
    var uid: Int,
    @ColumnInfo(name = "wid")
    var wid: Int,
    @ColumnInfo(name = "config")
    var config: String,
    @ColumnInfo(name = "previewImage")
    var previewImage: ByteArray
)