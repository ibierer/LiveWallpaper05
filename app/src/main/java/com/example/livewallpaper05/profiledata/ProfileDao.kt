package com.example.livewallpaper05.profiledata

import androidx.room.Dao
import androidx.room.Delete
import androidx.room.Insert
import androidx.room.OnConflictStrategy
import androidx.room.Query
import kotlinx.coroutines.flow.Flow

@Dao
interface ProfileDao {

    @Query("SELECT * FROM profile_table")
    fun getProfileData(): ProfileTable

    // this function runs the sql command "INSERT INTO profile_table VALUES (profileTable)"
    @Insert(onConflict = OnConflictStrategy.REPLACE)
    fun updateProfileData(profileTable: ProfileTable)

    // add list of strings to database as wallpapers
    @Insert(onConflict = OnConflictStrategy.REPLACE)
    fun addWallpapers(wallpapers: List<String>)

    // this function runs the sql command "DELETE FROM profile_table WHERE profileTable = table"
    @Delete
    fun deleteProfileData(table: ProfileTable)

    // this function runs the sql command "DELETE FROM profile_table"
    @Query("DELETE FROM profile_table")
    fun deleteAll()
}