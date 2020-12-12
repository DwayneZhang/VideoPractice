package com.dwayne.com.audioplayer.opengl;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

/**
 * @author Dwayne
 * @email dev1024@foxmail.com
 * @time 20/12/12 15:31
 * @change
 * @chang time
 * @class describe
 */

public class MyGLSurfaceView extends GLSurfaceView {

    private MyRender myRender;


    public MyGLSurfaceView(Context context) {
        this(context, null);
    }

    public MyGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        setEGLContextClientVersion(2);
        myRender = new MyRender(context);
        setRenderer(myRender);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }

    public void setYUVData(int width, int height, byte[] y, byte[] u, byte[] v) {
        if(myRender != null) {
            myRender.setYUVRenderData(width, height, y, u, v);
            requestRender();
        }
    }
}
