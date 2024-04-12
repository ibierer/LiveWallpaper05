package com.example.livewallpaper05

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.VideoView

class HelpActivity : AppCompatActivity() {
    private lateinit var videoView: VideoView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_help)
        videoView = findViewById(R.id.faq1VideoView)
        // Prepare the video file or URL
        //val videoPath = "android.resource://${packageName}/${R.raw.your_video_file}" // For a video file in the raw folder
    }
}