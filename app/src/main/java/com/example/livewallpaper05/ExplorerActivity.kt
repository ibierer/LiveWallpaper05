package com.example.livewallpaper05

import android.content.Context
import android.content.Intent
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.provider.MediaStore
import android.util.Log
import android.view.View
import android.view.WindowManager
import android.widget.Button
import android.widget.ImageView
import android.widget.LinearLayout
import android.widget.TextView
import android.widget.Toast
import androidx.activity.result.contract.ActivityResultContracts
import androidx.activity.viewModels
import android.widget.GridLayout
import androidx.activity.viewModels
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.Observer
import androidx.lifecycle.lifecycleScope
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperApplication
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperRepo
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModel
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModelFactory
import com.example.livewallpaper05.activewallpaperdata.ExplorerViewModel
import com.example.livewallpaper05.activewallpaperdata.ExplorerViewModelFactory
import com.example.livewallpaper05.profiledata.ProfileViewModel
import com.example.livewallpaper05.helpful_fragments.WallpaperFragment
import com.google.android.material.floatingactionbutton.FloatingActionButton
import com.google.firebase.FirebaseApp
import com.google.firebase.auth.FirebaseAuth
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import java.io.ByteArrayOutputStream
import java.sql.SQLException
import java.util.Properties
import java.sql.Connection
import java.sql.DriverManager
import java.sql.ResultSet


class ExplorerActivity : AppCompatActivity() {

    private var mNewWallpaper: FloatingActionButton? = null
    private var mWallpaperLayout: LinearLayout? = null
    private var mWallpaperGrid: GridLayout? = null

    /* User authentication data */
    private var bio: String? = null
    private lateinit var username: String

    private val stopWords = listOf<String>(
        "a",
        "about",
        "actually",
        "almost",
        "also",
        "although",
        "always",
        "am",
        "an",
        "and",
        "any",
        "are",
        "as",
        "at",
        "be",
        "became",
        "become",
        "but",
        "by",
        "can",
        "could",
        "did",
        "do",
        "does",
        "each",
        "either",
        "else",
        "for",
        "from",
        "had",
        "has",
        "hence",
        "how",
        "i",
        "if",
        "is",
        "in",
        "it",
        "its",
        "it's",
        "just",
        "may",
        "maybe",
        "me",
        "might",
        "mine",
        "must",
        "my",
        "neither",
        "nor",
        "not",
        "of",
        "oh",
        "ok",
        "when",
        "where",
        "whereas",
        "wherever",
        "whether",
        "which",
        "while",
        "who",
        "whom",
        "whoever",
        "whose",
        "why",
        "will",
        "with",
        "within",
        "without",
        "would",
        "yes",
        "yet",
        "you",
        "your"
    )


    private fun extractTags(comment: String): List<String> {
        val allTokens = comment.lowercase().split("\\s+".toRegex())
        val valTokens = arrayListOf<String>()
        for (token in allTokens) {
            if (stopWords.contains(token)) {
                continue;
            }
            valTokens.add(token)
        }

        // ML to choose keywords
        // For now, all non stop words are keywords

        return valTokens.toList()
    }

    private fun commentOnWallpaper(commenter: String, wid: Int, comment: String) {
        // send the comment to DB
        runUpdate("INSERT INTO Comments VALUES ($wid, '$commenter', '$comment');")

        var tags = extractTags(comment)
        for (tag in tags) {
            runUpdate("INSERT INTO WallpaperTags VALUES ($wid,'$tag');")
        }
    }

    private fun likeWallpaper(wid: Int, likerUsername: String) {
        runUpdate("INSERT INTO Likes VALUES ($wid, '$likerUsername');")
    }

    private fun getComments(wid: Int): List<String> {
        val res = runQuery("select * from Comments where wID = $wid;")
        var comments = ArrayList<String>()
        while (res.next()) {
            comments += res.getString(1) + res.getString(3)
        }
        return comments
    }

    private fun getCommentCount(wid: Int): Int {
        val res = runQuery("select count(*) from Comments where wID = $wid;")
        var count = 0
        if (res.next()) {
            count = res.getInt(1)
        }
        return count
    }

    // DONE
    fun getWallpapersWithSharedTagsFromUser(username: String): ResultSet {
        return runQuery(
            "select * from WallpaperTags natural join UserTags join" +
                    " wallpapers where WallpaperTags.wid = wallpapers.wid && UserTags.username = '$username';"
        )
    }


    private var connectionString =
        "jdbc:mysql://database-1.cxo8mkcogo8p.us-east-1.rds.amazonaws.com:3306/?user=admin";

    private fun establishConnection(): Connection {
        Class.forName("com.mysql.jdbc.Driver").newInstance()
        val conn = DriverManager.getConnection(
            connectionString, "admin", "UtahUtesLiveWallz!"
        )
        conn.createStatement().executeQuery("USE myDatabase;")
        return conn;
    }

    private fun runQuery(query: String): ResultSet {
        val connection = establishConnection();
        val statement = connection.createStatement()
        val retVal = statement.executeQuery(query)
        return retVal
    }

    private fun runUpdate(query: String) {
        val connection = establishConnection();
        val statement = connection.createStatement()
        statement.executeUpdate(query)
    }

    private lateinit var auth: FirebaseAuth

    // profile table data
    private val mProfileViewModel: ProfileViewModel by viewModels {
        ProfileViewModel.ProfileViewModelFactory((application as ActiveWallpaperApplication).wallpaperRepo)
    }

    // active wallpaper view model
    private val viewModel: ActiveWallpaperViewModel by viewModels {
        ActiveWallpaperViewModelFactory((application as ActiveWallpaperApplication).wallpaperRepo)
    }

    private val mExplorerViewModel: ExplorerViewModel by viewModels {
        ExplorerViewModelFactory((application as ActiveWallpaperApplication).wallpaperRepo)
    }

    data class Wallpaper(
        val wid: Int,
        val contents: String,
        val image: ByteArray?,
        val username: String,
        val lastModified: Long
    )

    private val wallpapersList = mutableListOf<Wallpaper>()

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
        super.onCreate(savedInstanceState)
        // Initialize firebase
        FirebaseApp.initializeApp(this)
        auth = FirebaseAuth.getInstance()
        if (auth.currentUser != null) {
            val sharedPreferences = getSharedPreferences("MyPreferences", Context.MODE_PRIVATE)
            viewModel.repo.uid = sharedPreferences.getInt("UID", 11)
            viewModel.repo.username = sharedPreferences.getString("USERNAME", "").toString()
        } else {
            viewModel.repo.uid = 11
            viewModel.repo.username = "Default User"
        }
        window.setFlags(
            WindowManager.LayoutParams.FLAG_FULLSCREEN,
            WindowManager.LayoutParams.FLAG_FULLSCREEN
        );
        setContentView(R.layout.activity_explorer)
        DatabaseConfig.loadProperties(applicationContext)

        mNewWallpaper = findViewById(R.id.b_new_wallpaperae)
        mWallpaperLayout = findViewById(R.id.sv_ll_wallpapersae)
        mWallpaperGrid = findViewById(R.id.sv_ll_gl_wallpapersae)
        viewModel.loadWidsFromMem(this)

        // set new wallpaper button click listener
        mNewWallpaper!!.setOnClickListener(this::newWallpaper)
        auth = FirebaseAuth.getInstance()
        if (auth.currentUser != null) {
            val sharedPreferences = getSharedPreferences("MyPreferences", Context.MODE_PRIVATE)
            username = sharedPreferences.getString("USERNAME", "").toString()
            viewModel.repo.uid = sharedPreferences.getInt("UID", 11)
            // put uid in local profile table
            Log.d("OKAY", "uid is ${viewModel.repo.uid}\n username is $username")

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
        viewModel.repo.currentUserProfile.observe(this, Observer { profileData ->
            if (profileData != null) {
                // if profile data username is Dummy_user do nothing
                if (profileData.username == "Dummy_user") {
                    return@Observer
                }

                if (username != "Default User"){
                    insertBio(profileData.bio, username)
                }

            }
        })

        //if no wallpapers exist, create default wallpaper
        if (viewModel.repo.wallpapers.value == null) {
            viewModel.createDefaultWallpaperTable(
                viewModel.repo.currentUserProfile.value!!.uid,
                viewModel.getWid(),
                viewModel.getConfig()
            )
        } else {
            // updated active wallpaper params with saved wallpaper data
            val activeConfig = viewModel.getConfig()
            val activeWid = viewModel.getWid()
            val activeLastModified = viewModel.getLastModified()
            // if wid is in saved wallpapers, update active wallpaper with saved wallpaper data
            //viewModel.saveSwitchWallpaper(activeWid, activeConfig, activeLastModified)
        }

        // link active wallpaper to active wallpaper live data via callback function
        viewModel.repo.activeWallpaper.observe(this, Observer { wallpaper ->
            var contained = false
            // if wallpapers is null return
            if (viewModel.repo.wallpapers.value == null) {
                return@Observer
            }

            for (w in viewModel.repo.wallpapers.value!!) {
                if (w.wid == wallpaper.wid && w.config == wallpaper.config){
                    contained = true
                    break
                }
            }
            if (!contained) {
                return@Observer
            }
        })

//        viewModel.populateExplore(getWallpapersWithSharedTagsFromUser("Jo"))

        // link saved wallpaper view elements to saved wallpaper live data via callback function
        viewModel.repo.wallpapers.observe(this, Observer { wallpapers ->
            var existingFragCount = mWallpaperGrid!!.childCount
            if (wallpapers != null && wallpapers.size != existingFragCount) {
                // update saved wallpaper ids
                //viewModel.saveWids(this)
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
                    val ref = ActiveWallpaperRepo.WallpaperRef()
                    ref.wallpaperId = wallpaper.wid
                    ref.fragmentId = fragment.id
                    ref.fragmentTag = tag
                    viewModel.updateWallpaperFragIds(ref)

                    used = used.plus(wallpaper.wid)
                }
            }
        })


        // observe amount of wallpapers in mWallpaperGrid and update listeners when it changes
        mWallpaperGrid!!.viewTreeObserver.addOnGlobalLayoutListener {
            updateFragListeners()
        }
    }

    private fun insertBio(bio: String, username: String) {
        GlobalScope.launch(Dispatchers.IO) {
            val jdbcConnectionString = ExplorerActivity.DatabaseConfig.getJdbcConnectionString()
            try {
                Class.forName("com.mysql.jdbc.Driver").newInstance()
                val connectionProperties = Properties()
                connectionProperties["user"] = ExplorerActivity.DatabaseConfig.getDbUser()
                connectionProperties["password"] = ExplorerActivity.DatabaseConfig.getDbPassword()
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

    private fun loadUserDataFromAWS(username: String) {
        val jdbcConnectionString = ExplorerActivity.DatabaseConfig.getJdbcConnectionString()
        try {
            Class.forName("com.mysql.jdbc.Driver").newInstance()
            val connectionProperties = Properties()
            connectionProperties["user"] = ExplorerActivity.DatabaseConfig.getDbUser()
            connectionProperties["password"] = ExplorerActivity.DatabaseConfig.getDbPassword()
            connectionProperties["useSSL"] = "false"
            DriverManager.getConnection(jdbcConnectionString, connectionProperties)
                .use { conn -> // this syntax ensures that connection will be closed whether normally or from exception
                    Log.d("LiveWallpaper05", "Connected to database")
                    val updateQuery = "SELECT * FROM users WHERE username = ?;"
                    val selectStatement = conn.prepareStatement(updateQuery)
                    selectStatement.setString(1, username)
                    val resultSet = selectStatement.executeQuery()
                    while (resultSet.next()) {
                        viewModel.repo.uid = resultSet.getInt("uid")
                        bio = resultSet.getString("bio")
                        val bioNullable = if (resultSet.wasNull()) null else bio
                        val profilePicture = resultSet.getBlob("profile_picture")
                        val profilePictureNullable =
                            if (resultSet.wasNull()) null else profilePicture
                    }
                    conn.close()
                }
        } catch (e: SQLException) {
            Log.d("OH_NO", e.message.toString())
        }
    }

    // creates popup dialog to prompt user to chose how to update profile picture
    private fun changeProfilePic(view: View) {
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
    private val cameraActivity = registerForActivityResult(ActivityResultContracts.StartActivityForResult()) { result ->
        if (result.resultCode == RESULT_OK) {
            val data = result.data
            val imageBitmap = data?.extras?.get("data") as Bitmap
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
            val jdbcConnectionString = ExplorerActivity.DatabaseConfig.getJdbcConnectionString()
            try {
                Class.forName("com.mysql.jdbc.Driver").newInstance()
                // connect to mysql server
                val connectionProperties = Properties()
                connectionProperties["user"] = ExplorerActivity.DatabaseConfig.getDbUser()
                connectionProperties["password"] = ExplorerActivity.DatabaseConfig.getDbPassword()
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

    //private fun showLoginDialog() {
    //    val dialog = Dialog(this)
    //    dialog.requestWindowFeature(android.view.Window.FEATURE_NO_TITLE)
    //    dialog.setCancelable(false)
    //    dialog.setContentView(R.layout.dialog_login)
    //    // create objects for different textBoxes
    //    //check username, query database if valid->update the viewModel
    //}

    private fun newWallpaper(view: View) {
        // save current wallpaper
        val activeConfig = viewModel.getConfig()
        viewModel.saveWallpaper(activeConfig)
        // create new empty wallpaper config
        val newId = viewModel.createWallpaperTable(-1)
        viewModel.saveWids(this)

        // set new wallpaper as active wallpaper
        viewModel.setNewId(newId)
    }

    private fun updateFragListeners() {
        // for each fragment in fragment list, set delete button listener
        val wallpaperFragIds = viewModel.getWallpaperFragIds()
        Log.d("LiveWallpaper05", "saved frag count: ${wallpaperFragIds.size}")
        for (ref in wallpaperFragIds) {
            val fragTag = ref.fragmentTag
            val frag = supportFragmentManager.findFragmentByTag(fragTag) ?: continue

            // connect delete button to delete wallpaper function
            frag.requireView().findViewById<FloatingActionButton>(R.id.b_delete_wallpaper)
                .setOnClickListener {
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
                // viewModel.setActiveWallpaperId(ref.wallpaperId)
                // disable active button for this wallpaper
                frag.requireView().findViewById<Button>(R.id.b_active_wallpaper).isEnabled = false
                // set frag active variable to true
                val wallFrag = frag as WallpaperFragment
                wallFrag.active = true
                // switch active wallpaper in repo (this data is linked to active wallpaper via live data observer in onCreate)
                viewModel.switchWallpaper(ref.wallpaperId)
                //viewModel.setNewId(ref.wallpaperId)
                viewModel.setWid(ref.wallpaperId)

                // open preview activity
                val intent = Intent(this, PreviewActivity::class.java)
                startActivity(intent)
            }

            // if repo.newId > 0 and ref.wallpaperId == repo.newId, click active button
            if (viewModel.getTransitionNewId() > 0 && ref.wallpaperId == viewModel.getTransitionNewId()) {
                viewModel.setNewId(-1)
                frag.requireView().findViewById<Button>(R.id.b_active_wallpaper).performClick()
            }
        }
    }

    object DatabaseConfig {
        private var properties: Properties? = null

        fun loadProperties(context: Context) {
            val rawResourceId = context.resources.getIdentifier("database_config", "raw", context.packageName)
            val inputStream = context.resources.openRawResource(rawResourceId)
            properties = Properties()
            properties?.load(inputStream)
        }

        fun getJdbcConnectionString(): String? {
            return properties?.getProperty("jdbcConnectionString")
        }

        fun getDbUser(): String? {
            return properties?.getProperty("dbUser")
        }

        fun getDbPassword(): String? {
            return properties?.getProperty("dbPassword")
        }

    }
}

