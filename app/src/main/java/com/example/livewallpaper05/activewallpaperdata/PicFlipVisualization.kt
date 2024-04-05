package com.example.livewallpaper05.activewallpaperdata

import android.graphics.Color
import com.example.livewallpaper05.R
import org.json.JSONObject

data class PicFlipVisualization (
    val visualizationType: String = "simulation",
    val simulationType: String = "picflip",
    var distance: Float = 0.5f,
    var fieldOfView: Float = 60.0f,
    var backgroundColor: Color = createColorFromInts(0, 0, 0, 0),
    var gravity: Float = 0.0f,
    var linearAcceleration: Float = 1.0f,
    var referenceFrameRotates: Boolean = true,
    var backgroundIsSolidColor: Boolean = false,
    var backgroundTexture: String = "ms_paint_colors"
) : Visualization() {
    constructor(jsonObject: JSONObject) : this(
        visualizationType = jsonObject.optString("visualization_type", "simulation"),
        simulationType = jsonObject.optString("simulation_type", "picflip"),
        distance = jsonObject.optDouble("distance", 0.5).toFloat(),
        fieldOfView = jsonObject.optDouble("field_of_view", 60.0).toFloat(),
        gravity = jsonObject.optDouble("gravity", 0.0).toFloat(),
        linearAcceleration = jsonObject.optDouble("linear_acceleration", 1.0).toFloat(),
        backgroundColor = jsonObject.optJSONObject("background_color")?.let { Visualization.jsonObjectToColor(it) }?: createColorFromInts(0, 0, 0, 0),
        referenceFrameRotates = jsonObject.optBoolean("reference_frame_rotates", true),
        backgroundIsSolidColor = jsonObject.optBoolean("background_is_solid_color", false),
        backgroundTexture = jsonObject.optString("background_texture", "ms_paint_colors")
    )

    override val relevantTextViewIds : List<Int> = listOf(
        R.id.distance_label,
        R.id.field_of_view_label,
        R.id.gravity_label,
        R.id.linear_acceleration_label
    )
    override val relevantSeekBarIds : List<Int> = listOf(
        R.id.distance_seekbar,
        R.id.field_of_view_seekbar,
        R.id.gravity_seekbar,
        R.id.linear_acceleration_seekbar
    )
    override val relevantEditTextIds: List<Int> = listOf(

    )
    override val relevantCheckBoxIds: List<Int> = listOf(

    )

    override fun toJsonObject() : JSONObject {
        val jsonObject = JSONObject()
        jsonObject.put("visualization_type", visualizationType)
        jsonObject.put("simulation_type", simulationType)
        jsonObject.put("distance", distance)
        jsonObject.put("field_of_view", fieldOfView)
        jsonObject.put("gravity", gravity)
        jsonObject.put("linear_acceleration", linearAcceleration)
        jsonObject.put("background_color", colorToJSONObject(backgroundColor))
        jsonObject.put("reference_frame_rotates", referenceFrameRotates)
        jsonObject.put("background_is_solid_color", backgroundIsSolidColor)
        jsonObject.put("background_texture", backgroundTexture)
        return jsonObject
    }

    override fun updateValues() {
        if (viewModel == null) {
            return
        }
        // get values from repo
        distance = viewModel!!.getDistanceFromOrigin()
        fieldOfView = viewModel!!.getFieldOfView()
        backgroundColor = viewModel!!.getColor()
        gravity = viewModel!!.getGravity()
        linearAcceleration = viewModel!!.getLinearAcceleration()

    }

}