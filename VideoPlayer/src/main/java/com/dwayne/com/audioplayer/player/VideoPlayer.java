package com.dwayne.com.audioplayer.player;

import android.text.TextUtils;

import com.dwayne.com.audioplayer.TimeInfoBean;
import com.dwayne.com.audioplayer.listener.OnCompleteListener;
import com.dwayne.com.audioplayer.listener.OnErrorListener;
import com.dwayne.com.audioplayer.listener.OnLoadListener;
import com.dwayne.com.audioplayer.listener.OnPauseResumeListener;
import com.dwayne.com.audioplayer.listener.OnPreparedListener;
import com.dwayne.com.audioplayer.listener.OnTimeInfoListener;
import com.dwayne.com.audioplayer.log.LogUtil;
import com.dwayne.com.audioplayer.opengl.MyGLSurfaceView;
import com.dwayne.com.audioplayer.util.VideoSupportUtil;

/**
 * @author Dwayne
 * @email dev1024@foxmail.com
 * @time 20/11/23 14:12
 * @change
 * @chang time
 * @class describe
 */

public class VideoPlayer {

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

    private static String source;
    private static boolean playNext;
    private OnPreparedListener onPreparedListener;
    private OnLoadListener onLoadListener;
    private OnPauseResumeListener onPauseResumeListener;
    private OnTimeInfoListener onTimeInfoListener;
    private OnErrorListener onErrorListener;
    private OnCompleteListener onCompleteListener;
    private static TimeInfoBean timeInfoBean;
    private MyGLSurfaceView myGLSurfaceView;
    private int duration = 0;

    public VideoPlayer() {
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

    public void setOnCompleteListener(OnCompleteListener onCompleteListener) {
        this.onCompleteListener = onCompleteListener;
    }

    public void setMyGLSurfaceView(MyGLSurfaceView myGLSurfaceView) {
        this.myGLSurfaceView = myGLSurfaceView;
    }

    public int getDuration() {
        return duration;
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
        timeInfoBean = null;
        duration = 0;
        new Thread(() -> n_stop()).start();
    }

    public void seek(int secds) {
        n_seek(secds);
    }

    public void playNext(String url) {
        source = url;
        playNext = true;
        stop();
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
        if(onLoadListener != null) {
            onLoadListener.onLoad(load);
        }
    }

    public void onCallTimeInfo(int currentTime, int totalTime) {
        if(onTimeInfoListener != null) {
            if(timeInfoBean == null) {
                timeInfoBean = new TimeInfoBean();
            }
            duration = totalTime;
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

    public void onCallComplete() {
        stop();
        if(onCompleteListener != null) {
            onCompleteListener.onComplete();
        }
    }

    public void onCallNext() {
        if(playNext) {
            playNext = false;
            prepare();
        }
    }

    public void onCallRenderYUV(int width, int height, byte[] y, byte[] u, byte[] v) {
        if(myGLSurfaceView != null) {
            myGLSurfaceView.setYUVData(width, height, y, u, v);
        }
    }

    public boolean onCallSupportMediaCodec(String ffcodecname) {
        return VideoSupportUtil.isSupportCodec(ffcodecname);
    }

    private native void n_prepare(String source);

    private native void n_start();

    private native void n_pause();

    private native void n_resume();

    private native void n_stop();

    private native void n_seek(int secds);

}
