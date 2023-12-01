package com.example.livewallpaper05

import android.content.Context
import android.opengl.GLSurfaceView
import android.util.Log
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperRepo
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModel
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class GLES3JNIView(context: Context, vm: ActiveWallpaperViewModel) : GLSurfaceView(context) {
    private val TAG = "GLES3JNI"
    private val DEBUG = true

    init {
        // Pick an EGLConfig with RGB8 color, 16-bit depth, no stencil,
        // supporting OpenGL ES 2.0 or later backwards-compatible versions.
        setEGLConfigChooser(8, 8, 8, 0, 16, 0)
        setEGLContextClientVersion(3)
        setRenderer(Renderer(context, vm))
    }

    class Renderer(context: Context, vm: ActiveWallpaperViewModel) : GLSurfaceView.Renderer {
        private var context: Context? = null
        //private var mRepo: ActiveWallpaperRepo? = repo
        private var mViewModel: ActiveWallpaperViewModel = vm

        fun Renderer(context: Context, repo: ActiveWallpaperRepo) {
            this.context = context
        }

        override fun onDrawFrame(gl: GL10) {
            // default values in ActiveWallpaperViewModel
            val accelData = mViewModel.getAccelerationData()
            val rotData = mViewModel.getRotationData()
            /**val rotTmp = mViewModel.getRotationRate()
            MainActivity.step(
                0.0f,
                0.0f,
                0.0f,
                rotTmp,
                0.0f,
                0.0f,
                0.0f
                )*/

            MainActivity.step(
                accelData[0],
                accelData[1],
                accelData[2],
                rotData[0],
                rotData[1],
                rotData[2],
                rotData[3],
                mViewModel.getRotationRate()
            )
        }

        override fun onSurfaceChanged(gl: GL10, width: Int, height: Int) {
            MainActivity.resize(width, height)
        }

        override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
            MainActivity.init(2)
        }
    }
}