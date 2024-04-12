package com.example.livewallpaper05

import android.graphics.drawable.BitmapDrawable
import android.media.MediaMetadataRetriever
import android.net.Uri
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Toast
import android.widget.VideoView

class HelpActivity : AppCompatActivity() {
    private lateinit var videoView1: VideoView
    private lateinit var videoView2: VideoView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_help)
        videoView1 = findViewById(R.id.faq1VideoView)
        videoView2 = findViewById(R.id.faq2VideoView)

        // Prepare the video file or URL
        val videoPath1 =
            "android.resource://${packageName}/${R.raw.create_wallpaper}" // For a video file in the raw folder
        val videoPath2 =
            "android.resource://${packageName}/${R.raw.create_account}" // For a video file in the raw folder


        // Set the video source for the VideoViews
        videoView1.setVideoURI(Uri.parse(videoPath1))
        videoView2.setVideoURI(Uri.parse(videoPath2))

        // Handle the exception by showing a toast with an error message
        //Toast.makeText(this, "Error loading videos", Toast.LENGTH_SHORT).show()

        val retriever = MediaMetadataRetriever()
        val retriever2 = MediaMetadataRetriever()
        retriever.setDataSource(this, Uri.parse(videoPath1))
        retriever2.setDataSource(this, Uri.parse(videoPath2))
        /*val bitmap = retriever.getFrameAtTime(0, MediaMetadataRetriever.OPTION_CLOSEST_SYNC)
        val bitmap2 = retriever2.getFrameAtTime(0, MediaMetadataRetriever.OPTION_CLOSEST_SYNC)
        videoView1.background = BitmapDrawable(resources, bitmap)
        videoView2.background = BitmapDrawable(resources, bitmap2)*/

        videoView1.setOnCompletionListener {
            // Reset the video to the beginning
            videoView1.seekTo(0)
        }

        // Set OnClickListener for the first VideoView
        videoView1.setOnClickListener {
            // Toggle playback
            togglePlayback(videoView1)
        }

        // Set OnClickListener for the second VideoView
        videoView2.setOnClickListener {
            togglePlayback(videoView2)
        }

        videoView2.setOnCompletionListener {
            videoView2.seekTo(0)
        }
    }

    private fun togglePlayback(videoView: VideoView?) {
        if (videoView!!.isPlaying) {
            videoView.pause()
        } else {
            videoView.start()
        }
    }
}