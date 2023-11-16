package com.example.livewallpaper05

import android.os.Parcel
import android.os.Parcelable

class WallpaperRepo() : Parcelable  {
    var rotationRate: Float = 0.0f

    constructor(parcel: Parcel) : this() {
        rotationRate = parcel.readFloat()
    }

    override fun writeToParcel(parcel: Parcel, flags: Int) {
        parcel.writeFloat(rotationRate)
    }

    override fun describeContents(): Int {
        return 0
    }

    companion object CREATOR : Parcelable.Creator<WallpaperRepo> {
        override fun createFromParcel(parcel: Parcel): WallpaperRepo {
            return WallpaperRepo(parcel)
        }

        override fun newArray(size: Int): Array<WallpaperRepo?> {
            return arrayOfNulls(size)
        }
    }


}