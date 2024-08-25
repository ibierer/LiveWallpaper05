package com.xendovo.activewallpaperdata

import android.graphics.Color
import com.xendovo.R
import org.json.JSONObject

data class PicFlipVisualization (
    val visualizationType: String = "simulation",
    val simulationType: String = "picflip",
    var distance: Float = 0.5f,
    var fieldOfView: Float = 60.0f,
    var backgroundColor: Color = createColorFromInts(0, 0, 0, 0),
    var gravity: Float = 2.0f,
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
        gravity = jsonObject.optDouble("gravity", 2.0).toFloat(),
        linearAcceleration = jsonObject.optDouble("linear_acceleration", 1.0).toFloat(),
        backgroundColor = jsonObject.optJSONObject("background_color")?.let {
            Companion.jsonObjectToColor(
                it
            )
        }?: createColorFromInts(0, 0, 0, 0),
        referenceFrameRotates = jsonObject.optBoolean("reference_frame_rotates", true),
        backgroundIsSolidColor = jsonObject.optBoolean("background_is_solid_color", false),
        backgroundTexture = jsonObject.optString("background_texture", "ms_paint_colors")
    )

    constructor(repo: ActiveWallpaperRepo) : this(
        visualizationType = "simulation",
        simulationType = "picflip",
        distance = repo.distanceFromOrigin.value!!,
        fieldOfView = repo.fieldOfView.value!!,
        gravity = repo.gravity.value!!,
        linearAcceleration = repo.linearAcceleration.value!!,
        backgroundColor = repo.color.value!!,
        referenceFrameRotates = repo.referenceFrameRotates.value!!,
        backgroundIsSolidColor = repo.backgroundIsSolidColor.value!!,
        backgroundTexture = repo.backgroundTexture.value!!
    )

    override val relevantTextViewIds: List<Int> = listOf(
        R.id.distance_label,
        R.id.field_of_view_label,
        R.id.gravity_label,
        R.id.linear_acceleration_label
    )
    override val relevantSeekBarIds: List<Int> = listOf(
        R.id.distance_seekbar,
        R.id.field_of_view_seekbar,
        R.id.gravity_seekbar,
        R.id.linear_acceleration_seekbar
    )
    override val relevantEditTextIds: List<Int> = listOf(

    )
    override val relevantCheckBoxIds: List<Int> = listOf(
        R.id.gyroscope_compensation_checkbox
    )
    override val relevantSpinnerIds: List<Int> = listOf(
        R.id.image_selection_spinner
    )
    override val relevantButtonIds: List<Int> = listOf(

    )
    override val relevantRadioButtonIds: List<Int> = listOf(
        R.id.solid_color_radio_button,
        R.id.image_radio_button
    )
    override val relevantRadioGroupIds: List<Int> = listOf(
        R.id.background_radio_group
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
}