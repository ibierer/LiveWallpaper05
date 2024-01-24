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
import android.widget.LinearLayout
import android.widget.ScrollView
import android.widget.TextView
import androidx.activity.result.contract.ActivityResultContracts
import androidx.activity.viewModels
import androidx.appcompat.app.AlertDialog
import androidx.constraintlayout.widget.ConstraintLayout
import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.Observer
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperApplication
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModel
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModelFactory
import com.example.livewallpaper05.helpful_fragments.WallpaperFragment
import com.example.livewallpaper05.profiledata.ProfileViewModel
import com.google.android.material.floatingactionbutton.FloatingActionButton

class ProfileActivity : AppCompatActivity() {

    private var mProfilePic: ImageView? = null
    private var mUsername: TextView? = null
    private var mBio: TextView? = null
    private var mNewWallpaper: FloatingActionButton? = null
    private var mWallpaperLayout: LinearLayout? = null

    // profile table data
    private val mProfileViewModel: ProfileViewModel by viewModels {
        ProfileViewModel.ProfileViewModelFactory((application as ActiveWallpaperApplication).profileRepo)
    }

    // active wallpaper view model
    private val mActiveWallpaperViewModel: ActiveWallpaperViewModel by viewModels {
        ActiveWallpaperViewModelFactory((application as ActiveWallpaperApplication).repository)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        // From Jo to Cam: connect to postgre mock server here and query basic profile info into class instance declared below
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_profile)

        mProfilePic = findViewById(R.id.iv_profile_pic)
        mUsername = findViewById(R.id.tv_username)
        mBio = findViewById(R.id.tv_biography)
        mNewWallpaper = findViewById(R.id.b_new_wallpaper)
        mWallpaperLayout = findViewById(R.id.sv_ll_wallpapers)

        // set profile pic click listener
        mProfilePic!!.setOnClickListener(this::changeProfilePic)

        // set new wallpaper button click listener
        mNewWallpaper!!.setOnClickListener(this::newWallpaper)

        // link profile view elements to profile live data via callback function
        mProfileViewModel.profileData.observe(this, Observer { profileData ->
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

        // create active wallpaper preview
        supportFragmentManager.beginTransaction()
            .add(mWallpaperLayout!!.id, WallpaperFragment.newInstance(true, mActiveWallpaperViewModel.getPreviewImg()))
            .commit()

    }

    // creates popup dialog to prompt user to chose how to update profile picture
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

    // opens camera to take picture for profile picture
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

    // opens phone photo gallery to grab picture for profile picture
    private val galleryActivity = registerForActivityResult(ActivityResultContracts.StartActivityForResult()) { result ->
        if (result.resultCode == RESULT_OK) {
            val data = result.data
            // get image from uri returned in data
            val imageBitmap = MediaStore.Images.Media.getBitmap(this.contentResolver, data?.data)
            updateProfilePicture(imageBitmap)
        }
    }

    // [PHASE OUT] calls view model to update local storage of profile picture
    fun updateProfilePicture(pic: Bitmap){
        // update profile pic in database
        mProfileViewModel.updateProfilePic(pic)
    }

    private fun showLoginDialog(){
        val dialog = Dialog(this)
        dialog.requestWindowFeature(android.view.Window.FEATURE_NO_TITLE)
        dialog.setCancelable(false)
        dialog.setContentView(R.layout.dialog_login)

    }

    fun newWallpaper(view: View){
        // save current wallpaper
        val activeConfig = mActiveWallpaperViewModel.getConfig()
        mProfileViewModel.updateSavedWallpapers(listOf(activeConfig.toString()))

        // create new empty wallpaper config

        // update active wallpaper to new wallpaper
    }

}

// rip profile data from postgres server into this
//data class Profile(val username: String, val name: String, val dateCreated: String, val bio: String)