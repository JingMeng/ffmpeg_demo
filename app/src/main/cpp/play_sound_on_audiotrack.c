//
// @author admin on 2017/12/11.
//

#include "com_sinieco_ffmpeg_demo_PlayController.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include <stdlib.h>
#include <stdio.h>
#include <jni.h>
#include <android/log.h>
#include <unistd.h>

#define MAX_AUDIO_FRAME_SIZE 48000 * 4
JNIEXPORT void JNICALL Java_com_sinieco_ffmpeg_1demo_PlayController_sound
        (JNIEnv *env, jobject jobj, jstring input_path, jstring output_path){
    const char* input_cstr = (*env)->GetStringUTFChars(env,input_path,NULL);
    const char* output_cstr = (*env)->GetStringUTFChars(env,output_path,NULL);

    av_register_all();

    AVFormatContext *pFormatCtx = avformat_alloc_context();

    //1.打开视频文件
    if(avformat_open_input(&pFormatCtx,input_cstr,NULL,NULL) != 0){
        return;
    }

    //2.获取音频信息
    if(avformat_find_stream_info(pFormatCtx,NULL) < 0){
        return;
    }

    int i = 0 , audio_index = -1 ;
    for(;i < pFormatCtx->nb_streams;i++){
        if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
            audio_index = i ;
            break ;
        }
    }

    AVCodecContext *pCodecCtx = pFormatCtx->streams[audio_index]->codec;
    //获取解码器
    AVCodec *pDecoder = avcodec_find_decoder(pCodecCtx->codec_id);
    if(pDecoder == NULL){
        return;
    }
    //打开解码器
    if(avcodec_open2(pCodecCtx,pDecoder,NULL)<0){
        return;
    }

    AVPacket *packet = (AVPacket*)av_malloc(sizeof(AVPacket));
    AVFrame *frame = av_frame_alloc();

    SwrContext *swrCtx = swr_alloc();
    //默认16bit 44100 PCM 统一音频采样格式和采样率
    //输入的采样格式
    enum AVSampleFormat input_sample_format = pCodecCtx->sample_fmt ;
    //输出的采样格式
    enum AVSampleFormat output_sample_format = AV_SAMPLE_FMT_S16 ;
    //输入的采样率
    int input_sample_rate = pCodecCtx->sample_rate ;
    //输出的采样率
    int output_sample_rate = input_sample_rate ;
    //输入的声道布局
    uint64_t input_channel_layout = pCodecCtx->channel_layout;
    uint64_t  output_channel_layout = AV_CH_LAYOUT_STEREO ;
    //设置参数
    swr_alloc_set_opts(swrCtx,output_channel_layout,output_sample_format,output_sample_rate,
            input_channel_layout,input_sample_format,input_sample_rate,0,NULL);

    swr_init(swrCtx);

    int out_channel_nb = av_get_channel_layout_nb_channels(output_channel_layout);

    //JNI 调用java函数
    //获取PlayController的类对象
    jclass controller_class = (*env)->GetObjectClass(env,jobj);
    //通过方法签名(javap -s 字节码文件可获取)，获取createAudioTrack方法的id
    jmethodID method_ID = (*env)->GetMethodID(env,controller_class,"createAudioTrack","(II)Landroid/media/AudioTrack;");
    //调用createAudioTrack方法,获取AudioTrack方法
    jobject audio_track = (*env)->CallObjectMethod(env,jobj,method_ID,output_sample_rate,out_channel_nb);

    //调用AudioTrack的play方法
    jclass audiotrack_class = (*env)->GetObjectClass(env,audio_track);
    jmethodID play_ID = (*env)->GetMethodID(env,audiotrack_class,"play","()V");
    (*env)->CallVoidMethod(env,audio_track,play_ID);

    //调用AudioTrack的write方法  (@NonNull byte[] audioData, int offsetInBytes, int sizeInBytes)
    jmethodID write_ID = (*env)->GetMethodID(env,audiotrack_class,"write","([BII)I");

    FILE *fp_pcm = fopen(output_cstr,"wb");
    uint8_t *out_buffer = (uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE);

    int got_frame = 0 , index = 0 , ret ;
    while(av_read_frame(pFormatCtx,packet) >=0 ){
        //如果是音频
        if(packet->stream_index == audio_index){
            ret = avcodec_decode_audio4(pCodecCtx,frame,&got_frame,packet) ;
            if(ret<0){
                //解码完成
            }
            if(got_frame>0){
                swr_convert(swrCtx,&out_buffer,MAX_AUDIO_FRAME_SIZE,(const uint8_t **)frame->data,frame->nb_samples);
                int out_buffer_size = av_samples_get_buffer_size(NULL,out_channel_nb,frame->nb_samples,output_sample_format,1);
                fwrite(out_buffer,1,out_buffer_size,fp_pcm);
                //将out_buffer缓冲区数据转成byte数组
                jbyteArray audio_sample_array = (*env)->NewByteArray(env,out_buffer_size);
                jbyte *pSample_byte =(*env)->GetByteArrayElements(env,audio_sample_array,NULL);
                //讲out_buffer中的数据复制到pSample_byte中
                memcpy(pSample_byte,out_buffer,out_buffer_size);
                //同步
                (*env)->ReleaseByteArrayElements(env,audio_sample_array,pSample_byte,0);
                //diaoyong AudioTrack的write方法
                (*env)->CallIntMethod(env,audio_track,write_ID,audio_sample_array,0,out_buffer_size);
                (*env)->DeleteLocalRef(env,audio_sample_array);
                usleep(1000*16);

            }
        }
        av_free_packet(packet);
    }

    av_frame_free(&frame);
    av_free(out_buffer);

    swr_free(&swrCtx);
    avcodec_free_context(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    (*env)->ReleaseStringUTFChars(env,input_path,input_cstr);
    (*env)->ReleaseStringUTFChars(env,output_path,output_cstr);



//    const char* input_cstr = (*env)->GetStringUTFChars(env,input_path,NULL);
//    const char* output_cstr = (*env)->GetStringUTFChars(env,output_path,NULL);
//    //注册组件
//    av_register_all();
//    AVFormatContext *pFormatCtx = avformat_alloc_context();
//    //打开音频文件
//    if(avformat_open_input(&pFormatCtx,input_cstr,NULL,NULL) != 0){
//        return;
//    }
//    //获取输入文件信息
//    if(avformat_find_stream_info(pFormatCtx,NULL) < 0){
//        return;
//    }
//    //获取音频流索引位置
//    int i = 0, audio_stream_idx = -1;
//    for(; i < pFormatCtx->nb_streams;i++){
//        if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
//            audio_stream_idx = i;
//            break;
//        }
//    }
//
//    //获取解码器
//    AVCodecContext *codecCtx = pFormatCtx->streams[audio_stream_idx]->codec;
//    AVCodec *codec = avcodec_find_decoder(codecCtx->codec_id);
//    if(codec == NULL){
//        return;
//    }
//    //打开解码器
//    if(avcodec_open2(codecCtx,codec,NULL) < 0){
//        return;
//    }
//    //压缩数据
//    AVPacket *packet = (AVPacket *)av_malloc(sizeof(AVPacket));
//    //解压缩数据
//    AVFrame *frame = av_frame_alloc();
//    //frame->16bit 44100 PCM 统一音频采样格式与采样率
//    SwrContext *swrCtx = swr_alloc();
//
//    //重采样设置参数-------------start
//    //输入的采样格式
//    enum AVSampleFormat in_sample_fmt = codecCtx->sample_fmt;
//    //输出采样格式16bit PCM
//    enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
//    //输入采样率
//    int in_sample_rate = codecCtx->sample_rate;
//    //输出采样率
//    int out_sample_rate = in_sample_rate;
//    //获取输入的声道布局
//    //根据声道个数获取默认的声道布局（2个声道，默认立体声stereo）
//    //av_get_default_channel_layout(codecCtx->channels);
//    uint64_t in_ch_layout = codecCtx->channel_layout;
//    //输出的声道布局（立体声）
//    uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;
//
//    swr_alloc_set_opts(swrCtx,
//                       out_ch_layout,out_sample_fmt,out_sample_rate,
//                       in_ch_layout,in_sample_fmt,in_sample_rate,
//                       0, NULL);
//    swr_init(swrCtx);
//
//    //输出的声道个数
//    int out_channel_nb = av_get_channel_layout_nb_channels(out_ch_layout);
//
//    //重采样设置参数-------------end
//
//    //JNI begin------------------
//    //JasonPlayer
//    jclass player_class = (*env)->GetObjectClass(env,jobj);
//
//    //AudioTrack对象
//    jmethodID create_audio_track_mid = (*env)->GetMethodID(env,player_class,"createAudioTrack","(II)Landroid/media/AudioTrack;");
//    jobject audio_track = (*env)->CallObjectMethod(env,jobj,create_audio_track_mid,out_sample_rate,out_channel_nb);
//
//    //调用AudioTrack.play方法
//    jclass audio_track_class = (*env)->GetObjectClass(env,audio_track);
//    jmethodID audio_track_play_mid = (*env)->GetMethodID(env,audio_track_class,"play","()V");
//    (*env)->CallVoidMethod(env,audio_track,audio_track_play_mid);
//
//    //AudioTrack.write
//    jmethodID audio_track_write_mid = (*env)->GetMethodID(env,audio_track_class,"write","([BII)I");
//
//    //JNI end------------------
//    FILE *fp_pcm = fopen(output_cstr,"wb");
//
//    //16bit 44100 PCM 数据
//    uint8_t *out_buffer = (uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE);
//
//    int got_frame = 0,index = 0, ret;
//    //不断读取压缩数据
//    while(av_read_frame(pFormatCtx,packet) >= 0){
//        //解码音频类型的Packet
//        if(packet->stream_index == audio_stream_idx){
//            //解码
//            ret = avcodec_decode_audio4(codecCtx,frame,&got_frame,packet);
//
//            if(ret < 0){
//            }
//            //解码一帧成功
//            if(got_frame > 0){
//                swr_convert(swrCtx, &out_buffer, MAX_AUDIO_FRAME_SIZE,(const uint8_t **)frame->data,frame->nb_samples);
//                //获取sample的size
//                int out_buffer_size = av_samples_get_buffer_size(NULL, out_channel_nb,
//                                                                 frame->nb_samples, out_sample_fmt, 1);
//                fwrite(out_buffer,1,out_buffer_size,fp_pcm);
//
//                //out_buffer缓冲区数据，转成byte数组
//                jbyteArray audio_sample_array = (*env)->NewByteArray(env,out_buffer_size);
//                jbyte* sample_bytep = (*env)->GetByteArrayElements(env,audio_sample_array,NULL);
//                //out_buffer的数据复制到sampe_bytep
//                memcpy(sample_bytep,out_buffer,out_buffer_size);
//                //同步
//                (*env)->ReleaseByteArrayElements(env,audio_sample_array,sample_bytep,0);
//
//                //AudioTrack.write PCM数据
//                (*env)->CallIntMethod(env,audio_track,audio_track_write_mid,
//                                      audio_sample_array,0,out_buffer_size);
//                //释放局部引用
//                (*env)->DeleteLocalRef(env,audio_sample_array);
//                usleep(1000 * 16);
//            }
//        }
//
//        av_free_packet(packet);
//    }
//
//    av_frame_free(&frame);
//    av_free(out_buffer);
//
//    swr_free(&swrCtx);
//    avcodec_close(codecCtx);
//    avformat_close_input(&pFormatCtx);
//
//    (*env)->ReleaseStringUTFChars(env,input_path,input_cstr);
//    (*env)->ReleaseStringUTFChars(env,output_path,output_cstr);
}