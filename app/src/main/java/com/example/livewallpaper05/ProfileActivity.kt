package com.example.livewallpaper05

import android.app.Dialog
import android.content.Intent
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.provider.MediaStore
import android.util.Log
import android.view.View
import android.widget.Button
import android.widget.GridLayout
import android.widget.ImageView
import android.widget.LinearLayout
import android.widget.TextView
import android.widget.Toast
import androidx.activity.result.contract.ActivityResultContracts
import androidx.activity.viewModels
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.Observer
import androidx.lifecycle.lifecycleScope
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperApplication
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModel
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModelFactory
import com.example.livewallpaper05.helpful_fragments.WallpaperFragment
import com.example.livewallpaper05.profiledata.ProfileViewModel
import com.example.livewallpaper05.savedWallpapers.SavedWallpaperRepo.WallpaperRef
import com.example.livewallpaper05.savedWallpapers.SavedWallpaperViewModel
import com.firebase.ui.auth.AuthUI
import com.google.android.material.floatingactionbutton.FloatingActionButton
import com.google.firebase.auth.FirebaseAuth
import com.google.firebase.auth.FirebaseUser
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import java.io.InputStream
import java.sql.DriverManager
import java.sql.SQLException
import java.util.Properties

class ProfileActivity : AppCompatActivity() {

    private var mProfilePic: ImageView? = null
    private var mUsername: TextView? = null
    private var mBio: TextView? = null
    private var mNewWallpaper: FloatingActionButton? = null
    private var mWallpaperLayout: LinearLayout? = null
    private var mWallpaperGrid: GridLayout? = null

    /* User authentication data */
    private var authUser: FirebaseUser? = null
    private var username: String? = null
    private var loginRegisterButton: Button? = null
    private var logoutButton: Button? = null

    // auth for firebase
    private lateinit var auth: FirebaseAuth

    // profile table data
    private val mProfileViewModel: ProfileViewModel by viewModels {
        ProfileViewModel.ProfileViewModelFactory((application as ActiveWallpaperApplication).profileRepo)
    }

    // saved wallpaper data
    private val mSavedWallpaperViewModel: SavedWallpaperViewModel by viewModels {
        SavedWallpaperViewModel.SavedWallpaperViewModelFactory((application as ActiveWallpaperApplication).savedWallpaperRepo)
    }

    // active wallpaper view model
    private val mActiveWallpaperViewModel: ActiveWallpaperViewModel by viewModels {
        ActiveWallpaperViewModelFactory((application as ActiveWallpaperApplication).repository)
    }

    public override fun onStart() {
        super.onStart()
        // Check if user is signed in (non-null) and update UI accordingly.
        val currentUser = auth.currentUser
        if (currentUser != null) {
            Log.d("AUTH", "Current user: $currentUser")
            // load from AWS
        } else {
            Log.d("AUTH", "No user logged in")
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        // From Jo to Cam: connect to aws MySQL server here and query basic profile info into class instance declared below
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_profile)

        mProfilePic = findViewById(R.id.iv_profile_pic)
        mUsername = findViewById(R.id.tv_username)
        mBio = findViewById(R.id.tv_biography)
        mNewWallpaper = findViewById(R.id.b_new_wallpaper)
        mWallpaperLayout = findViewById(R.id.sv_ll_wallpapers)
        mWallpaperGrid = findViewById(R.id.sv_ll_gl_wallpapers)
        loginRegisterButton = findViewById(R.id.loginRegisterButton)
        logoutButton = findViewById(R.id.logoutButton)


        /* Used to securely access db credentials and keep out of source code */
        val inputStream: InputStream = resources.openRawResource(R.raw.database_config)
        val properties = Properties()
        properties.load(inputStream)

        // Set values in DatabaseConfig object
        DatabaseConfig.jdbcConnectionString = properties.getProperty("jdbcConnectionString", "")
        DatabaseConfig.dbUser = properties.getProperty("dbUser", "")
        DatabaseConfig.dbPassword = properties.getProperty("dbPassword", "")
        // set up loginPageButton
        loginRegisterButton!!.setOnClickListener {
            val loginPageIntent = Intent(this, Login::class.java)
            startActivity(loginPageIntent)
        }

        logoutButton!!.setOnClickListener {
            FirebaseAuth.getInstance().signOut()
            loginRegisterButton!!.visibility = View.VISIBLE
            Toast.makeText(this, "signed out", Toast.LENGTH_SHORT).show()
        }

        // set profile pic click listener
        mProfilePic!!.setOnClickListener(this::changeProfilePic)

        // set new wallpaper button click listener
        mNewWallpaper!!.setOnClickListener(this::newWallpaper)


        // link profile view elements to profile live data via callback function
        mProfileViewModel.profileData.observe(this, Observer { profileData ->
            if (profileData != null) {
                // if profile data username is Dummy_user do nothing
                if (profileData.username == "Dummy_user") {
                    return@Observer
                }
                mUsername!!.text = profileData.username
                mBio!!.text = profileData.bio
                val imageData = profileData.profilepic
                if (imageData.isNotEmpty()) {
                    val imageBitmap = BitmapFactory.decodeByteArray(imageData, 0, imageData.size)
                    mProfilePic!!.setImageBitmap(imageBitmap)
                } else {
                    mProfilePic!!.setImageResource(R.drawable.baseline_account_circle_24)
                }
            }
        })

        // if no wallpapers exist, create default wallpaper
        if (mSavedWallpaperViewModel.savedWallpapers.value == null) {
            mSavedWallpaperViewModel.createDefaultWallpaperTable(
                mActiveWallpaperViewModel.getWid(),
                mActiveWallpaperViewModel.getConfig()
            )
        }

        // link active wallpaper to active wallpaper live data via callback function
        mSavedWallpaperViewModel.activeWallpaper.observe(this, Observer { wallpaper ->
            // set active wallpaper wid to wallpaper wid
            mActiveWallpaperViewModel.setWid(wallpaper.wid)
            // [TODO] use this to update active wallpaper with saved wallpaper data
            // load new active wallpaper config
            mActiveWallpaperViewModel.loadConfig(wallpaper)
        })

        // link saved wallpaper view elements to saved wallpaper live data via callback function
        mSavedWallpaperViewModel.savedWallpapers.observe(this, Observer { wallpapers ->
            if (wallpapers != null) {
                // clear wallpaper layout
                mWallpaperGrid!!.removeAllViews()

                // add each wallpaper to layout
                for (wallpaper in wallpapers) {
                    // find if wallpaper is active
                    val is_active = wallpaper.wid == mActiveWallpaperViewModel.getWid()
                    // create random color bitmap preview based on wid
                    val preview = mActiveWallpaperViewModel.getPreviewImg(wallpaper.wid)
                    // create fragment
                    val fragment = WallpaperFragment.newInstance(is_active, preview, wallpaper.wid)
                    val tag = "wallpaper_" + wallpaper.wid.toString()
                    // add fragment to grid
                    supportFragmentManager.beginTransaction()
                        .add(mWallpaperGrid!!.id, fragment, tag)
                        .commit()

                    // add fragment id to list
                    val ref = WallpaperRef()
                    ref.wallpaperId = wallpaper.wid
                    ref.fragmentId = fragment.id
                    ref.fragmentTag = tag
                    mSavedWallpaperViewModel.updateWallpaperFragIds(ref)
                }
            }
        })

        // observe amount of wallpapers in mWallpaperGrid and update listeners when it changes
        mWallpaperGrid!!.viewTreeObserver.addOnGlobalLayoutListener {
            updateFragListeners()
        }

        /* Added code that connects to database and gathers values for the existing users
        * this will need to be modified to check whether the current user already exists inside the database:
        * if so: pull relevant user data (profile data, userID)
        * otherwise: insert user into database to pull from later in this function. */

        auth = FirebaseAuth.getInstance()
        if (auth.currentUser != null) {
            username = intent.getStringExtra("USERNAME")
            lifecycleScope.launch {
                // User is signed in, get (username, bio, profile_picture, uid, etc) from AWS
                loadUserDataFromAWS(username) //TODO: implement function
            }
        } else {
            // User is not signed in
        }

    }

    object DatabaseConfig {
        lateinit var jdbcConnectionString: String
        lateinit var dbUser: String
        lateinit var dbPassword: String
    }

    private suspend fun loadUserDataFromAWS(username: String?) {

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
            val galleryIntent =
                Intent(Intent.ACTION_PICK, MediaStore.Images.Media.EXTERNAL_CONTENT_URI)
            try {
                galleryActivity.launch(galleryIntent)
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }
        dialog.show()
    }

    // opens camera to take picture for profile picture
    private val cameraActivity =
        registerForActivityResult(ActivityResultContracts.StartActivityForResult()) { result ->
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
    private val galleryActivity =
        registerForActivityResult(ActivityResultContracts.StartActivityForResult()) { result ->
            if (result.resultCode == RESULT_OK) {
                val data = result.data
                // get image from uri returned in data
                val imageBitmap =
                    MediaStore.Images.Media.getBitmap(this.contentResolver, data?.data)
                updateProfilePicture(imageBitmap)
            }
        }

    // [PHASE OUT] calls view model to update local storage of profile picture
    fun updateProfilePicture(pic: Bitmap) {
        // update profile pic in database
        mProfileViewModel.updateProfilePic(pic)
    }

    private fun showLoginDialog() {
        val dialog = Dialog(this)
        dialog.requestWindowFeature(android.view.Window.FEATURE_NO_TITLE)
        dialog.setCancelable(false)
        dialog.setContentView(R.layout.dialog_login)
        // create objects for different textBoxes
        //check username, query database if valid->update the viewModel
    }

    fun newWallpaper(view: View) {
        // save current wallpaper
        val activeConfig = mActiveWallpaperViewModel.getConfig()
        mSavedWallpaperViewModel.saveWallpaper(activeConfig)
        // create new empty wallpaper config
        mSavedWallpaperViewModel.createWallpaperTable(-1)
    }

    fun updateFragListeners() {
        // for each fragment in fragment list, set delete button listener
        var removeList = mutableListOf<WallpaperRef>()
        val wallpaperFragIds = mSavedWallpaperViewModel.getWallpaperFragIds()
        Log.d("LiveWallpaper05", "saved frag count: ${wallpaperFragIds.size}")
        for (ref in wallpaperFragIds) {
            val fragTag = ref.fragmentTag
            val frag = supportFragmentManager.findFragmentByTag(fragTag)
            // if fragment doesn't exist yet, skip
            if (frag == null) {
                continue
            }
            // connect delete button to delete wallpaper function
            frag.requireView().findViewById<FloatingActionButton>(R.id.b_delete_wallpaper)
                .setOnClickListener {
                    // if wallpaper is active, make pop up telling user to switch wallpaper before removing
                    val activeId = mActiveWallpaperViewModel.getWid()
                    if (ref.wallpaperId == activeId) {
                        val dialog = AlertDialog.Builder(this)
                        dialog.setTitle("Active Wallpaper")
                        dialog.setMessage("Please switch wallpapers before removing this wallpaper.")
                        dialog.setPositiveButton("Ok") { _, _ -> }
                        dialog.show()
                        return@setOnClickListener
                    }
                    // delete wallpaper from database
                    mSavedWallpaperViewModel.deleteWallpaper(ref.wallpaperId)
                    // remove fragment from grid
                    supportFragmentManager.beginTransaction().remove(frag).commit()
                    removeList.add(ref)
                    mSavedWallpaperViewModel.removeWallpaperFragId(ref)
                }

            // connect set active button to set active wallpaper function
            frag.requireView().findViewById<Button>(R.id.b_active_wallpaper)
                .setOnClickListener {
                    // enable all active buttons
                    for (r in wallpaperFragIds) {
                        val fragTag = r.fragmentTag
                        val innerFrag = supportFragmentManager.findFragmentByTag(fragTag)
                        if (innerFrag != null) {
                            innerFrag.requireView()
                                .findViewById<Button>(R.id.b_active_wallpaper).isEnabled = true
                            val wallFrag = innerFrag as WallpaperFragment
                            wallFrag.active = false
                        }
                    }
                    // disable active button for this wallpaper
                    frag.requireView().findViewById<Button>(R.id.b_active_wallpaper).isEnabled =
                        false
                    // set frag active variable to true
                    val wallFrag = frag as WallpaperFragment
                    wallFrag.active = true

                    // switch active wallpaper in repo (this data is linked to active wallpaper via live data observer in onCreate)
                    mSavedWallpaperViewModel.switchWallpaper(ref.wallpaperId)
                }
        }
    }
}