package com.example.livewallpaper05

import android.content.Context
import android.opengl.GLSurfaceView
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperRepo
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModel
import kotlinx.coroutines.selects.select
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
            PreviewActivity.step(
                0.0f,
                0.0f,
                0.0f,
                rotTmp,
                0.0f,
                0.0f,
                0.0f
                )*/

            PreviewActivity.step(
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
            PreviewActivity.resize(width, height)
        }

        override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
            val boxJSON = "{\n" +
                    "   \"type\": \"box\"\n" +
                    "}"
            val naiveJSON = "{\n" +
                    "   \"type\": \"naive\"\n" +
                    "}"
            val picflipJSON = "{\n" +
                    "   \"type\": \"picflip\"\n" +
                    "}"
            val triangleJSON = "{\n" +
                    "   \"type\": \"triangle\"\n" +
                    "}"
            val graphJSON = "{\n" +
                    "   \"type\": \"graph\",\n" +
                    "   \"settings\": \"1/((sqrt(x^2 + y^2) - 2 + 1.25cos(t))^2 + (z - 1.5sin(t))^2) + 1/((sqrt(x^2 + y^2) - 2 - 1.25cos(t))^2 + (z + 1.5sin(t))^2) = 1.9\"\n" +
                    "}"

            var selectionJSON = boxJSON
            when (mViewModel.getSimulationType()) {
                0 -> {
                    selectionJSON = boxJSON
                }
                1 -> {
                    selectionJSON = naiveJSON
                }
                2 -> {
                    selectionJSON = picflipJSON
                }
                3 -> {
                    selectionJSON = triangleJSON
                }
                4 -> {
                    selectionJSON = graphJSON
                }

            }

            PreviewActivity.init(selectionJSON)
        }
    }
}