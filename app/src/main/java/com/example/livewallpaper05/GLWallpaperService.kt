package com.example.livewallpaper05

import android.app.ActivityManager
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.opengl.GLSurfaceView
import android.service.wallpaper.WallpaperService
import android.view.SurfaceHolder
import android.widget.Toast
import androidx.activity.viewModels
import androidx.appcompat.app.AppCompatActivity
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperApplication
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperRepo
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModel
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModelFactory

class GLWallpaperService() : WallpaperService() {

    override fun onCreateEngine(): Engine {
        return GLEngine()
    }

    inner class GLEngine : Engine() {
        private var glSurfaceView: WallpaperGLSurfaceView? = null
        private var rendererSet = false
        override fun onCreate(surfaceHolder: SurfaceHolder) {
            super.onCreate(surfaceHolder)
            glSurfaceView = WallpaperGLSurfaceView(this@GLWallpaperService)

            // create view model from config string
            val viewModel = ActiveWallpaperViewModel(ActiveWallpaperRepo.getInstance((application as ActiveWallpaperApplication).applicationScope))

            // Check if the system supports OpenGL ES 2.0.
            val activityManager = getSystemService(ACTIVITY_SERVICE) as ActivityManager
            val configurationInfo = activityManager
                .deviceConfigurationInfo
            val supportsEs3 = configurationInfo.reqGlEsVersion >= 0x3
            val renderer: GLES3JNIView.Renderer = GLES3JNIView.Renderer(this@GLWallpaperService, viewModel)
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
