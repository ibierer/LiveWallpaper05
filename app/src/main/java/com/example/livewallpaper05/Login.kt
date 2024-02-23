package com.example.livewallpaper05

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.text.TextUtils
import android.widget.Button
import android.widget.Toast
import com.example.livewallpaper05.databinding.ActivityLoginBinding
import com.google.android.material.textfield.TextInputEditText
import com.google.firebase.auth.FirebaseAuth

class Login : AppCompatActivity() {
    private lateinit var binding: ActivityLoginBinding
    private lateinit var etEmail: TextInputEditText
    private lateinit var etPassword: TextInputEditText
    private lateinit var loginButton: Button
    private lateinit var regPageButton: Button
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
                        // Sign in success, update UI with the signed-in user's information
                        Toast.makeText(applicationContext, "Login successful!", Toast.LENGTH_SHORT).show()
                        val profilePageIntent = Intent(this, ProfileActivity::class.java)
                        startActivity(profilePageIntent)
                        finish()
                    } else {
                        // If sign in fails, display a message to the user.
                        Toast.makeText(this, "Authentication failed.", Toast.LENGTH_SHORT).show()
                    }
                }
        }
    }
}