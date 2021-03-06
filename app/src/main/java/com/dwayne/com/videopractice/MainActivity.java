package com.dwayne.com.videopractice;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.SeekBar;
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
    private SeekBar seekBar;
    private int position;
    private boolean isSeeking = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        tvTime = findViewById(R.id.tv_time);
        myGLSurfaceView = findViewById(R.id.gl_surface_view);
        seekBar = findViewById(R.id.seek_bar);

        videoPlayer = new VideoPlayer();
        videoPlayer.setMyGLSurfaceView(myGLSurfaceView);
        videoPlayer.setOnPreparedListener(() -> {
            LogUtil.d("open success");
            videoPlayer.start();
        });
        videoPlayer.setOnLoadListener(load -> LogUtil.d(load ? "loading" : "playing"));
        videoPlayer.setOnPauseResumeListener(pause -> LogUtil.d(pause ? "pause" :
                "resume"));
        videoPlayer.setOnTimeInfoListener(timeInfoBean -> {
            Message message = Message.obtain();
            message.what = 1;
            message.obj = timeInfoBean;
            handler.sendMessage(message);
        });
        videoPlayer.setOnErrorListener((code, msg) -> LogUtil.e(String.format("error " +
                "code:%d, msg:%s", code, msg)));
        videoPlayer.setOnCompleteListener(() -> LogUtil.d("play complete"));

        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress,
                                          boolean fromUser) {
                position = progress * videoPlayer.getDuration() / 100;
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                isSeeking = true;
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                videoPlayer.seek(position);
                isSeeking = false;
            }
        });
    }


    public void begin(View view) {
        videoPlayer.setSource("/sdcard/Pictures/夺冠.mp4");
//        videoPlayer.setSource("http://clips.vorwaerts-gmbh.de/big_buck_bunny.mp4");
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
                if(!isSeeking && timeInfoBean.getTotalTime() > 0) {
                    seekBar.setProgress(timeInfoBean.getCurrentTime() * 100 / timeInfoBean.getTotalTime());
                }
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