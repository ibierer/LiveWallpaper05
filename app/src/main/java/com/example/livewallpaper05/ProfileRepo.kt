package com.example.livewallpaper05

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Environment
import java.io.File
import java.io.FileOutputStream

class ProfileRepo() {
    private var username: String? = null
    private var bio: String? = null
    private var uid: Int? = null
    private var pic: Bitmap? = null
    private var picPath: String? = null

    init {
        username = "Username"
        bio = "Bio"
        uid = 0
        pic = null

        if (picPath != null) {
            // load picture from local storage
            pic = BitmapFactory.decodeFile(picPath)
        }
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
        if (Environment.getExternalStorageState() == Environment.MEDIA_MOUNTED){
            val file = File(
                Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES).absolutePath,
                "profile_pic.jpg")
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