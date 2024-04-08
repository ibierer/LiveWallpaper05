package com.example.livewallpaper05

import android.content.Context
import android.os.Bundle
import android.util.Log
import android.view.WindowManager
import android.widget.Button
import android.widget.GridLayout
import androidx.activity.viewModels
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperApplication
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperRepo
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModel
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModelFactory
import com.example.livewallpaper05.helpful_fragments.WallpaperFragment
import com.google.android.material.floatingactionbutton.FloatingActionButton
import com.google.firebase.FirebaseApp
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import java.util.Properties
import java.sql.Connection
import java.sql.DriverManager
import java.sql.ResultSet


class ExplorerActivity : AppCompatActivity() {

    private var mTestButton: Button? = null
    private var mWallpaperGrid: GridLayout? = null
    private val stopWords = listOf<String>("a", "about", "actually", "almost", "also", "although",
        "always", "am", "an", "and", "any", "are", "as", "at", "be", "became", "become", "but", "by",
        "can", "could", "did", "do", "does", "each", "either", "else", "for", "from", "had", "has",
        "hence", "how", "i", "if", "is", "in", "it", "its", "it's", "just", "may", "maybe", "me",
        "might", "mine", "must", "my", "neither", "nor", "not", "of", "oh", "ok", "when", "where",
        "whereas", "wherever", "whether", "which", "while", "who", "whom", "whoever", "whose", "why",
        "will", "with", "within", "without", "would", "yes", "yet", "you", "your")


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
        val res = runQuery("select contents from WallpaperTags natural join UserTags join" +
                " wallpapers where WallpaperTags.wid = wallpapers.wid && UserTags.username = '$username';")
        var wallpapers = ArrayList<String>()
        while (res.next()) {
            wallpapers += res.getString(1)
        }
        return wallpapers.toList()
    }


    private var connectionString = "jdbc:mysql://database-1.cxo8mkcogo8p.us-east-1.rds.amazonaws.com:3306/?user=admin";

    private fun establishConnection() : Connection {
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

    private fun runUpdate(query: String){
        val connection = establishConnection();
        val statement = connection.createStatement()
        statement.executeUpdate(query)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        // Initialize firebase
        FirebaseApp.initializeApp(this)
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