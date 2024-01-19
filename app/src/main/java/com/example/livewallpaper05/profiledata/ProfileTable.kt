package com.example.livewallpaper05.profiledata

import android.graphics.Bitmap
import androidx.room.ColumnInfo
import androidx.room.Entity
import androidx.room.PrimaryKey
import androidx.room.TypeConverter
import java.time.LocalDate

@Entity(tableName = "profile_table")
data class ProfileTable (
    @field:ColumnInfo(name = "uid")
    @field:PrimaryKey(autoGenerate = true)
    var uid: Int = 0,
    @field:ColumnInfo(name = "username")
    var username: String,
    @field:ColumnInfo(name = "name")
    var name: String,
    @field:ColumnInfo(name = "bio")
    var bio: String,
    @field:ColumnInfo(name = "DateCreated")
    var DateCreated : LocalDate,
    @field:ColumnInfo(name = "profilepic", typeAffinity = ColumnInfo.BLOB)
    var profilepic: ByteArray
)

