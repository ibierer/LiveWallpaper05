package com.example.livewallpaper05

import android.content.Context
import android.opengl.GLSurfaceView
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class GLES3JNIView(context: Context) : GLSurfaceView(context) {
    init {
        // Pick an EGLConfig with RGB8 color, 16-bit depth, no stencil,
        // supporting OpenGL ES 2.0 or later backwards-compatible versions.
        setEGLConfigChooser(8, 8, 8, 0, 16, 0)
        setEGLContextClientVersion(3)
        setRenderer(Renderer())
    }

    class Renderer : GLSurfaceView.Renderer {

        override fun onDrawFrame(gl: GL10) {
            PreviewActivity.step(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false)
        }

        override fun onSurfaceChanged(gl: GL10, width: Int, height: Int) {
            PreviewActivity.resize(width, height, 1)
        }

        override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
            PreviewActivity.init("")
        }
    }
}