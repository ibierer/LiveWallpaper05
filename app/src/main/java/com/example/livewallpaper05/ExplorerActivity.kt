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
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_explorer)

        mTestButton = findViewById(R.id.b_test_button)

        mTestButton?.setOnClickListener {
            // launch async task to connect to postgre mock server
            GlobalScope.launch(Dispatchers.IO) {
                // connect to mysql server
                val connectionProps = Properties()
                //connectionProps.put("user", "postgres")
                connectionProps.put("user", "admin")
                connectionProps.put("password", "UtahUtesLiveWallz!")
                //connectionProps.put("ssl", "true")
                val jdbcConnectionString = "jdbc:mysql://database-1.cxo8mkcogo8p.us-east-1.rds.amazonaws.com:3306/?user=admin"

                val host = "database-1.cxo8mkcogo8p.us-east-1.rds.amazonaws.com"
                //val host = "database-2.cxo8mkcogo8p.us-east-1.rds.amazonaws.com"
                val port = 3306
                try {
                    Class.forName("com.mysql.jdbc.Driver").newInstance()
                    // connect to mysql server
                    Log.d("LiveWallpaper05", "Point A!")
                    val conn = DriverManager.getConnection(
                        jdbcConnectionString, "admin", "UtahUtesLiveWallz!"
                        /*connectionProps*/
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
                } /*catch (e: SQLException) {
                    Log.d("LiveWallpaper05", e.printStackTrace().toString())
                }*/ catch (e: Exception) {
                    Log.d("LiveWallpaper05", e.printStackTrace().toString())
                }
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