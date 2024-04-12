package com.example.livewallpaper05.helpful_fragments

import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.constraintlayout.motion.widget.MotionLayout
import androidx.fragment.app.Fragment
import com.example.livewallpaper05.ExplorerActivity
import com.example.livewallpaper05.HelpActivity
import com.example.livewallpaper05.PreviewActivity
import com.example.livewallpaper05.ProfileActivity
import com.example.livewallpaper05.R
import com.google.android.material.floatingactionbutton.FloatingActionButton

/**
 * A simple [Fragment] subclass.
 * Use the [Toolbar.newInstance] factory method to
 * create an instance of this fragment.
 */
class Toolbar : Fragment() {
    private var mToolbarAnimate: MotionLayout? = null
    private var mOpenToolbar: Boolean = false
    private var mBExplore: FloatingActionButton? = null
    private var mBProfile: FloatingActionButton? = null
    private var mBEditor: FloatingActionButton? = null
    private var mBHelp: FloatingActionButton? = null
    private var mBExpand: FloatingActionButton? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
    }

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        var rootview = inflater.inflate(R.layout.fragment_toolbar, container, false)

        // setup view vars
        mToolbarAnimate = rootview?.findViewById(R.id.motion_toolbar)
        mBExplore = rootview?.findViewById(R.id.b_explore_toolbar)
        mBProfile = rootview?.findViewById(R.id.b_profile_toolbar)
        mBEditor = rootview?.findViewById(R.id.b_editor_toolbar)
        mBHelp = rootview?.findViewById(R.id.b_help_toolbar)
        mBExpand = rootview?.findViewById(R.id.b_expand_toolbar)

        // set onClick listeners
        mBExplore?.setOnClickListener { onClick(R.id.b_explore_toolbar) }
        mBProfile?.setOnClickListener { onClick(R.id.b_profile_toolbar) }
        mBEditor?.setOnClickListener { onClick(R.id.b_editor_toolbar) }
        mBHelp?.setOnClickListener { onClick(R.id.b_help_toolbar) }
        mBExpand?.setOnClickListener { onClick(R.id.b_expand_toolbar) }

        // Inflate the layout for this fragment
        return rootview
    }

    private fun onClick(view: Int) {
        // get name of parent activity
        val parent = context as Context
        // disable button leading to parent
        when (parent) {
            is PreviewActivity -> {
                mBEditor?.isEnabled = false
            }
            is ExplorerActivity -> {
                mBExplore?.isEnabled = false
            }
            is ProfileActivity -> {
                mBProfile?.isEnabled = false
            }
            is HelpActivity -> {
                mBHelp?.isEnabled = false
            }
        }

        when (view) {
            R.id.b_expand_toolbar -> {
                if (mOpenToolbar) {
                    mToolbarAnimate?.transitionToStart()
                    mOpenToolbar = false
                } else {
                    mToolbarAnimate?.transitionToEnd()
                    mOpenToolbar = true
                }
            }

            R.id.b_explore_toolbar -> {
                val exploreIntent = Intent(activity, ExplorerActivity::class.java)
                startActivity(exploreIntent)
            }

            R.id.b_editor_toolbar -> {
                val editorIntent = Intent(activity, PreviewActivity::class.java)
                startActivity(editorIntent)
            }

            R.id.b_profile_toolbar -> {
                val profileIntent = Intent(activity, ProfileActivity::class.java)
                startActivity(profileIntent)
            }

            R.id.b_help_toolbar -> {
                Log.d("Toolbar", "In help toolbar!!")
                val helpIntent = Intent(activity, HelpActivity::class.java)
                startActivity(helpIntent)
            }
        }
    }

}