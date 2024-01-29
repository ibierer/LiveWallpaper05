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

    @WorkerThread
    suspend fun insert() {
        if(data.value != null)
            mProfileDao.updateProfileData(data.value!!)
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
}