package com.example.livewallpaper05.profiledata

import android.graphics.Bitmap
import androidx.lifecycle.LiveData
import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperRepo
import java.io.ByteArrayOutputStream

class ProfileViewModel(val repo: ActiveWallpaperRepo) : ViewModel() {

    fun updateProfilePic(pic: Bitmap) {
        // get byte array from bitmap
        val stream = ByteArrayOutputStream()
        pic.compress(Bitmap.CompressFormat.PNG, 100, stream)
        val byteArray = stream.toByteArray()
        // create new profile table with updated profile pic
        var profile = ProfileTable(
            // TODO: it should be possible to avoid the try catch below here
            0,
            0,
            "username",
            "bio",
            byteArray
        )
        try {
            profile = ProfileTable(
                repo.currentUserProfile.value!!.uid,
                0,
                repo.currentUserProfile.value!!.username,
                repo.currentUserProfile.value!!.bio,
                byteArray
                //profileData.value!!.savedWallpapers
            )
        } catch (_: Exception) {

        }
        // update profile table
        repo.setProfile(profile)
    }

    class ProfileViewModelFactory(private val repo: ActiveWallpaperRepo) : ViewModelProvider.Factory {
        override fun <T : ViewModel> create(modelClass: Class<T>): T {
            if (modelClass.isAssignableFrom(ProfileViewModel::class.java)) {
                @Suppress("UNCHECKED_CAST")
                return ProfileViewModel(repo) as T
            }
            throw IllegalArgumentException("Unknown ViewModel class")
        }
    }

}