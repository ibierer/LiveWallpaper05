package com.example.livewallpaper05.profiledata

import android.graphics.Bitmap
import androidx.room.ColumnInfo
import androidx.room.Entity
import androidx.room.PrimaryKey

@Entity(tableName = "profile_table")
data class ProfileTable (
    @field:ColumnInfo(name = "username")
    @field:PrimaryKey
    var username: String,
    @field:ColumnInfo(name = "bio")
    var bio: String,
    @field:ColumnInfo(name = "uid")
    var uid: Int,
    @field:ColumnInfo(name = "profilepic", typeAffinity = ColumnInfo.BLOB)
    var profilepic: ByteArray
)