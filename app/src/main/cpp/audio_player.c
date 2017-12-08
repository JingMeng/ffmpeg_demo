//
// @author admin on 2017/12/8.
//
#include "com_sinieco_ffmpeg_demo_PlayController.h"
#include <android/log.h>
#include <stdlib.h>
#define MAX_AUDIO_FRME_SIZE 48000*4

#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"

JNIEXPORT void JNICALL Java_com_sinieco_ffmpeg_1demo_PlayController_sound
    (JNIEnv *env, jobject jobj, jstring soundInput , jstring soundOutput){
    const char* sound_input_cstr = (*env)->GetStringUTFChars(env,soundInput,NULL);
    const char* sound_output_cstr = (*env)->GetStringUTFChars(env,soundOutput,NULL);

    //1.注册所有组件
    av_register_all();

    AVFormatContext *pFormatCxt = avformat_alloc_context();
    //2.打开音频文件
    if(avformat_open_input(&pFormatCxt,sound_input_cstr,NULL,NULL) != 0){
        return;
    }
    //3.获取音频文件的信息
    if(avformat_find_stream_info(pFormatCxt,NULL) < 0){
        return;
    }

    //获取音频流的索引位置
    int i = 0 , audio_index = -1  ;
    for( ; i < pFormatCxt->nb_streams ; i++){
        if(pFormatCxt->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
            audio_index = i ;
            break ;
        }
    }

    //获取解码器
    AVCodecContext *pCodecCtx = pFormatCxt->streams[audio_index]->codec;
    AVCodec *Decoder = avcodec_find_decoder(pCodecCtx->codec_id);

    if(Decoder == NULL){
        return;
    }

    if(avcodec_open2(pCodecCtx,Decoder,NULL) < 0 ){
        return;
    }

    AVPacket *packet = (AVPacket *)av_malloc(sizeof(AVPacket));
    AVFrame *frame = av_frame_alloc();
    SwrContext *swrCtx = swr_alloc();

    //重采样设置参数
    //输入的采样格式
    enum AVSampleFormat in_sample_fmt = pCodecCtx->sample_fmt ;
    //输出的采样格式
    enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16 ;
    //输入采样率
    int in_sample_rate = pCodecCtx->sample_rate ;
    //输出采样率
    int out_sample_rate = 44100 ;
    //获取输入的声道布局
    //根据声道个数获取某人的声道布局（2个声道，默认立体声stereo）
    //av_get_default_channel_layout
    uint64_t in_ch_layout = pCodecCtx->channel_layout ;
    //输出的声道布局
    uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO ;

    swr_alloc_set_opts(swrCtx,out_ch_layout,out_sample_fmt,out_sample_rate,
                              in_ch_layout,in_sample_fmt,in_sample_rate,0,NULL);

    swr_init(swrCtx);

    //输出声道的个数
    int out_channel_nb = av_get_channel_layout_nb_channels(out_ch_layout);
    //重采样设置参数 完成

    uint8_t *out_buffer = (uint8_t *)av_malloc(MAX_AUDIO_FRME_SIZE);

    FILE * outputFile = fopen(sound_output_cstr,"wb");

    int got_frame = 0 , index = 0 ,ret ;

    while(av_read_frame(pFormatCxt,packet) >= 0){
        ret = avcodec_decode_audio4(pCodecCtx,frame,&got_frame,packet);
        if(ret < 0){
            //解码完成
        }

        if(got_frame > 0){
            swr_convert(swrCtx,&out_buffer,MAX_AUDIO_FRME_SIZE,frame->data,frame->nb_samples);
            int out_buffer_size = av_samples_get_buffer_size(NULL,out_channel_nb,frame->nb_samples,out_sample_fmt,1);
            fwrite(out_buffer,1,out_buffer_size,outputFile);
        }

        av_free_packet(packet);
    }

    fclose(outputFile);
    av_frame_free(&frame);
    av_free(out_buffer);
    swr_free(&swrCtx);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCxt);

    (*env)->ReleaseStringUTFChars(env,soundInput,sound_input_cstr);
    (*env)->ReleaseStringUTFChars(env,soundOutput,sound_output_cstr);

}
