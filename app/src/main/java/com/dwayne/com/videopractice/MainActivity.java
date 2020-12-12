package com.dwayne.com.videopractice;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.dwayne.com.audioplayer.TimeInfoBean;
import com.dwayne.com.audioplayer.log.LogUtil;
import com.dwayne.com.audioplayer.opengl.MyGLSurfaceView;
import com.dwayne.com.audioplayer.player.VideoPlayer;
import com.dwayne.com.audioplayer.util.TimeUtil;

public class MainActivity extends AppCompatActivity {

    private VideoPlayer videoPlayer;
    private TextView tvTime;
    private MyGLSurfaceView myGLSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        tvTime = findViewById(R.id.tv_time);
        myGLSurfaceView = findViewById(R.id.gl_surface_view);

        videoPlayer = new VideoPlayer();
        videoPlayer.setMyGLSurfaceView(myGLSurfaceView);
        videoPlayer.setOnPreparedListener(() -> {
            LogUtil.d("open success");
            videoPlayer.start();
        });
        videoPlayer.setOnLoadListener(load -> LogUtil.d(load ? "loading" : "playing"));
        videoPlayer.setOnPauseResumeListener(pause -> LogUtil.d(pause ? "pause" : "resume"));
        videoPlayer.setOnTimeInfoListener(timeInfoBean -> {
            Message message = Message.obtain();
            message.what = 1;
            message.obj = timeInfoBean;
            handler.sendMessage(message);
        });
        videoPlayer.setOnErrorListener((code, msg)-> LogUtil.e(String.format("error code:%d, msg:%s", code, msg)));
        videoPlayer.setOnCompleteListener(()->LogUtil.d("play complete"));
    }


    public void begin(View view) {
//        audioPlayer.setSource("/storage/emulated/0/Download/dcjlxk.mp3");
        videoPlayer.setSource("/sdcard/Download/夺冠.mp4");
        videoPlayer.prepare();
    }

    public void pause(View view) {
        videoPlayer.pause();
    }

    public void resume(View view) {
        videoPlayer.resume();
    }

    Handler handler = new Handler() {
        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            if(msg.what == 1) {
                TimeInfoBean timeInfoBean = (TimeInfoBean) msg.obj;
                tvTime.setText(String.format("%s/%s",
                        TimeUtil.secdsToDateFormat(timeInfoBean.getCurrentTime()),
                        TimeUtil.secdsToDateFormat(timeInfoBean.getTotalTime())));
            }
        }
    };

    public void stop(View view) {
        videoPlayer.stop();
    }

    public void seek(View view) {
        videoPlayer.seek(195);
    }

    public void next(View view) {
//        audioPlayer.playNext("/storage/emulated/0/Download/dcjlxk.mp3");
    }
}