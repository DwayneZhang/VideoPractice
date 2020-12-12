package com.dwayne.com.audioplayer.opengl;

import android.content.Context;
import android.opengl.GLES20;

import com.dwayne.com.audioplayer.log.LogUtil;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;

/**
 * @author Dwayne
 * @email dev1024@foxmail.com
 * @time 20/12/12 15:40
 * @change
 * @chang time
 * @class describe
 */

public class LoadShaderUtil {

    public static String readRawTxt(Context context, int rawId) {
        InputStream inputStream = context.getResources().openRawResource(rawId);
        BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream));
        StringBuffer sb = new StringBuffer();
        String line;
        try {
            while((line = reader.readLine()) != null) {
                sb.append(line).append("\n");
            }
            reader.close();
        } catch(Exception e) {
            e.printStackTrace();
        }
        return sb.toString();
    }

    public static int loadShader(int shaderType, String source) {
        int shader = GLES20.glCreateShader(shaderType);
        if(shader != 0) {
            GLES20.glShaderSource(shader, source);
            GLES20.glCompileShader(shader);
            int[] compile = new int[1];
            GLES20.glGetShaderiv(shader, GLES20.GL_COMPILE_STATUS, compile, 0);
            if(compile[0] != GLES20.GL_TRUE) {
                LogUtil.d("shader compile error");
                GLES20.glDeleteShader(shader);
                shader = 0;
            }
        }
        return shader;
    }

    public static int createProgram(String vertexSource, String fragmentSource) {
        // 加载顶点着色器
        int vertexShader = loadShader(GLES20.GL_VERTEX_SHADER, vertexSource);
        if(vertexShader == 0) {
            return 0;
        }
        // 加载片元着色器
        int fragmentShader = loadShader(GLES20.GL_FRAGMENT_SHADER, fragmentSource);
        if(fragmentShader == 0) {
            return 0;
        }
        // 创建着色器程序
        int program = GLES20.glCreateProgram();
        // 若程序创建成功则向程序中加入顶点着色器与片元着色器
        if(program != 0) {
            // 向程序中加入顶点着色器
            GLES20.glAttachShader(program, vertexShader);
            // 向程序中加入片元着色器
            GLES20.glAttachShader(program, fragmentShader);
            // 链接程序
            GLES20.glLinkProgram(program);
            // 存放链接成功program数量的数组
            int[] linsStatus = new int[1];
            // 获取program的链接情况
            GLES20.glGetProgramiv(program, GLES20.GL_LINK_STATUS, linsStatus, 0);
            // 若链接失败则报错并删除程序
            if(linsStatus[0] != GLES20.GL_TRUE) {
                LogUtil.d("link program error");
                GLES20.glDeleteProgram(program);
                program = 0;
            }
        }

        // 释放shader资源
        GLES20.glDeleteShader(vertexShader );
        GLES20.glDeleteShader(fragmentShader);
        return program;

    }
}
