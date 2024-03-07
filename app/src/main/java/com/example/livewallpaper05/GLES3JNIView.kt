package com.example.livewallpaper05

import android.content.Context
import android.graphics.Bitmap
import android.graphics.Color
import android.opengl.GLSurfaceView
import android.os.SystemClock
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModel
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
        setRenderer(Renderer(vm))
    }

    class Renderer(vm: ActiveWallpaperViewModel) : GLSurfaceView.Renderer {
        private var mViewModel: ActiveWallpaperViewModel = vm

        override fun onDrawFrame(gl: GL10) {
            // default values in ActiveWallpaperViewModel
            val accelData = mViewModel.getAccelerationData()
            val rotData = mViewModel.getRotationData()
            val linearAccelData = mViewModel.getLinearAccelerationData()
            val speed = mViewModel.getSpeed()
            // update linear acceleration data
            linearAccelData[0] = linearAccelData[0] * speed
            linearAccelData[1] = linearAccelData[1] * speed
            linearAccelData[2] = linearAccelData[2] * speed

            // Run C++ visualization logic and render OpenGL view
            PreviewActivity.step(
                accelData[0],
                accelData[1],
                accelData[2],
                rotData[0],
                rotData[1],
                rotData[2],
                rotData[3],
                linearAccelData[0],
                linearAccelData[1],
                linearAccelData[2],
                mViewModel.getRotationRate()
            )

            // Get screen buffer if requested
            if(mViewModel.getScreenBuffer > 0){
                mViewModel.getScreenBuffer = 0
                val byteArray: ByteArray = PreviewActivity.getScreenBuffer()
                // Construct Bitmap from ByteArray
                val bitmap = Bitmap.createBitmap(mViewModel.width, mViewModel.height, Bitmap.Config.ARGB_8888)
                // Set pixel data manually by iterating over the byte array
                val buffer = ByteBuffer.wrap(byteArray)
                for (y in mViewModel.height - 1 downTo 0) { // Reverse order for y-axis
                    for (x in 0 until mViewModel.width) {
                        // Extract RGB components from the byte array and set the pixel
                        val r = buffer.get().toInt() and 0xFF
                        val g = buffer.get().toInt() and 0xFF
                        val b = buffer.get().toInt() and 0xFF
                        val a = buffer.get().toInt() and 0xFF
                        bitmap.setPixel(x, y, Color.argb(a, r, g, b))
                    }
                }
                mViewModel.liveDataBitmap.postValue(Bitmap.createScaledBitmap(bitmap, bitmap.width / 4, bitmap.height / 4, true))
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
            //Log.d("Livewallpaper", "orientation: $orientation")
            PreviewActivity.resize(width, height, orientation)
            mViewModel.width = width
            mViewModel.height = height
        }

        override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
            // get color rgba values from view model
            val color = mViewModel.getColor()
            val r = (color.red()*255).toInt()
            val g = (color.green()*255).toInt()
            val b = (color.blue()*255).toInt()
            val a = (color.alpha()*255).toInt()
            val eq = mViewModel.getEquation()

            val boxJSON = """{
                    "visualization_type": "simulation",
                    "simulation_type": "naive",
                    "fluid_surface": "false",
                    "particle_count": 1000,
                    "smooth_sphere_surface": "true",
                    "gravity": 0.0,
                    "reference_frame_rotates": "false",
                    "background_is_solid_color": "true",
                    "background_texture": "ms_paint_colors",
                    "background_color": {"r": $r, "g": $g, "b": $b, "a": $a}
                }""".trimIndent()

            val nbodyJSON = """{
                    "visualization_type": "simulation",
                    "simulation_type": "nbody",
                    "background_is_solid_color": "false",
                    "background_texture": "ms_paint_colors",
                    "background_color": {"r": $r, "g": $g, "b": $b, "a": $a}
                }""".trimIndent()

            val naiveJSON = """{
                    "visualization_type": "simulation",
                    "simulation_type": "naive",
                    "fluid_surface": "true",
                    "particle_count": 1000,
                    "smooth_sphere_surface": "true",
                    "gravity": 0.0,
                    "reference_frame_rotates": "false",
                    "background_is_solid_color": "false",
                    "background_texture": "ms_paint_colors",
                    "background_color": {"r": $r, "g": $g, "b": $b, "a": $a}
                }""".trimIndent()

            val picflipJSON = """{
                    "visualization_type": "simulation",
                    "simulation_type": "picflip",
                    "gravity": 0.0,
                    "reference_frame_rotates": "true",
                    "background_is_solid_color": "false",
                    "background_texture": "ms_paint_colors",
                    "background_color": {"r": $r, "g": $g, "b": $b, "a": $a}
                }""".trimIndent()

            val triangleJSON = """{
                    "visualization_type": "other",
                    "background_is_solid_color": "false",
                    "background_texture": "mandelbrot",
                    "background_color": {"r": $r, "g": $g, "b": $b, "a": $a}
                }""".trimIndent()

            val graphJSON = """{
                    "visualization_type": "graph",
                    "reference_frame_rotates": "false",
                    "background_is_solid_color": "false",
                    "background_texture": "ms_paint_colors",
                    "vector_points_positive": "false",
                    "background_color": {"r": $r, "g": $g, "b": $b, "a": $a},
                    "equation": "$eq"
                }""".trimIndent()

            /*"saturn": "1/(x^2 + y^2 + z^2) + 1/((1.2*sqrt(x^2 + y^2) - 2.7)^20 + (20z)^20) = 1"
            "pill": "4 = x^2 + y^2 + ((abs(z - 1.15) - abs(z + 1.15))/2 + z)^2"
            "settings": "1/((sqrt(x^2 + y^2) - 2 + 1.25cos(t))^2 + (z - 1.5sin(t))^2) + 1/((sqrt(x^2 + y^2) - 2 - 1.25cos(t))^2 + (z + 1.5sin(t))^2) = 1.9"
            "settings": "1/((sqrt(x^2 + y^2) - 1.5 + sin(t))^2 + (z + cos(t))^2) + 1/((sqrt(x^2 + y^2) - 1.5 + sin(t + 2π/3))^2 + (z + cos(t + 2π/3))^2) + 1/((sqrt(x^2 + y^2) - 1.5 + sin(t + 4π/3))^2 + (z + cos(t + 4π/3))^2) = 5"
            "settings": "1/((sqrt(x^2 + y^2) - 1.6 + sin(t))^2 + (z + cos(t))^2) + 1/((sqrt(x^2 + y^2) - 1.6 + sin(t + π/2))^2 + (z + cos(t + π/2))^2) + 1/((sqrt(x^2 + y^2) - 1.6 + sin(t + π))^2 + (z + cos(t + π))^2) + 1/((sqrt(x^2 + y^2) - 1.6 + sin(t + 3π/2))^2 + (z + cos(t + 3π/2))^2) = 7.3"
            "settings": "sin(sqrt(x^2 + y^2 + z^2) - t) = 0"
            "settings": "x^2 + ((y - (x^2)^(1/3.0)))^2 = 1"
            "settings": "(0.5x)^2 + (abs(0.5x) - 0.5y)^2 = 1"
            "settings": "sqrt((x - sin(z - t))^2 + (y - cos(z - t))^2) = 1"
            "settings": "3cos(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t)/2)/2)/2)/2)/2)/2)/2)/2)/2)/2)/sqrt(x^2 + y^2 + z^2) = z"
            "settings": "x^2 + y^2 = 4"
            "settings": "-(0.5x)^100 + 1 = 0.5abs(y)"
            "settings": "x^2 + y^2 + z^2 = 4"
            "settings": "(0.5x)^2 + (0.5y)^40 + (0.5z)^2 = 1"
            "settings": ".2/((x + 2.5sin(t))^2 + (y + 2.5cos(t))^2 + z^2) + 1/((x - 0.5sin(t))^2 + (y - 0.5cos(t))^2 + z^2) = 1"
            "settings": "1/sqrt((sqrt(x^2 + z^2) - 3)^2 + y^2) - z = 1"
            "settings": "3/sqrt((sqrt(x^2 + (z - 6sin(t))^2) - 3)^2 + y^2) - z = 3"
            "settings": "1/sqrt((sqrt(x^2 + z^2) - 3)^2 + y^2) - z + 1/sqrt((sqrt(y^2 + (z - 3)^2) - 1.5)^2 + x^2) = 1"
            "settings": "1/sqrt((sqrt((x + 4.1 + sin(t)/3)^2 + y^2) - 3)^2 + z^2) + 1/sqrt((sqrt(x^2 + z^2) - 3)^2 + y^2) + 1/sqrt((sqrt((x - 4.1 - sin(t)/3)^2 + y^2) - 3)^2 + z^2) = 3"
            "settings": "(x^4 + y^4 + 2x^2y^2 - 2x^2 - 2y^2 + 1.3*2^y)((x^2 + y^2 - y + .6)^2 - 2x^2)(x^2 + y^2 - 4) = 0"
            "settings": "x^2 + y^2 = z^2"
            "settings": "1/(x^40 + y^40 + z^40) + 1/((x + 4sin(t))^40 + (y + 4cos(t))^40 + z^40) = 1"
            "settings": "1/((abs(0.3x) + 0.25)^20 + (abs(0.3y) + 0.25)^20 + (abs(0.3z) + 0.25)^20) - 1/((x)^2 + (y)^2)^100 = 1"
            "settings": "1/(x^2 + (.75(y + 1.2))^4 + (3z)^40) - .025/((x - .7)^2 + (y + .9)^2) - .025/((x + .7)^2 + (y + .9)^2) - .06/((x - .9)^2 + (y + .85)^2) - .06/((x + .9)^2 + (y + .85)^2) - 1/((4x)^2 + (4(y + .6))^2 + (10(z - .25))^10)^10 - 1/((2x)^2 + (2(y + .6))^2 + (5z)^40)^10 - 1/((1.5x)^2 + (1.4(y + 1.6))^2 + (5z)^40)^10 + 1/((4x)^2 + (y - 1)^40 + (10(z - .255))^2)^10 + 1/((4x/(y - 1))^40 + (3(y - 2.2))^40 + (10(z + .2y - .6))^10)^40 = 1"
            "settings": "1/(80(x + 2)^2 + 80y^2 + z^10)^10 + 1/(80(x - 2)^2 + 80y^2 + z^10)^10 + 1/(8(x + 2)^2 + 8y^2 + 10000000000000000000000000(z - 0.75)^40)^10 + 1/(8(x + 2)^2 + 8y^2 + 10000000000000000000000000(z + 0.75)^40)^10 + 1/(8(x - 2)^2 + 8y^2 + 10000000000000000000000000(z - 0.75)^40)^10 + 1/(8(x - 2)^2 + 8y^2 + 10000000000000000000000000(z + 0.75)^40)^10 + 1/(10(x - 2)^2 + 10(y - 0.25)^2 + 20z^2)^2 + 1/(10(x + 2)^2 + 10(y - 0.25)^2 + 20z^2)^2 + 1/(0.00002x^10 + 10000000000(y - 0.5 - (0.25x)^4)^10 + z^2)^10 = 1"
            "settings": "sqrt(sin(x)^2 + sin(y)^2 + sin(z)^2) - 0.5sin(t) = 1"
            "settings": "sin(x) + sin(y) + sin(z) = 0"
            "settings": "sin(x^2 + y^2) = cos(xy)"
            "settings": "sqrt(x^2 + y^2) = 1/sqrt((sqrt(x^2 + y^2))^2 + z^2)"
            "settings": "abs(y) = 1/sqrt(x^2 + y^2)"
            "settings": "abs(z) = 1/(x^2 + y^2) - 0.12"
            "settings": "x^3 + 5x^2 + xy^2 - 5y^2 = 0"
            "settings": "xyz = 1\"\n" +
            "settings": "x + y + z = xy"
            "settings": "(x/3)^40 + (y/3)^40 + (z/1.5)^40 + (((x - y)/6)^2 + ((y - x)/6)^2)^20 + (((-x - y)/6)^2 + ((x + y)/6)^2)^20 = 0.99"
            "settings": "sqrt(x^2+y^2) = sin(t) + 1"
            "settings": "1/((0.5x)^2 + (0.5y)^2 + (0.5z)^2) - 1/(((0.5x) + sin(t))^2 + (0.5y + cos(t))^2 + (0.5z)^2)^40 = 1"
            "settings": "1/(x^2 + y^2) + 1/((x - sin(z - t))^2 + (y + cos(z - t))^2)^40 = 1"
            "settings": "x^2 + y^2 = 3atan(y/x)^2"
            "settings": "x^2 + ((y^2)sin(y)) + ((z^2)cos(z)) = 1"
            "settings": "(x^2)sin(x)+ ((y^2)cos(y))+ ((z^2)sin(z)) = sin(t)^2"
            "settings": "(x + zsin(z + t))^2 + (y + zcos(z + t))^2 = 1"
            "settings": "1/(x^2 + ((y^2)sin(y)) + z^2) + 0.89/((x - 3sin(t))^2+(y - 3cos(t))^2 + z^40) = 1"*/

            var selectionJSON = boxJSON
            when (mViewModel.getVisualizationType()) {
                0 -> {
                    selectionJSON = nbodyJSON
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