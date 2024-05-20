package com.example.livewallpaper05

import android.content.Context
import android.content.res.Configuration
import android.opengl.GLSurfaceView
import android.service.wallpaper.WallpaperService
import android.util.Log
import android.view.SurfaceHolder
import android.view.WindowManager
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperApplication
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperRepo
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModel

class GLWallpaperService : WallpaperService() {

    val viewModel: ActiveWallpaperViewModel by lazy {
        ActiveWallpaperViewModel(ActiveWallpaperRepo.getInstance(application, (application as ActiveWallpaperApplication).applicationScope))
    }

    override fun onCreateEngine(): Engine {
        return GLEngine()
    }

    override fun onConfigurationChanged(newConfig: Configuration) {
        super.onConfigurationChanged(newConfig)
        
        @Suppress("DEPRECATION")
        val display = (getSystemService(Context.WINDOW_SERVICE) as WindowManager).defaultDisplay
        Log.d("GLWallpaperService", "Rotation changed to ${display.rotation}")

        // update view model with new orientation
        viewModel.repo.updateOrientation(display.rotation)
    }

    inner class GLEngine : Engine() {
        private val glSurfaceView by lazy { WallpaperGLSurfaceView(this@GLWallpaperService) }
        private var rendererSet = false

        override fun onCreate(surfaceHolder: SurfaceHolder) {
            super.onCreate(surfaceHolder)
            glSurfaceView.setEGLContextClientVersion(3)
            glSurfaceView.setRenderer(Renderer(viewModel))
            rendererSet = true
        }

        internal inner class WallpaperGLSurfaceView(context: Context?) : GLSurfaceView(context) {
            override fun getHolder(): SurfaceHolder {
                return surfaceHolder
            }

            /*fun onWallpaperDestroy() {
                super.onDetachedFromWindow()
            }*/
        }

        override fun onVisibilityChanged(visible: Boolean) {
            super.onVisibilityChanged(visible)
            if (rendererSet) {
                if (visible) {
                    glSurfaceView.onResume()
                } else {
                    glSurfaceView.onPause()
                }
            }
        }

        /*override fun onDestroy() {
            super.onDestroy()
            glSurfaceView.onWallpaperDestroy()
        }*/
    }
}
