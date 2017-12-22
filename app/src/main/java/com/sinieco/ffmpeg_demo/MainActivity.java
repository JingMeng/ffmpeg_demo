package com.sinieco.ffmpeg_demo;

import android.Manifest;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Surface;
import android.view.View;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    private VideoView video ;
    private PlayController controller;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        video = (VideoView)findViewById(R.id.video_view);
        XPermissionUtils.requestPermissions(this, 0, new String[]{
                Manifest.permission.READ_EXTERNAL_STORAGE,
                Manifest.permission.WRITE_EXTERNAL_STORAGE
        }, new XPermissionUtils.OnPermissionListener() {
            @Override
            public void onPermissionGranted() {

            }

            @Override
            public void onPermissionDenied() {

            }
        });

        controller = new PlayController();
    }

//    //视频播放
//    public void play(View view){
//        String path = Environment.getExternalStorageDirectory()+File.separator+"input.mp4";
//        Surface surface = video.getHolder().getSurface();
//        controller.render(path,surface);
//    }

    //视频解码
//    public void decode(View view){
//        String inPath = Environment.getExternalStorageDirectory()+ File.separator+"input.mp4";
//        String outPath = Environment.getExternalStorageDirectory()+ File.separator+"output_1280x720_yuv420p.yuv";
//        VideoUtils.decode(inPath,outPath);
//    }


    //音频解码
    public void play(View view){
//        String inPath = Environment.getExternalStorageDirectory()+ File.separator+"input.mp3";
//        String outPath = Environment.getExternalStorageDirectory()+ File.separator+"output.pcm";
//        controller.sound(inPath,outPath);
        String inPath = Environment.getExternalStorageDirectory()+ File.separator+"input.mp4";
        Surface surface = video.getHolder().getSurface();
        controller.play(inPath,surface);
    }


}
