package com.example.livewallpaper05

import android.app.Dialog
import android.content.Context
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
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperRepo.WallpaperRef
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModel
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModelFactory
import com.example.livewallpaper05.helpful_fragments.WallpaperFragment
import com.example.livewallpaper05.profiledata.ProfileViewModel
import com.google.android.material.floatingactionbutton.FloatingActionButton
import com.google.firebase.auth.FirebaseAuth
import com.google.firebase.auth.FirebaseUser
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import java.io.ByteArrayOutputStream
import java.io.InputStream
import java.sql.DriverManager
import java.sql.SQLException
import java.util.Properties
import kotlin.properties.Delegates

class ProfileActivity : AppCompatActivity() {

    private var mProfilePic: ImageView? = null
    private var mUsername: TextView? = null
    private var mBio: TextView? = null
    private var mNewWallpaper: FloatingActionButton? = null
    private var mWallpaperLayout: LinearLayout? = null
    private var mWallpaperGrid: GridLayout? = null

    /* User authentication data */
    private var authUser: FirebaseUser? = null
    private var uid by Delegates.notNull<Int>()
    private var bio: String? = null
    private lateinit var username: String
    private var loginRegisterButton: Button? = null
    private var logoutButton: Button? = null

    // auth for firebase
    private lateinit var auth: FirebaseAuth

    // profile table data
    private val mProfileViewModel: ProfileViewModel by viewModels {
        ProfileViewModel.ProfileViewModelFactory((application as ActiveWallpaperApplication).profileRepo)
    }

    // active wallpaper view model
    private val viewModel: ActiveWallpaperViewModel by viewModels {
        ActiveWallpaperViewModelFactory((application as ActiveWallpaperApplication).wallpaperRepo)
    }

    public override fun onStart() {
        super.onStart()
        //(application as ActiveWallpaperApplication).printProfilesAndWallpapersToLogcat("ProfileActivity.onStart()")
        // Check if user is signed in (non-null) and update UI accordingly.
        val currentUser = auth.currentUser
        if (currentUser != null) {
            Log.d("AUTH", "Current user: $currentUser")
            // load from AWS
        } else { // Not registered/logged in
            username = "Default User"
            Log.d("AUTH", "No user logged in")
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        // From Jo to Cam: connect to aws MySQL server here and query basic profile info into class instance declared below
        super.onCreate(savedInstanceState)
        //(application as ActiveWallpaperApplication).printProfilesAndWallpapersToLogcat("ProfileActivity.onCrea8()")
        setContentView(R.layout.activity_profile)

        mProfilePic = findViewById(R.id.iv_profile_pic)
        mUsername = findViewById(R.id.tv_username)
        mBio = findViewById(R.id.tv_biography)
        mNewWallpaper = findViewById(R.id.b_new_wallpaper)
        mWallpaperLayout = findViewById(R.id.sv_ll_wallpapers)
        mWallpaperGrid = findViewById(R.id.sv_ll_gl_wallpapers)
        loginRegisterButton = findViewById(R.id.loginRegisterButton)
        logoutButton = findViewById(R.id.logoutButton)

        viewModel.loadWidsFromMem(this)

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
            val currentUser = FirebaseAuth.getInstance().currentUser
            if (currentUser != null) {
                // User is signed in, perform sign-out
                FirebaseAuth.getInstance().signOut()
                loginRegisterButton!!.visibility = View.VISIBLE
                mUsername!!.text = "Default User"
                Toast.makeText(this, "Signed out", Toast.LENGTH_SHORT).show()
            } else {
                // User is not signed in, display a toast
                logoutButton!!.visibility = View.GONE
                Toast.makeText(this, "You are not signed in", Toast.LENGTH_SHORT).show()
            }
        }
        // set profile pic click listener
        mProfilePic!!.setOnClickListener(this::changeProfilePic)
        // set new wallpaper button click listener
        mNewWallpaper!!.setOnClickListener(this::newWallpaper)
        auth = FirebaseAuth.getInstance()
        if (auth.currentUser != null) {
            val sharedPreferences = getSharedPreferences("MyPreferences", Context.MODE_PRIVATE)
            username = sharedPreferences.getString("USERNAME", "").toString()
            uid = sharedPreferences.getInt("UID", 11)
            // put uid in local profile table
            Log.d("OKAY", "uid is $uid\n username is $username")
            mUsername!!.text = username
            loginRegisterButton!!.visibility = View.GONE
            logoutButton!!.visibility = View.VISIBLE
            lifecycleScope.launch {
                // User is signed in, get (username, bio, profile_picture, uid, etc) from AWS
                Log.d("OKAY", "Calling LoadUserDataFromAWS")
                loadUserDataFromAWS(username)
            }
        } else {
            // User is not signed in
            Log.d("OH_NO", "User not signed in!")
        }
        // link profile view elements to profile live data via callback function
        mProfileViewModel.currentUserProfile.observe(this, Observer { profileData ->
            if (profileData != null) {
                // if profile data username is Dummy_user do nothing
                if (profileData.username == "Dummy_user") {
                    return@Observer
                }
                mUsername!!.text = username
                if (bio != null){
                    mBio!!.text = bio
                }
                else{
                    mBio!!.text = profileData.bio
                }
                if (username != "Default User"){
                    insertBio(profileData.bio, username)
                }
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
        if (viewModel.savedWallpapers.value == null) {
            viewModel.createDefaultWallpaperTable(
                mProfileViewModel.currentUserProfile.value!!.uid,
                viewModel.getWid(),
                viewModel.getConfig()
            )
        } else {
            // updated active wallpaper params with saved wallpaper data
            val activeConfig = viewModel.getConfig()
            val activeWid = viewModel.getWid()
            // if wid is in saved wallpapers, update active wallpaper with saved wallpaper data
            viewModel.saveSwitchWallpaper(activeWid, activeConfig)
        }

        // link active wallpaper to active wallpaper live data via callback function
        viewModel.activeWallpaper.observe(this, Observer { wallpaper ->
            // if wallpaper is not the same as saved wallpaper, return
            // if wallpaper not in wallpapers, return
            var contained = false
            for (w in viewModel.savedWallpapers.value!!) {
                if (w.wid == wallpaper.wid && w.config == wallpaper.config){
                    contained = true
                    break
                }
            }
            if (!contained) {
                return@Observer
            }

            // set active wallpaper wid to wallpaper wid
            viewModel.setWid(wallpaper.wid)
            // [TODO] use this to update active wallpaper with saved wallpaper data
            // load new active wallpaper config
            viewModel.loadConfig(wallpaper)
        })

        // link saved wallpaper view elements to saved wallpaper live data via callback function
        viewModel.mRepo.wallpapers.observe(this, Observer { wallpapers ->
            var existingFragCount = mWallpaperGrid!!.childCount
            if (wallpapers != null && wallpapers.size != existingFragCount) {
                // update saved wallpaper ids
                viewModel.saveWids(this)
                // clear wallpaper layout
                mWallpaperGrid!!.removeAllViews()
                var used = listOf<Int>()

                // add each wallpaper to layout
                for (wallpaper in wallpapers) {
                    // if already made wallpaper skip
                    if (used.contains(wallpaper.wid)) {
                        continue
                    }
                    // find if wallpaper is active
                    val is_active = wallpaper.wid == viewModel.getWid()
                    // create random color bitmap preview based on wid
                    val preview = viewModel.getPreviewImg(wallpaper.wid)
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
                    viewModel.updateWallpaperFragIds(ref)

                    used = used.plus(wallpaper.wid)
                }
            }
        })

        /*
        viewModel.savedWallpapers.observe(this, Observer { wallpapers ->

        })
        */

        // observe amount of wallpapers in mWallpaperGrid and update listeners when it changes
        mWallpaperGrid!!.viewTreeObserver.addOnGlobalLayoutListener {
            updateFragListeners()
        }

        //findViewById<ImageView>(R.id.iv_profile_pic).setOnClickListener {
        //    (application as ActiveWallpaperApplication).printProfilesAndWallpapersToLogcat("iv_profile_pic setOnClickListener")
        //}
        //(application as ActiveWallpaperApplication).printProfilesAndWallpapersToLogcat("ProfileActivity.onCrea8#2()")
    }


    private fun insertBio(bio: String, username: String) {
        GlobalScope.launch(Dispatchers.IO) {
            val jdbcConnectionString = ProfileActivity.DatabaseConfig.jdbcConnectionString
            try {
                Class.forName("com.mysql.jdbc.Driver").newInstance()
                val connectionProperties = Properties()
                connectionProperties["user"] = DatabaseConfig.dbUser
                connectionProperties["password"] = DatabaseConfig.dbPassword
                connectionProperties["useSSL"] = "false"
                DriverManager.getConnection(jdbcConnectionString, connectionProperties)
                    .use { conn ->
                        val updateQuery = "UPDATE users SET bio = ? WHERE username = ?;"
                        val updateStatement = conn.prepareStatement(updateQuery)
                        updateStatement.setString(1, bio)
                        updateStatement.setString(2, username)
                        updateStatement.executeUpdate()
                        conn.close()
                    }
            } catch (e: SQLException) {
                Log.e("LiveWallpaper05", e.printStackTrace().toString())
            }
        }
    }

    object DatabaseConfig {
        lateinit var jdbcConnectionString: String
        lateinit var dbUser: String
        lateinit var dbPassword: String
    }

    private suspend fun loadUserDataFromAWS(username: String) {
        val jdbcConnectionString = ProfileActivity.DatabaseConfig.jdbcConnectionString
        try {
            Class.forName("com.mysql.jdbc.Driver").newInstance()
            val connectionProperties = Properties()
            connectionProperties["user"] = DatabaseConfig.dbUser
            connectionProperties["password"] = DatabaseConfig.dbPassword
            connectionProperties["useSSL"] = "false"
            DriverManager.getConnection(jdbcConnectionString, connectionProperties)
                .use { conn -> // this syntax ensures that connection will be closed whether normally or from exception
                    Log.d("LiveWallpaper05", "Connected to database")
                    val updateQuery = "SELECT * FROM users WHERE username = ?;"
                    val selectStatement = conn.prepareStatement(updateQuery)
                    selectStatement.setString(1, username)
                    val resultSet = selectStatement.executeQuery()
                    while (resultSet.next()) {
                        uid = resultSet.getInt("uid")
                        bio = resultSet.getString("bio")
                        val bioNullable = if (resultSet.wasNull()) null else bio
                        val profilePicture = resultSet.getBlob("profile_picture")
                        val profilePictureNullable =
                            if (resultSet.wasNull()) null else profilePicture
                        //val dateCreated = resultSet.getString("date_created")
                        runOnUiThread {
                            mBio!!.text = bioNullable ?: "No biography"
                            if (profilePictureNullable != null) {
                                val profilePictureBytes = profilePictureNullable.getBytes(
                                    1,
                                    profilePictureNullable.length().toInt()
                                )
                                val bitmap = BitmapFactory.decodeByteArray(
                                    profilePictureBytes,
                                    0,
                                    profilePictureBytes.size
                                )
                                mProfilePic!!.setImageBitmap(bitmap)
                            }
                        }
                    }
                    conn.close()
                }
        } catch (e: SQLException) {
            Log.d("OH_NO", e.message.toString())
        }
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

    // [TODO] [PHASE OUT] calls view model to update local storage of profile picture
    private fun updateProfilePicture(pic: Bitmap) {
        // update profile pic in database
        mProfileViewModel.updateProfilePic(pic)
        insertProfilePicture(username, pic)
    }

    private fun insertProfilePicture(username: String, image: Bitmap) {
        GlobalScope.launch(Dispatchers.IO) {
            // write aws test code here -------------
            val jdbcConnectionString = ProfileActivity.DatabaseConfig.jdbcConnectionString
            try {
                Class.forName("com.mysql.jdbc.Driver").newInstance()
                // connect to mysql server
                val connectionProperties = Properties()
                connectionProperties["user"] = DatabaseConfig.dbUser
                connectionProperties["password"] = DatabaseConfig.dbPassword
                connectionProperties["useSSL"] = "false"
                DriverManager.getConnection(jdbcConnectionString, connectionProperties)
                    .use { conn -> // this syntax ensures that connection will be closed whether normally or from exception
                        Log.d("OH_YES", "Connected to database in insertProfilePicture!")
                        val updateQuery = "UPDATE users SET profile_picture = ? WHERE username = ?;"
                        conn.prepareStatement(updateQuery).use { updateStatement ->
                            val convertedImage = convertBitmapToByteArray(image)
                            updateStatement.setBytes(1, convertedImage)
                            updateStatement.setString(2, username)
                            updateStatement.executeUpdate()
                        }
                    }
            } catch (e: SQLException) {
                Log.d("OH_NO", e.message.toString())
            }
        }
    }

    // Helper function to convert Bitmap to byte array
    private fun convertBitmapToByteArray(bitmap: Bitmap): ByteArray {
        val stream = ByteArrayOutputStream()
        bitmap.compress(Bitmap.CompressFormat.PNG, 100, stream)
        return stream.toByteArray()
    }

    private fun showLoginDialog() {
        val dialog = Dialog(this)
        dialog.requestWindowFeature(android.view.Window.FEATURE_NO_TITLE)
        dialog.setCancelable(false)
        dialog.setContentView(R.layout.dialog_login)
        // create objects for different textBoxes
        //check username, query database if valid->update the viewModel
    }

    private fun newWallpaper(view: View) {
        // save current wallpaper
        val activeConfig = viewModel.getConfig()
        viewModel.saveWallpaper(activeConfig)
        // create new empty wallpaper config
        viewModel.createWallpaperTable(-1)
        viewModel.saveWids(this)

        // force user to preview activity
        //val intent = Intent(this, PreviewActivity::class.java)
        //startActivity(intent)

    }

    private fun updateFragListeners() {
        // for each fragment in fragment list, set delete button listener
        val wallpaperFragIds = viewModel.getWallpaperFragIds()
        Log.d("LiveWallpaper05", "saved frag count: ${wallpaperFragIds.size}")
        for (ref in wallpaperFragIds) {
            val fragTag = ref.fragmentTag
            val frag = supportFragmentManager.findFragmentByTag(fragTag) ?: continue
            // if fragment doesn't exist yet, skip

            // connect delete button to delete wallpaper function
            frag.requireView().findViewById<FloatingActionButton>(R.id.b_delete_wallpaper).setOnClickListener {
                // if wallpaper is active, make pop up telling user to switch wallpaper before removing
                val activeId = viewModel.getWid()
                if (ref.wallpaperId == activeId) {
                    val dialog = AlertDialog.Builder(this)
                    dialog.setTitle("Active Wallpaper")
                    dialog.setMessage("Please switch wallpapers before removing this wallpaper.")
                    dialog.setPositiveButton("Ok") { _, _ -> }
                    dialog.show()
                    return@setOnClickListener
                }
                // delete wallpaper from database
                viewModel.deleteWallpaper(ref.wallpaperId)
                // remove fragment from grid
                supportFragmentManager.beginTransaction().remove(frag).commit()
                viewModel.removeWallpaperFragId(ref)
            }

            // connect set active button to set active wallpaper function
            frag.requireView().findViewById<Button>(R.id.b_active_wallpaper).setOnClickListener {
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
                frag.requireView().findViewById<Button>(R.id.b_active_wallpaper).isEnabled = false
                // set frag active variable to true
                val wallFrag = frag as WallpaperFragment
                wallFrag.active = true
                // switch active wallpaper in repo (this data is linked to active wallpaper via live data observer in onCreate)
                viewModel.switchWallpaper(ref.wallpaperId)
            }
        }
    }
}