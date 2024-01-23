package com.example.livewallpaper05

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.WindowManager
import android.widget.Button

class ExplorerActivity : AppCompatActivity() {

    private var mPreviewButton: Button? = null
    private var mTestButton: Button? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_explorer)

        mTestButton = findViewById(R.id.b_test_button)

        mTestButton?.setOnClickListener {
            // write aws test code here -------------
        }
    }
}