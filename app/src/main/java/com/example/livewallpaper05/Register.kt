package com.example.livewallpaper05

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.text.TextUtils
import android.util.Log
import android.widget.Button
import android.widget.TextView
import android.widget.Toast
import androidx.lifecycle.lifecycleScope
import androidx.room.Database
import com.example.livewallpaper05.databinding.ActivityRegisterBinding
import com.google.android.material.textfield.TextInputEditText
import com.google.firebase.auth.FirebaseAuth
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
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

        loginTextView.setOnClickListener {
            val loginPageIntent = Intent(this, Login::class.java)
            startActivity(loginPageIntent)
            finish()
        }

        registerButton.setOnClickListener {
            val email: String = etEmail.text.toString()
            val password: String = etPassword.text.toString()
            val username: String = etUsername.text.toString()
            if (TextUtils.isEmpty(email)) {
                Toast.makeText(this, "Enter email", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }
            if (TextUtils.isEmpty(password)) {
                Toast.makeText(this, "Enter password", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }
            if (TextUtils.isEmpty(username)) {
                Toast.makeText(this, "Enter username", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }


            lifecycleScope.launch {
                val userExists = isUserInDB(username)

                if (userExists) {
                    // User already exists, display a message or handle accordingly
                    Toast.makeText(applicationContext, "User already exists.", Toast.LENGTH_SHORT)
                        .show()
                } else {
                    // User doesn't exist, proceed with user creation
                    createUserWithEmailAndPassword(email, password)
                    insertIntoUsers(username, email)
                }
            }

        }

    }

    private fun createUserWithEmailAndPassword(email: String, password: String) {
        mAuth.createUserWithEmailAndPassword(email, password)
            .addOnCompleteListener(this) { task ->
                if (task.isSuccessful) {
                    // User creation success, update UI or perform other actions
                    Toast.makeText(
                        applicationContext,
                        "Registration successful!",
                        Toast.LENGTH_SHORT
                    ).show()
                    // Optionally, you can navigate to another activity or perform other actions here
                    val intent = Intent(this, Login::class.java)
                    startActivity(intent)
                    finish()
                } else {
                    // If user creation fails, display a message to the user
                    Toast.makeText(
                        this@Register,
                        "Registration failed.",
                        Toast.LENGTH_SHORT
                    ).show()
                }
            }
    }

    suspend fun isUserInDB(username: String): Boolean {
        return withContext(Dispatchers.IO) {
            val jdbcConnectionString = ProfileActivity.DatabaseConfig.jdbcConnectionString
            var userExists = false

            try {
                Class.forName("com.mysql.jdbc.Driver").newInstance()
                // connect to mysql server
                val connectionProperties = Properties()
                connectionProperties["user"] = ProfileActivity.DatabaseConfig.dbUser
                connectionProperties["password"] = ProfileActivity.DatabaseConfig.dbPassword
                connectionProperties["useSSL"] = "false"

                DriverManager.getConnection(jdbcConnectionString, connectionProperties)
                    .use { conn ->
                        // this syntax ensures that the connection will be closed whether normally or from an exception

                        val useDbQuery = "USE myDatabase;"
                        val useDbStatement = conn.prepareStatement(useDbQuery)
                        useDbStatement.execute()
                        val checkUserQuery = "SELECT COUNT(*) FROM users WHERE username = ?;"
                        val checkUserStatement = conn.prepareStatement(checkUserQuery)
                        checkUserStatement.setString(1, username)
                        val resultSet = checkUserStatement.executeQuery()

                        resultSet.next()
                        val userCount = resultSet.getInt(1)
                        userExists = userCount > 0
                    }
            } catch (e: SQLException) {
                Log.d("CHECKDBUSER", e.message.toString())
            }
            userExists
        }
    }

    fun insertIntoUsers(username: String, name: String) {
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
                        conn.close()
                    }
            } catch (e: SQLException) {
                Log.e("LiveWallpaper05", e.printStackTrace().toString())
                Log.d("LiveWallpaper05", e.toString())
            }
        }
    }

}
