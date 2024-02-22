package com.example.livewallpaper05.helpful_fragments

import android.graphics.Bitmap
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.ImageView
import androidx.fragment.app.Fragment
import com.example.livewallpaper05.R

class WallpaperFragment : Fragment() {

    var active: Boolean = false
    var prevBitmap: Bitmap? = null

    private var mIvPreviewPane: ImageView? = null
    private var mBactiveWallpaper: Button? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
    }

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        // get fragment ui view object
        var rootview = inflater.inflate(R.layout.fragment_wallpaper, container, false)

        mIvPreviewPane = rootview?.findViewById(R.id.iv_preview)
        mBactiveWallpaper = rootview?.findViewById(R.id.b_active_wallpaper)

        // set preview pane image
        mIvPreviewPane!!.setImageBitmap(prevBitmap)
        if(active) {
            mBactiveWallpaper!!.isEnabled = false
        }

        return rootview
    }

    companion object {
        fun newInstance(isActive: Boolean, preview: Bitmap): Fragment {
            val frag = WallpaperFragment()
            frag.active = isActive
            frag.prevBitmap = preview
            return frag
        }
    }


}