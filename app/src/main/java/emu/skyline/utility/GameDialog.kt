/*
 * SPDX-License-Identifier: LGPL-3.0-or-later
 * Copyright © 2020 Skyline Team and Contributors (https://github.com/skyline-emu/)
 */

package emu.skyline.utility

import android.content.ComponentName
import android.content.Intent
import android.content.pm.ShortcutInfo
import android.content.pm.ShortcutManager
import android.graphics.drawable.Icon
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.DialogFragment
import emu.skyline.EmulationActivity
import emu.skyline.R
import emu.skyline.adapter.AppItem
import kotlinx.android.synthetic.main.game_dialog.*

class GameDialog() : DialogFragment() {
    var item: AppItem? = null

    constructor(item: AppItem) : this() {
        this.item = item
    }

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        return requireActivity().layoutInflater.inflate(R.layout.game_dialog, container)
    }

    override fun onActivityCreated(savedInstanceState: Bundle?) {
        super.onActivityCreated(savedInstanceState)
        if (item is AppItem) {
            game_icon.setImageBitmap(item?.icon)
            game_title.text = item?.title
            game_subtitle.text = item?.subTitle
            val shortcutManager = activity?.getSystemService(ShortcutManager::class.java)!!
            game_pin.isEnabled = shortcutManager.isRequestPinShortcutSupported
            game_pin.setOnClickListener {
                val info = ShortcutInfo.Builder(context, item?.title)
                info.setShortLabel(item?.meta?.name!!)
                info.setActivity(ComponentName(context!!, EmulationActivity::class.java))
                info.setIcon(Icon.createWithBitmap(item?.icon))
                val intent = Intent(context, EmulationActivity::class.java)
                intent.data = item?.uri
                intent.action = Intent.ACTION_VIEW
                info.setIntent(intent)
                shortcutManager.requestPinShortcut(info.build(), null)
            }
            game_play.setOnClickListener {
                val intent = Intent(activity, EmulationActivity::class.java)
                intent.data = item?.uri
                startActivity(intent)
            }
        } else
            activity?.supportFragmentManager?.beginTransaction()?.remove(this)?.commit()
    }
}
