package com.xendovo.activewallpaperdata

import android.graphics.Color
import com.xendovo.R
import org.json.JSONObject

data class NaiveFluidVisualization (
    val visualizationType: String = "simulation",
    val simulationType: String = "naive",
    var distance: Float = 0.5f,
    var fieldOfView: Float = 60.0f,
    var backgroundColor: Color = createColorFromInts(0, 0, 0, 0),
    var fluidSurface: Boolean = true,
    var particleCount: Int = 1000,
    var smoothSphereSurface: Boolean = false,
    var gravity: Float = 2.0f,
    var linearAcceleration: Float = 1.75f,
    var efficiency: Float = 1.0f,
    var referenceFrameRotates: Boolean = true,
    var backgroundIsSolidColor: Boolean = false,
    var backgroundTexture: String = "rgb_cube"
) : Visualization() {

    constructor(jsonObject: JSONObject) : this(
        visualizationType = jsonObject.optString("visualization_type", "simulation"),
        simulationType = jsonObject.optString("simulation_type", "naive"),
        distance = jsonObject.optDouble("distance", 0.5).toFloat(),
        fieldOfView = jsonObject.optDouble("field_of_view", 60.0).toFloat(),
        backgroundColor = jsonObject.optJSONObject("background_color")?.let {
            Companion.jsonObjectToColor(
                it
            )
        }?: createColorFromInts(0, 0, 0, 0),
        fluidSurface = jsonObject.optBoolean("fluid_surface", false),
        particleCount = jsonObject.optInt("particle_count", 1000),
        smoothSphereSurface = jsonObject.optBoolean("smooth_sphere_surface", true),
        gravity = jsonObject.optDouble("gravity", 2.0).toFloat(),
        linearAcceleration = jsonObject.optDouble("linear_acceleration", 1.75).toFloat(),
        efficiency = jsonObject.optDouble("efficiency", 1.0).toFloat(),
        referenceFrameRotates = jsonObject.optBoolean("reference_frame_rotates", true),
        backgroundIsSolidColor = jsonObject.optBoolean("background_is_solid_color", false),
        backgroundTexture = jsonObject.optString("background_texture", "rgb_cube")
    )

    constructor(repo: ActiveWallpaperRepo) : this(
        visualizationType = "simulation",
        simulationType = "naive",
        distance = repo.distanceFromOrigin.value!!,
        fieldOfView = repo.fieldOfView.value!!,
        backgroundColor = repo.color.value!!,
        fluidSurface = repo.fluidSurface.value!!,
        particleCount = repo.particleCount.value!!,
        smoothSphereSurface = repo.smoothSphereSurface.value!!,
        gravity = repo.gravity.value!!,
        linearAcceleration = repo.linearAcceleration.value!!,
        efficiency = repo.efficiency.value!!,
        referenceFrameRotates = repo.referenceFrameRotates.value!!,
        backgroundIsSolidColor = repo.backgroundIsSolidColor.value!!,
        backgroundTexture = repo.backgroundTexture.value!!
    )

    override val relevantTextViewIds: List<Int> = listOf(
        R.id.distance_label,
        R.id.field_of_view_label,
        R.id.gravity_label,
        R.id.linear_acceleration_label,
        R.id.efficiency_label
    )
    override val relevantSeekBarIds: List<Int> = listOf(
        R.id.distance_seekbar,
        R.id.field_of_view_seekbar,
        R.id.gravity_seekbar,
        R.id.linear_acceleration_seekbar,
        R.id.efficiency_seekbar
    )
    override val relevantEditTextIds: List<Int> = listOf(

    )
    override val relevantCheckBoxIds: List<Int> = listOf(
        R.id.fluid_surface_checkbox,
        R.id.gyroscope_compensation_checkbox,
        R.id.smooth_sphere_surface_checkbox
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
        jsonObject.put("fluid_surface", fluidSurface)
        jsonObject.put("particle_count", particleCount)
        jsonObject.put("smooth_sphere_surface", smoothSphereSurface)
        jsonObject.put("distance", distance)
        jsonObject.put("field_of_view", fieldOfView)
        jsonObject.put("gravity", gravity)
        jsonObject.put("linear_acceleration", linearAcceleration)
        jsonObject.put("background_color", colorToJSONObject(backgroundColor))
        jsonObject.put("efficiency", efficiency)
        jsonObject.put("reference_frame_rotates", referenceFrameRotates)
        jsonObject.put("background_is_solid_color", backgroundIsSolidColor)
        jsonObject.put("background_texture", backgroundTexture)
        return jsonObject
    }
}