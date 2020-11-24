package com.dwayne.com.audioplayer.log;

import android.util.Log;

import com.dwayne.com.audioplayer.BuildConfig;

/**
 * @author Dwayne
 * @email dev1024@foxmail.com
 * @time 20/11/23 14:30
 * @change
 * @chang time
 * @class describe
 */

public class LogUtil {

    public static void d(String msg) {

        if(!BuildConfig.DEBUG) return;
        Log.d("DZ", msg);
    }
}
