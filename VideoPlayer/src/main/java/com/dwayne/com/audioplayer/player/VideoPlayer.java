package com.dwayne.com.audioplayer.player;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.text.TextUtils;
import android.view.Surface;

import com.dwayne.com.audioplayer.TimeInfoBean;
import com.dwayne.com.audioplayer.listener.OnCompleteListener;
import com.dwayne.com.audioplayer.listener.OnErrorListener;
import com.dwayne.com.audioplayer.listener.OnLoadListener;
import com.dwayne.com.audioplayer.listener.OnPauseResumeListener;
import com.dwayne.com.audioplayer.listener.OnPreparedListener;
import com.dwayne.com.audioplayer.listener.OnTimeInfoListener;
import com.dwayne.com.audioplayer.log.LogUtil;
import com.dwayne.com.audioplayer.opengl.MyGLSurfaceView;
import com.dwayne.com.audioplayer.opengl.MyRender;
import com.dwayne.com.audioplayer.util.VideoSupportUtil;

import java.io.IOException;
import java.nio.ByteBuffer;

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
    private MediaFormat mediaFormat;
    private MediaCodec mediaCodec;
    private Surface surface;
    private MediaCodec.BufferInfo bufferInfo;

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
        myGLSurfaceView.getMyRender().setOnSurfaceCreateListener(s -> {
            if(surface == null) {
                surface = s;
                LogUtil.d("onSurfaceCreate");
            }
        });
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
        new Thread(() -> {
            n_stop();
            releaseMediaCodec();
        }).start();
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
            myGLSurfaceView.getMyRender().setRenderType(MyRender.RENDER_YUV);
            myGLSurfaceView.setYUVData(width, height, y, u, v);
        }
    }

    public boolean onCallSupportMediaCodec(String ffcodecname) {
        return VideoSupportUtil.isSupportCodec(ffcodecname);
    }

    /**
     * 初始化MediaCodec
     *
     * @param codecName
     * @param width
     * @param height
     * @param csd_0
     * @param csd_1
     */
    public void initMediaCodec(String codecName, int width, int height, byte[] csd_0,
                               byte[] csd_1) {
        if(surface != null) {
            try {
                myGLSurfaceView.getMyRender().setRenderType(MyRender.RENDER_MEDIACODEC);
                String mime = VideoSupportUtil.findVideoCodecName(codecName);
                mediaFormat = MediaFormat.createVideoFormat(mime, width, height);
                mediaFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, width * height);
                mediaFormat.setByteBuffer("csd-0", ByteBuffer.wrap(csd_0));
                mediaFormat.setByteBuffer("csd-1", ByteBuffer.wrap(csd_1));
                LogUtil.d("mediaFormat---->" + mediaFormat.toString());

                mediaCodec = MediaCodec.createDecoderByType(mime);
                bufferInfo = new MediaCodec.BufferInfo();
                mediaCodec.configure(mediaFormat, surface, null, 0);
                mediaCodec.start();
            } catch(Exception e) {
                e.printStackTrace();
            }
        } else {
            if(onErrorListener != null) {
                onErrorListener.onError(2001, "surface is null");
            }
        }

    }

    public void decodeAVPacket(int dataSize, byte[] data) {
        if(surface != null && dataSize > 0 && data != null && mediaCodec != null) {
            try {
                int inputBufferIndex = mediaCodec.dequeueInputBuffer(10);
                if(inputBufferIndex >= 0) {
                    ByteBuffer byteBuffer = mediaCodec.getInputBuffers()[inputBufferIndex];
                    byteBuffer.clear();
                    byteBuffer.put(data);
                    mediaCodec.queueInputBuffer(inputBufferIndex, 0, dataSize, 0, 0);
                }
                int outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 10);
                while(outputBufferIndex >= 0) {
                    mediaCodec.releaseOutputBuffer(outputBufferIndex, true);
                    outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 10);
                }
            } catch(Exception e) {
                e.printStackTrace();
            }

        }
    }

    private void releaseMediaCodec() {
        if(mediaCodec != null) {
            try {
                mediaCodec.flush();
                mediaCodec.stop();
                mediaCodec.release();
            } catch(Exception e) {
                e.printStackTrace();
            }

            mediaCodec = null;
            mediaFormat = null;
            bufferInfo = null;
        }
    }

    private native void n_prepare(String source);

    private native void n_start();

    private native void n_pause();

    private native void n_resume();

    private native void n_stop();

    private native void n_seek(int secds);

}
