package com.vizbox4d.activewallpaperdata

import android.graphics.Color
import com.vizbox4d.R
import kotlinx.coroutines.Job
import org.json.JSONObject

data class NBodyVisualization (
    private var job: Job? = null,
    val visualizationType: String = "simulation",
    val simulationType: String = "nbody",
    var distance: Float = 0.5f,
    var fieldOfView: Float = 60.0f,
    var backgroundColor: Color = createColorFromInts(0, 0, 0, 0),
    var backgroundIsSolidColor: Boolean = true,
    var backgroundTexture: String = "ms_paint_colors"
) : Visualization() {

    constructor(jsonObject: JSONObject) : this(
        visualizationType = jsonObject.optString("visualization_type", "simulation"),
        simulationType = jsonObject.optString("simulation_type", "nbody"),
        distance = jsonObject.optDouble("distance", 0.5).toFloat(),
        fieldOfView = jsonObject.optDouble("field_of_view", 60.0).toFloat(),
        backgroundColor = jsonObject.optJSONObject("background_color")?.let {
            Companion.jsonObjectToColor(
                it
            )
        }?: createColorFromInts(0, 0, 0, 0),
        backgroundIsSolidColor = jsonObject.optBoolean("background_is_solid_color", true),
        backgroundTexture = jsonObject.optString("background_texture", "ms_paint_colors")
    )

    constructor(repo: ActiveWallpaperRepo) : this(
        visualizationType = "simulation",
        simulationType = "nbody",
        distance = repo.distanceFromOrigin.value!!,
        fieldOfView = repo.fieldOfView.value!!,
        backgroundColor = repo.color.value!!,
        backgroundIsSolidColor = repo.backgroundIsSolidColor.value!!,
        backgroundTexture = repo.backgroundTexture.value!!
    )

    override val relevantTextViewIds : List<Int> = listOf(
        R.id.distance_label,
        R.id.field_of_view_label
    )

    override val relevantSeekBarIds : List<Int> = listOf(
        R.id.distance_seekbar,
        R.id.field_of_view_seekbar
    )
    override val relevantEditTextIds: List<Int> = listOf(

    )
    override val relevantCheckBoxIds: List<Int> = listOf(

    )
    override val relevantSpinnerIds: List<Int> = listOf(

    )
    override val relevantButtonIds: List<Int> = listOf(

    )
    override val relevantRadioButtonIds: List<Int> = listOf(

    )
    override val relevantRadioGroupIds: List<Int> = listOf(

    )

    override fun toJsonObject() : JSONObject {
        val jsonObject = JSONObject()
        jsonObject.put("visualization_type", visualizationType)
        jsonObject.put("simulation_type", simulationType)
        jsonObject.put("distance", distance)
        jsonObject.put("field_of_view", fieldOfView)
        jsonObject.put("background_color", colorToJSONObject(backgroundColor))
        jsonObject.put("background_is_solid_color", backgroundIsSolidColor)
        jsonObject.put("background_texture", backgroundTexture)
        return jsonObject
    }
}