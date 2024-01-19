package com.example.livewallpaper05

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.WindowManager
import android.widget.Button

class ExplorerActivity : AppCompatActivity() {

    private var mPreviewButton: Button? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_explorer)

        // Check if user is logged in or signed up, if not send user to login/signup screen

        // query to set profile object
        // if they are signed up, query for explore wallpapers via recommendation algorithm and send those to the UI

    }
}