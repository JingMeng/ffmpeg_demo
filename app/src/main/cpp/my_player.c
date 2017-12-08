//
// @author admin on 2017/12/7.
//

#include "com_sinieco_ffmpeg_demo_PlayController.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/log.h>
#include <libavformat/avformat.h>
#include <libyuv.h>




JNIEXPORT void JNICALL Java_com_sinieco_ffmpeg_1demo_PlayController_render
        (JNIEnv *env, jobject jobj, jstring path_jstr, jobject surface){
    const char* path_cstr = (*env)->GetStringUTFChars(env,path_jstr,NULL);

    //1.注册组件
    av_register_all();

    AVFormatContext *pFormatCtx = avformat_alloc_context();
    //2.打开视频文件
    if(avformat_open_input(&pFormatCtx,path_cstr,NULL,NULL) != 0){
        return;
    }

    //3.获取视频信息
    if(avformat_find_stream_info(pFormatCtx,NULL) < 0){
        return;
    }

    //找到视频流的索引
    int video_index = -1 ;
    int i = 0 ;
    for ( ; i < pFormatCtx->nb_streams ; i++) {
        if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
            video_index = i ;
            break;
        }
    }

    //获取解码器上下文
    AVCodecContext *CodecCxt = pFormatCtx->streams[video_index]->codec;
    //4.获取解码器
    AVCodec *pDecoder = avcodec_find_decoder(CodecCxt->codec_id);
    if(pDecoder == NULL){
        return;
    }

    //5.打开解码器
    if(avcodec_open2(CodecCxt,pDecoder,NULL)<0){
        return;
    }

    AVPacket *packet = (AVPacket *)av_malloc(sizeof(AVPacket));

    AVFrame *yuvFrame = av_frame_alloc();
    AVFrame *rgbFrame = av_frame_alloc() ;

    ANativeWindow *window = ANativeWindow_fromSurface(env,surface);
    ANativeWindow_Buffer outBuffer ;

    int len , got_frame ,frame_count = 0 ;
    while(av_read_frame(pFormatCtx,packet) >= 0 ){
        len = avcodec_decode_video2(CodecCxt,yuvFrame,&got_frame,packet);
        if(got_frame){
            ANativeWindow_setBuffersGeometry(window,CodecCxt->width,CodecCxt->height,WINDOW_FORMAT_RGBA_8888);
            ANativeWindow_lock(window,&outBuffer,NULL);
            avpicture_fill((AVPicture *)rgbFrame,outBuffer.bits,PIX_FMT_RGBA,CodecCxt->width,CodecCxt->height);
            I420ToARGB(yuvFrame->data[0],yuvFrame->linesize[0],
                       yuvFrame->data[2],yuvFrame->linesize[2],
                       yuvFrame->data[1],yuvFrame->linesize[1],
                       rgbFrame->data[0],rgbFrame->linesize[0],
                       CodecCxt->width,CodecCxt->height );
            ANativeWindow_unlockAndPost(window);
            usleep(1000*16);
        }
        av_free_packet(packet);
    }

    ANativeWindow_release(window);
    av_frame_free(&yuvFrame);
    avcodec_close(CodecCxt);
    avformat_free_context(pFormatCtx);
    (*env)->ReleaseStringUTFChars(env,path_jstr,path_cstr);

}

