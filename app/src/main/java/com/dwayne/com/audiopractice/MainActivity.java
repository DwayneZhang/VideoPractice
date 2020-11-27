package com.dwayne.com.audiopractice;

import android.os.Bundle;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;

import com.dwayne.com.audioplayer.log.LogUtil;
import com.dwayne.com.audioplayer.player.AudioPlayer;

public class MainActivity extends AppCompatActivity {

    private com.dwayne.com.audioplayer.player.AudioPlayer audioPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        audioPlayer = new AudioPlayer();
        audioPlayer.setOnPreparedListener(() -> {
            LogUtil.d("open success");
            audioPlayer.start();
        });
    }


    public void begin(View view) {
//        audioPlayer.setSource("/storage/emulated/0/Download/dcjlxk.mp3");
        audioPlayer.setSource("http://www.170mv.com/kw/antiserver.kuwo.cn/anti.s?rid=MUSIC_90991360&response=res&format=mp3|aac&type=convert_url&br=128kmp3&agent=iPhone&callback=getlink&jpcallback");
        audioPlayer.prepare();
    }
}