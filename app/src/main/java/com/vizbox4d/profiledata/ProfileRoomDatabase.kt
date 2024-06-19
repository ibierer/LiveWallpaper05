package com.vizbox4d.profiledata
//import android.content.Context
//import android.util.Log
import androidx.room.Database
//import androidx.room.Room
import androidx.room.RoomDatabase
//import androidx.room.TypeConverter
//import androidx.room.TypeConverters
//import kotlinx.coroutines.CoroutineScope
//import androidx.sqlite.db.SupportSQLiteDatabase
//import kotlinx.coroutines.Dispatchers
//import kotlinx.coroutines.launch
//import java.sql.DriverManager
//import java.time.LocalDateTime
//import kotlin.jvm.Volatile

@Database(entities = [ProfileTable::class], version = 3, exportSchema = false)
abstract class ProfileRoomDatabase : RoomDatabase() {
    abstract fun profileDao(): ProfileDao

    // make db singleton
    companion object {

        //@Volatile
        //private var mInstance: ProfileRoomDatabase? = null
        //fun getDatabase(
        //    context: Context,
        //    scope: CoroutineScope
        //): ProfileRoomDatabase {
        //    return mInstance?: synchronized(this){
        //        val instance = Room.databaseBuilder(
        //            context.applicationContext,
        //            ProfileRoomDatabase::class.java, "profile.db"
        //        )
        //            .addCallback(RoomDatabaseCallback(scope))
        //            .fallbackToDestructiveMigration()
        //            .build()
        //        mInstance = instance
        //        instance
        //    }
        //}
        //
        //private class RoomDatabaseCallback(
        //    private val scope: CoroutineScope
        //): Callback() {
        //    override fun onCreate(db: SupportSQLiteDatabase) {
        //        super.onCreate(db)
        //        mInstance?.let {database ->
        //            scope.launch(Dispatchers.IO) {
        //                populateDbTask(database.profileDao())
        //            }
        //        }
        //    }
        //
        //    // seed database
        //    fun populateDbTask(profileDao: ProfileDao) {
        //        val tmpImg = ByteArray(0)
        //        profileDao.updateProfileData(ProfileTable(0, 0,"Dummy_User", "Hello World!", tmpImg))
        //    }
        //}
    }
}