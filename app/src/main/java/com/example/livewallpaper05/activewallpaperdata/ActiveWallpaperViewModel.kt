package com.example.livewallpaper05.activewallpaperdata

import android.app.Activity
import android.content.res.Resources
import android.graphics.Bitmap
import android.graphics.Color
import android.hardware.SensorManager
import android.util.Log
import androidx.lifecycle.*
import com.example.livewallpaper05.activewallpaperdata.ActiveWallpaperRepo.WallpaperRef
import com.example.livewallpaper05.savedWallpapers.SavedWallpaperRow
import org.json.JSONObject
import java.math.RoundingMode
import java.util.Random

/**
 * View Model to keep a reference to the active wallpaper data
 */
class ActiveWallpaperViewModel(val repo: ActiveWallpaperRepo) : ViewModel() {

    // initialize local variables for active wallpaper
    /*lateinit var visualization : Visualization
    var width: Int = 0
    var height: Int = 0
    private var mBitmap : Bitmap = Bitmap.createBitmap(500, 500, Bitmap.Config.ARGB_8888) as Bitmap
    var liveDataBitmap : MutableLiveData<Bitmap> = MutableLiveData<Bitmap>(mBitmap)
    var getScreenBuffer: Int = 0
    var saveAsNew: Int = 0
    // reference repo from constructor value
    val mRepo: ActiveWallpaperRepo = repo
    var isCollapsed = false

    // initialize local variables for saved wallpapers
    val activeWallpaper: LiveData<SavedWallpaperRow> = repo.activeWallpaper
    val savedWallpapers: LiveData<List<SavedWallpaperRow>> = repo.wallpapers*/

    // GETTERS - here are all the methods for getting data from the repo -------------------

    // grab bitmap from repo
    fun getPreviewImg(seed: Int): Bitmap {
        val rng = Random()
        rng.setSeed(seed.toLong())
        val color = Color.argb(255,
            rng.nextInt(256),
            rng.nextInt(256),
            rng.nextInt(256))

        val screenWidth = Resources.getSystem().displayMetrics.widthPixels
        val screenHeight = Resources.getSystem().displayMetrics.heightPixels
        val imgWidth = screenWidth/4
        val imgHeight = screenHeight/4

        val tmp = Bitmap.createBitmap(imgWidth, imgHeight, Bitmap.Config.ARGB_8888)
        tmp.eraseColor(color)

        return tmp
    }

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
        return repo.visualizationSelection
    }

    fun getGraph(): Int {
        return repo.graphSelection
    }

    // return orientation from repo
    fun getOrientation(): Int {
        return repo.orientation
    }

    // return color from repo
    fun getColor(): Color {
        return repo.color.value!!
    }

    // return fps from repo
    fun getFPS(): MutableLiveData<Float> {
        return repo.fps
    }

    // return last frame time value from repo
    fun getLastFrame(): Long {
        return repo.lastFrame
    }

    // return config string from repo
    // [TODO] fix to use dynamic config generation
    fun getConfig(): String {
        val config = JSONObject()
        // store simulation type, background color, and settings (with default values for now
        config.put("name", "New Wallpaper")
        config.put("type", repo.visualizationSelection)
        val color: Color = getColor()
        config.put("background_color", JSONObject("{\"r\": ${color.red()}, \"g\": ${color.green()}, \"b\": ${color.blue()}, \"a\": ${color.alpha()} }"))
        val eq = if (repo.userDefinedEquation != "") repo.userDefinedEquation else "1/((sqrt(x^2 + y^2) - 2 + 1.25cos(t))^2 + (z - 1.5sin(t))^2) + 1/((sqrt(x^2 + y^2) - 2 - 1.25cos(t))^2 + (z + 1.5sin(t))^2) = 1.9"
        config.put("settings", eq)

        // [TODO] temp solution
        when (repo.visualizationSelection){
            0 -> {
                //var sim = NBodyVisualization()
                config.put("distance", getDistanceFromOrigin())
                config.put("field_of_view", getFieldOfView())
            }
            1 -> {
                val sim = NaiveFluidVisualization()
                config.put("distance", getDistanceFromOrigin())
                config.put("field_of_view", getFieldOfView())
                config.put("background_is_solid_color", sim.backgroundIsSolidColor)
                config.put("background_texture", sim.backgroundTexture)
                //config.put("gravity", getGravity())
                //config.put("linear_acceleration", getLinearAcceleration())
                //config.put("efficiency", getEfficiency())
                config.put("particle_count", sim.particleCount)
            }
            2 -> {
                val sim = PicFlipVisualization()
                config.put("distance", sim.distance)
                config.put("field_of_view", sim.fieldOfView)
                config.put("background_is_solid_color", sim.backgroundIsSolidColor)
                config.put("background_texture", sim.backgroundTexture)
                config.put("gravity", sim.gravity)
                config.put("linear_acceleration", sim.linearAcceleration)
            }
            3 -> {
                val sim = TriangleVisualization()
                config.put("distance", sim.distance)
                config.put("field_of_view", sim.fieldOfView)
                config.put("background_is_solid_color", sim.backgroundIsSolidColor)
                config.put("background_texture", sim.backgroundTexture)
            }
            4 -> {
                val graph = GraphVisualization()
                config.put("distance", graph.distance)
                config.put("field_of_view", graph.fieldOfView)
                config.put("background_is_solid_color", graph.backgroundIsSolidColor)
                config.put("background_texture", graph.backgroundTexture)
                //config.put("equation", graph.equation)
            }
        }

        return config.toString()

    }

    // get active wallpaper id from repo
    fun getWid(): Int {
        return repo.wid
    }

    fun getLastModified(): Long {
        return repo.lastModified
    }

    // get equation string from repo
    fun getEquation(): String {
        return repo.userDefinedEquation
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

    // get the wallpaper id list for the wallpaper preview fragment(s)
    fun getWallpaperFragIds() : MutableList<WallpaperRef> {
        return repo.wallpaperFragIds
    }

    // SETTERS - here are all the methods for updating data in the repo -------------------------

    // update visualization type in repo
    fun updateSimulationType(type: String) {
        repo.visualizationName.postValue(type)
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
        if (selection != repo.visualizationSelection) {
            repo.visualizationSelection = selection
            return true
        }
        return false
    }

    fun updateGraphSelection(selection: Int): Boolean {
        if (selection != repo.graphSelection) {
            repo.graphSelection = selection
            return true
        }
        return false
    }

    // update color in repo
    fun updateColor(color: Color, back: Boolean = false) {
        // scale values to 0-1 range
        // if any value is greater than 1
        val r = if (color.red() > 1) color.red() / 255.0f else color.red()
        val g = if (color.green() > 1) color.green() / 255.0f else color.green()
        val b = if (color.blue() > 1) color.blue() / 255.0f else color.blue()
        val a = if (color.alpha() > 1) color.alpha() / 255.0f else color.alpha()

        // update color in repo
        if (back){
            repo.color.postValue(Color.valueOf(r, g, b, a))
        } else {
            repo.updateColor(r, g, b, a)
        }
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

    // update wallpaper id in repo
    fun setWid(wid: Int) {
        repo.wid = wid
    }

    // update preview image in repo
    fun updatePreviewImg(preview: Bitmap?) {
        if (preview != null)
            repo.preview = preview
    }

    // update equation string in repo
    fun updateEquation(toString: String) {
        repo.userDefinedEquation = toString
    }

    // update gravity value in repo
    fun updateGravity(value: Float) {
        repo.gravity.value = value
    }

    // update efficiency value in repo
    fun updateEfficiency(value: Float) {
        repo.efficiency.value = value
    }

    // update particle count value in repo
    fun updateParticleCount(value: Int) {
        repo.particleCount.value = value
    }

    // update linear acceleration value in repo
    fun updateLinearAcceleration(value: Float) {
        repo.linearAcceleration.value = value
    }

    // update wallpaper preview fragments list in repo
    fun updateWallpaperFragIds(wallpaperRef: WallpaperRef) {
        // if wallpaperRef is already in list ignore
        val data = repo.wallpaperFragIds
        for (r in data){
            if (r.wallpaperId == wallpaperRef.wallpaperId) {
                return
            }
        }
        // add new wallpaperRef to list
        data.add(wallpaperRef)
    }

    // update viz name in repo
    private fun updateVizualizationName(s: String) {
        repo.visualizationName.postValue(s)
    }

    // OTHER - other helper methods ----------------------------------------------------------
    // register sensor events to repo sensor manager
    fun registerSensorEvents(manager: SensorManager) {
        repo.registerSensors(manager)
    }

    // load config table into repo
    fun loadConfig(table: SavedWallpaperRow) {
        // prevent reloading same config
        if (repo.wid == table.wid && repo.savedConfig == table.config) {
            return
        }
        try {
            repo.wid = table.wid
            val configJson = JSONObject(table.config)

            repo.visualizationSelection = configJson.getInt("type")
            //val red = configJson.getJSONObject("background_color").getInt("r").toFloat()
            //val green = configJson.getJSONObject("background_color").getInt("g").toFloat()
            //val blue = configJson.getJSONObject("background_color").getInt("b").toFloat()
            //val alpha = configJson.getJSONObject("background_color").getInt("a").toFloat()

            // update visualization specific parameters
            // [TODO] - add all viz parameters here and in repo (requires getter and setter functions as well)
            when (repo.visualizationSelection) {
                0 -> {
                    val sim = NBodyVisualization(configJson)
                    updateDistanceFromCenter(sim.distance)
                    updateFieldOfView(sim.fieldOfView)
                    updateColor(sim.backgroundColor)
                    //updateColor(Color.valueOf(red, green, blue, alpha))
                    updateVizualizationName("N-Body Simulation")
                }
                1 -> {
                    val sim = NaiveFluidVisualization(configJson)
                    updateDistanceFromCenter(sim.distance)
                    updateFieldOfView(sim.fieldOfView)
                    updateColor(sim.backgroundColor)
                    //updateColor(Color.valueOf(red, green, blue, alpha))
                    updateGravity(sim.gravity)
                    updateLinearAcceleration(sim.linearAcceleration)
                    updateEfficiency(sim.efficiency)
                    updateParticleCount(sim.particleCount)
                    updateVizualizationName("Naive Fluid Simulation")
                }
                2 -> {
                    val sim = PicFlipVisualization(configJson)
                    updateDistanceFromCenter(sim.distance)
                    updateFieldOfView(sim.fieldOfView)
                    updateColor(sim.backgroundColor)
                    //updateColor(Color.valueOf(red, green, blue, alpha))
                    updateGravity(sim.gravity)
                    updateLinearAcceleration(sim.linearAcceleration)
                    updateVizualizationName("PicFlip Simulation")
                }
                3 -> {
                    val sim = TriangleVisualization(configJson)
                    updateDistanceFromCenter(sim.distance)
                    updateFieldOfView(sim.fieldOfView)
                    updateColor(sim.backgroundColor)
                    //updateColor(Color.valueOf(red, green, blue, alpha))
                    updateVizualizationName("Triangle Visualization")
                }
                4 -> {
                    val sim = GraphVisualization(configJson)
                    updateDistanceFromCenter(sim.distance)
                    updateFieldOfView(sim.fieldOfView)
                    updateColor(sim.backgroundColor)
                    //updateColor(Color.valueOf(red, green, blue, alpha))
                    updateEquation(sim.equation)
                    updateVizualizationName("Graph Visualization")
                }
            }

            // store config string in repo
            repo.savedConfig = table.config

        } catch (e: Exception) {
            Log.d("LiveWallpaperLoad", "Error loading config: $e")
            Log.d("LiveWallpaperLoad", "Config: ${table.config}")
            return
        }
    }

    // switch active wallpaper
    fun switchWallpaper(wid: Int) {
        repo.setLiveWallpaperData(wid)
    }

    // save current list of wids to memory
    fun saveWids(activity: Activity){
        // get wids from repo
        val wids = repo.getSavedWids()
        // save wids to shared prefs
        activity.getSharedPreferences("livewallpaper05", 0).edit()
            .putString("savedWids", wids).apply()
    }

    // load saved wids from memory
    fun loadWidsFromMem(activity: Activity) {
        try {
            // load wids from shared prefs
            val wids =
                activity.getSharedPreferences("livewallpaper05", 0).getString("savedWids", "")
            if (wids == null || wids == "") {
                return
            }
            // set wids in repo
            repo.setSavedWids(wids)
        } catch (e: Exception) {
            Log.d("LiveWallpaper05", "Error loading wids: $e")
        }
    }

    // save wallpaper from config string
    fun saveWallpaper(config: String) : Int {
        // create new wallpaper table with given data
        var wallpaper: SavedWallpaperRow
        try {
            wallpaper = SavedWallpaperRow(
                repo.activeWallpaper.value!!.uid,
                repo.activeWallpaper.value!!.wid,
                config,
                ByteArray(0),
                System.currentTimeMillis()
            )

            // update profile table
            //mRepo.setWallpaper(wallpaper)
            repo.saveActiveWallpaper(wallpaper)
        } catch (_: Exception) {
            // update active wallpaper
            wallpaper = SavedWallpaperRow(
                repo.uid,
                repo.wid,
                getConfig(),
                ByteArray(0),
                System.currentTimeMillis()
            )
            repo.activeWallpaper.postValue(wallpaper)
            //repo.saveActiveWallpaper(wallpaper)
        }
        return wallpaper.wid
    }

    // delete wallpaper
    fun deleteWallpaper(wid: Int) {
        repo.deleteWallpaper(wid)
    }

    // create new wallpaper table
    fun createWallpaperTable(id: Int): Int {
        val newWallpaper = repo.createWallpaperTable(id)
        repo.setWallpaper(newWallpaper)
        return newWallpaper.wid
    }

    // create wallpaper save with default values
    fun createDefaultWallpaperTable(uid: Int, wid: Int, config: String) {
        val wallpaper = SavedWallpaperRow(
            uid,
            wid,
            config,
            ByteArray(0),
            System.currentTimeMillis()
        )
        repo.setWallpaper(wallpaper)
    }

    // remove specified wallpapers from fragment list via wallpaper reference
    fun removeWallpaperFragId(wallpaperRef: WallpaperRef) {
        // remove wallpaperRef from list
        val data = repo.wallpaperFragIds
        var toRemove = listOf<WallpaperRef>()
        for (r in data) {
            if (r.wallpaperId == wallpaperRef.wallpaperId) {
                toRemove = toRemove.plus(r)
            }
        }
        data.removeAll(toRemove)
    }

    // save active wallpaper and switch to new wallpaper. This allows us to save
    // the wallpaper before changing it, ensuring no changes are forgotten
    fun saveSwitchWallpaper(activeWid: Int, activeConfig: String, lastModified: Long) {
        repo.saveSwitchWallpaper(0, activeWid, activeConfig, lastModified)
    }

    fun setNewId(newId: Int) {
        repo.setTransitionNewId(newId)
    }

    fun getTransitionNewId(): Int {
        return repo.getTransitionNewId()
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