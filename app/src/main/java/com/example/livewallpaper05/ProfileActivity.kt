package com.example.livewallpaper05

import android.app.Dialog
import android.content.Intent
import android.graphics.Bitmap
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.provider.MediaStore
import android.view.View
import android.widget.ImageView
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AlertDialog
import com.example.livewallpaper05.profiledata.ProfileRepo

class ProfileActivity : AppCompatActivity() {

    private var mProfilePic: ImageView? = null
    private var mRepo: ProfileRepo? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_profile)

        mProfilePic = findViewById(R.id.profile_pic)
        mRepo = ProfileRepo()

        // set profile pic if it exists
        if (mRepo!!.getPic() != null) {
            mProfilePic!!.setImageBitmap(mRepo!!.getPic())
        }
        mProfilePic!!.setOnClickListener(this::changeProfilePic)

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
            mProfilePic!!.setImageBitmap(imageBitmap)
            mRepo!!.updateProfilePic(imageBitmap)
        }
    }

    private val galleryActivity = registerForActivityResult(ActivityResultContracts.StartActivityForResult()) { result ->
        if (result.resultCode == RESULT_OK) {
            val data = result.data
            // get image from uri returned in data
            val imageBitmap = MediaStore.Images.Media.getBitmap(this.contentResolver, data?.data)
            mProfilePic!!.setImageBitmap(imageBitmap)
            mRepo!!.updateProfilePic(imageBitmap)
        }
    }

    private fun showLoginDialog(){
        val dialog = Dialog(this)
        dialog.requestWindowFeature(android.view.Window.FEATURE_NO_TITLE)
        dialog.setCancelable(false)
        dialog.setContentView(R.layout.dialog_login)

    }

}