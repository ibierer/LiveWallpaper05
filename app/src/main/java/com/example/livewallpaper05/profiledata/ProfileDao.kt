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

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    fun updateProfileData(profileTable: ProfileTable)

    @Delete
    fun deleteProfileData(table: ProfileTable)

    @Query("DELETE FROM profile_table")
    fun deleteAll()
}