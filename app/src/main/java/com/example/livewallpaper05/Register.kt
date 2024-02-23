package com.example.livewallpaper05

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.text.TextUtils
import android.util.Log
import android.widget.Button
import android.widget.TextView
import android.widget.Toast
import androidx.room.Database
import com.example.livewallpaper05.databinding.ActivityRegisterBinding
import com.google.android.material.textfield.TextInputEditText
import com.google.firebase.auth.FirebaseAuth
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import java.sql.DriverManager
import java.sql.SQLException
import java.util.Properties

class Register : AppCompatActivity() {
    private lateinit var etEmail: TextInputEditText
    private lateinit var etPassword: TextInputEditText
    private lateinit var etUsername: TextInputEditText
    private lateinit var registerButton: Button
    private lateinit var loginTextView: TextView
    private lateinit var mAuth: FirebaseAuth
    /*public override fun onStart() {
        super.onStart()
        // Check if user is signed in (non-null) and update UI accordingly.
        val currentUser = mAuth.currentUser
        if (currentUser != null) {
            val profilePageIntent = Intent(this, ProfileActivity::class.java)
            startActivity(profilePageIntent)
            finish()
        }
    }*/

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        val binding: ActivityRegisterBinding = ActivityRegisterBinding.inflate(layoutInflater)
        setContentView(binding.root)
        mAuth = FirebaseAuth.getInstance()
        etEmail = binding.email
        etPassword = binding.password
        registerButton = binding.registerButton
        loginTextView = binding.loginNowTV
        etUsername = binding.usernameEditText

        loginTextView.setOnClickListener{
            val loginPageIntent = Intent(this, Login::class.java)
            startActivity(loginPageIntent)
            finish()
        }

        registerButton.setOnClickListener{
            val email : String = etEmail.text.toString()
            val password : String = etPassword.text.toString()
            val username : String = etUsername.text.toString()
            if (TextUtils.isEmpty(email)){
                Toast.makeText(this, "Enter email", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }
            if (TextUtils.isEmpty(password)){
                Toast.makeText(this, "Enter password", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }
            if (TextUtils.isEmpty(username)){
                Toast.makeText(this, "Enter username", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }

            mAuth.createUserWithEmailAndPassword(email, password)
                .addOnCompleteListener(this) { task ->
                    if (task.isSuccessful) {
                        Toast.makeText(this, "Account created!", Toast.LENGTH_SHORT,).show()
                        insertIntoUsers(username, email)
                        // Navigate to the login page
                        val loginPageIntent = Intent(this, Login::class.java)
                        startActivity(loginPageIntent)
                        finish()
                    } else {
                        // If sign in fails, display a message to the user.
                        Toast.makeText(this, "Authentication failed.", Toast.LENGTH_SHORT,).show()
                    }
                }

        }

    }
    fun insertIntoUsers(username: String, name: String){
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
                    .use { conn -> // this syntax ensures that connection will be closed whether normally or from exception
                        Log.d("LiveWallpaper05", "Connected to database")
                        val useDbQuery = "USE myDatabase;"
                        val statement = conn.prepareStatement(useDbQuery)
                        statement.execute()
                        Log.d("LiveWallpaper05", "Pt A REACHED")
                        val insertQuery = "INSERT INTO users (username, name) VALUES (?, ?);"
                        val preparedStatement = conn.prepareStatement(insertQuery)
                        preparedStatement.setString(1, username)
                        preparedStatement.setString(2, name)
                        preparedStatement.executeUpdate()
                        Log.d("LiveWallpaper05", "Pt B REACHED")
                        conn.close()
                    }
            } catch (e: SQLException) {
                Log.e("LiveWallpaper05", e.printStackTrace().toString())
                Log.d("LiveWallpaper05", e.toString())
            }
        }
    }

}