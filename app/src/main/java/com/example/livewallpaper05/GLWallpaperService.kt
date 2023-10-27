package com.example.livewallpaper05

import android.app.ActivityManager
import android.content.Context
import android.opengl.GLSurfaceView
import android.service.wallpaper.WallpaperService
import android.view.SurfaceHolder
import android.widget.Toast

class GLWallpaperService : WallpaperService() {
    override fun onCreateEngine(): Engine {
        return GLEngine()
    }

    inner class GLEngine : Engine() {
        private var glSurfaceView: WallpaperGLSurfaceView? = null
        private var rendererSet = false
        override fun onCreate(surfaceHolder: SurfaceHolder) {
            super.onCreate(surfaceHolder)
            glSurfaceView = WallpaperGLSurfaceView(this@GLWallpaperService)

            // Check if the system supports OpenGL ES 2.0.
            val activityManager = getSystemService(ACTIVITY_SERVICE) as ActivityManager
            val configurationInfo = activityManager
                .deviceConfigurationInfo
            val supportsEs3 = configurationInfo.reqGlEsVersion >= 0x3
            val renderer: GLES3JNIView.Renderer = GLES3JNIView.Renderer(this@GLWallpaperService)
            rendererSet = if (supportsEs3) {
                glSurfaceView!!.setEGLContextClientVersion(3)
                glSurfaceView!!.setRenderer(renderer)
                true
            } else {
                Toast.makeText(
                    this@GLWallpaperService,
                    "This device does not support OpenGL ES 2.0.",
                    Toast.LENGTH_LONG
                ).show()
                return
            }
        }

        internal inner class WallpaperGLSurfaceView(context: Context?) : GLSurfaceView(context) {
            override fun getHolder(): SurfaceHolder {
                return surfaceHolder
            }

            fun onWallpaperDestroy() {
                super.onDetachedFromWindow()
            }
        }

        override fun onVisibilityChanged(visible: Boolean) {
            super.onVisibilityChanged(visible)
            if (rendererSet) {
                if (visible) {
                    glSurfaceView!!.onResume()
                } else {
                    glSurfaceView!!.onPause()
                }
            }
        }

        override fun onDestroy() {
            super.onDestroy()
            glSurfaceView!!.onWallpaperDestroy()
        }
    }
}
