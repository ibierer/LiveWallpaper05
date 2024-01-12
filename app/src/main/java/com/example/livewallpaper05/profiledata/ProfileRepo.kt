package com.example.livewallpaper05.profiledata

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Environment
import androidx.annotation.WorkerThread
import androidx.lifecycle.MutableLiveData
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import java.io.File
import java.io.FileOutputStream
import kotlinx.coroutines.launch

class ProfileRepo private constructor(profileDao: ProfileDao) {

    val data = MutableLiveData<ProfileTable>()
    private var mProfileDao: ProfileDao = profileDao
    // create flow to observe profile pic data


    fun setProfile(profileTable: ProfileTable){
        mScope.launch(Dispatchers.IO){
            mProfileDao.updateProfileData(profileTable)
            val profileInfo = mProfileDao.getProfileData()
            if(profileInfo != null){
                insert()
                data.postValue(profileInfo)
            }
        }
    }

    @WorkerThread
    suspend fun deleteAll(){
        mProfileDao.deleteAll()
    }

    @WorkerThread
    suspend fun deleteProfile(table: ProfileTable){
        mProfileDao.deleteProfileData(table)
    }

    @WorkerThread
    suspend fun insert() {
        if(data.value != null)
            mProfileDao.updateProfileData(data.value!!)
    }

    @Suppress("RedundantSuspendModifier")
    @WorkerThread
    suspend fun updateProfile(profileTable: ProfileTable){
        mProfileDao.updateProfileData(profileTable)
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