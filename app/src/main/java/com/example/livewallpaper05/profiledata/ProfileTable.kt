package com.example.livewallpaper05.profiledata

import androidx.room.ColumnInfo
import androidx.room.Embedded
import androidx.room.Entity
import androidx.room.Ignore
import androidx.room.PrimaryKey
import androidx.room.Relation
import androidx.room.TypeConverter
import java.time.LocalDate

@Entity(tableName = "profile_table")
data class ProfileTable (
    @field:PrimaryKey(autoGenerate = true)
    @field:ColumnInfo(name = "uid")
    var uid: Int = 0,
    @field:ColumnInfo(name = "username")
    var username: String,
    @field:ColumnInfo(name = "bio")
    var bio: String,
//    @field:ColumnInfo(name = "DateCreated")
//    var DateCreated : String,
    @field:ColumnInfo(name = "profilepic", typeAffinity = ColumnInfo.BLOB)
    var profilepic: ByteArray
) {
    // force override funcs for profile because of byte array
    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false

        other as ProfileTable

        if (uid != other.uid) return false
        if (username != other.username) return false
        if (bio != other.bio) return false

        return true
    }

    override fun hashCode(): Int {
        var result = uid
        result = 31 * result + username.hashCode()
        result = 31 * result + bio.hashCode()
        return result
    }
}