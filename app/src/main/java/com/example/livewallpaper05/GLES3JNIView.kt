package com.example.livewallpaper05

import android.content.Context
import android.hardware.SensorManager
import android.opengl.GLSurfaceView
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModel
import com.example.livewallpaper05.activewallpaperdata.NBodyVisualization
import com.example.livewallpaper05.activewallpaperdata.NaiveFluidVisualization
import com.example.livewallpaper05.activewallpaperdata.PicFlipVisualization
import com.example.livewallpaper05.activewallpaperdata.TriangleVisualization
import com.example.livewallpaper05.activewallpaperdata.GraphVisualization
import com.example.livewallpaper05.activewallpaperdata.Visualization
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class GLES3JNIView(context: Context, vm: ActiveWallpaperViewModel) : GLSurfaceView(context) {
    init {
        // Pick an EGLConfig with RGB8 color, 16-bit depth, no stencil,
        // supporting OpenGL ES 2.0 or later backwards-compatible versions.
        setEGLConfigChooser(8, 8, 8, 0, 16, 0)
        setEGLContextClientVersion(3)
        setRenderer(Renderer(context, vm))
    }

    class Renderer(private val context: Context, private var mViewModel: ActiveWallpaperViewModel) : GLSurfaceView.Renderer {

        override fun onDrawFrame(gl: GL10) {
            //PreviewActivity.step(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false)
            val accelData = mViewModel.getAccelerationData()
            val rotData = mViewModel.getRotationData()
            val linearAccelData = mViewModel.getLinearAccelerationData()
            val multiplier = mViewModel.getLinearAcceleration()
            PreviewActivity.step(
                accelData[0],
                accelData[1],
                accelData[2],
                rotData[0],
                rotData[1],
                rotData[2],
                rotData[3],
                linearAccelData[0] * multiplier,
                linearAccelData[1] * multiplier,
                linearAccelData[2] * multiplier,
                mViewModel.getDistanceFromOrigin(),
                mViewModel.getFieldOfView(),
                mViewModel.getGravity(),
                mViewModel.getEfficiency(),
                mViewModel.getVectorDirection()
            )
        }

        override fun onSurfaceChanged(gl: GL10, width: Int, height: Int) {
            mViewModel.registerSensorEvents(context.getSystemService(Context.SENSOR_SERVICE) as SensorManager)
            PreviewActivity.resize(width, height, mViewModel.getOrientation())
        }

        override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
            val visualization : Visualization = when (mViewModel.getVisualization()) {
                0 -> NBodyVisualization()
                1 -> NaiveFluidVisualization()
                2 -> PicFlipVisualization()
                3 -> TriangleVisualization()
                4 -> GraphVisualization()
                else -> throw IllegalArgumentException("Invalid visualization type")
            }
            PreviewActivity.init(visualization.toJsonObject().toString())
        }
    }
}