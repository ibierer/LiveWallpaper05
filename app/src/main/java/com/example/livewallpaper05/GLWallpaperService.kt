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
import com.example.livewallpaper05.profiledata.ProfileRoomDatabase
import com.example.livewallpaper05.savedWallpapers.SavedWallpaperRoomDatabase

class GLWallpaperService() : WallpaperService() {

    private val savedWallpaperDatabase by lazy { SavedWallpaperRoomDatabase.getDatabase(this)}
    private val profileDatabase by lazy { ProfileRoomDatabase.getDatabase(this, (application as ActiveWallpaperApplication).applicationScope)}
    val viewModel: ActiveWallpaperViewModel by lazy {
        ActiveWallpaperViewModel(ActiveWallpaperRepo.getInstance(application, savedWallpaperDatabase.wallpaperDao(), profileDatabase.profileDao(), (application as ActiveWallpaperApplication).applicationScope))
    }

    override fun onCreateEngine(): Engine {
        return GLEngine()
    }

    override fun onConfigurationChanged(newConfig: Configuration) {
        super.onConfigurationChanged(newConfig)
        val display = (getSystemService(Context.WINDOW_SERVICE) as WindowManager).defaultDisplay
        Log.d("GLWallpaperService", "Rotation changed to ${display.rotation}")

        // update view model with new orientation
        viewModel.repo.updateOrientation(display.rotation)
    }

    inner class GLEngine : Engine() {
        private var glSurfaceView: WallpaperGLSurfaceView? = null
        private var rendererSet = false

        override fun onCreate(surfaceHolder: SurfaceHolder) {
            super.onCreate(surfaceHolder)
            glSurfaceView = WallpaperGLSurfaceView(this@GLWallpaperService)

            // Check if the system supports OpenGL ES 2.0.
            val activityManager = getSystemService(ACTIVITY_SERVICE) as ActivityManager
            val configurationInfo = activityManager.deviceConfigurationInfo
            val supportsEs3 = configurationInfo.reqGlEsVersion >= 0x3
            val renderer: GLES3JNIView.Renderer = GLES3JNIView.Renderer(glSurfaceView!!.context, viewModel, glSurfaceView!!)
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
