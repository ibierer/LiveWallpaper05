package com.example.livewallpaper05

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.view.WindowManager
import android.widget.Button
import java.sql.*
import java.time.LocalDateTime
import java.sql.Connection
import java.sql.PreparedStatement
import java.sql.SQLException

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
            Log.d("LiveWallpaper05", "In on Click Listener")
            val url = "jdbc:postgresql://localhost:5432/postgres"
            val connection = DriverManager.getConnection(url, "postgres", "UtahUtesLiveWallz!")
                // check connection is valid here
            Log.d("LiveWallpaper05", connection.isValid(0).toString())

            Log.d("LiveWallpaper05", "Driver manager connected!")
            val query = connection.prepareStatement("SELECT * FROM users WHERE username = test")
            Log.d("LiveWallpaper05", "GOT HERE!")
            val dateCreated = LocalDateTime.now().toString()
            val insertQuery =
            connection.prepareStatement("INSERT INTO users(username, name, bio, date_created) VALUES('Cam', 'Cameron Steensma, 'The Greatest', $dateCreated, null);")
            val insertRes = insertQuery.executeQuery()
            Log.d("LiveWallpaper05", insertRes.toString())
            val result = query.executeQuery()
            Log.d("LiveWallpaper05", result.toString())
//                profileDao.updateProfileData(ProfileTable(result.getInt("uid"), result.getString("username"), result.getString("name"),
//                    result.getString("bio"), result.getString("date_created"), result.getBytes("profile_pic")))
            }
//            catch(exception: Exception){
//                Log.d("LiveWallpaper05", exception.toString())
//            }

        }

}