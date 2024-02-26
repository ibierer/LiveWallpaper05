package com.example.livewallpaper05

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
    private lateinit var binding: ActivityLoginBinding
    private lateinit var etEmail: TextInputEditText
    private lateinit var etPassword: TextInputEditText
    private lateinit var loginButton: Button
    private lateinit var regPageButton: Button
    private lateinit var mAuth: FirebaseAuth
    private lateinit var username : String

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        // Inflate the binding object
        setContentView(R.layout.activity_login)
        mAuth = FirebaseAuth.getInstance()
        etEmail = findViewById(R.id.email)
        etPassword = findViewById(R.id.password)
        loginButton = findViewById(R.id.loginButton)
        regPageButton = findViewById(R.id.registerPageButton)

        val profileBinding = ActivityProfileBinding.inflate(layoutInflater)
        // Access views from the layout using the binding object
        regPageButton.setOnClickListener {
            val registerPageIntent = Intent(this, Register::class.java)
            startActivity(registerPageIntent)
        }

        loginButton.setOnClickListener{
            val email = etEmail.text.toString()
            val password = etPassword.text.toString()
            if (TextUtils.isEmpty(email)){
                Toast.makeText(this, "Enter email", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }
            if (TextUtils.isEmpty(password)){
                Toast.makeText(this, "Enter password", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }
            mAuth.signInWithEmailAndPassword(email, password)
                .addOnCompleteListener(this) { task ->
                    if (task.isSuccessful) {
                        lifecycleScope.launch {//used to get username from AWS db
                            getUsernameFromEmail(email)
                        }
                        // Sign in success, update UI with the signed-in user's information
                        Toast.makeText(applicationContext, "Login successful!", Toast.LENGTH_SHORT).show()
                        profileBinding.loginRegisterButton.visibility = View.GONE
                        val profilePageIntent = Intent(this, ProfileActivity::class.java).apply {
                            putExtra("USERNAME", username)
                        }
                        startActivity(profilePageIntent)
                        finish()
                    } else {
                        // If sign in fails, display a message to the user.
                        Toast.makeText(this, "Authentication failed.", Toast.LENGTH_SHORT).show()
                    }
                }
        }
    }

    suspend fun getUsernameFromEmail(email : String) {
        return withContext(Dispatchers.IO) {
            val jdbcConnectionString = ProfileActivity.DatabaseConfig.jdbcConnectionString
            try {
                Class.forName("com.mysql.jdbc.Driver").newInstance()
                val connectionProperties = Properties()
                connectionProperties["user"] = ProfileActivity.DatabaseConfig.dbUser
                connectionProperties["password"] = ProfileActivity.DatabaseConfig.dbPassword
                connectionProperties["useSSL"] = "false"
                DriverManager.getConnection(jdbcConnectionString, connectionProperties)
                    .use { conn ->
                        val useDbQuery = "USE myDatabase;"
                        val useDbStatement = conn.prepareStatement(useDbQuery)
                        useDbStatement.execute()
                        val getUsernameQuery = "SELECT username FROM users WHERE email = ?;"
                        val checkUserStatement = conn.prepareStatement(getUsernameQuery)
                        checkUserStatement.setString(1, email)
                        val resultSet = checkUserStatement.executeQuery()
                        username = resultSet.getString("username")
                    }
            } catch (e: SQLException) {
                Log.d("getUsernameFromEmail", e.message.toString())
            }
        }
    }
}