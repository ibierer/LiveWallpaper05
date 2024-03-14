package com.example.livewallpaper05

import android.content.Context
import android.graphics.Bitmap
import android.graphics.Color
import android.opengl.GLSurfaceView
import android.os.SystemClock
import android.util.Log
import android.view.View
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperViewModel
import com.google.firebase.auth.FirebaseAuth
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import org.json.JSONObject
import java.io.ByteArrayOutputStream
import java.nio.ByteBuffer
import java.sql.DriverManager
import java.sql.SQLException
import java.util.Properties
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

    class Renderer(private val context: Context, vm: ActiveWallpaperViewModel, var view: View) :
        GLSurfaceView.Renderer {
        private var mViewModel: ActiveWallpaperViewModel = vm
        var auth: FirebaseAuth? = null
        var username: String? = "Default User"
        var uid: Int? = 11 //uid for default user


        override fun onDrawFrame(gl: GL10) {
            // default values in ActiveWallpaperViewModel
            val accelData = mViewModel.getAccelerationData()
            val rotData = mViewModel.getRotationData()
            val linearAccelData = mViewModel.getLinearAccelerationData()
            val multiplier = mViewModel.getLinearAcceleration()

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
                mViewModel.getEfficiency()
            )

            // Get screen buffer if requested
            if (mViewModel.getScreenBuffer > 0) {
                mViewModel.getScreenBuffer = 0
                val byteArray: ByteArray = PreviewActivity.getScreenBuffer()
                // Construct Bitmap from ByteArray
                val bitmap = Bitmap.createBitmap(
                    mViewModel.width,
                    mViewModel.height,
                    Bitmap.Config.ARGB_8888
                )
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

                //mViewModel.liveDataBitmap.postValue(Bitmap.createScaledBitmap(bitmap, bitmap.width / 4, bitmap.height / 4, true))

                val width = bitmap.width
                val height = bitmap.height
                val size = minOf(width, height)
                val startX = (width - size) / 2
                val startY = (height - size) / 2

                val squareBitmap = Bitmap.createBitmap(bitmap, startX, startY, size, size)

                val scaledSquareBitmap = Bitmap.createScaledBitmap(squareBitmap, 512, 512, true)
                val outputStream = ByteArrayOutputStream()

                scaledSquareBitmap.compress(Bitmap.CompressFormat.PNG, 100, outputStream)
                val blob = outputStream.toByteArray()

                mViewModel.liveDataBitmap.postValue(scaledSquareBitmap)

                if (mViewModel.saveAsNew > 0) {
                    mViewModel.saveAsNew = 0
                    /* insert wallpaper to DB (JSON contents, blob image, uid */
                    auth = FirebaseAuth.getInstance()
                    if (auth!!.currentUser != null) {
                        val sharedPreferences =
                            context.getSharedPreferences("MyPreferences", Context.MODE_PRIVATE)
                        username = sharedPreferences.getString("USERNAME", "").toString()
                        uid = sharedPreferences.getInt("UID", 0)
                        val color = mViewModel.getColor()
                        val eq = mViewModel.getEquation()
                        val selectionJSON = determineSelectionJSON(color, eq)
                        /*GlobalScope.launch {
                            insertWallpaper(selectionJSON, blob, username!!)
                        }*/
                    }
                } else { // SaveAsNew not selected

                }
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
            mViewModel.width = width
            mViewModel.height = height
        }

        override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
            // get color rgba values from view model
            val color = mViewModel.getColor()
            val r = (color.red() * 255).toInt()
            val g = (color.green() * 255).toInt()
            val b = (color.blue() * 255).toInt()
            val a = (color.alpha() * 255).toInt()
            val eq = mViewModel.getEquation()

            val nbodyJSON = """{
                    "visualization_type": "simulation",
                    "simulation_type": "nbody",
                    "distance": 0.5,
                    "field_of_view": 60.0,
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
                    "distance": 0.5,
                    "field_of_view": 60.0,
                    "gravity": 0.0,
                    "linear_acceleration": 1.0,
                    "efficiency": 1.0,
                    "reference_frame_rotates": "false",
                    "background_is_solid_color": "false",
                    "background_texture": "ms_paint_colors",
                    "background_color": {"r": $r, "g": $g, "b": $b, "a": $a}
                }""".trimIndent()

            val picflipJSON = """{
                    "visualization_type": "simulation",
                    "simulation_type": "picflip",
                    "distance": 0.5,
                    "field_of_view": 60.0,
                    "gravity": 0.0,
                    "linear_acceleration": 1.0,
                    "reference_frame_rotates": "true",
                    "background_is_solid_color": "false",
                    "background_texture": "ms_paint_colors",
                    "background_color": {"r": $r, "g": $g, "b": $b, "a": $a}
                }""".trimIndent()

            val triangleJSON = """{
                    "visualization_type": "other",
                    "distance": 0.5,
                    "field_of_view": 60.0,
                    "background_is_solid_color": "false",
                    "background_texture": "mandelbrot",
                    "background_color": {"r": $r, "g": $g, "b": $b, "a": $a}
                }""".trimIndent()

            val graphJSON = """{
                    "visualization_type": "graph",
                    "reference_frame_rotates": "false",
                    "distance": 0.5,
                    "field_of_view": 60.0,
                    "background_is_solid_color": "false",
                    "background_texture": "ms_paint_colors",
                    "vector_points_positive": "false",
                    "background_color": {"r": $r, "g": $g, "b": $b, "a": $a},
                    "equation": "$eq"
                }""".trimIndent()

            lateinit var selectionJSON: String
            when (mViewModel.getVisualizationType()) {
                0 -> {
                    selectionJSON = nbodyJSON
                    //val gravity: Float = jsonObject.getDouble("gravity").toFloat()
                    //mViewModel.mRepo.gravity.postValue(gravity)
                }

                1 -> {
                    selectionJSON = naiveJSON
                    val jsonObject: JSONObject = JSONObject(selectionJSON)
                    val gravity: Float = jsonObject.getDouble("gravity").toFloat()
                    val linearAcceleration: Float =
                        jsonObject.getDouble("linear_acceleration").toFloat()
                    val efficiency: Float = jsonObject.getDouble("efficiency").toFloat()
                    mViewModel.mRepo.gravity.postValue(gravity)
                    mViewModel.mRepo.linearAcceleration.postValue(linearAcceleration)
                    mViewModel.mRepo.efficiency.postValue(efficiency)
                }

                2 -> {
                    selectionJSON = picflipJSON
                    val jsonObject: JSONObject = JSONObject(selectionJSON)
                    val gravity: Float = jsonObject.getDouble("gravity").toFloat()
                    val linearAcceleration: Float =
                        jsonObject.getDouble("linear_acceleration").toFloat()
                    mViewModel.mRepo.gravity.postValue(gravity)
                    mViewModel.mRepo.linearAcceleration.postValue(linearAcceleration)
                }

                3 -> {
                    selectionJSON = triangleJSON
                }

                4 -> {
                    selectionJSON = graphJSON
                }
            }
            val jsonObject: JSONObject = JSONObject(selectionJSON)
            val distance: Float = jsonObject.getDouble("distance").toFloat()
            val fieldOfView: Float = jsonObject.getDouble("field_of_view").toFloat()
            mViewModel.mRepo.distanceFromOrigin.postValue(distance)
            mViewModel.mRepo.fieldOfView.postValue(fieldOfView)
            PreviewActivity.init(selectionJSON)
        }

        fun determineSelectionJSON(color: Color, eq: String): String {
            val r = (color.red() * 255).toInt()
            val g = (color.green() * 255).toInt()
            val b = (color.blue() * 255).toInt()
            val a = (color.alpha() * 255).toInt()

            val nbodyJSON = """{
            "visualization_type": "simulation",
            "simulation_type": "nbody",
            "distance": 0.5,
            "field_of_view": 60.0,
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
            "distance": 0.5,
            "field_of_view": 60.0,
            "gravity": 0.0,
            "linear_acceleration": 1.0,
            "reference_frame_rotates": "false",
            "background_is_solid_color": "false",
            "background_texture": "ms_paint_colors",
            "background_color": {"r": $r, "g": $g, "b": $b, "a": $a}
        }""".trimIndent()

            val picflipJSON = """{
            "visualization_type": "simulation",
            "simulation_type": "picflip",
            "distance": 0.5,
            "field_of_view": 60.0,
            "gravity": 0.0,
            "linear_acceleration": 1.0,
            "reference_frame_rotates": "true",
            "background_is_solid_color": "false",
            "background_texture": "ms_paint_colors",
            "background_color": {"r": $r, "g": $g, "b": $b, "a": $a}
        }""".trimIndent()

            val triangleJSON = """{
            "visualization_type": "other",
            "distance": 0.5,
            "field_of_view": 60.0,
            "background_is_solid_color": "false",
            "background_texture": "mandelbrot",
            "background_color": {"r": $r, "g": $g, "b": $b, "a": $a}
        }""".trimIndent()

            val graphJSON = """{
            "visualization_type": "graph",
            "reference_frame_rotates": "false",
            "distance": 0.5,
            "field_of_view": 60.0,
            "background_is_solid_color": "false",
            "background_texture": "ms_paint_colors",
            "vector_points_positive": "false",
            "background_color": {"r": $r, "g": $g, "b": $b, "a": $a},
            "equation": "$eq"
        }""".trimIndent()

            lateinit var selectionJSON: String
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
            return selectionJSON
        }

        suspend fun insertWallpaper(contents: String, image: ByteArray, username: String) {
            withContext(Dispatchers.IO) {
                Log.d("CAMERON", "IN Coroutine")
                val jdbcConnectionString = ProfileActivity.DatabaseConfig.jdbcConnectionString
                try {
                    Log.d("CAMERON", "IN Try")
                    Class.forName("com.mysql.jdbc.Driver").newInstance()
                    val connectionProperties = Properties()
                    connectionProperties["user"] = ProfileActivity.DatabaseConfig.dbUser
                    connectionProperties["password"] = ProfileActivity.DatabaseConfig.dbPassword
                    connectionProperties["useSSL"] = "true"

                    DriverManager.getConnection(jdbcConnectionString, connectionProperties)
                        .use { conn ->
                            Log.d("CAMERON", "Connected to database")
                            val useDbQuery = "USE myDatabase;"
                            conn.prepareStatement(useDbQuery).use { statement ->
                                statement.execute()
                                val insertQuery =
                                    "INSERT INTO wallpapers (contents, image, username) VALUES (?, ?, ?);"
                                conn.prepareStatement(insertQuery).use { preparedStatement ->
                                    preparedStatement.setString(1, contents)
                                    preparedStatement.setBytes(2, image)
                                    preparedStatement.setString(3, username)
                                    preparedStatement.executeUpdate()
                                }
                            }
                        }
                } catch (e: SQLException) {
                    Log.e("OH_NO", "Error inserting wallpaper: ${e.message}", e)
                } catch (e: Exception) {
                    Log.e("OH_NO", "Unexpected error: ${e.message}", e)
                }
            }
        }
    }
}