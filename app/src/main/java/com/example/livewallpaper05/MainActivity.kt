package com.example.livewallpaper05

import android.app.Activity
import android.os.Bundle

class MainActivity : Activity() {

    var mView: GLES3JNIView? = null

    override fun onCreate(icicle: Bundle?) {
        super.onCreate(icicle)
        mView = GLES3JNIView(application)
        setContentView(mView)
    }

    override fun onPause() {
        super.onPause()
        mView!!.onPause()
    }

    override fun onResume() {
        super.onResume()
        mView!!.onResume()
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