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

    fun updateProfilePic(pic: Bitmap) {
        // get byte array from bitmap
        val stream = ByteArrayOutputStream()
        pic.compress(Bitmap.CompressFormat.PNG, 100, stream)
        val byteArray = stream.toByteArray()
        // create new profile table with updated profile pic
        var profile = ProfileTable(
            0,
            0,
            "username",
            "bio",
            byteArray
        )
        try {
            profile = ProfileTable(
                profileData.value!!.uid,
                0,
                profileData.value!!.username,
                profileData.value!!.bio,
                byteArray
                //profileData.value!!.savedWallpapers
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