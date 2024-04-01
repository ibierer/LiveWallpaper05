package com.example.livewallpaper05

import android.os.Bundle
import android.util.Log
import android.view.WindowManager
import android.widget.Button
import androidx.appcompat.app.AppCompatActivity
import com.google.firebase.FirebaseApp
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import java.sql.DriverManager
import java.util.Properties

class ExplorerActivity : AppCompatActivity() {

    private var mPreviewButton: Button? = null
    private var mTestButton: Button? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        // Initialize firebase
        FirebaseApp.initializeApp(this)
        window.setFlags(
            WindowManager.LayoutParams.FLAG_FULLSCREEN,
            WindowManager.LayoutParams.FLAG_FULLSCREEN
        );
        setContentView(R.layout.activity_explorer)

        mTestButton = findViewById(R.id.b_test_button)

        mTestButton?.setOnClickListener {
            // launch async task to connect to postgre mock server
            GlobalScope.launch(Dispatchers.IO) {
                // write aws test code here -------------
                val jdbcConnectionString = ProfileActivity.DatabaseConfig.jdbcConnectionString
                try {
                    Class.forName("com.mysql.jdbc.Driver").newInstance()
                    // connect to mysql server
                    val connectionProperties = Properties()
                    connectionProperties["user"] = ProfileActivity.DatabaseConfig.dbUser
                    connectionProperties["password"] = ProfileActivity.DatabaseConfig.dbPassword
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
}