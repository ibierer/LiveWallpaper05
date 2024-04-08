package com.example.livewallpaper05

import android.content.Context
import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.text.TextUtils
import android.util.Log
import android.view.View
import android.widget.Button
import android.widget.Toast
import androidx.lifecycle.lifecycleScope
import com.example.livewallpaper05.databinding.ActivityLoginBinding
import com.example.livewallpaper05.databinding.ActivityProfileBinding
import com.google.android.material.textfield.TextInputEditText
import com.google.firebase.auth.FirebaseAuth
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.sql.DriverManager
import java.sql.SQLException
import java.util.Properties

class Login : AppCompatActivity() {
    private lateinit var etEmail: TextInputEditText
    private lateinit var etPassword: TextInputEditText
    private lateinit var loginButton: Button
    private lateinit var regPageButton: Button
    private lateinit var mAuth: FirebaseAuth

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        // Inflate the binding object
        setContentView(R.layout.activity_login)
        mAuth = FirebaseAuth.getInstance()
        etEmail = findViewById(R.id.email)
        etPassword = findViewById(R.id.password)
        loginButton = findViewById(R.id.loginButton)
        regPageButton = findViewById(R.id.registerPageButton)

        // Access views from the layout using the binding object
        regPageButton.setOnClickListener {
            val registerPageIntent = Intent(this, Register::class.java)
            startActivity(registerPageIntent)
        }

        loginButton.setOnClickListener {
            val email = etEmail.text.toString()
            val password = etPassword.text.toString()
            // Check if email or password are empty
            if (TextUtils.isEmpty(email)) {
                Toast.makeText(this, "Enter email", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }
            if (TextUtils.isEmpty(password)) {
                Toast.makeText(this, "Enter password", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }
            mAuth.signInWithEmailAndPassword(email, password)
                .addOnCompleteListener(this) { task ->
                    if (task.isSuccessful) {
                        lifecycleScope.launch {
                            try {
                                val (username, uid) = getUsernameAndUidFromEmail(email)
                                // Switch to the main thread before UI operations
                                withContext(Dispatchers.Main) {
                                    // Sign in success, update UI with the signed-in user's information
                                    Toast.makeText(
                                        applicationContext,
                                        "Login successful!",
                                        Toast.LENGTH_SHORT
                                    ).show()
                                    val sharedPreferences =
                                        getSharedPreferences("MyPreferences", Context.MODE_PRIVATE)
                                    val editor = sharedPreferences.edit()
                                    editor.putString("USERNAME", username)
                                    editor.putInt("UID", uid)
                                    editor.apply()
                                    val profilePageIntent =
                                        Intent(this@Login, ProfileActivity::class.java)
                                    startActivity(profilePageIntent)
                                    finish()
                                }
                            } catch (e: Exception) {
                                Log.e("LoginActivity", "Error getting username: ${e.message}")
                            }
                        }
                    } else {
                        Toast.makeText(this, "Authentication failed.", Toast.LENGTH_SHORT).show()
                    }
                }
        }
    }

    private suspend fun getUsernameAndUidFromEmail(email: String): Pair<String, Int> {
        return withContext(Dispatchers.IO) {
            val jdbcConnectionString = ExplorerActivity.DatabaseConfig.getJdbcConnectionString()
            var username = ""
            var uid = 0

            try {
                Class.forName("com.mysql.jdbc.Driver").newInstance()
                val connectionProperties = Properties()
                connectionProperties["user"] = ExplorerActivity.DatabaseConfig.getDbUser()
                connectionProperties["password"] = ExplorerActivity.DatabaseConfig.getDbPassword()
                connectionProperties["useSSL"] = "false"
                DriverManager.getConnection(jdbcConnectionString, connectionProperties)
                    .use { conn ->
                        val getUsernameQuery = "SELECT uid, username FROM users WHERE email = ?;"
                        val checkUserStatement = conn.prepareStatement(getUsernameQuery)
                        checkUserStatement.setString(1, email)
                        val resultSet = checkUserStatement.executeQuery()
                        if (resultSet.next()) {
                            // Assign the value to the 'username' variable
                            username = resultSet.getString("username")
                            uid = resultSet.getInt("uid")
                            Log.d("LOGIN", "username is $username, uid is $uid")
                        } else {
                            Log.d("SQL_ERROR", "No username exists for the given email: $email")
                        }
                    }
            } catch (e: SQLException) {
                Log.d("SQL_ERROR", e.message.toString())
            }
            Pair(username, uid)
        }
    }
}