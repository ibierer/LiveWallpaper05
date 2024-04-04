package com.example.livewallpaper05.activewallpaperdata

import android.content.Context
import android.graphics.Bitmap
import android.graphics.Color
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager
import androidx.annotation.WorkerThread
import androidx.lifecycle.MutableLiveData
import com.example.livewallpaper05.R
import com.example.livewallpaper05.savedWallpapers.SavedWallpaperDao
import com.example.livewallpaper05.savedWallpapers.SavedWallpaperRow
import com.example.livewallpaper05.profiledata.ProfileDao
import com.example.livewallpaper05.profiledata.ProfileTable
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch

class ActiveWallpaperRepo private constructor (val context: Context, wallpaperDao: SavedWallpaperDao, profileDao: ProfileDao) : SensorEventListener {

    // initialize default values for active wallpaper
    var wid: Int = 0
    var equation: String = ""
    var color: MutableLiveData<Color> = MutableLiveData<Color>(Color.valueOf(0.0f,0.0f,0.0f,0.0f))
    var orientation: Int = 0
    var fps: MutableLiveData<Float> = MutableLiveData<Float>(0.0f)
    var lastFrame: Long = 0
    var distanceFromOrigin: MutableLiveData<Float> = MutableLiveData<Float>(0.5f)
    var fieldOfView: MutableLiveData<Float> = MutableLiveData<Float>(60.0f)
    var gravity: MutableLiveData<Float> = MutableLiveData<Float>(0.0f)
    var linearAcceleration: MutableLiveData<Float> = MutableLiveData<Float>(0.0f)
    var efficiency: MutableLiveData<Float> = MutableLiveData<Float>(0.0f)
    var particleCount: MutableLiveData<Int> = MutableLiveData<Int>(1000)
    val flipNormals: MutableLiveData<Boolean> = MutableLiveData<Boolean>(false)
    var rotationData: Array<Float> = arrayOf(0.0f,0.0f,0.0f,0.0f)
    var accelerationData: Array<Float> = arrayOf(0.0f,0.0f,0.0f)
    var linearAccelerationData: Array<Float> = arrayOf(0.0f,0.0f,0.0f)
    var visualizationSelection: Int = 0
    var visualizationName = MutableLiveData<String>("")
    var preview: Bitmap? = null
    var savedConfig: String = ""
    var savedWids: List<Int> = listOf(1)
    var rememberColorPickerValue: Int = 0

    // initialize values for saved wallpaper info
    val activeWallpaper: MutableLiveData<SavedWallpaperRow> = MutableLiveData()
    val wallpapers = MutableLiveData<List<SavedWallpaperRow>>()
    val wallpaperFragIds: MutableList<WallpaperRef> = mutableListOf()

    val mWallpaperDao: SavedWallpaperDao = wallpaperDao
    private var lastId: Int = 1

    private lateinit var mSensorManager: SensorManager

    // helper methods

    fun getSavedWids(): String{
        var widString = ""
        var used = listOf<Int>()

        for (w in savedWids){
            if (used.contains(w)){
                continue
            }
            widString += w.toString() + ","
            used = used.plus(w)
        }
        // strip last comma
        widString = widString.substring(0, widString.length - 1)
        return widString
    }

    // create new wallpaper table
    fun createWallpaperTable(id: Int) : SavedWallpaperRow {
        // create unique id
        if (id > 0){
            lastId = id
        } else {
            lastId += 1
        }
        var wid = lastId

        // create new wallpaper table with default config
        val wallpaper = SavedWallpaperRow(
            context.resources.getInteger(R.integer.default_profile_id),
            wid,
            //"{\n" +
            //        "\"name\": \"New Wallpaper\",\n" +
            //        "\"type\": \"0\",\n" +
            //        "\"background_color\": {\"r\": 51, \"g\": 51, \"b\": 77, \"a\": 255},\n" +
            //        "\"settings\": \"x^2+y^2+z^2=1\"\n" +
            //        "}",
            ActiveWallpaperViewModel.NBodyVisualization().toString(),
            ByteArray(0)
        )

        savedWids = savedWids.plus(wid)

        return wallpaper
    }

    // save wallpaper
    fun setWallpaper(wallpaper: SavedWallpaperRow) {
        if (wallpaper.config != "") {
            mScope.launch(Dispatchers.IO) {
                if(wallpapers.value != null) {
                    // if wallpaper not in list, add it
                    val list = wallpapers.value!!.toMutableList()
                    if (!containsWallpaper(wallpaper)) {
                        list.add(wallpaper)
                        wallpapers.postValue(list)
                    } else {
                        // replace wallpaper in list
                        for (i in 0 until list.size) {
                            if (list[i].wid == wallpaper.wid) {
                                list[i] = wallpaper
                                break
                            }
                        }
                        wallpapers.postValue(list)
                    }
                    syncWallpaperDao(wallpaper, list)
                } else {
                    wallpapers.postValue(listOf(wallpaper))
                    syncWallpaperDao(wallpaper, listOf(wallpaper).toMutableList())
                }
            }
        }
    }

    // check if wallpaper is in the local list of wallpapers
    private fun containsWallpaper(wallpaper: SavedWallpaperRow) : Boolean {
        if (wallpapers.value != null) {
            for (w in wallpapers.value!!) {
                if (w.wid == wallpaper.wid) {
                    return true
                }
            }
        }
        return false
    }

    // set active wallpaper live data to wallpaper given by id
    fun setLiveWallpaperData(wid: Int) {
        mScope.launch(Dispatchers.IO) {
            var wallpaper = mWallpaperDao.getWallpaperData(wid)
            if (wallpaper != null) {
                activeWallpaper.postValue(wallpaper)
            } else {
                // try again until wallpaper is found
                var allWallpapers = mWallpaperDao.getAllWallpapers()
                activeWallpaper.postValue(allWallpapers[0])
            }
        }
    }

    // sync wallpaper dao with given list of wallpapers (local wallpapers)
    private fun syncWallpaperDao(newWallpaper: SavedWallpaperRow, newWallpapers: MutableList<SavedWallpaperRow>){
        // sync wallpapers with database, removing dead values
        var allWallpapers = mWallpaperDao.getAllWallpapers()
        var validWids = listOf<Int>()
        // if list is not the same size as wallpapers clear and add all wallpapers
        if (newWallpapers != null) {
            // for wallpapers in all wallpapers, if wid is not in saved wids, delete it
            for (wallpaper in allWallpapers) {
                if (!newWallpapers.contains(wallpaper) && !savedWids.contains(wallpaper.wid)){
                    mWallpaperDao.deleteWallpaper(wallpaper.wid)
                } else {
                    validWids = validWids.plus(wallpaper.wid)
                }
            }

            // save all wallpapers in new wallpapers to database
            for (wallpaper in newWallpapers) {
                mWallpaperDao.saveWallpaper(wallpaper)
            }

            // if savedWids is > 1, replace default wallpaper with saved wallpaper
            if (savedWids.size > 1) {
                // find default wallpaper (wid = 1) and replace it with saved wallpaper
                var default = newWallpapers.find { it.wid == 1 }
                var saved = allWallpapers.find { it.wid == savedWids[1] }
                if (default != null && saved != null) {
                    newWallpapers.remove(default)
                    newWallpapers.add(saved)
                }
            }

            // for valid wid add that wallpaper from all wallpapers to new wallpapers
            for (wallpaper in allWallpapers) {
                if (validWids.contains(wallpaper.wid) && !newWallpapers.contains(wallpaper)){
                    newWallpapers.add(wallpaper)
                }
            }

            // sync new wallpapers with local wallpapers list
            wallpapers.postValue(newWallpapers)

        }
    }

    // delete wallpaper
    fun deleteWallpaper(wid: Int) {
        mScope.launch(Dispatchers.IO) {
            mWallpaperDao.deleteWallpaper(wid)
            // remove wallpaper from list
            val list = wallpapers.value!!.toMutableList()
            for (i in 0 until list.size) {
                if (list[i].wid == wid) {
                    list.removeAt(i)
                    // remove wid from saved wids
                    savedWids = savedWids.filter { it != wid }
                    break
                }
            }
            wallpapers.postValue(list)
        }
    }

    // switch which wallpaper is currently active
    fun saveSwitchWallpaper(activeUid: Int, activeWid: Int, activeConfig: String) {
        mScope.launch(Dispatchers.IO) {
            val wallpaper = SavedWallpaperRow(
                activeUid,
                activeWid,
                activeConfig,
                ByteArray(0)
            )

            if(wallpapers.value != null) {
                // if wallpaper not in list, add it
                val list = wallpapers.value!!.toMutableList()
                if (!containsWallpaper(wallpaper)) {
                    list.add(wallpaper)
                    wallpapers.postValue(list)
                } else {
                    // replace wallpaper in list
                    for (i in 0 until list.size) {
                        if (list[i].wid == wallpaper.wid) {
                            list[i] = wallpaper
                            break
                        }
                    }
                    wallpapers.postValue(list)
                }
                syncWallpaperDao(wallpaper, list)
            } else {
                wallpapers.postValue(listOf(wallpaper))
                syncWallpaperDao(wallpaper, listOf(wallpaper).toMutableList())
            }

            // update activeWallpaper live data
            activeWallpaper.postValue(wallpaper)
        }
    }

    // setup companion object so repo can be created from any thread and still be a singleton
    companion object {
        @Volatile
        private var instance: ActiveWallpaperRepo? = null
        private lateinit var mScope: CoroutineScope

        @Synchronized
        fun getInstance(
            context: Context,
            savedWallpaperDao: SavedWallpaperDao,
            profileDao: ProfileDao,
            scope: CoroutineScope
        ): ActiveWallpaperRepo {
            return instance ?: ActiveWallpaperRepo(context, savedWallpaperDao, profileDao).also {
                instance = it
                mScope = scope
            }
        }
    }

    // register sensor events to repo sensor manager
    fun registerSensors(mSensorManager: SensorManager){
        this.mSensorManager = mSensorManager
        mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR), SensorManager.SENSOR_DELAY_GAME)
        mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER), SensorManager.SENSOR_DELAY_GAME)
        mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_LINEAR_ACCELERATION), SensorManager.SENSOR_DELAY_GAME)
    }

    // update sensor data variables when sensor values change
    override fun onSensorChanged(p0: SensorEvent?) {
        if (p0 != null) {
            if (p0.sensor.type == Sensor.TYPE_ROTATION_VECTOR) {
                rotationData = p0.values.toTypedArray()
            }
            if (p0.sensor.type == Sensor.TYPE_ACCELEROMETER) {
                accelerationData = p0.values.toTypedArray()
            }
            if (p0.sensor.type == Sensor.TYPE_LINEAR_ACCELERATION) {
                linearAccelerationData = p0.values.toTypedArray()
            }
        }
    }

    // do nothing when sensor accuracy changes
    override fun onAccuracyChanged(p0: Sensor?, p1: Int) {
        // Do nothing
    }

    // update orientation value
    fun updateOrientation(orient: Int) {
        this.orientation = orient
    }

    // update saved wids after they're extracted from memory
    fun setSavedWids(wids: String) {
        savedWids = wids.split(",").map { it.toInt() }
    }

    fun removeWid(wid: Int) {
        savedWids = savedWids.filter { it != wid }
    }

    // helper structures
    class WallpaperRef {
        var wallpaperId: Int = 0
        var fragmentId: Int = 0
        var fragmentTag: String = ""
    }




    val currentUserProfile = MutableLiveData<ProfileTable>(
        ProfileTable(
            context.resources.getInteger(R.integer.default_profile_id),
            0,
            "Default User",
            context.resources.getString(R.string.biography),
            ByteArray(0)
        )
    )

    private var mProfileDao: ProfileDao = profileDao

    fun setProfile(profileTable: ProfileTable){
        mScope.launch(Dispatchers.IO){
            //val profileInfo = mProfileDao.getProfileData()
            val profileInfo = profileTable
            if(profileInfo != null){
                currentUserProfile.postValue(profileInfo)
                mProfileDao.updateProfileData(profileTable)
            }
        }
    }

    @WorkerThread
    suspend fun insert() {
        if(currentUserProfile.value != null)
            mProfileDao.updateProfileData(currentUserProfile.value!!)
    }

    //companion object {
    //    @Volatile
    //    private var instance: ProfileRepo? = null
    //    private lateinit var mScope: CoroutineScope
    //
    //    @Synchronized
    //    fun getInstance(
    //        context: Context,
    //        profileDao: ProfileDao,
    //        scope: CoroutineScope
    //    ): ProfileRepo {
    //        return instance ?: ProfileRepo(context, profileDao).also {
    //            instance = it
    //            mScope = scope
    //        }
    //    }
    //}
}