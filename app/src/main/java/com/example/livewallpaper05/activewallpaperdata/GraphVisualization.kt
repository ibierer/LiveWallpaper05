package com.example.livewallpaper05.activewallpaperdata

import android.graphics.Color
import com.example.livewallpaper05.R
import org.json.JSONObject

data class GraphVisualization (
    val visualizationType: String = "graph",
    var distance: Float = 0.5f,
    var fieldOfView: Float = 60.0f,
    var backgroundColor: Color = createColorFromInts(0, 0, 0, 0),
    var referenceFrameRotates: Boolean = false,
    var backgroundIsSolidColor: Boolean = false,
    var backgroundTexture: String = "ms_paint_colors",
    var vectorPointsPositive: Boolean = false,
    var equation: String = "1/((sqrt(x^2 + y^2) - 1.5 + sin(t))^2 + (z + cos(t))^2) + 1/((sqrt(x^2 + y^2) - 1.5 + sin(t + 2π/3))^2 + (z + cos(t + 2π/3))^2) + 1/((sqrt(x^2 + y^2) - 1.5 + sin(t + 4π/3))^2 + (z + cos(t + 4π/3))^2) = 5"
) : Visualization() {

    constructor(jsonObject: JSONObject) : this(
        visualizationType = jsonObject.optString("visualization_type", "graph"),
        distance = jsonObject.optDouble("distance", 0.5).toFloat(),
        fieldOfView = jsonObject.optDouble("field_of_view", 60.0).toFloat(),
        backgroundColor = jsonObject.optJSONObject("background_color")?.let { Visualization.jsonObjectToColor(it) }?: createColorFromInts(0, 0, 0, 0),
        referenceFrameRotates = jsonObject.optBoolean("reference_frame_rotates", false),
        backgroundIsSolidColor = jsonObject.optBoolean("background_is_solid_color", false),
        backgroundTexture = jsonObject.optString("background_texture", "ms_paint_colors"),
        vectorPointsPositive = jsonObject.optBoolean("vector_points_positive", false),
        equation = jsonObject.optString("equation", "1/((sqrt(x^2 + y^2) - 1.5 + sin(t))^2 + (z + cos(t))^2) + 1/((sqrt(x^2 + y^2) - 1.5 + sin(t + 2π/3))^2 + (z + cos(t + 2π/3))^2) + 1/((sqrt(x^2 + y^2) - 1.5 + sin(t + 4π/3))^2 + (z + cos(t + 4π/3))^2) = 5")
    )

    override val relevantTextViewIds : List<Int> = listOf(
        R.id.distance_label,
        R.id.field_of_view_label,
        R.id.tv_equation,
        R.id.tv_syntax_check
    )
    override val relevantSeekBarIds : List<Int> = listOf(
        R.id.distance_seekbar,
        R.id.field_of_view_seekbar
    )
    override val relevantEditTextIds: List<Int> = listOf(
        R.id.et_equation
    )
    override val relevantCheckBoxIds: List<Int> = listOf(
        R.id.flip_normals_checkbox
    )
    override val relevantButtonIds: List<Int> = listOf(
        R.id.done_button
    )

    override fun toJsonObject() : JSONObject {
        val jsonObject = JSONObject()
        jsonObject.put("visualization_type", visualizationType)
        jsonObject.put("distance", distance)
        jsonObject.put("field_of_view", fieldOfView)
        jsonObject.put("background_color", colorToJSONObject(backgroundColor))
        jsonObject.put("reference_frame_rotates", referenceFrameRotates)
        jsonObject.put("background_is_solid_color", backgroundIsSolidColor)
        jsonObject.put("background_texture", backgroundTexture)
        jsonObject.put("vector_points_positive", vectorPointsPositive)
        jsonObject.put("equation", equation)
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
        equation = viewModel!!.getEquation()
    }

}