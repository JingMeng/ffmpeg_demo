package com.sinieco.ffmpeg_demo;

import android.Manifest;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
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
    }

    public void decode(View view){
        String inPath = Environment.getExternalStorageDirectory()+ File.separator+"input.mp4";
        String outPath = Environment.getExternalStorageDirectory()+ File.separator+"720x1280_output.yuv";
        VideoUtils.decode(inPath,outPath);
    }


}
