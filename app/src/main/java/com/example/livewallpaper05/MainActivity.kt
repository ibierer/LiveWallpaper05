package com.example.livewallpaper05

import android.app.Activity
import android.os.Bundle
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperApplication

class MainActivity : Activity() {

    override fun onCreate(icicle: Bundle?) {
        super.onCreate(icicle)
        ActiveWallpaperApplication.mView = GLES3JNIView(application)
        setContentView(ActiveWallpaperApplication.mView)
    }

    override fun onPause() {
        super.onPause()
        ActiveWallpaperApplication.mView!!.onPause()
    }

    override fun onResume() {
        super.onResume()
        ActiveWallpaperApplication.mView!!.onResume()
    }

    companion object {
        // Used to load the 'livewallpaper05' library on application startup.
        init {
            System.loadLibrary("livewallpaper05")
        }

        external fun init(visualization: String)
        external fun resize(width: Int, height: Int)
        external fun step(
            acc_x: Float,
            acc_y: Float,
            acc_z: Float,
            rot_x: Float,
            rot_y: Float,
            rot_z: Float,
            rot_w: Float,
            linear_acc_x: Float,
            linear_acc_y: Float,
            linear_acc_z: Float,
            distance: Float,
            field_of_view: Float,
            gravity: Float,
            efficiency: Float,
            flip_normals: Boolean
        )
    }
}