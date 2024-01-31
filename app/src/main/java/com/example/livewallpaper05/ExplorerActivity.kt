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
import java.util.Properties

class ExplorerActivity : AppCompatActivity() {

    private var mPreviewButton: Button? = null
    private var mTestButton: Button? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_explorer)

        mTestButton = findViewById(R.id.b_test_button)

        mTestButton?.setOnClickListener {
            // connect to mysql server
            val connectionProps = Properties()
            connectionProps["user"] = "admin"
            //connectionProps["user"] = "postgres"
            connectionProps["password"] = "UtahUtesLiveWallz!"
            //connectionProps["password"] = "password"
            try {
                //Class.forName("org.postgresql.Driver").newInstance()
                Class.forName("com.mysql.jdbc.Driver").newInstance()
                var conn = DriverManager.getConnection(
                    "jdbc:" + "mysql" + "://" +
                            "database-1.cxo8mkcogo8p.us-east-1.rds.amazonaws.com" +
                            ":" + "3306" + "/" +
                            "myDatabase",
                    connectionProps
                )
                /*var conn = DriverManager.getConnection(
                 "jdbc:" + "postgres" + "://" +
                            "database-2.cxo8mkcogo8p.us-east-1.rds.amazonaws.com" +
                            ":" + "5432" + "/" +
                            "myDatabase2",
                    connectionProps
                )*/
                Log.d("LiveWallpaper05", "made MySQL connection to AWS!")
            } catch (e: SQLException) {
                Log.d("LiveWallpaper05", "SQL Exception: ${e.message}")
                e.printStackTrace()
            } catch (e: Exception){
                Log.d("LiveWallpaper05", "SQL Exception: ${e.message}")
                e.printStackTrace()
            }

            // write aws test code here -------------
            /*
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

             */
//                profileDao.updateProfileData(ProfileTable(result.getInt("uid"), result.getString("username"), result.getString("name"),
//                    result.getString("bio"), result.getString("date_created"), result.getBytes("profile_pic")))
            }
//            catch(exception: Exception){
//                Log.d("LiveWallpaper05", exception.toString())
//            }

        }

}