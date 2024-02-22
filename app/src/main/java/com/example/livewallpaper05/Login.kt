package com.example.livewallpaper05

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.example.livewallpaper05.databinding.ActivityLoginBinding

class Login : AppCompatActivity() {
    private lateinit var binding: ActivityLoginBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // Inflate the binding object
        binding = ActivityLoginBinding.inflate(layoutInflater)
        val view = binding.root
        setContentView(view)

        // Access views from the layout using the binding object
        val registerButton = binding.registerPageButton
        registerButton.setOnClickListener {
            val registerPageIntent = Intent(this, Register::class.java)
            startActivity(registerPageIntent)
        }
    }
}