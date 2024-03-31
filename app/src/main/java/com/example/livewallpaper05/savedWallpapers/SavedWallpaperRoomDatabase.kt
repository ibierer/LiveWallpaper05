package com.example.livewallpaper05.savedWallpapers

import android.content.Context
import androidx.room.Database
import androidx.room.Room
import androidx.room.RoomDatabase
import androidx.sqlite.db.SupportSQLiteDatabase
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch

// create wallpaper database
@Database(entities = [SavedWallpaperRow::class], version = 1, exportSchema = false)
abstract class SavedWallpaperRoomDatabase : RoomDatabase() {

    abstract fun wallpaperDao(): SavedWallpaperDao

    // make db singleton
    companion object {

        @Volatile
        private var mInstance: SavedWallpaperRoomDatabase? = null
        fun getDatabase(
            context: Context
        ): SavedWallpaperRoomDatabase {
            return mInstance?: synchronized(this){
                val instance = Room.databaseBuilder(
                    context.applicationContext,
                    SavedWallpaperRoomDatabase::class.java, "wallpaper.db"
                )
                    .addCallback(RoomDatabaseCallback())
                    .fallbackToDestructiveMigration()
                    .build()
                mInstance = instance
                instance
            }
        }

        private class RoomDatabaseCallback: Callback() {
            override fun onCreate(db: SupportSQLiteDatabase) {
                super.onCreate(db)
                mInstance?.let {database ->
                    CoroutineScope(Dispatchers.Main).launch(Dispatchers.IO) {
                        populateDbTask(database.wallpaperDao())
                    }
                }
            }

            // seed database
            suspend fun populateDbTask(wallpaperDao: SavedWallpaperDao) {
                wallpaperDao.saveWallpaper(SavedWallpaperRow(0, ""))
            }
        }
    }
}