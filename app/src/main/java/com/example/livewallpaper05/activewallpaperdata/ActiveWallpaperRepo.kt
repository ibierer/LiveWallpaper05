package com.example.livewallpaper05.activewallpaperdata

import android.content.Context
import android.graphics.Bitmap
import android.graphics.Color
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager
import androidx.lifecycle.MutableLiveData
import com.example.livewallpaper05.R
import com.example.livewallpaper05.savedWallpapers.SavedWallpaperDao
import com.example.livewallpaper05.savedWallpapers.SavedWallpaperRow
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.sync.Mutex

class ActiveWallpaperRepo private constructor (val context: Context, wallpaperDao: SavedWallpaperDao) : SensorEventListener {

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
    var savedWids: List<Int> = listOf(0)

    // initialize values for saved wallpaper info
    val activeWallpaper: MutableLiveData<SavedWallpaperRow> = MutableLiveData()
    val wallpapers = MutableLiveData<List<SavedWallpaperRow>>()
    val wallpaperFragIds: MutableList<WallpaperRef> = mutableListOf()
    //private val syncMutex: Mutex = Mutex()

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
        // if lastid is in saved wids, increment last id until it is not
        while (savedWids.contains(lastId)){
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

    fun saveActiveWallpaper(wallpaperRow: SavedWallpaperRow){
        // set wallpaper in wallpapers to use new value
        for (wallpaper in wallpapers.value!!){
            if (wallpaper.wid == wallpaperRow.wid){
                wallpaper.config = wallpaperRow.config
                break
            }
        }

        mScope.launch(Dispatchers.IO) {
            mWallpaperDao.saveWallpaper(wallpaperRow)
        }
    }

    // save wallpaper and update as active wallpaper
    fun setWallpaper(wallpaper: SavedWallpaperRow) {
        if (wallpaper.config != "") {
            if(wallpapers.value != null) {
                // if wallpaper not in list, add it
                //var list = wallpapers.value!!.toMutableList()
                if (!containsWallpaper(wallpaper)) {
                    //list.add(wallpaper)
                    wallpapers.value = wallpapers.value!!.plus(wallpaper)
                    //wallpapers.postValue(list)
                } else {
                    for (w in wallpapers.value!!){
                        if (w.wid == wallpaper.wid){
                            w.config = wallpaper.config
                            w.previewImage = wallpaper.previewImage
                        }
                    }
                    //list = wallpapers.value!!.toMutableList()
                }
                syncWallpaperDao(wallpapers.value!!.toMutableList())
            } else {
                //wallpapers.postValue(listOf(wallpaper))
                syncWallpaperDao(listOf(wallpaper).toMutableList())
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
    private fun syncWallpaperDao(newWallpapers: MutableList<SavedWallpaperRow>){
        // sync wallpapers with database, removing dead values
        mScope.launch(Dispatchers.IO) {
            syncMutex.lock()
            var allWallpapers = mWallpaperDao.getAllWallpapers()
            var validWids = listOf<Int>()
            // if list is not the same size as wallpapers clear and add all wallpapers
            if (newWallpapers != null) {
                // for wallpapers in all wallpapers, if wid is not in saved wids, delete it
                for (wallpaper in allWallpapers) {
                    if(wallpaper.config == "")
                        continue

                    if (!newWallpapers.contains(wallpaper) && !savedWids.contains(wallpaper.wid)) {
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
                    var default = newWallpapers.find { it.wid == 0 }
                    var saved = allWallpapers.find { it.wid == savedWids[1] }
                    if (default != null && saved != null) {
                        newWallpapers.remove(default)
                        newWallpapers.add(saved)
                    }
                }

                // for valid wid add that wallpaper from all wallpapers to new wallpapers
                for (wallpaper in allWallpapers) {
                    // WARNING: contains only checks for dup objects not wids
                    if (validWids.contains(wallpaper.wid) && !newWallpapers.contains(wallpaper)) {
                        newWallpapers.add(wallpaper)
                    }
                }

                // check for duplicates
                var uniqueWallpaperWids = mutableListOf<Int>()
                val wallpaperIterator = newWallpapers.iterator()
                //for (wallpaper in newWallpapers) {
                while (wallpaperIterator.hasNext()) {
                    val wallpaper = wallpaperIterator.next()
                    if (!uniqueWallpaperWids.contains(wallpaper.wid)) {
                        uniqueWallpaperWids.add(wallpaper.wid)
                    } else {
                        // delete duplicate wallpaper
                        //mWallpaperDao.deleteWallpaper(wallpaper.wid)
                        //newWallpapers.remove(wallpaper)
                        wallpaperIterator.remove()
                    }
                }

                // sync new wallpapers with local wallpapers list
                wallpapers.postValue(newWallpapers)
            }
            syncMutex.unlock()
        }
    }

    // delete wallpaper
    fun deleteWallpaper(wid: Int) {
        mScope.launch(Dispatchers.IO) {
            mWallpaperDao.deleteWallpaper(wid)
            syncMutex.lock()
            // remove wid from saved wids
            savedWids = savedWids.filter { it != wid }
            // remove wallpaper from wallpapers list
            var list = wallpapers.value!!.toMutableList()
            list = list.filter { it.wid != wid }.toMutableList()
            wallpapers.postValue(list)
            syncMutex.unlock()
        }
        // remove wid from savedWids
        //savedWids = savedWids.filter { it != wid }
        // run sync to update local and saved list of wallpapers
        //syncWallpaperDao(wallpapers.value!!.toMutableList())

        // remove wallpaper from list
        //val list = wallpapers.value!!.toMutableList()
          /*
            for (i in 0 until list.size) {
                if (list[i].wid == wid) {
                    list.removeAt(i)
                    // remove wid from saved wids
                    savedWids = savedWids.filter { it != wid }
                    break
                }
            }
        */
        // add all values that are not removal wid to empty list
        /*
        var list = listOf<SavedWallpaperRow>()
        for (wallpaper in wallpapers.value!!) {
            if (wallpaper.wid != wid) {
                list = list.plus(wallpaper)
            }
        }
        */

        //wallpapers.postValue(list)
    }

    // switch which wallpaper is currently active
    fun saveSwitchWallpaper(activeUid: Int, activeWid: Int, activeConfig: String) {
        //mScope.launch(Dispatchers.IO) {
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
                //wallpapers.postValue(list)
            }
            syncWallpaperDao(list)
        } else {
            //wallpapers.postValue(listOf(wallpaper))
            syncWallpaperDao(listOf(wallpaper).toMutableList())
        }

        // update activeWallpaper live data
        activeWallpaper.postValue(wallpaper)
        //}
    }

    // setup companion object so repo can be created from any thread and still be a singleton
    companion object {
        @Volatile
        private var instance: ActiveWallpaperRepo? = null
        private lateinit var mScope: CoroutineScope
        private val syncMutex: Mutex = Mutex()

        @Synchronized
        fun getInstance(
            context: Context,
            savedWallpaperDao: SavedWallpaperDao,
            scope: CoroutineScope
        ): ActiveWallpaperRepo {
            return instance ?: ActiveWallpaperRepo(context, savedWallpaperDao).also {
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

    // helper structures
    class WallpaperRef {
        var wallpaperId: Int = 0
        var fragmentId: Int = 0
        var fragmentTag: String = ""
    }
}