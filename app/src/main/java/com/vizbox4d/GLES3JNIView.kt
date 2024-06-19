package com.vizbox4d

import android.content.Context
import android.opengl.GLSurfaceView
import android.os.Build
import android.os.SystemClock
import androidx.annotation.RequiresApi
import com.vizbox4d.activewallpaperdata.ActiveWallpaperViewModel
import org.json.JSONObject
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class GLES3JNIView(context: Context, vm: ActiveWallpaperViewModel) : GLSurfaceView(context) {

    init {
        // Pick an EGLConfig with RGB8 color, 16-bit depth, no stencil,
        // supporting OpenGL ES 2.0 or later backwards-compatible versions.
        setEGLConfigChooser(8, 8, 8, 0, 16, 0)
        setEGLContextClientVersion(3)
        setRenderer(Renderer(context, vm, 0))
    }
}

class Renderer(private val context: Context, private var mViewModel: ActiveWallpaperViewModel, private val mode: Int) : GLSurfaceView.Renderer {

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
            mViewModel.getVectorDirection(),
            mViewModel.getOrientation(),
            mode
        )

        // get time after frame
        val currentFrame: Long = SystemClock.elapsedRealtimeNanos()
        // calculate time elapsed
        var timeElapsed: Float = (currentFrame - mViewModel.getLastFrame()) / 1000000000.0f
        timeElapsed = 1.0f / timeElapsed
        // update fps in view model with fps from spf
        mViewModel.updateFPS(timeElapsed)
        // update last frame in view model
        mViewModel.updateLastFrame(currentFrame)
    }

    @RequiresApi(Build.VERSION_CODES.R)
    override fun onSurfaceChanged(gl: GL10, width: Int, height: Int) {
        if(mode == 0){
            mViewModel.updateOrientation(context.display!!.rotation)
        }
        PreviewActivity.resize(width, height, mode)
    }

    override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
        val jsonConfig: JSONObject = mViewModel.repo.visualization.toJsonObject()
        when (mViewModel.repo.getVisualizationSelection()) {
            0 -> {
                // Do nothing
            }
            1 -> {
                mViewModel.repo.gravity.postValue(jsonConfig.getDouble("gravity").toFloat())
                mViewModel.repo.linearAcceleration.postValue(jsonConfig.getDouble("linear_acceleration").toFloat())
                mViewModel.repo.efficiency.postValue(jsonConfig.getDouble("efficiency").toFloat())
                mViewModel.repo.referenceFrameRotates.postValue(jsonConfig.getBoolean("reference_frame_rotates"))
            }
            2 -> {
                mViewModel.repo.gravity.postValue(jsonConfig.getDouble("gravity").toFloat())
                mViewModel.repo.linearAcceleration.postValue(jsonConfig.getDouble("linear_acceleration").toFloat())
                mViewModel.repo.referenceFrameRotates.postValue(jsonConfig.getBoolean("reference_frame_rotates"))
            }
            3 -> {
                // Do nothing
            }
            4 -> {
                jsonConfig.put("equation", mViewModel.repo.currentEquation.value)
                mViewModel.repo.referenceFrameRotates.postValue(jsonConfig.getBoolean("reference_frame_rotates"))
                mViewModel.repo.flipNormals.postValue(jsonConfig.getBoolean("vector_points_positive"))
            }
        }
        mViewModel.repo.distanceFromOrigin.postValue(jsonConfig.getDouble("distance").toFloat())
        mViewModel.repo.fieldOfView.postValue(jsonConfig.getDouble("field_of_view").toFloat())
        mViewModel.repo.backgroundIsSolidColor.postValue(jsonConfig.getBoolean("background_is_solid_color"))
        mViewModel.repo.backgroundTexture.postValue(jsonConfig.getString("background_texture"))
        mViewModel.repo.color.postValue(mViewModel.repo.visualization.jsonObjectToColor(jsonConfig.getJSONObject("background_color")))
        //Log.d("GLES3JNIView", jsonConfig.toString())
        PreviewActivity.init(jsonConfig.toString(), mode)
    }
}