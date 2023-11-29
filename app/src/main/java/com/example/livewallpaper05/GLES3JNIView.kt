package com.example.livewallpaper05

import android.content.Context
import android.opengl.GLSurfaceView
import android.util.Log
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperRepo
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class GLES3JNIView(context: Context, repo: ActiveWallpaperRepo) : GLSurfaceView(context) {
    private val TAG = "GLES3JNI"
    private val DEBUG = true

    init {
        // Pick an EGLConfig with RGB8 color, 16-bit depth, no stencil,
        // supporting OpenGL ES 2.0 or later backwards-compatible versions.
        setEGLConfigChooser(8, 8, 8, 0, 16, 0)
        setEGLContextClientVersion(3)
        setRenderer(Renderer(context, repo))
    }

    class Renderer(context: Context, repo: ActiveWallpaperRepo) : GLSurfaceView.Renderer {
        private var context: Context? = null
        private var mRepo: ActiveWallpaperRepo? = repo

        fun Renderer(context: Context, repo: ActiveWallpaperRepo) {
            this.context = context
        }

        override fun onDrawFrame(gl: GL10) {
            // change to repo data
            if(mRepo != null){
                //x_rot = mRepo!!.rotationRate
                MainActivity.step(mRepo!!.accelerationData[0],
                    mRepo!!.accelerationData[1],
                    mRepo!!.accelerationData[2],
                    mRepo!!.rotationData[0],
                    mRepo!!.rotationData[1],
                    mRepo!!.rotationData[2],
                    mRepo!!.rotationData[3])
            } else {
                MainActivity.step(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f)
            }
        }

        override fun onSurfaceChanged(gl: GL10, width: Int, height: Int) {
            MainActivity.resize(width, height)
        }

        override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
            MainActivity.init()
        }
    }
}