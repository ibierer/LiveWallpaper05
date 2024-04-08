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

    private var mProfilePic: ImageView? = null
    private var mUsername: TextView? = null
    private var mBio: TextView? = null
    private var mNewWallpaper: FloatingActionButton? = null
    private var mWallpaperLayout: LinearLayout? = null
    private var mWallpaperGrid: GridLayout? = null

    /* User authentication data */
    private var bio: String? = null
    private lateinit var username: String
    private var loginRegisterButton: Button? = null
    private var logoutButton: Button? = null

    private var mTestButton: Button? = null
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
    fun getWallpapersWithSharedTagsFromUser(username: String): List<String> {
        val res = runQuery(
            "select contents from WallpaperTags natural join UserTags join" +
                    " wallpapers where WallpaperTags.wid = wallpapers.wid && UserTags.username = '$username';"
        )
        var wallpapers = ArrayList<String>()
        while (res.next()) {
            wallpapers += res.getString(1)
        }
        return wallpapers.toList()
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
        mTestButton = findViewById(R.id.b_test_button)
        mWallpaperGrid = findViewById(R.id.sv_ll_gl_wallpapers)


        mTestButton?.setOnClickListener {
            // launch async task to connect to postgre mock server
            GlobalScope.launch(Dispatchers.IO) {
                // write aws test code here -------------
                try {
                    var res = getWallpapersWithSharedTagsFromUser("Jo")
                    for (r in res) {
                        Log.d("LiveWallpaper05", r)
                    }
//                    commentOnWallpaper("Jo", 1, "test comment 1")
                } catch (e: Exception) {
                    Log.d("LiveWallpaper05", e.printStackTrace().toString())
                }
            }
        }
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

