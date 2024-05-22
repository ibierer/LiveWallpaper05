package com.example.livewallpaper05

import android.content.Context
import android.opengl.GLSurfaceView
import android.os.SystemClock
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
        setRenderer(Renderer(vm))
    }
}

class Renderer(private var mViewModel: ActiveWallpaperViewModel) : GLSurfaceView.Renderer {

    override fun onDrawFrame(gl: GL10) {
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

        // get time after frame
        val currentFrame = SystemClock.elapsedRealtimeNanos()
        // calculate time elapsed
        var timeElapsed = (currentFrame - mViewModel.getLastFrame()) / 1000000000.0f
        timeElapsed = 1.0f / timeElapsed.toFloat()
        // update fps in view model with fps from spf
        mViewModel.updateFPS(timeElapsed)
        // update last frame in view model
        mViewModel.updateLastFrame(currentFrame)
    }

    override fun onSurfaceChanged(gl: GL10, width: Int, height: Int) {
        PreviewActivity.resize(width, height, mViewModel.getOrientation())
    }

    override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
        //val visualization : Visualization = when (mViewModel.getVisualization()) {
        val visualization : Visualization = when (0) {
            0 -> NBodyVisualization() // Broken when service->activity
            1 -> NaiveFluidVisualization() // Broken when service->activity
            2 -> PicFlipVisualization()
            3 -> TriangleVisualization()
            4 -> GraphVisualization() // Broken when service->activity
            else -> throw IllegalArgumentException("Invalid visualization type")
        }
        PreviewActivity.init(visualization.toJsonObject().toString())
    }
}