package com.example.livewallpaper05.activewallpaperdata

import android.annotation.SuppressLint
import android.content.Context
import android.graphics.Bitmap
import android.graphics.Color
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager
import android.util.Log
import androidx.lifecycle.MutableLiveData
import com.example.livewallpaper05.ProfileActivity
import com.example.livewallpaper05.R
import com.example.livewallpaper05.profiledata.ProfileDao
import com.example.livewallpaper05.profiledata.ProfileTable
import com.example.livewallpaper05.savedWallpapers.SavedWallpaperDao
import com.example.livewallpaper05.savedWallpapers.SavedWallpaperRow
import com.google.firebase.auth.FirebaseAuth
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.DelicateCoroutinesApi
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import kotlinx.coroutines.sync.Mutex
import kotlinx.coroutines.withContext
import java.io.ByteArrayOutputStream
import java.sql.DriverManager
import java.sql.SQLException
import java.util.Properties

class ActiveWallpaperRepo private constructor(
    val context: Context,
    private val wallpaperDao: SavedWallpaperDao,
    profileDao: ProfileDao
) : SensorEventListener {
    private lateinit var auth: FirebaseAuth

    // initialize default values for active wallpaper
    var wid: Int = 0
    val lastModified: Long = 0
    var uid: Int = 0
    var username: String = "Default User"
    var equation: String = ""
    var color: MutableLiveData<Color> =
        MutableLiveData<Color>(Color.valueOf(0.0f, 0.0f, 0.0f, 0.0f))
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
    var rotationData: Array<Float> = arrayOf(0.0f, 0.0f, 0.0f, 0.0f)
    var accelerationData: Array<Float> = arrayOf(0.0f, 0.0f, 0.0f)
    var linearAccelerationData: Array<Float> = arrayOf(0.0f, 0.0f, 0.0f)
    var visualizationSelection: Int = 0
    var visualizationName = MutableLiveData<String>("")
    var preview: Bitmap? = null
    var savedConfig: String = ""
    var savedWids: List<Int> = listOf(0)
    var rememberColorPickerValue: Int = 0

    // initialize values for saved wallpaper info
    val activeWallpaper: MutableLiveData<SavedWallpaperRow> = MutableLiveData<SavedWallpaperRow>()
    val wallpapers = MutableLiveData<List<SavedWallpaperRow>>()
    val wallpaperFragIds: MutableList<WallpaperRef> = mutableListOf()
    //private val syncMutex: Mutex = Mutex()

    private var lastId: Int = 1

    private lateinit var mSensorManager: SensorManager

    // ViewModel state
    lateinit var visualization: Visualization
    var width: Int = 0
    var height: Int = 0
    private var mBitmap: Bitmap = Bitmap.createBitmap(500, 500, Bitmap.Config.ARGB_8888) as Bitmap
    var liveDataBitmap: MutableLiveData<Bitmap> = MutableLiveData<Bitmap>(mBitmap)
    var getScreenBuffer: Int = 0
    var isCollapsed = false
    // initialize local variables for saved wallpapers
    //val savedWallpapers: LiveData<List<SavedWallpaperRow>> = repo.wallpapers

    // helper methods
    fun getSavedWids(): List<Int> {
        return savedWids.distinct()
    }

    private fun getLocalWidsByUID(uid: Int): List<Tuple> {
        return wallpaperDao.getAllWallpapersByUID(uid)
    }

    // create new wallpaper table
    fun createWallpaperTable(id: Int): SavedWallpaperRow {
        // create unique id
        if (id > 0) {
            lastId = id
        } else {
            lastId += 1
        }
        // if lastid is in saved wids, increment last id until it is not
        while (savedWids.contains(lastId)) {
            lastId += 1
        }

        val wid = lastId

        // create new wallpaper table with default config
        val wallpaper = SavedWallpaperRow(
            context.resources.getInteger(R.integer.default_profile_id),
            wid,
            NBodyVisualization().toString(),
            ByteArray(0),
            System.currentTimeMillis()
        )

        savedWids = savedWids.plus(wid)

        return wallpaper
    }

    fun saveActiveWallpaper(wallpaperRow: SavedWallpaperRow) {
        // set wallpaper in wallpapers to use new value
        for (wallpaper in wallpapers.value!!) {
            if (wallpaper.wid == wallpaperRow.wid) {
                wallpaper.config = wallpaperRow.config
                break
            }
        }
        // update active wallpaper
        activeWallpaper.postValue(wallpaperRow)

        mScope.launch(Dispatchers.IO) {
            wallpaperDao.saveWallpaper(wallpaperRow)
        }
    }

    /* insert wallpaper to AWS DB (JSON contents, blob image, uid
    *  syncronize with RoomDB before posting to server */
    @OptIn(DelicateCoroutinesApi::class)
    suspend fun synchronizeWithServer() {
        // Non-registered user, don't sync with server
        if (uid == context.resources.getInteger(R.integer.default_profile_id)) {
            return
        }
        // construct list of Pair/Tuple (wid, lastModified)
        withContext(Dispatchers.IO){
            // One with all wids on AWS for the user
            val widsRoom: List<Tuple> = getLocalWidsByUID(uid)
            // another with wids in Room for the user
            val widsServer: List<Tuple> = getServerWidsByUID(uid)
            for (tuple in widsServer){
                if (!widsRoom.any{ it.wid == tuple.wid }) {
                    // insert visualization from server to local db
                }
            }
            for (tuple in widsRoom){
                if (!widsServer.any{ it.wid == tuple.wid }){
                    // insert visualization from local db to server
                }
            }

            // Synchronize existing wids with matching lastModified timestamps
            for (tupleServer in widsServer) {
                val tupleRoom = widsRoom.find { it.wid == tupleServer.wid }
                if (tupleRoom != null) {
                    if (tupleServer.lastModified > tupleRoom.lastModified) {
                        // Insert visualization from server to local db
                    } else if (tupleServer.lastModified < tupleRoom.lastModified) {
                        // Insert visualization from local db to server
                    }
                }
            }
        }








        auth = FirebaseAuth.getInstance()
        val outputStream = ByteArrayOutputStream()
        liveDataBitmap.value?.compress(Bitmap.CompressFormat.PNG, 100, outputStream)
        val convertedBitmap: ByteArray = outputStream.toByteArray()
        // REGISTERED USER
        if (auth.currentUser != null) {
            val sharedPreferences = context.getSharedPreferences("MyPreferences", Context.MODE_PRIVATE)
            username = sharedPreferences.getString("USERNAME", "").toString()
            uid = sharedPreferences.getInt("UID", 0)
            val visualizationDetails = visualization.toJsonObject()
            GlobalScope.launch {
                insertWallpaper(visualizationDetails.toString(), convertedBitmap, username)
            }
        } else {
            // DEFAULT USER
            uid = 11
            val visualizationDetails = visualization.toJsonObject()
            GlobalScope.launch {
                insertWallpaper(visualizationDetails.toString(), convertedBitmap, username)
            }
        }

    }

    private fun getServerWidsByUID(uid: Int): List<Tuple> {
        val jdbcConnectionString = ProfileActivity.DatabaseConfig.jdbcConnectionString
        val tuples = mutableListOf<Tuple>()
        Log.d("SQL", "in getServerWidsByUID")
        try {
            Class.forName("com.mysql.jdbc.Driver").newInstance()
            val connectionProperties = Properties()
            connectionProperties["user"] = ProfileActivity.DatabaseConfig.dbUser
            connectionProperties["password"] = ProfileActivity.DatabaseConfig.dbPassword
            connectionProperties["useSSL"] = "false"
            DriverManager.getConnection(jdbcConnectionString, connectionProperties)
                .use { conn ->
                    Log.d("SQL", "Connection made")
                    val selectQuery =
                        "SELECT wid, lastModified FROM wallpapers where uid = ?;"
                    conn.prepareStatement(selectQuery).use { preparedStatement ->
                        preparedStatement.setInt(1, uid)
                        val resultSet = preparedStatement.executeQuery()
                        while (resultSet.next()){
                            val wid = resultSet.getInt("wid")
                            val lastModified = resultSet.getLong("lastModified")
                            val widTuple = Tuple(wid, lastModified)
                            tuples.add(widTuple)
                        }
                    }
                }
        } catch (e: SQLException) {
            Log.e("SQL_ERROR", "Error getting wid and lastModified column values: ${e.message}", e)
        } catch (e: Exception) {
            Log.e("SQL_ERROR", "Unexpected error: ${e.message}", e)
        }
        return tuples
    }


    private suspend fun insertWallpaper(contents: String, image: ByteArray, username: String) {
        withContext(Dispatchers.IO) {
            val jdbcConnectionString = ProfileActivity.DatabaseConfig.jdbcConnectionString
            Log.d("SQL", "in Insert")
            try {
                Class.forName("com.mysql.jdbc.Driver").newInstance()
                val connectionProperties = Properties()
                connectionProperties["user"] = ProfileActivity.DatabaseConfig.dbUser
                connectionProperties["password"] = ProfileActivity.DatabaseConfig.dbPassword
                connectionProperties["useSSL"] = "false"
                DriverManager.getConnection(jdbcConnectionString, connectionProperties)
                    .use { conn ->
                        Log.d("SQL", "Connection made")
                        val insertQuery =
                            "INSERT INTO wallpapers (contents, image, username) VALUES (?, ?, ?);"
                        conn.prepareStatement(insertQuery).use { preparedStatement ->
                            preparedStatement.setString(1, contents)
                            preparedStatement.setBytes(2, image)
                            preparedStatement.setString(3, username)
                            preparedStatement.executeUpdate()
                        }
                    }
            } catch (e: SQLException) {
                Log.e("SQL_ERROR", "Error inserting wallpaper: ${e.message}", e)
            } catch (e: Exception) {
                Log.e("SQL_ERROR", "Unexpected error: ${e.message}", e)
            }
        }
    }

    // save wallpaper and update as active wallpaper
    fun setWallpaper(wallpaper: SavedWallpaperRow) {
        if (wallpaper.config != "") {
            if (wallpapers.value != null) {
                // if wallpaper not in list, add it
                //var list = wallpapers.value!!.toMutableList()
                if (!containsWallpaper(wallpaper)) {
                    //list.add(wallpaper)
                    wallpapers.value = wallpapers.value!!.plus(wallpaper)
                    //wallpapers.postValue(list)
                } else {
                    for (w in wallpapers.value!!) {
                        if (w.wid == wallpaper.wid) {
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
    private fun containsWallpaper(wallpaper: SavedWallpaperRow): Boolean {
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
            val wallpaper = wallpaperDao.getWallpaperData(wid).value!!
            activeWallpaper.postValue(wallpaper)
        }
    }

    // sync wallpaper dao with given list of wallpapers (local wallpapers)
    private fun syncWallpaperDao(newWallpapers: MutableList<SavedWallpaperRow>) {
        // sync wallpapers with database, removing dead values
        mScope.launch(Dispatchers.IO) {
            syncMutex.lock()
            val allWallpapers = wallpaperDao.getAllWallpapers()
            var validWids = listOf<Int>()
            // if list is not the same size as wallpapers clear and add all wallpapers
            // for wallpapers in all wallpapers, if wid is not in saved wids, delete it
            for (wallpaper in allWallpapers) {
                if (wallpaper.config == "")
                    continue

                if (!newWallpapers.contains(wallpaper) && !savedWids.contains(wallpaper.wid)) {
                    wallpaperDao.deleteWallpaper(wallpaper.wid)
                } else {
                    validWids = validWids.plus(wallpaper.wid)
                }
            }

            // save all wallpapers in new wallpapers to database
            for (wallpaper in newWallpapers) {
                wallpaperDao.saveWallpaper(wallpaper)
            }

            // if savedWids is > 1, replace default wallpaper with saved wallpaper
            if (savedWids.size > 1) {
                // find default wallpaper (wid = 1) and replace it with saved wallpaper
                val default = newWallpapers.find { it.wid == 0 }
                val saved = allWallpapers.find { it.wid == savedWids[1] }
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
            val uniqueWallpaperWids = mutableListOf<Int>()
            val wallpaperIterator = newWallpapers.iterator()
            //for (wallpaper in newWallpapers) {
            while (wallpaperIterator.hasNext()) {
                val wallpaper = wallpaperIterator.next()
                if (!uniqueWallpaperWids.contains(wallpaper.wid)) {
                    uniqueWallpaperWids.add(wallpaper.wid)
                } else {
                    // delete duplicate wallpaper
                    //wallpaperDao.deleteWallpaper(wallpaper.wid)
                    //newWallpapers.remove(wallpaper)
                    wallpaperIterator.remove()
                }
            }

            // sync new wallpapers with local wallpapers list
            wallpapers.postValue(newWallpapers)

            // find active wallpaper from saved and set local variable
            val active = newWallpapers.find { it.wid == wid }
            activeWallpaper.postValue(active!!)
            syncMutex.unlock()
        }
    }

    // delete wallpaper
    fun deleteWallpaper(wid: Int) {
        mScope.launch(Dispatchers.IO) {
            wallpaperDao.deleteWallpaper(wid)
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
    fun saveSwitchWallpaper(
        activeUid: Int,
        activeWid: Int,
        activeConfig: String,
        lastModified: Long
    ) {
        //mScope.launch(Dispatchers.IO) {
        val wallpaper = SavedWallpaperRow(
            activeUid,
            activeWid,
            activeConfig,
            ByteArray(0),
            lastModified
        )
        this.uid = activeUid

        if (wallpapers.value != null) {
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
        @SuppressLint("StaticFieldLeak")
        @Volatile
        private var instance: ActiveWallpaperRepo? = null
        private lateinit var mScope: CoroutineScope
        private val syncMutex: Mutex = Mutex()

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
    fun registerSensors(mSensorManager: SensorManager) {
        this.mSensorManager = mSensorManager
        mSensorManager.registerListener(
            this,
            mSensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR),
            SensorManager.SENSOR_DELAY_GAME
        )
        mSensorManager.registerListener(
            this,
            mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER),
            SensorManager.SENSOR_DELAY_GAME
        )
        mSensorManager.registerListener(
            this,
            mSensorManager.getDefaultSensor(Sensor.TYPE_LINEAR_ACCELERATION),
            SensorManager.SENSOR_DELAY_GAME
        )
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

    fun setProfile(profileTable: ProfileTable) {
        mScope.launch(Dispatchers.IO) {
            currentUserProfile.postValue(profileTable)
            mProfileDao.updateProfileData(profileTable)
        }
    }

    fun updateColor(r: Float, g: Float, b: Float, a: Float) {
        color.value = Color.valueOf(r, g, b, a)
        //color.postValue(Color.valueOf(r, g, b, a))
    }
}

data class Tuple(val wid: Int, val lastModified: Long)