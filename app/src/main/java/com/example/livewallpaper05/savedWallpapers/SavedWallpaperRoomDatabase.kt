package com.example.livewallpaper05.savedWallpapers

import android.content.Context
import androidx.room.Database
import androidx.room.Room
import androidx.room.RoomDatabase
import androidx.sqlite.db.SupportSQLiteDatabase
import com.example.livewallpaper05.savedWallpapers.SavedWallpaperDao
import com.example.livewallpaper05.savedWallpapers.SavedWallpaperTable
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch

// create wallpaper database
@Database(entities = [SavedWallpaperTable::class], version = 1, exportSchema = false)
abstract class SavedWallpaperRoomDatabase : RoomDatabase() {
    abstract fun wallpaperDao(): SavedWallpaperDao

    // make db singleton
    companion object {

        @Volatile
        private var mInstance: SavedWallpaperRoomDatabase? = null
        fun getDatabase(
            context: Context,
            scope: CoroutineScope
        ): SavedWallpaperRoomDatabase {
            return mInstance?: synchronized(this){
                val instance = Room.databaseBuilder(
                    context.applicationContext,
                    SavedWallpaperRoomDatabase::class.java, "wallpaper.db"
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
                        populateDbTask(database.wallpaperDao())
                    }
                }
            }

            // seed database
            suspend fun populateDbTask(wallpaperDao: SavedWallpaperDao) {
                wallpaperDao.saveWallpaper(SavedWallpaperTable(0, ""))
            }
        }
    }
}