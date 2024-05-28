package com.example.livewallpaper05

import android.content.Context
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
        ActiveWallpaperViewModel(ActiveWallpaperRepo.getInstance(application, ActiveWallpaperApplication.applicationScope))
    }

    override fun onCreateEngine(): Engine {
        return GLEngine()
    }

    inner class GLEngine : Engine(), SurfaceHolder.Callback {
        private val glSurfaceView by lazy { WallpaperGLSurfaceView(application) }
        private var rendererSet = false

        override fun onCreate(surfaceHolder: SurfaceHolder) {
            super.onCreate(surfaceHolder)
            surfaceHolder.addCallback(this)
            glSurfaceView.setEGLContextClientVersion(3)
            glSurfaceView.setRenderer(Renderer(application, viewModel, 1))
            rendererSet = true
        }

        override fun onDestroy() {
            super.onDestroy()
            surfaceHolder.removeCallback(this)
        }

        override fun onSurfaceRedrawNeeded(holder: SurfaceHolder?) {
            super.onSurfaceRedrawNeeded(holder)
            val display = (getSystemService(Context.WINDOW_SERVICE) as WindowManager).defaultDisplay
            val rotation = display.rotation
            // Detect whether orientation has switched from portrait to landscape or vice versa
            val needsRefactored: Boolean = viewModel.repo.orientation % 2 != rotation % 2
            viewModel.repo.updateOrientation(rotation)
            if(needsRefactored) {
                glSurfaceView.onPause()
                glSurfaceView.onResume()
            }
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

        override fun surfaceCreated(holder: SurfaceHolder) {

        }

        override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {

        }

        /*override fun onDestroy() {
            super.onDestroy()
            glSurfaceView.onWallpaperDestroy()
        }*/

        override fun surfaceDestroyed(holder: SurfaceHolder) {

        }
    }
}
