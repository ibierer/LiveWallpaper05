package com.example.livewallpaper05

import android.os.Bundle
import android.util.Log
import android.view.WindowManager
import android.widget.Button
import androidx.appcompat.app.AppCompatActivity
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import java.sql.DriverManager
import java.sql.SQLException
import java.util.Properties

class ExplorerActivity : AppCompatActivity() {

    private var mPreviewButton: Button? = null
    private var mTestButton: Button? = null


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
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
                    Log.d("LiveWallpaper05", "Point A!")
                    val conn = DriverManager.getConnection(
                        jdbcConnectionString, "admin", "UtahUtesLiveWallz!"
                    )

                    Log.d("LiveWallpaper05", "Connected to database")

                    //query "SELECT * FROM users;"
                    val query = "SELECT * FROM users;"
                    val query2 = "SHOW DATABASES;"
                    val statement = conn.createStatement()
                    val result = statement.executeQuery(query2)
                    while (result.next()) {
                        Log.d("LiveWallpaper05", "DB returned: " + result.getString("Database"))
                    }
                } catch (e: Exception) {
                    Log.d("LiveWallpaper05", e.printStackTrace().toString())
                }
            }

        }
    }

}