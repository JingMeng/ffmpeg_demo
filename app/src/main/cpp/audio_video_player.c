//
// @author admin on 2017/12/22.
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <android/log.h>
#include "com_sinieco_ffmpeg_demo_PlayController.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libyuv.h"
#include <pthread.h>
#include <android/native_window_jni.h>
//流的个数，视频流，音频流，字幕流... ,这里只考虑视频和音频
#define MAX_STREAM 2
//视频线程和视频解码器上下文在数组中的索引位置
#define VIDEO_IN_ARRAY_POSITION 0
//音频线程和音频解码器上下文在数组中的索引位置
#define AUDIO_IN_ARRAY_POSITION 1

//将一些通用的变量封装成结构体
struct Player {
    //封装格式上下文
    AVFormatContext *input_format_ctx ;
    //视频流的索引
    int video_stream_index ;
    //音频流的索引
    int audio_stream_index ;
    //解码器上下文数组，包括视频解码器和音频解码器
    AVCodecContext *input_codec_ctx[MAX_STREAM];
    //解码线程的数组，包括视频解码线程和音频解码线程
    pthread_t decode_thread[MAX_STREAM];
    ANativeWindow *nativeWindow ;
};

void init_format_ctx(struct Player *player ,const char* path){
    av_register_all();
    AVFormatContext *format_cxt = avformat_alloc_context();
    //打开输入视频
    if(avformat_open_input(&format_cxt,path,NULL,NULL)!=0){
        return;
    }
    //获取视频信息
    if(avformat_find_stream_info(format_cxt,NULL) < 0){
        return;
    }
    //找到视频流和音频流的索引位置
    int i = 0 ;
    for(;i<format_cxt->nb_streams;i++){
        if(format_cxt->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
            player->audio_stream_index = i ;
        } else if (format_cxt->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
            player->video_stream_index = i ;
        }
    }
    player->input_format_ctx = format_cxt ;
}

void init_decode_ctx(struct Player *player ,int index ){
    AVFormatContext *format_ctx = player->input_format_ctx;
    //获取视频解码器上下文
    AVCodecContext *codec_ctx = format_ctx->streams[player->video_stream_index]->codec;
    AVCodec* codec = avcodec_find_decoder(codec_ctx->codec_id);
    if(codec_ctx == NULL){
        return;
    }
    if(avcodec_open2(codec_ctx,codec,NULL)<0){
        return;
    }
    player->input_codec_ctx[index] = codec_ctx ;
}

void decode_video_prepare(JNIEnv *env,struct Player *player , jobject surface){
    player->nativeWindow = ANativeWindow_fromSurface(env,surface);
}

void decode_video(struct Player *player , AVPacket *packet){
    AVFrame *yuv_frame = av_frame_alloc();
    AVFrame *rgb_frame = av_frame_alloc();
    ANativeWindow_Buffer buffer ;
    AVCodecContext *codec_ctx = player->input_codec_ctx[VIDEO_IN_ARRAY_POSITION];
    int got_frame ;
    avcodec_decode_video2(codec_ctx,yuv_frame,&got_frame,packet);
    if(got_frame){
        __android_log_print(ANDROID_LOG_INFO,"打印-----","%s","???????????????????????");
        ANativeWindow_setBuffersGeometry(player->nativeWindow,codec_ctx->width,codec_ctx->height,WINDOW_FORMAT_RGBA_8888);
        ANativeWindow_lock(player->nativeWindow,&buffer,NULL);
        avpicture_fill((AVPicture *)rgb_frame,buffer.bits,AV_PIX_FMT_RGBA,codec_ctx->width,codec_ctx->height);
        I420ToABGR(yuv_frame->data[0],yuv_frame->linesize[0],
                    yuv_frame->data[2],yuv_frame->linesize[2],
                    yuv_frame->data[1],yuv_frame->linesize[1],
                    rgb_frame->data[0],rgb_frame->linesize[0],
                    codec_ctx->width,codec_ctx->height);
        ANativeWindow_unlockAndPost(player->nativeWindow);
        usleep(1000*16);
    }
    av_frame_free(&yuv_frame);
    av_frame_free(&rgb_frame);
}

void* play_thread(void* args){
    struct Player *player = (struct Player*) args ;

    AVFormatContext *format_ctx = player->input_format_ctx;
    AVPacket *packet = (AVPacket*)av_malloc(sizeof(AVPacket));
    int video_fram_count = 0 ;
    while (av_read_frame(format_ctx,packet) >= 0){
        if(packet->stream_index == player->video_stream_index){
            decode_video(player,packet);
        }
        av_free_packet(packet);
    }
}

JNIEXPORT void JNICALL Java_com_sinieco_ffmpeg_1demo_PlayController_play
        (JNIEnv *env, jobject jobj, jstring input_path, jobject surface){
    const char* input_path_cstr = (*env)->GetStringUTFChars(env,input_path,NULL);
    struct Player *player = (struct Player*) malloc(sizeof(struct Player));
    //初始化封装格式上下文，打开视频文件获取视频信息
    init_format_ctx(player,input_path_cstr);
    //获取视频和音频解码器上下文和解码器
    init_decode_ctx(player,VIDEO_IN_ARRAY_POSITION);
    init_decode_ctx(player,AUDIO_IN_ARRAY_POSITION);
    //播放视频前的准备工作
    decode_video_prepare(env,player,surface);
    //创建播放视频的线程
    pthread_create(&(player->decode_thread[VIDEO_IN_ARRAY_POSITION]),NULL,play_thread,(void*) player);
    //

//    (*env)->ReleaseStringUTFChars(env,input_path,input_path_cstr);
}
