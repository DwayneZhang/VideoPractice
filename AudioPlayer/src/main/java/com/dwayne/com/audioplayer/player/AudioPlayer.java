package com.dwayne.com.audioplayer.player;

import android.text.TextUtils;

import com.dwayne.com.audioplayer.TimeInfoBean;
import com.dwayne.com.audioplayer.listener.OnErrorListener;
import com.dwayne.com.audioplayer.listener.OnLoadListener;
import com.dwayne.com.audioplayer.listener.OnPauseResumeListener;
import com.dwayne.com.audioplayer.listener.OnPreparedListener;
import com.dwayne.com.audioplayer.listener.OnTimeInfoListener;
import com.dwayne.com.audioplayer.log.LogUtil;

/**
 * @author Dwayne
 * @email dev1024@foxmail.com
 * @time 20/11/23 14:12
 * @change
 * @chang time
 * @class describe
 */

public class AudioPlayer {

    static {
        System.loadLibrary("audioplayer-lib");
        System.loadLibrary("avutil-55");
        System.loadLibrary("swresample-2");
        System.loadLibrary("avcodec-57");
        System.loadLibrary("avformat-57");
        System.loadLibrary("swscale-4");
        System.loadLibrary("postproc-54");
        System.loadLibrary("avfilter-6");
        System.loadLibrary("avdevice-57");
    }

    private String source;
    private OnPreparedListener onPreparedListener;
    private OnLoadListener onLoadListener;
    private OnPauseResumeListener onPauseResumeListener;
    private OnTimeInfoListener onTimeInfoListener;
    private OnErrorListener onErrorListener;
    private static TimeInfoBean timeInfoBean;

    public AudioPlayer() {
    }

    public void setSource(String source) {
        this.source = source;
    }

    public void setOnPreparedListener(OnPreparedListener onPreparedListener) {
        this.onPreparedListener = onPreparedListener;
    }

    public void setOnLoadListener(OnLoadListener onLoadListener) {
        this.onLoadListener = onLoadListener;
    }

    public void setOnPauseResumeListener(OnPauseResumeListener onPauseResumeListener) {
        this.onPauseResumeListener = onPauseResumeListener;
    }

    public void setOnTimeInfoListener(OnTimeInfoListener onTimeInfoListener) {
        this.onTimeInfoListener = onTimeInfoListener;
    }

    public void setOnErrorListener(OnErrorListener onErrorListener) {
        this.onErrorListener = onErrorListener;
    }

    public void prepare() {
        if(TextUtils.isEmpty(source)) {
            LogUtil.d("source not be empty!");
            return;
        }
        new Thread(() -> n_prepare(source)).start();
    }

    public void start() {
        if(TextUtils.isEmpty(source)) {
            LogUtil.d("source not be empty!");
            return;
        }

        new Thread(() -> n_start()).start();
    }

    public void pause() {
        n_pause();
        if(onPauseResumeListener != null) {
            onPauseResumeListener.onPause(true);
        }
    }

    public void resume() {
        n_resume();
        if(onPauseResumeListener != null) {
            onPauseResumeListener.onPause(false);
        }
    }

    public void stop() {
        new Thread(() -> n_stop()).start();
    }

    /**
     * 给jni层调用
     */
    public void onCallPrepare() {
        if(onPreparedListener != null) {
            onPreparedListener.onPrepared();
        }
    }

    public void onCallLoad(boolean load) {
        if(onLoadListener != null){
            onLoadListener.onLoad(load);
        }
    }

    public void onTimeInfo(int currentTime, int totalTime) {
        if(onTimeInfoListener != null) {
            if(timeInfoBean == null) {
                timeInfoBean = new TimeInfoBean();
            }
            timeInfoBean.setCurrentTime(currentTime);
            timeInfoBean.setTotalTime(totalTime);
            onTimeInfoListener.onTimeInfo(timeInfoBean);
        }
    }

    public void onCallError(int code, String msg) {
        stop();
        if(onErrorListener != null) {
            onErrorListener.onError(code, msg);
        }
    }

    private native void n_prepare(String  source);
    private native void n_start();
    private native void n_pause();
    private native void n_resume();
    private native void n_stop();

}
