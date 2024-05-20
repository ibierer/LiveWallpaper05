package com.example.livewallpaper05.activewallpaperdata

import android.graphics.Color
import org.json.JSONObject

abstract class Visualization {
    var viewModel: ActiveWallpaperViewModel? = null

    abstract val relevantTextViewIds: List<Int>
    abstract val relevantSeekBarIds: List<Int>
    abstract val relevantEditTextIds: List<Int>
    abstract val relevantCheckBoxIds: List<Int>
    abstract val relevantButtonIds: List<Int>
    abstract val relevantSpinnerIds: List<Int>
    abstract fun toJsonObject(): JSONObject
    fun colorToJSONObject(color: Color): JSONObject {
        val colorJSONObject = JSONObject()
        colorJSONObject.put("r", (color.red() * 255.0f).toInt())
        colorJSONObject.put("g", (color.green() * 255.0f).toInt())
        colorJSONObject.put("b", (color.blue() * 255.0f).toInt())
        colorJSONObject.put("a", (color.alpha() * 255.0f).toInt())
        return colorJSONObject
    }
    fun jsonObjectToColor(colorJSONObject: JSONObject): Color {
        val r = colorJSONObject.getDouble("r").toFloat()
        val g = colorJSONObject.getDouble("g").toFloat()
        val b = colorJSONObject.getDouble("b").toFloat()
        val a = colorJSONObject.getDouble("a").toFloat()
        return Color.valueOf(r / 255, g / 255, b / 255, a / 255)
    }

    companion object {
        fun createColorFromInts(r: Int, g: Int, b: Int, a: Int): Color {
            val rf = r.toFloat() / 255.0f
            val gf = g.toFloat() / 255.0f
            val bf = b.toFloat() / 255.0f
            val af = a.toFloat() / 255.0f
            return Color.valueOf(rf, gf, bf, af)
        }
        fun jsonObjectToColor(colorJSONObject: JSONObject): Color {
            val r = colorJSONObject.getDouble("r").toFloat()
            val g = colorJSONObject.getDouble("g").toFloat()
            val b = colorJSONObject.getDouble("b").toFloat()
            val a = colorJSONObject.getDouble("a").toFloat()
            return Color.valueOf(r/255, g/255, b/255, a/255)
        }
    }
}