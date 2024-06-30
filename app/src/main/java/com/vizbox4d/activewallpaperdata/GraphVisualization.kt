package com.vizbox4d.activewallpaperdata

import android.graphics.Color
import com.vizbox4d.R
import org.json.JSONObject

data class GraphVisualization (
    val visualizationType: String = "graph",
    var distance: Float = 0.5f,
    var fieldOfView: Float = 90.0f,
    var backgroundColor: Color = createColorFromInts(0, 0, 0, 0),
    var referenceFrameRotates: Boolean = false,
    var backgroundIsSolidColor: Boolean = false,
    var backgroundTexture: String = "rgb_cube",
    var vectorPointsPositive: Boolean = false,
    var equation: String = "1/((sqrt(x^2 + y^2) - 1.5 + sin(t))^2 + (z + cos(t))^2) + 1/((sqrt(x^2 + y^2) - 1.5 + sin(t + 2π/3))^2 + (z + cos(t + 2π/3))^2) + 1/((sqrt(x^2 + y^2) - 1.5 + sin(t + 4π/3))^2 + (z + cos(t + 4π/3))^2) = 5"
) : Visualization() {

    constructor(jsonObject: JSONObject) : this(
        visualizationType = jsonObject.optString("visualization_type", "graph"),
        distance = jsonObject.optDouble("distance", 0.5).toFloat(),
        fieldOfView = jsonObject.optDouble("field_of_view", 90.0).toFloat(),
        backgroundColor = jsonObject.optJSONObject("background_color")?.let {
            Companion.jsonObjectToColor(
                it
            )
        }?: createColorFromInts(0, 0, 0, 0),
        referenceFrameRotates = jsonObject.optBoolean("reference_frame_rotates", false),
        backgroundIsSolidColor = jsonObject.optBoolean("background_is_solid_color", false),
        backgroundTexture = jsonObject.optString("background_texture", "rgb_cube"),
        vectorPointsPositive = jsonObject.optBoolean("vector_points_positive", false),
        equation = jsonObject.optString("equation", "1/((sqrt(x^2 + y^2) - 1.5 + sin(t))^2 + (z + cos(t))^2) + 1/((sqrt(x^2 + y^2) - 1.5 + sin(t + 2π/3))^2 + (z + cos(t + 2π/3))^2) + 1/((sqrt(x^2 + y^2) - 1.5 + sin(t + 4π/3))^2 + (z + cos(t + 4π/3))^2) = 5")
    )

    constructor(repo: ActiveWallpaperRepo) : this(
        visualizationType = "graph",
        distance = repo.distanceFromOrigin.value!!,
        fieldOfView = repo.fieldOfView.value!!,
        backgroundColor = repo.color.value!!,
        referenceFrameRotates = repo.referenceFrameRotates.value!!,
        backgroundIsSolidColor = repo.backgroundIsSolidColor.value!!,
        backgroundTexture = repo.backgroundTexture.value!!,
        vectorPointsPositive = repo.flipNormals.value!!,
        equation = repo.currentEquation.value
    )

    override val relevantTextViewIds: List<Int> = listOf(
        R.id.distance_label,
        R.id.field_of_view_label,
        R.id.equation_selection_label,
        R.id.tv_equation_name,
        R.id.tv_equation_value,
        R.id.tv_syntax_check
    )
    override val relevantSeekBarIds: List<Int> = listOf(
        R.id.distance_seekbar,
        R.id.field_of_view_seekbar
    )
    override val relevantEditTextIds: List<Int> = listOf(
        R.id.et_equation_name,
        R.id.et_equation_value
    )
    override val relevantCheckBoxIds: List<Int> = listOf(
        R.id.flip_normals_checkbox
    )
    override val relevantSpinnerIds: List<Int> = listOf(
        R.id.default_graph_selection_spinner,
        R.id.saved_graph_selection_spinner,
        R.id.image_selection_spinner
    )
    override val relevantButtonIds: List<Int> = listOf(
        R.id.new_button,
        R.id.copy_button,
        R.id.delete_button
    )
    override val relevantRadioButtonIds: List<Int> = listOf(
        R.id.solid_color_radio_button,
        R.id.image_radio_button,
        R.id.default_equations_radio_button,
        R.id.saved_equations_radio_button
    )
    override val relevantRadioGroupIds: List<Int> = listOf(
        R.id.background_radio_group,
        R.id.equation_radio_group
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
}