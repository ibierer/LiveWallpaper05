package com.example.livewallpaper05

import android.app.ActivityManager
import android.content.Context
import android.content.res.Configuration
import android.opengl.GLSurfaceView
import android.service.wallpaper.WallpaperService
import android.util.Log
import android.view.SurfaceHolder
import android.view.WindowManager
import android.widget.Toast
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperApplication
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperRepo
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModel

class GLWallpaperService() : WallpaperService() {

    override fun onCreateEngine(): Engine {
        return GLEngine()
    }

    override fun onConfigurationChanged(newConfig: Configuration) {
        super.onConfigurationChanged(newConfig)
        val display = (getSystemService(Context.WINDOW_SERVICE) as WindowManager).defaultDisplay
        Log.d("GLWallpaperService", "Rotation changed to ${display.rotation}")
        // update view model with new orientation
        val viewModel: ActiveWallpaperViewModel by lazy {
            ActiveWallpaperViewModel(ActiveWallpaperRepo.getInstance((application as ActiveWallpaperApplication).applicationScope))
        }
        viewModel.mRepo.updateOrientation(display.rotation)
    }

    inner class GLEngine : Engine() {
        private var glSurfaceView: WallpaperGLSurfaceView? = null
        private var rendererSet = false

        private val viewModel: ActiveWallpaperViewModel by lazy {
            ActiveWallpaperViewModel(ActiveWallpaperRepo.getInstance((application as ActiveWallpaperApplication).applicationScope))
        }

        override fun onCreate(surfaceHolder: SurfaceHolder) {
            super.onCreate(surfaceHolder)
            glSurfaceView = WallpaperGLSurfaceView(this@GLWallpaperService)

            // Check if the system supports OpenGL ES 2.0.
            val activityManager = getSystemService(ACTIVITY_SERVICE) as ActivityManager
            val configurationInfo = activityManager.deviceConfigurationInfo
            val supportsEs3 = configurationInfo.reqGlEsVersion >= 0x3
            val renderer: GLES3JNIView.Renderer = GLES3JNIView.Renderer(viewModel)
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
