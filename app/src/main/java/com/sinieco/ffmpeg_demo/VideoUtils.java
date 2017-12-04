package com.sinieco.ffmpeg_demo;

/**
 * @author BaiMeng on 2017/12/4.
 */

public class VideoUtils {

    static{
        System.loadLibrary("avutil-54");
        System.loadLibrary("swresample-1");
        System.loadLibrary("avcodec-56");
        System.loadLibrary("avformat-56");
        System.loadLibrary("swscale-3");
        System.loadLibrary("postproc-53");
        System.loadLibrary("avfilter-5");
        System.loadLibrary("avdevice-56");
        System.loadLibrary("myffmpeg");
    }

    public native static void decode(String inPath ,String outPath);

}
