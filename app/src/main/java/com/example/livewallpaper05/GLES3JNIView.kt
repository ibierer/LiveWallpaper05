package com.example.livewallpaper05

import android.content.Context
import android.opengl.GLSurfaceView
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class GLES3JNIView(context: Context) : GLSurfaceView(context) {
    private val TAG = "GLES3JNI"
    private val DEBUG = true

    init {
        // Pick an EGLConfig with RGB8 color, 16-bit depth, no stencil,
        // supporting OpenGL ES 2.0 or later backwards-compatible versions.
        setEGLConfigChooser(8, 8, 8, 0, 16, 0)
        setEGLContextClientVersion(3)
        setRenderer(Renderer(context))
    }

    class Renderer(context: Context) : GLSurfaceView.Renderer {
        private var context: Context? = null

        fun Renderer(context: Context) {
            this.context = context
        }

        override fun onDrawFrame(gl: GL10) {
            MainActivity.step(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false)
        }

        override fun onSurfaceChanged(gl: GL10, width: Int, height: Int) {
            MainActivity.resize(width, height)
        }

        override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
            MainActivity.init("")
        }
    }
}