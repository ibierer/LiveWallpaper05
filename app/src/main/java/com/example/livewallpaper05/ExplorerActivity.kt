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

class ExplorerActivity : AppCompatActivity() {

    private var mPreviewButton: Button? = null
    private var mTestButton: Button? = null


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        // Initialize firebase
        FirebaseApp.initializeApp(this)
        getWindow().setFlags(
            WindowManager.LayoutParams.FLAG_FULLSCREEN,
            WindowManager.LayoutParams.FLAG_FULLSCREEN
        );
        setContentView(R.layout.activity_explorer)

        mTestButton = findViewById(R.id.b_test_button)

        mTestButton?.setOnClickListener {
            // launch async task to connect to postgre mock server
            GlobalScope.launch(Dispatchers.IO) {
                // write aws test code here -------------
                val jdbcConnectionString = 
                    "jdbc:mysql://database-1.cxo8mkcogo8p.us-east-1.rds.amazonaws.com:3306/?user=admin"
                //val host = "database-1.cxo8mkcogo8p.us-east-1.rds.amazonaws.com"
                try {
                    Class.forName("com.mysql.jdbc.Driver").newInstance()
                    // connect to mysql server
                    val conn = DriverManager.getConnection(
                        jdbcConnectionString, "admin", "UtahUtesLiveWallz!"
                    )

                    Log.d("LiveWallpaper05", "Connected to database")
                    val query = "USE myDatabase; SELECT * FROM users;"
                    val statement = conn.createStatement()
                    val result = statement.executeQuery(query)
                    while (result.next()) {
                        val username = result.getString("username")
                        val name = result.getString("name")
                        val bio = result.getString("bio")
                        var resultStr = "User Returned: Username: $username, Name:  $name"
                        if (bio != null){
                            resultStr += ", bio: $bio"
                        }
                        Log.d("LiveWallpaper05", resultStr)
                        /*can use similar idea from login dialog to check whether user with username 'value'
                        already exists and if so, pull information from db.*/
                    }

                } catch (e: Exception) {
                    Log.d("LiveWallpaper05", e.printStackTrace().toString())
                }
            }

        }
    }

}