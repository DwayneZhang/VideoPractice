package com.dwayne.com.audiopractice;

import android.os.Bundle;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

    private com.dwayne.com.audioplayer.player.AudioPlayer audioPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        audioPlayer = new com.dwayne.com.audioplayer.player.AudioPlayer();
    }


    public void init(View view) {
        audioPlayer.setSource("http://ep.sycdn.kuwo.cn/b438beb2dca7993705fa23bdf997679d" +
                "/5fbb561b/resource/n1/13/34/1474260193.mp3");
        audioPlayer.init();
    }
}