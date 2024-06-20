package com.vizbox4d.activewallpaperdata

import androidx.lifecycle.*
import java.math.RoundingMode

/**
 * View Model to keep a reference to the active wallpaper data
 */
class ActiveWallpaperViewModel(val repo: ActiveWallpaperRepo) : ViewModel() {

    // return distance value from repo
    fun getDistanceFromOrigin(): Float {
        return repo.distanceFromOrigin.value!!
    }

    // return field of view value from repo
    fun getFieldOfView(): Float {
        return repo.fieldOfView.value!!
    }

    // return rotation data from repo
    fun getRotationData(): Array<Float> {
        return repo.rotationData
    }

    // return acceleration data from repo
    fun getAccelerationData(): Array<Float> {
        return repo.accelerationData
    }

    // return linear acceleration data from repo
    fun getLinearAccelerationData(): Array<Float> {
        return repo.linearAccelerationData
    }

    // return simulation type from repo
    fun getVisualization(): Int {
        return repo.getVisualizationSelection()
    }

    // return orientation from repo
    fun getOrientation(): Int {
        return repo.orientation
    }

    // return fps from repo
    fun getFPS(): MutableLiveData<Float> {
        return repo.fps
    }

    // return last frame time value from repo
    fun getLastFrame(): Long {
        return repo.lastFrame
    }

    // get efficiency value from repo
    fun getEfficiency(): Float {
        return repo.efficiency.value!!
    }

    // get vector direction from repo
    fun getVectorDirection(): Boolean {
        return repo.flipNormals.value!!
    }

    // get linear acceleration value from repo
    fun getLinearAcceleration(): Float {
        return repo.linearAcceleration.value!!
    }

    // get gravity value from repo
    fun getGravity(): Float {
        return repo.gravity.value!!
    }

    // update orientation in repo
    fun updateOrientation(orient: Int) {
        repo.updateOrientation(orient)
    }

    // update distance from origin in repo
    fun updateDistanceFromCenter(distance: Float) {
        repo.distanceFromOrigin.value = distance
    }

    // update field of view in repo
    fun updateFieldOfView(angle: Float) {
        repo.fieldOfView.value = angle
    }

    // update simulation type in repo, return true if value changed
    fun updateVisualizationSelection(selection: Int): Boolean {
        if (selection != repo.getVisualizationSelection()) {
            repo.visualization = repo.visualizationIntToVisualizationObject(selection)
            return true
        }
        return false
    }

    // update fps in repo (scale float to 2 decimal places)
    fun updateFPS(fps: Float) {
        // update fps in repo
        val rounded = fps.toBigDecimal().setScale(2, RoundingMode.UP).toDouble()
        repo.fps.postValue(rounded.toFloat())
    }

    // update last frame time value in repo
    fun updateLastFrame(new: Long){
        repo.lastFrame = new
    }

    // update gravity value in repo
    fun updateGravity(value: Float) {
        repo.gravity.value = value
    }

    // update efficiency value in repo
    fun updateEfficiency(value: Float) {
        repo.efficiency.value = value
    }

    // update linear acceleration value in repo
    fun updateLinearAcceleration(value: Float) {
        repo.linearAcceleration.value = value
    }

    fun saveVisualizationState() {
        repo.saveVisualizationState()
    }

}

/**
 * Factory for constructing ActiveWallpaperViewModel with repository parameter.
 *
 * This ensures that when a view model is created, it will be created with the same instance/reference no matter
 * what activity or fragment it is created from.
 */
class ActiveWallpaperViewModelFactory(private val repo: ActiveWallpaperRepo) : ViewModelProvider.Factory {
    override fun <T : ViewModel> create(modelClass: Class<T>): T {
        if (modelClass.isAssignableFrom(ActiveWallpaperViewModel::class.java)) {
            @Suppress("UNCHECKED_CAST")
            return ActiveWallpaperViewModel(repo) as T
        }
        throw IllegalArgumentException("Unknown ViewModel class")
    }
}