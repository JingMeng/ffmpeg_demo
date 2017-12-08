package com.sinieco.ffmpeg_demo;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.view.Surface;

/**
 * @author BaiMeng on 2017/12/7.
 */

public class PlayController {

    public native void  render(String videoPath , Surface suface);

    public native void sound(String inputPath , String outPath );

    public AudioTrack createAudioTrack(){
        int sampleRateInHz = 44100 ;
        int audioFormat = AudioFormat.ENCODING_PCM_16BIT ;
        int channelConfig = AudioFormat.CHANNEL_OUT_STEREO;
        int minBufferSize = AudioTrack.getMinBufferSize(sampleRateInHz, channelConfig, audioFormat);
        return new  AudioTrack(AudioManager.STREAM_MUSIC,sampleRateInHz,channelConfig,audioFormat,minBufferSize,AudioTrack.MODE_STREAM);
    }

    static {
        System.loadLibrary("avutil-54");
        System.loadLibrary("swresample-1");
        System.loadLibrary("avcodec-56");
        System.loadLibrary("avformat-56");
        System.loadLibrary("swscale-3");
        System.loadLibrary("postproc-53");
        System.loadLibrary("avfilter-5");
        System.loadLibrary("avdevice-56");
        System.loadLibrary("myplayer");
    }

}
