package com.example.livewallpaper05.profiledata

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.lifecycle.LiveData
import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.launch
import java.io.ByteArrayOutputStream

class ProfileViewModel(repo: ProfileRepo) : ViewModel() {

    val profileData: LiveData<ProfileTable> = repo.data
    private var mRepo = repo

    val data: LiveData<ProfileTable>
        get() = profileData


    fun insert(profile: ProfileTable) = viewModelScope.launch {
        mRepo.updateProfile(profile)
    }

    fun setProfile(profile: ProfileTable) {
        if(profileData != null)
            mRepo.setProfile(profile)
    }

    fun removeProfile(key: ProfileTable) = viewModelScope.launch {
        mRepo.deleteProfile(key)
    }

    fun deleteAll() = viewModelScope.launch {
        mRepo.deleteAll()
    }

    fun updateProfilePic(pic: Bitmap) {
        // get byte array from bitmap
        val stream = ByteArrayOutputStream()
        pic.compress(Bitmap.CompressFormat.PNG, 100, stream)
        val byteArray = stream.toByteArray()
        // decode byte array to bitmap
        val bitmap = BitmapFactory.decodeByteArray(byteArray, 0, byteArray.size)
        // create new profile table with updated profile pic
        var profile = ProfileTable(
            "username",
            "bio",
            0,
            byteArray
        )
        try {
            profile = ProfileTable(
                profileData.value!!.username,
                profileData.value!!.bio,
                profileData.value!!.uid,
                byteArray
            )
        } catch (e: Exception) {}
        // update profile table
        mRepo.setProfile(profile)

    }

    class ProfileViewModelFactory(private val repo: ProfileRepo) : ViewModelProvider.Factory {
        override fun <T : ViewModel> create(modelClass: Class<T>): T {
            if (modelClass.isAssignableFrom(ProfileViewModel::class.java)) {
                @Suppress("UNCHECKED_CAST")
                return ProfileViewModel(repo) as T
            }
            throw IllegalArgumentException("Unknown ViewModel class")
        }
    }

}