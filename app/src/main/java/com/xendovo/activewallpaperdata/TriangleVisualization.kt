package com.xendovo.activewallpaperdata

import android.graphics.Color
import com.xendovo.R
import org.json.JSONObject

data class TriangleVisualization (
    val visualizationType: String = "other",
    var distance: Float = 0.08f,
    var fieldOfView: Float = 120.0f,
    var backgroundColor: Color = createColorFromInts(0, 0, 0, 0),
    var backgroundIsSolidColor: Boolean = false,
    var backgroundTexture: String = "mandelbrot"
) : Visualization() {

    constructor(jsonObject: JSONObject) : this(
        visualizationType = jsonObject.optString("visualization_type", "other"),
        distance = jsonObject.optDouble("distance", 0.08).toFloat(),
        fieldOfView = jsonObject.optDouble("field_of_view", 120.0).toFloat(),
        backgroundColor = jsonObject.optJSONObject("background_color")?.let {
            Companion.jsonObjectToColor(
                it
            )
        }?: createColorFromInts(0, 0, 0, 0),
        backgroundIsSolidColor = jsonObject.optBoolean("background_is_solid_color", false),
        backgroundTexture = jsonObject.optString("background_texture", "mandelbrot")
    )

    constructor(repo: ActiveWallpaperRepo) : this(
        visualizationType = "other",
        distance = repo.distanceFromOrigin.value!!,
        fieldOfView = repo.fieldOfView.value!!,
        backgroundColor = repo.color.value!!,
        backgroundIsSolidColor = repo.backgroundIsSolidColor.value!!,
        backgroundTexture = repo.backgroundTexture.value!!
    )

    override val relevantTextViewIds: List<Int> = listOf(
        R.id.distance_label,
        R.id.field_of_view_label
    )
    override val relevantSeekBarIds: List<Int> = listOf(
        R.id.distance_seekbar,
        R.id.field_of_view_seekbar
    )
    override val relevantEditTextIds: List<Int> = listOf(

    )
    override val relevantCheckBoxIds: List<Int> = listOf(

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
        jsonObject.put("distance", distance)
        jsonObject.put("field_of_view", fieldOfView)
        jsonObject.put("background_color", colorToJSONObject(backgroundColor))
        jsonObject.put("background_is_solid_color", backgroundIsSolidColor)
        jsonObject.put("background_texture", backgroundTexture)
        return jsonObject
    }
}