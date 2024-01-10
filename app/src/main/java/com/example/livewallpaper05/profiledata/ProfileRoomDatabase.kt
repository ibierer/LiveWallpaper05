package com.example.livewallpaper05.profiledata

import android.content.Context
import androidx.room.Database
import androidx.room.Room
import androidx.room.RoomDatabase
import kotlinx.coroutines.CoroutineScope
import androidx.sqlite.db.SupportSQLiteDatabase
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
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
                        populateDbTask(database.profileDao())
                    }
                }
            }

            suspend fun populateDbTask(profileDao: ProfileDao) {
                profileDao.updateProfileData(ProfileTable("Dummy_user", "Dummy_Bio"))
            }
        }
    }
}