package com.example.livewallpaper05

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Environment
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperApplication
import com.example.livewallpaper05.profiledata.ProfileTable
import com.example.livewallpaper05.profiledata.ProfileViewModel
import com.example.livewallpaper05.profiledata.ProfileViewModelFactory
import java.io.File
import java.io.FileOutputStream
import androidx.activity.viewModels
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.Observer


class ProfileManager : AppCompatActivity() {

    private var username: String = "Username"
    private var bio: String = "Bio"
    private var uid: Int = 0
    private var pic: Bitmap? = null
    private var picPath: String? = null

    // profile table data
    private var mProfileTable: ProfileTable? = null
    private val mViewModel: ProfileViewModel by viewModels {
        ProfileViewModelFactory((application as ActiveWallpaperApplication).profileRepo)
    }
    private val flowObserver: Observer<ProfileTable> =
        Observer { profileTable ->
            if (mProfileTable != null) {
                mProfileTable = profileTable
            }
        }

    init {
        // load profile picture from memory if it's path exists
        if (picPath != null) {
            // load picture from local storage
            pic = BitmapFactory.decodeFile(picPath)
        }
        // connect db to view model
        mViewModel.profileData.observe(this, flowObserver)

        // update values if profile database has values
        if (mProfileTable != null){
            username = mProfileTable!!.username
            bio = mProfileTable!!.bio
            uid = mProfileTable!!.uid
            pic = mProfileTable!!.profilepic
            updateProfilePic(pic!!)
        }
    }

    fun saveProfile(){
        // run after profile is created or edited
        val profile = ProfileTable(
            username = this.username,
            bio = this.bio,
            uid = this.uid,
            profilepic = this.pic!!
        )

        mProfileTable = profile
        mViewModel.setProfile(profile)
    }

    fun getUsername(): String? {
        return username
    }

    fun setUsername(username: String) {
        this.username = username
    }

    fun getBio(): String? {
        return bio
    }

    fun setBio(bio: String) {
        this.bio = bio
    }

    fun getUid(): Int? {
        return uid
    }

    fun setUid(uid: Int) {
        this.uid = uid
    }

    fun getPic(): Bitmap? {
        return pic
    }

    fun updateProfilePic(pic: Bitmap) {
        this.pic = pic

        // save picture locally
        if (Environment.getExternalStorageState() == Environment.MEDIA_MOUNTED) {
            val file = File(
                Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES).absolutePath,
                "profile_pic.jpg"
            )
            if (file.exists()) file.delete()
            try {
                val out = FileOutputStream(file)
                pic.compress(Bitmap.CompressFormat.JPEG, 100, out)
                out.flush()
                out.close()
            } catch (e: Exception) {
                e.printStackTrace()
            }

            picPath = file.absolutePath
        }
    }

}