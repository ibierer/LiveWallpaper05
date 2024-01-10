package com.example.livewallpaper05.profiledata

import androidx.lifecycle.LiveData
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.launch

class ProfileViewModel(private val repo: ProfileRepo) : ViewModel() {

    val profileData: LiveData<ProfileTable> = repo.data

    fun insert(profile: ProfileTable) = viewModelScope.launch {
        repo.updateProfile(profile)
    }

    fun setProfile(profile: ProfileTable) {
        repo.setProfile(profile)
    }

    fun removeProfile(key: ProfileTable) = viewModelScope.launch {
        repo.deleteProfile(key)
    }

    fun deleteAll() = viewModelScope.launch {
        repo.deleteAll()
    }

}

class ProfileViewModelFactory(private val repo: ProfileRepo) : androidx.lifecycle.ViewModelProvider.Factory {
    override fun <T : androidx.lifecycle.ViewModel> create(modelClass: Class<T>): T {
        if (modelClass.isAssignableFrom(ProfileViewModel::class.java)) {
            @Suppress("UNCHECKED_CAST")
            return ProfileViewModel(repo) as T
        }
        throw IllegalArgumentException("Unknown ViewModel class")
    }
}