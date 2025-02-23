package com.example.livewallpaper05

import android.content.Context
import android.graphics.Bitmap
import android.graphics.Color
import android.hardware.SensorManager
import android.opengl.GLSurfaceView
import android.os.SystemClock
import android.util.Log
import android.view.View
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModel
import com.example.livewallpaper05.activewallpaperdata.GraphVisualization
import com.example.livewallpaper05.activewallpaperdata.NBodyVisualization
import com.example.livewallpaper05.activewallpaperdata.NaiveFluidVisualization
import com.example.livewallpaper05.activewallpaperdata.PicFlipVisualization
import com.example.livewallpaper05.activewallpaperdata.TriangleVisualization
import com.google.firebase.auth.FirebaseAuth
import org.json.JSONObject
import java.io.ByteArrayOutputStream
import java.nio.ByteBuffer
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class GLES3JNIView(context: Context, vm: ActiveWallpaperViewModel) : GLSurfaceView(context) {
    private val TAG = "GLES3JNI"
    private val DEBUG = true

    init {
        // Pick an EGLConfig with RGB8 color, 24-bit depth, no stencil,
        // supporting OpenGL ES 2.0 or later backwards-compatible versions.
        setEGLContextClientVersion(3)
        setEGLConfigChooser(8, 8, 8, 8, 24, 8)
        setRenderer(Renderer(context, vm, this))
    }

    class Renderer(private val context: Context, vm: ActiveWallpaperViewModel, var view: View) : GLSurfaceView.Renderer {
        private var mViewModel: ActiveWallpaperViewModel = vm
        private var auth: FirebaseAuth? = null
        var username: String? = "Default User"
        var uid: Int? = 11 //uid for default user

        override fun onDrawFrame(gl: GL10) {
            // default values in ActiveWallpaperViewModel
            val accelData = mViewModel.getAccelerationData()
            val rotData = mViewModel.getRotationData()
            val linearAccelData = mViewModel.getLinearAccelerationData()
            val multiplier = mViewModel.getLinearAcceleration()

            // update visualization values
            mViewModel.repo.visualization.updateValues()

            // Run C++ visualization logic and render OpenGL view
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

            // Get screen buffer if requested
            if (mViewModel.repo.getScreenBuffer > 0) {
                mViewModel.repo.getScreenBuffer = 0
                val byteArray: ByteArray = PreviewActivity.getScreenBuffer()
                // Construct Bitmap from ByteArray
                val bitmap = Bitmap.createBitmap(
                    mViewModel.repo.width,
                    mViewModel.repo.height,
                    Bitmap.Config.ARGB_8888
                )
                // Set pixel data manually by iterating over the byte array
                val buffer = ByteBuffer.wrap(byteArray)
                for (y in mViewModel.repo.height - 1 downTo 0) { // Reverse order for y-axis
                    for (x in 0 until mViewModel.repo.width) {
                        // Extract RGB components from the byte array and set the pixel
                        val r = buffer.get().toInt() and 0xFF
                        val g = buffer.get().toInt() and 0xFF
                        val b = buffer.get().toInt() and 0xFF
                        val a = buffer.get().toInt() and 0xFF
                        bitmap.setPixel(x, y, Color.argb(a, r, g, b))
                    }
                }

                val width = bitmap.width
                val height = bitmap.height
                val size = minOf(width, height)
                val startX = (width - size) / 2
                val startY = (height - size) / 2

                val squareBitmap = Bitmap.createBitmap(bitmap, startX, startY, size, size)

                val scaledSquareBitmap = Bitmap.createScaledBitmap(squareBitmap, 512, 512, true)
                val outputStream = ByteArrayOutputStream()

                scaledSquareBitmap.compress(Bitmap.CompressFormat.PNG, 100, outputStream)
                mViewModel.repo.liveDataBitmap.postValue(scaledSquareBitmap)
                //`mViewModel.repo.liveDataBitmap.postValue(bitmap)
            }

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
            val orientation = mViewModel.getOrientation()
            PreviewActivity.resize(width, height, orientation)
            mViewModel.repo.width = width
            mViewModel.repo.height = height
            // register sensor event listeners
            mViewModel.registerSensorEvents(context.getSystemService(Context.SENSOR_SERVICE) as SensorManager)
        }

        override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
            var jsonConfig: JSONObject = JSONObject()
            val loadedConfig = JSONObject(mViewModel.getConfig())
            Log.d("VISUALIZATION loadedConfig = ", loadedConfig.toString())

            when (mViewModel.getVisualization()) {
                0 -> {
                    mViewModel.repo.visualization = NBodyVisualization(loadedConfig)
                    jsonConfig = mViewModel.repo.visualization.toJsonObject()
                }
                1 -> {
                    mViewModel.repo.visualization = NaiveFluidVisualization(loadedConfig)
                    jsonConfig = mViewModel.repo.visualization.toJsonObject()
                    mViewModel.repo.gravity.postValue(jsonConfig.getDouble("gravity").toFloat())
                    mViewModel.repo.linearAcceleration.postValue(jsonConfig.getDouble("linear_acceleration").toFloat())
                    mViewModel.repo.efficiency.postValue(jsonConfig.getDouble("efficiency").toFloat())
                }
                2 -> {
                    mViewModel.repo.visualization = PicFlipVisualization(loadedConfig)
                    jsonConfig = mViewModel.repo.visualization.toJsonObject()
                    mViewModel.repo.gravity.postValue(jsonConfig.getDouble("gravity").toFloat())
                    mViewModel.repo.linearAcceleration.postValue(jsonConfig.getDouble("linear_acceleration").toFloat())
                }
                3 -> {
                    mViewModel.repo.visualization = TriangleVisualization(loadedConfig)
                    jsonConfig = mViewModel.repo.visualization.toJsonObject()
                }
                4 -> {
                    mViewModel.repo.visualization = GraphVisualization(loadedConfig)
                    jsonConfig = mViewModel.repo.visualization.toJsonObject()
                    if (mViewModel.repo.graphSelection != 0){
                        jsonConfig.put("equation", context.resources.getStringArray(R.array.graph_options)[mViewModel.repo.graphSelection])
                    }
                }
            }
                //mViewModel.repo.visualization.viewModel = mViewModel
            Log.d("VISUALIZATION jsonConfig = ", jsonConfig.toString())
            mViewModel.repo.distanceFromOrigin.postValue(jsonConfig.getDouble("distance").toFloat())
            mViewModel.repo.fieldOfView.postValue(jsonConfig.getDouble("field_of_view").toFloat())
            mViewModel.updateColor(mViewModel.repo.visualization.jsonObjectToColor(jsonConfig.getJSONObject("background_color")), true)
            PreviewActivity.init(jsonConfig.toString())
        }
    }
}