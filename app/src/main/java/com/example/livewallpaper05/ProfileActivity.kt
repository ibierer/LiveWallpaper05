package com.example.livewallpaper05

import android.app.Dialog
import android.content.Intent
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.provider.MediaStore
import android.view.View
import android.widget.ImageView
import android.widget.TextView
import androidx.activity.result.contract.ActivityResultContracts
import androidx.activity.viewModels
import androidx.appcompat.app.AlertDialog
import androidx.core.graphics.drawable.toBitmap
import androidx.lifecycle.Observer
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperApplication
import com.example.livewallpaper05.profiledata.ProfileTable
import com.example.livewallpaper05.profiledata.ProfileViewModel
import java.io.ByteArrayOutputStream

class ProfileActivity : AppCompatActivity() {

    private var mProfilePic: ImageView? = null
    private var mUsername: TextView? = null
    private var mBio: TextView? = null
    //private var mProfileManager: ProfileManager? = null

    // profile table data
    private var mProfileTable: ProfileTable? = null
    private val mViewModel: ProfileViewModel by viewModels {
        ProfileViewModel.ProfileViewModelFactory((application as ActiveWallpaperApplication).profileRepo)
    }

    private val flowObserver: Observer<ProfileTable> =
        Observer { profileTable ->
            if (mProfileTable != null) {
                mProfileTable = profileTable
            }
        }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_profile)

        // connect db to view model
        mViewModel.profileData.observe(this, flowObserver)

        mProfilePic = findViewById(R.id.iv_profile_pic)
        mUsername = findViewById(R.id.tv_username)
        mBio = findViewById(R.id.tv_biography)

        // set profile pic click listener
        mProfilePic!!.setOnClickListener(this::changeProfilePic)

        // link profile view elements to profile live data
        mViewModel.profileData.observe(this, Observer { profileData ->
            if(profileData != null){
                // if profile data username is Dummy_user do nothing
                if(profileData.username == "Dummy_user"){
                    return@Observer
                }
                mUsername!!.text = profileData.username
                mBio!!.text = profileData.bio
                val imageData = profileData.profilepic
                if(imageData != null && imageData.isNotEmpty()){
                    val imageBitmap = BitmapFactory.decodeByteArray(imageData, 0, imageData.size)
                    mProfilePic!!.setImageBitmap(imageBitmap)
                } else {
                    mProfilePic!!.setImageResource(R.drawable.baseline_account_circle_24)
                }
            }
        })
    }

    fun changeProfilePic(view: View) {
        // build dialog to choose between media and camera
        val dialog = AlertDialog.Builder(this)
        dialog.setTitle("Change Profile Picture")
        dialog.setMessage("Would you like to take a picture or choose from your gallery?")
        dialog.setPositiveButton("Camera") { _, _ ->
            // open camera
            val cameraIntent = Intent(MediaStore.ACTION_IMAGE_CAPTURE)
            try {
                cameraActivity.launch(cameraIntent)
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }
        dialog.setNegativeButton("Gallery") { _, _ ->
            // open gallery
            val galleryIntent = Intent(Intent.ACTION_PICK, MediaStore.Images.Media.EXTERNAL_CONTENT_URI)
            try {
                galleryActivity.launch(galleryIntent)
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }
        dialog.show()
    }

    private val cameraActivity = registerForActivityResult(ActivityResultContracts.StartActivityForResult()) { result ->
        if (result.resultCode == RESULT_OK) {
            val data = result.data
            val imageBitmap = data?.extras?.get("data") as Bitmap
            if (imageBitmap == null) {
                return@registerForActivityResult
            }
            updateProfilePicture(imageBitmap)
        }
    }

    private val galleryActivity = registerForActivityResult(ActivityResultContracts.StartActivityForResult()) { result ->
        if (result.resultCode == RESULT_OK) {
            val data = result.data
            // get image from uri returned in data
            val imageBitmap = MediaStore.Images.Media.getBitmap(this.contentResolver, data?.data)
            updateProfilePicture(imageBitmap)
        }
    }

    fun updateProfilePicture(pic: Bitmap){
        // update profile pic in database
        mViewModel.updateProfilePic(pic)
    }

    private fun showLoginDialog(){
        val dialog = Dialog(this)
        dialog.requestWindowFeature(android.view.Window.FEATURE_NO_TITLE)
        dialog.setCancelable(false)
        dialog.setContentView(R.layout.dialog_login)

    }

}