package com.example.livewallpaper05

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.view.WindowManager
import android.widget.Button
import java.sql.DriverManager
import java.time.LocalDateTime

class ExplorerActivity : AppCompatActivity() {

    private var mPreviewButton: Button? = null
    private var mTestButton: Button? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_explorer)

        mTestButton = findViewById(R.id.b_test_button)

        mTestButton?.setOnClickListener {
            // write aws test code here -------------
            val tmpImg = ByteArray(0)
            val url = "jdbc:postgresql://localhost:5432/LiveWallzDB"
            try {
                val connection = DriverManager
                    .getConnection(url, "postgres", "password")
                // check connection is valid here
                Log.d("LiveWallpaper05", connection.isValid(0).toString())
                val query = connection.prepareStatement("SELECT * FROM users WHERE username = test")
                val dateCreated = LocalDateTime.now().toString()
                val insertQuery =
                    connection.prepareStatement("INSERT INTO users(username, name, bio, date_created, profile_pic) VALUES('Cam', 'Cameron Steensma, 'The Greatest', $dateCreated, null);")
                val insertRes = insertQuery.executeQuery()
                Log.d("LiveWallpaper05", insertRes.toString())
                val result = query.executeQuery()
                Log.d("LiveWallpaper05", result.toString())
//                profileDao.updateProfileData(ProfileTable(result.getInt("uid"), result.getString("username"), result.getString("name"),
//                    result.getString("bio"), result.getString("date_created"), result.getBytes("profile_pic")))
            }
            catch(exception: Exception){
                Log.d("LiveWallpaper05", exception.toString())
            }

        }
    }
}