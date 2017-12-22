package com.sinieco.ffmpeg_demo;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.PixelFormat;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
 * @author BaiMeng on 2017/12/7.
 */

public class VideoView extends SurfaceView {
    public VideoView(Context context) {
        this(context,null);
    }

    public VideoView(Context context, AttributeSet attrs) {
        this(context, attrs ,0);
    }

    public VideoView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
        Bitmap bitmap = BitmapFactory.decodeFile(null,null);
    }

    private void init() {
        SurfaceHolder holder = getHolder();
        holder.setFormat(PixelFormat.RGBA_8888);
    }
}
