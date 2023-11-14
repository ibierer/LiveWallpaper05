package com.example.livewallpaper05

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Button

class Explorer : AppCompatActivity() {

    private var mPreviewButton: Button? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_explorer)

        mPreviewButton = findViewById(R.id.preview_button)
        mPreviewButton!!.setText("Open Live Wallpaper Preview")
        mPreviewButton!!.setOnClickListener {
            val intent = Intent(this, MainActivity::class.java)
            startActivity(intent)
        }
    }
}