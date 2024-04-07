package com.example.livewallpaper05

import android.content.Context
import android.os.Bundle
import android.util.Log
import android.view.WindowManager
import android.widget.Button
import androidx.activity.viewModels
import androidx.appcompat.app.AppCompatActivity
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperApplication
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModel
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModelFactory
import com.google.firebase.FirebaseApp
import com.google.firebase.auth.FirebaseAuth
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import java.sql.DriverManager
import java.util.Properties

class ExplorerActivity : AppCompatActivity() {

    private var mTestButton: Button? = null
    private lateinit var auth: FirebaseAuth
    private val viewModel: ActiveWallpaperViewModel by viewModels {
        ActiveWallpaperViewModelFactory((application as ActiveWallpaperApplication).wallpaperRepo)
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
        }
        else{
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

        mTestButton?.setOnClickListener {
            // launch async task to connect to postgre mock server
            GlobalScope.launch(Dispatchers.IO) {
                // write aws test code here -------------
                val jdbcConnectionString = DatabaseConfig.getJdbcConnectionString()
                try {
                    Class.forName("com.mysql.jdbc.Driver").newInstance()
                    // connect to mysql server
                    val connectionProperties = Properties()
                    connectionProperties["user"] = DatabaseConfig.getDbUser()
                    connectionProperties["password"] = DatabaseConfig.getDbPassword()
                    connectionProperties["useSSL"] = "false"
                    DriverManager.getConnection(jdbcConnectionString, connectionProperties)
                        .use { conn ->
                            Log.d("LiveWallpaper05", "Connected to database (Explorer Activity)")
                            val query = "SELECT * FROM users;"
                            val statement = conn.createStatement()
                            val result = statement.executeQuery(query)
                            while (result.next()) {
                                val username = result.getString("username")
                                val name = result.getString("name")
                                val bio = result.getString("bio")
                                var resultStr = "User Returned: Username: $username, Name:  $name"
                                if (bio != null) {
                                    resultStr += ", bio: $bio"
                                }
                                Log.d("LiveWallpaper05", resultStr)
                            }
                        }

                } catch (e: Exception) {
                    Log.d("LiveWallpaper05", e.printStackTrace().toString())
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
}

