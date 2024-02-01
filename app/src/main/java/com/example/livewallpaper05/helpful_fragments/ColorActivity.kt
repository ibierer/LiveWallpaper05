package com.example.livewallpaper05.helpful_fragments

import android.content.Intent
import android.graphics.Bitmap
import android.os.Bundle
import android.widget.Button
import android.widget.ImageView
import android.widget.SeekBar
import androidx.appcompat.app.AppCompatActivity
import com.example.livewallpaper05.R

class ColorActivity : AppCompatActivity() {
    private var mRedSlider: SeekBar? = null
    private var mGreenSlider: SeekBar? = null
    private var mBlueSlider: SeekBar? = null
    private var mAlphaSlider: SeekBar? = null
    private var mColorView: ImageView? = null
    private var mAcceptColor: Button? = null
    private var mColorBitmap: Bitmap? = null

    private var mRed: Int = 200
    private var mGreen: Int = 200
    private var mBlue: Int = 200
    private var mAlpha: Int = 255

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_color)

        // fill mColorBitmap with mRed, mGreen, mBlue, mAlpha
        mColorBitmap = Bitmap.createBitmap(1, 1, Bitmap.Config.ARGB_8888)
        mColorBitmap?.setPixel(0, 0, android.graphics.Color.argb(mAlpha, mRed, mGreen, mBlue))

        // init sliders
        mRedSlider = findViewById(R.id.sb_red)
        mGreenSlider = findViewById(R.id.sb_green)
        mBlueSlider = findViewById(R.id.sb_blue)
        mAlphaSlider = findViewById(R.id.sb_alpha)
        mColorView = findViewById(R.id.iv_color_preview)
        mAcceptColor = findViewById(R.id.btn_accept)

        // fill mColorView with scaled mColorBitmap
        mColorView?.setImageBitmap(Bitmap.createScaledBitmap(mColorBitmap!!, 200, 200, false))

        // set slider listeners
        mRedSlider?.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar, progress: Int, b: Boolean) {
                mRed = progress
                updateColorImage()
            }
            override fun onStartTrackingTouch(seekBar: SeekBar) {
                // do nothing
            }
            override fun onStopTrackingTouch(seekBar: SeekBar) {
                // do nothing
            }
        })

        mGreenSlider?.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar, progress: Int, b: Boolean) {
                mGreen = progress
                updateColorImage()
            }
            override fun onStartTrackingTouch(seekBar: SeekBar) {
                // do nothing
            }
            override fun onStopTrackingTouch(seekBar: SeekBar) {
                // do nothing
            }
        })

        mBlueSlider?.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar, progress: Int, b: Boolean) {
                mBlue = progress
                updateColorImage()
            }
            override fun onStartTrackingTouch(seekBar: SeekBar) {
                // do nothing
            }
            override fun onStopTrackingTouch(seekBar: SeekBar) {
                // do nothing
            }
        })

        mAlphaSlider?.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar, progress: Int, b: Boolean) {
                mAlpha = progress
                updateColorImage()
            }
            override fun onStartTrackingTouch(seekBar: SeekBar) {
                // do nothing
            }
            override fun onStopTrackingTouch(seekBar: SeekBar) {
                // do nothing
            }
        })

        mAcceptColor?.setOnClickListener {
            val color = android.graphics.Color.argb(
                mAlpha,
                mRed,
                mGreen,
                mBlue
            )
            var data = Intent()
            //data.setData(Uri.parse(color.toString()))
            data.putExtra("color", color)
            setResult(RESULT_OK, data)
            finish()
        }
    }

    fun updateColorImage(){
        mColorBitmap?.setPixel(0, 0, android.graphics.Color.argb(mAlpha, mRed, mGreen, mBlue))
        mColorView?.setImageBitmap(Bitmap.createScaledBitmap(mColorBitmap!!, 100, 100, false))
    }
}