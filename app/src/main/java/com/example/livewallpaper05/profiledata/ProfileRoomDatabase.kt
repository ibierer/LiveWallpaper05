package com.example.livewallpaper05.profiledata

import android.content.Context
import androidx.room.Database
import androidx.room.Room
import androidx.room.RoomDatabase
import kotlinx.coroutines.CoroutineScope
import androidx.sqlite.db.SupportSQLiteDatabase
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import java.sql.DriverManager
import kotlin.jvm.Volatile

@Database(entities = [ProfileTable::class], version = 1, exportSchema = false)
abstract class ProfileRoomDatabase : RoomDatabase() {
    abstract fun profileDao(): ProfileDao

    // make db singleton
    companion object {

        @Volatile
        private var mInstance: ProfileRoomDatabase? = null
        fun getDatabase(
            context: Context,
            scope: CoroutineScope
        ): ProfileRoomDatabase {
            return mInstance?: synchronized(this){
                val instance = Room.databaseBuilder(
                    context.applicationContext,
                    ProfileRoomDatabase::class.java, "profile.db"
                )
                    .addCallback(RoomDatabaseCallback(scope))
                    .fallbackToDestructiveMigration()
                    .build()
                mInstance = instance
                instance
            }
        }

        private class RoomDatabaseCallback(
            private val scope: CoroutineScope
        ): RoomDatabase.Callback() {
            override fun onCreate(db: SupportSQLiteDatabase) {
                super.onCreate(db)
                mInstance?.let {database ->
                    scope.launch(Dispatchers.IO) {
                        //database.profileDao().getUserProfileData()
                        populateDbTask(database.profileDao())
                    }
                }
            }

            // seed database
            suspend fun populateDbTask(profileDao: ProfileDao) {
                val tmpImg = ByteArray(0)
                val url = "jdbc:postgresql://localhost:5432/example"
                val connection = DriverManager
                    .getConnection(url, "postgres", "postgres")

                // check connection is valid here
                println(connection.isValid(0))

                val query = connection.prepareStatement("SELECT * FROM users WHERE username = test")
                val result = query.executeQuery()
                profileDao.updateProfileData(ProfileTable(result.getInt("uid"), result.getString("username"), result.getString("name"),
                    result.getString("bio"), result.getString("date_created"), result.getBytes("profile_pic")))
            }
        }
    }
}