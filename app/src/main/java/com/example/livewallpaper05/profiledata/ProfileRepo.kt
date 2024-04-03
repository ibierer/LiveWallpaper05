package com.example.livewallpaper05.profiledata

import android.content.Context
import androidx.annotation.WorkerThread
import androidx.core.content.ContentProviderCompat.requireContext
import androidx.lifecycle.MutableLiveData
import androidx.room.ColumnInfo
import androidx.room.PrimaryKey
import com.example.livewallpaper05.R
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch

class ProfileRepo private constructor(context: Context, profileDao: ProfileDao) {

    val currentUserProfile = MutableLiveData<ProfileTable>(
        ProfileTable(
            context.resources.getInteger(R.integer.default_profile_id),
            0,
            "Default User",
            context.resources.getString(R.string.biography),
            ByteArray(0)
        )
    )

    private var mProfileDao: ProfileDao = profileDao

    fun setProfile(profileTable: ProfileTable){
        mScope.launch(Dispatchers.IO){
            //val profileInfo = mProfileDao.getProfileData()
            val profileInfo = profileTable
            if(profileInfo != null){
                currentUserProfile.postValue(profileInfo)
                mProfileDao.updateProfileData(profileTable)
            }
        }
    }

    @WorkerThread
    suspend fun insert() {
        if(currentUserProfile.value != null)
            mProfileDao.updateProfileData(currentUserProfile.value!!)
    }

    companion object {
        @Volatile
        private var instance: ProfileRepo? = null
        private lateinit var mScope: CoroutineScope

        @Synchronized
        fun getInstance(
            context: Context,
            profileDao: ProfileDao,
            scope: CoroutineScope
        ): ProfileRepo {
            return instance ?: ProfileRepo(context, profileDao).also {
                instance = it
                mScope = scope
            }
        }
    }
}