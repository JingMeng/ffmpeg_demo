//
// @author admin on 2017/12/6.
//

#include "com_sinieco_ffmpeg_demo_VideoUtils.h"

#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <jni.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>

JNIEXPORT void JNICALL Java_com_sinieco_ffmpeg_1demo_VideoUtils_decode
        (JNIEnv *env, jclass jcla, jstring inPath, jstring outPath){
    const char* input_cstr = (*env)->GetStringUTFChars(env,inPath,NULL);
    const char* output_cstr = (*env)->GetStringUTFChars(env,outPath,NULL);
    // 1 注册ffmpeg所有组件
    av_register_all();
    //获取ffmpeg封装格式上下文
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    // 2 打开输入视频文件,返回0成功，返回其他失败
    if(avformat_open_input(&pFormatCtx,input_cstr,NULL,NULL)!=0){
        return;
    }
    //3 获取视频文件的信息,返回>=0成功，<0 失败
    if(avformat_find_stream_info(pFormatCtx,NULL)<0){
        return;
    }
    //一个视频文件在解码时会被分解成多个AVStream，这有AVStream有视频的，有音频的，有中文字幕的
    //有英文字幕的，这里要做的是找到视频的AVStream在AVStream数组中的索引
    int video_index = -1 ;
    int i = 0 ;
    for (; i < pFormatCtx->nb_streams ; i++) {
        if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
            video_index = i ;
            break ;
        }
    }

    //获取解码器上下文
    AVCodecContext *pCodecCxt = pFormatCtx->streams[video_index]->codec;
    //获取解码器
    AVCodec *pCondec = avcodec_find_decoder(pCodecCxt->codec_id);
    //没有对应解码器
    if(pCondec ==NULL){
        return;
    }
    //成功返回0，失败放回负数
    if(avcodec_open2(pCodecCxt,pCondec,NULL) < 0){
        return;
    }

    //初始化AVPacket指针，每一帧视频都封装成一个AVPacket
    AVPacket *packet = (AVPacket *)av_malloc(sizeof(AVPacket));
    AVFrame *frame = av_frame_alloc();
    AVFrame *yuvFrame = av_frame_alloc();
    //缓冲区分配内存
    uint8_t *out_buffer = (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P,pCodecCxt->width,pCodecCxt->height));
    //初始化缓冲区
    avpicture_fill((AVPicture *) yuvFrame ,out_buffer,AV_PIX_FMT_YUV420P,pCodecCxt->width,pCodecCxt->height);

    //创建输出文件
    FILE *outFile = fopen(output_cstr,"wb");

    //用于像素格式转换或者缩放的结构体
    struct SwsContext *sws_ctx = sws_getContext(pCodecCxt->width,pCodecCxt->height,pCodecCxt->pix_fmt,        //输入类型的大小和格式
                                                 pCodecCxt->width,pCodecCxt->height,AV_PIX_FMT_YUV420P,      //输出类型的大小和格式
                                                 SWS_BILINEAR,NULL,NULL,NULL );

    int len , got_frame ,framecount = 0 ;
    //读取每一帧的视频，返回0成功，小于0失败
    while(av_read_frame(pFormatCtx,packet) >= 0 ){
        //解码AVPacket—>AVFrame(失败返回负数，成功返回字节数)
        //Zero if no frame could be decompressed, otherwise, it is nonzero.
        //*get_frame是入参出参，如果没有视频帧可以被解码，返回0，否则返回非0
        len = avcodec_decode_video2(pCodecCxt,frame,&got_frame,packet);
        if(got_frame){
            //转为指定的YUV420p像素帧（frame—>yuvFrame）
            sws_scale(sws_ctx,frame->data,frame->linesize,0,frame->height,
                        yuvFrame->data,yuvFrame->linesize);
            //向YUV文件保存解码后的帧数据
            //yuvFrame->YUV文件
            //Y:U:V  = 4:1:1  (人眼对亮度敏感，所以Y的可调范围广)
            int y_size = pCodecCxt->width*pCodecCxt->height;
            //写入Y
            fwrite(yuvFrame->data[0],1,y_size,outFile);
            //写入U
            fwrite(yuvFrame->data[1],1,y_size/4,outFile);
            //写入V
            fwrite(yuvFrame->data[2],1,y_size/4,outFile);
        }
        //释放此帧的AVPacket
        av_free_packet(packet);
    }

    fclose(outFile);

    av_frame_free(&frame);
    avcodec_close(pCodecCxt);
    avformat_free_context(pFormatCtx);

    (*env)->ReleaseStringUTFChars(env,inPath,input_cstr);
    (*env)->ReleaseStringUTFChars(env,outPath,output_cstr);

//    const char* input_cstr = (*env)->GetStringUTFChars(env,inPath,NULL);
//    const char* output_cstr = (*env)->GetStringUTFChars(env,outPath,NULL);
//
//    //1.注册组件
//    av_register_all();
//
//    //封装格式上下文
//    AVFormatContext *pFormatCtx = avformat_alloc_context();
//
//    //2.打开输入视频文件
//    if(avformat_open_input(&pFormatCtx,input_cstr,NULL,NULL) != 0){
//
//        return;
//    }
//    //3.获取视频信息
//    if(avformat_find_stream_info(pFormatCtx,NULL) < 0){
//
//        return;
//    }
//
//    //视频解码，需要找到视频对应的AVStream所在pFormatCtx->streams的索引位置
//    int video_stream_idx = -1;
//    int i = 0;
//    for(; i < pFormatCtx->nb_streams;i++){
//        //根据类型判断，是否是视频流
//        if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
//            video_stream_idx = i;
//            break;
//        }
//    }
//
//    //4.获取视频解码器
//    AVCodecContext *pCodeCtx = pFormatCtx->streams[video_stream_idx]->codec;
//    AVCodec *pCodec = avcodec_find_decoder(pCodeCtx->codec_id);
//    if(pCodec == NULL){
//        return;
//    }
//
//    //5.打开解码器
//    if(avcodec_open2(pCodeCtx,pCodec,NULL) < 0){
//        return;
//    }
//
//    //编码数据
//    AVPacket *packet = (AVPacket *)av_malloc(sizeof(AVPacket));
//
//    //像素数据（解码数据）
//    AVFrame *frame = av_frame_alloc();
//    AVFrame *yuvFrame = av_frame_alloc();
//
//    //只有指定了AVFrame的像素格式、画面大小才能真正分配内存
//    //缓冲区分配内存
//    uint8_t *out_buffer = (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, pCodeCtx->width, pCodeCtx->height));
//    //初始化缓冲区
//    avpicture_fill((AVPicture *)yuvFrame, out_buffer, AV_PIX_FMT_YUV420P, pCodeCtx->width, pCodeCtx->height);
//
//
//    //输出文件
//    FILE* fp_yuv = fopen(output_cstr,"wb");
//
//    //用于像素格式转换或者缩放
//    struct SwsContext *sws_ctx = sws_getContext(
//            pCodeCtx->width, pCodeCtx->height, pCodeCtx->pix_fmt,
//            pCodeCtx->width, pCodeCtx->height, AV_PIX_FMT_YUV420P,
//            SWS_BILINEAR, NULL, NULL, NULL);
//
//    int len ,got_frame, framecount = 0;
//    //6.一阵一阵读取压缩的视频数据AVPacket
//    while(av_read_frame(pFormatCtx,packet) >= 0){
//        //解码AVPacket->AVFrame
//        len = avcodec_decode_video2(pCodeCtx, frame, &got_frame, packet);
//
//        //Zero if no frame could be decompressed
//        //非零，正在解码
//        if(got_frame){
//            //frame->yuvFrame (YUV420P)
//            //转为指定的YUV420P像素帧
//            sws_scale(sws_ctx,
//                      frame->data,frame->linesize, 0, frame->height,
//                      yuvFrame->data, yuvFrame->linesize);
//
//            //向YUV文件保存解码之后的帧数据
//            //AVFrame->YUV
//            //一个像素包含一个Y
//            int y_size = pCodeCtx->width * pCodeCtx->height;
//            fwrite(yuvFrame->data[0], 1, y_size, fp_yuv);
//            fwrite(yuvFrame->data[1], 1, y_size/4, fp_yuv);
//            fwrite(yuvFrame->data[2], 1, y_size/4, fp_yuv);
//
//        }
//
//        av_free_packet(packet);
//    }
//
//    fclose(fp_yuv);
//
//    av_frame_free(&frame);
//    avcodec_close(pCodeCtx);
//    avformat_free_context(pFormatCtx);
//
//    (*env)->ReleaseStringUTFChars(env,inPath,input_cstr);
//    (*env)->ReleaseStringUTFChars(env,outPath,output_cstr);

}

