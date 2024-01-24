package com.example.livewallpaper05.profiledata

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Environment
import androidx.annotation.WorkerThread
import androidx.lifecycle.MutableLiveData
import androidx.room.TypeConverter
import com.google.gson.Gson
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import java.io.File
import java.io.FileOutputStream
import kotlinx.coroutines.launch

class ProfileRepo private constructor(profileDao: ProfileDao) {

    val data = MutableLiveData<ProfileTable>()
    val wallpapers = MutableLiveData<List<String>>()

    private var mProfileDao: ProfileDao = profileDao

    fun setProfile(profileTable: ProfileTable){
        mScope.launch(Dispatchers.IO){
            //val profileInfo = mProfileDao.getProfileData()
            val profileInfo = profileTable
            if(profileInfo != null){
                data.postValue(profileInfo)
                mProfileDao.updateProfileData(profileTable)
            }
        }
    }

    fun setWallpapers(wallpaper: List<String>) {
        mScope.launch(Dispatchers.IO) {
            if (wallpaper != null) {
                wallpapers.postValue(wallpaper)
                // convert wallpaper list to string
                mProfileDao.addWallpapers(wallpaper)
            }
        }
    }

    @WorkerThread
    suspend fun insert() {
        if(data.value != null)
            mProfileDao.updateProfileData(data.value!!)
        if (wallpapers.value != null)
            mProfileDao.addWallpapers(wallpapers.value!!)
    }

    companion object {
        @Volatile
        private var instance: ProfileRepo? = null
        private lateinit var mScope: CoroutineScope

        @Synchronized
        fun getInstance(
            profileDao: ProfileDao,
            scope: CoroutineScope
        ): ProfileRepo {
            return instance ?: ProfileRepo(profileDao).also {
                instance = it
                mScope = scope
            }
        }
    }

    // code for converting list of wallpaper configs to string
    class ListConverter {
        @TypeConverter
        fun fromList(list: List<String>): String {
            return Gson().toJson(list)
        }

        @TypeConverter
        fun toList(string: String): List<String> {
            return try{
                Gson().fromJson<List<String>>(string, List::class.java)
            } catch (e: Exception) {
                emptyList()
            }
        }
    }
}