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
                val blob = outputStream.toByteArray()

                mViewModel.repo.liveDataBitmap.postValue(scaledSquareBitmap)

                if (mViewModel.repo.saveAsNew > 0) {
                    mViewModel.repo.saveAsNew = 0
                    /* insert wallpaper to DB (JSON contents, blob image, uid */
                    auth = FirebaseAuth.getInstance()
                    if (auth!!.currentUser != null) {
                        val sharedPreferences =
                            context.getSharedPreferences("MyPreferences", Context.MODE_PRIVATE)
                        username = sharedPreferences.getString("USERNAME", "").toString()
                        uid = sharedPreferences.getInt("UID", 0)
                        val visualizationDetails = mViewModel.repo.visualization!!.toJsonObject()
                        GlobalScope.launch {
                            insertWallpaper(visualizationDetails.toString(), blob, username!!)
                        }
                    } else {
                        username = "Default User"
                        uid = 11
                        val visualizationDetails = mViewModel.repo.visualization!!.toJsonObject()
                        GlobalScope.launch {
                            insertWallpaper(visualizationDetails.toString(), blob, username!!)
                        }
                    }
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
            mViewModel.repo.width = width
            mViewModel.repo.height = height
            // register sensor event listeners
            mViewModel.registerSensorEvents(context.getSystemService(Context.SENSOR_SERVICE) as SensorManager)
        }

        override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {

            var jsonConfig: JSONObject = JSONObject()
            var selectionJSON: String = ""

            val loadedConfig = JSONObject(mViewModel.getConfig())

            when (mViewModel.getVisualization()) {
                0 -> {
                    mViewModel.repo.visualization = NBodyVisualization(loadedConfig)
                    mViewModel.repo.visualization.viewModel = mViewModel
                    selectionJSON = mViewModel.repo.visualization.toJsonObject().toString()
                    jsonConfig = JSONObject(selectionJSON)

                }
                1 -> {
                    mViewModel.repo.visualization = NaiveFluidVisualization(loadedConfig)
                    jsonConfig = mViewModel.repo.visualization.toJsonObject()
                    val gravity: Float = jsonConfig.getDouble("gravity").toFloat()
                    val linearAcceleration: Float = jsonConfig.getDouble("linear_acceleration").toFloat()
                    val efficiency: Float = jsonConfig.getDouble("efficiency").toFloat()
                    mViewModel.repo.gravity.postValue(gravity)
                    mViewModel.repo.linearAcceleration.postValue(linearAcceleration)
                    mViewModel.repo.efficiency.postValue(efficiency)

                }
                2 -> {
                    mViewModel.repo.visualization = PicFlipVisualization(loadedConfig)
                    jsonConfig = mViewModel.repo.visualization.toJsonObject()
                    val gravity: Float = jsonConfig.getDouble("gravity").toFloat()
                    val linearAcceleration: Float = jsonConfig.getDouble("linear_acceleration").toFloat()
                    mViewModel.repo.gravity.postValue(gravity)
                    mViewModel.repo.linearAcceleration.postValue(linearAcceleration)

                }
                3 -> {
                    mViewModel.repo.visualization = TriangleVisualization(loadedConfig)
                    selectionJSON = mViewModel.repo.visualization.toJsonObject().toString()
                    jsonConfig = JSONObject(selectionJSON)

                }
                4 -> {
                    mViewModel.repo.visualization = GraphVisualization(loadedConfig)
                    selectionJSON = mViewModel.repo.visualization.toJsonObject().toString()
                    jsonConfig = JSONObject(selectionJSON)

                }
            }
            //Log.d("VISUALIZATION = ", jsonConfig.toString())
            val distance: Float = jsonConfig.getDouble("distance").toFloat()
            val fieldOfView: Float = jsonConfig.getDouble("field_of_view").toFloat()
            mViewModel.repo.distanceFromOrigin.postValue(distance)
            mViewModel.repo.fieldOfView.postValue(fieldOfView)
            val backgroundColorJSONObject: JSONObject = jsonConfig.getJSONObject("background_color")
            val backgroundColor: Color = mViewModel.repo.visualization.jsonObjectToColor(backgroundColorJSONObject)
            //mViewModel.repo.color.postValue(backgroundColor)
            mViewModel.updateColor(backgroundColor, true)
            PreviewActivity.init(jsonConfig.toString())
        }

        private suspend fun insertWallpaper(contents: String, image: ByteArray, username: String) {
            withContext(Dispatchers.IO) {
                val jdbcConnectionString = ProfileActivity.DatabaseConfig.jdbcConnectionString
                Log.d("SQL", "in Insert")
                try {
                    Class.forName("com.mysql.jdbc.Driver").newInstance()
                    val connectionProperties = Properties()
                    connectionProperties["user"] = ProfileActivity.DatabaseConfig.dbUser
                    connectionProperties["password"] = ProfileActivity.DatabaseConfig.dbPassword
                    connectionProperties["useSSL"] = "false"
                    DriverManager.getConnection(jdbcConnectionString, connectionProperties)
                        .use { conn ->
                            Log.d("SQL", "Connection made")
                            val insertQuery =
                                "INSERT INTO wallpapers (contents, image, username) VALUES (?, ?, ?);"
                            conn.prepareStatement(insertQuery).use { preparedStatement ->
                                preparedStatement.setString(1, contents)
                                preparedStatement.setBytes(2, image)
                                preparedStatement.setString(3, username)
                                preparedStatement.executeUpdate()
                            }
                        }
                } catch (e: SQLException) {
                    Log.e("SQL_ERROR", "Error inserting wallpaper: ${e.message}", e)
                } catch (e: Exception) {
                    Log.e("SQL_ERROR", "Unexpected error: ${e.message}", e)
                }
            }
        }
    }
}