//
// Created by righere on 16-11-22.
//

#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavfilter/avfilter.h"
#include "libswscale/swscale.h"
#include "stdio.h"
#include "src/render/SDL_sysrender.h"
#include "src/video/SDL_sysvideo.h"
#include "libavutil/imgutils.h"
#include "SDL.h"
#include <cutil.c>


#ifdef ANDROID
#include <jni.h>
#include <android/log.h>
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, "(>_<)", format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  "(^_^)", format, ##__VA_ARGS__)
#else
#define LOGE(format, ...)  printf("(>_<) " format "\n", ##__VA_ARGS__)
#define LOGI(format, ...)  printf("(^_^) " format "\n", ##__VA_ARGS__)
#endif



//屏幕参数
int SCREEN_W ;
int SCREEN_H ;

//设置buffer输出格式，YUV：1， RGB：0
#define BUFFER_FMT_YUV 0
/**回调java方法重新测量Surface的高度*/
void remeasure_surface_size(int32_t  height){
  // retrieve the JNI environment.
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();

    // retrieve the Java instance of the SDLActivity
    jobject activity = (jobject)SDL_AndroidGetActivity();

    // find the Java class of the activity. It should be SDLActivity or a subclass of it.
       jclass clazz = (*env)->FindClass(env,"org/libsdl/app/SDLActivity");
 //  jclass clazz(env->GetObjectClass(activity));

    // find the identifier of the method to call
    jmethodID method_id =  (*env)->GetMethodID(env,clazz, "reMeasureSurface", "(I)V");

    // effectively call the Java method
     (*env)->CallVoidMethod(env,activity, method_id,height);

    // clean up the local references.
    // (*env)->DeleteLocalRef(activity);
 //    (*env)->DeleteLocalRef(clazz);
}
void resize_sdl_rect(SDL_Rect* sdlRect,int vwidth,int vheight){
      SCREEN_W=(int)getScreenW();
     SCREEN_H=(int)getScreenH();
   int cheight=(int)((1.0*vheight*SCREEN_W)/vwidth);
        sdlRect->x=0;
        sdlRect->y=0;
        sdlRect->w=SCREEN_W;
       sdlRect->h=cheight;
       if(cheight>SCREEN_H){
       sdlRect->h=SCREEN_H;
        }
        remeasure_surface_size(cheight);
}
int main(int argc, char** argv)
{

    //FFmpeg Parameters
    AVFormatContext *pFormatCtx;
    int             streamIdx;
    AVCodecContext  *pCodecCtx;
    AVCodecParameters *avCodecParameters;
    AVCodec         *pCodec;
    AVFrame         *pFrame, *pFrame_out;
    AVPacket        *packet;

    //size buffer
    uint8_t *out_buffer;

    static struct SwsContext *img_convert_ctx;

//SDL Parameters
    SDL_Window     *sdlWindow;
    SDL_Texture    *sdlTexture;
     SDL_Rect sdlRect;
    SDL_Renderer   *renderer;
    SDL_Event     event;

    if(argc < 2)
    {
        LOGE("no media input!");
        return -1;
    }
    //获取文件名
    const char* mediaUri = (const char *) argv[1];
  //  SCREEN_W=atoi(argv[2]);
    // SCREEN_H=atoi(argv[3]);
    SCREEN_W=(int)getScreenW();
     SCREEN_H=(int)getScreenH();
     //LOGE(SCREEN_W);
    av_register_all();

    //分配一个AVFormatContext
    pFormatCtx = avformat_alloc_context();

    //判断文件流是否能打开
    if(avformat_open_input(&pFormatCtx, mediaUri, NULL, NULL) != 0){
        LOGE("Couldn't open input stream! \n");
        return -1;
    }
    //判断能够找到文件流信息
    if(avformat_find_stream_info(pFormatCtx, NULL) < 0){
        LOGE("couldn't find open stream information !\n");
        return -1;
    }
    //打印文件信息
    av_dump_format(pFormatCtx, -1, mediaUri, 0);
    streamIdx=-1;
    for(int i=0; i<pFormatCtx->nb_streams; i++)
        //新版本ffmpeg将AVCodecContext *codec替换成*codecpar
        if(pFormatCtx->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_VIDEO) {
            streamIdx=i;
            break;
        }
    if(streamIdx == -1){
        LOGE("Couldn't find a video stream !\n");
        return -1;
    }


    // Get a pointer to the codec context for the video stream
    avCodecParameters = pFormatCtx->streams[streamIdx]->codecpar;

    // Find the decoder for the video stream
    pCodec=avcodec_find_decoder(avCodecParameters->codec_id);


    if(pCodec){
        LOGI("find decoder: %d", avCodecParameters->codec_id);
    }

    //alloc a codecContext
    pCodecCtx = avcodec_alloc_context3(pCodec);


    //transform
    if(avcodec_parameters_to_context(pCodecCtx,avCodecParameters) < 0){
        LOGE("copy the codec parameters to context fail!");
        return -1;
    }
    //打开codec
    int errorCode = avcodec_open2(pCodecCtx, pCodec, NULL);
    if(errorCode < 0){
        LOGE("Unable to open codec!\n");
        return errorCode;
    };

    //alloc frame of ffmpeg decode
    pFrame = av_frame_alloc();

    if(pFrame == NULL){
        LOGE("Unable to allocate an AVFrame!\n");
        return -1;
    }

    //decode packet
    packet = av_packet_alloc();

    pFrame_out = av_frame_alloc();
#if BUFFER_FMT_YUV
    //output frame for SDL
    enum AVPixelFormat pixel_fmt = AV_PIX_FMT_YUV420P;

#else
    //output RGBFrame
    enum AVPixelFormat pixel_fmt = AV_PIX_FMT_RGBA;
#endif
    out_buffer = av_malloc((size_t) av_image_get_buffer_size(pixel_fmt,pCodecCtx->width,pCodecCtx->height,1));
    av_image_fill_arrays(pFrame_out->data,pFrame_out->linesize, out_buffer,
                         pixel_fmt,pCodecCtx->width,pCodecCtx->height,1);
    //transform size and format of CodecCtx,立方插值
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
                                     pCodecCtx->width, pCodecCtx->height, pixel_fmt,
                                     SWS_BICUBIC, NULL,
                                     NULL, NULL);

    //初始化SDL
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        LOGE("SDL_Init failed %s" ,SDL_GetError());
        exit(1);
    }
    //默认全屏可以不用设置,根据视频宽高自适应
    int vwidth= pCodecCtx->width;
    int vheight= pCodecCtx->height;
    resize_sdl_rect(&sdlRect,vwidth,vheight);
    //设置window属性
    sdlWindow = SDL_CreateWindow("Convexd_SDL",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
                             SCREEN_W, SCREEN_H,SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL);

    if(!sdlWindow) {
        LOGE("SDL: could not set video mode - exiting\n");
        exit(1);
    }
    //create renderer and set parameter
    renderer = SDL_CreateRenderer(sdlWindow,-1,0);

#if BUFFER_FMT_YUV
    Uint32 sdl_out_fmt = SDL_PIXELFORMAT_IYUV;
#else
    Uint32 sdl_out_fmt = SDL_PIXELFORMAT_RGBA32;
#endif
    //Allocate a place to put our yuv image on that screen,set sdl_display_resolution
    sdlTexture = SDL_CreateTexture(renderer,
                            sdl_out_fmt,SDL_TEXTUREACCESS_STREAMING,
                            pCodecCtx->width, pCodecCtx->height);
    // Read frames
    while (av_read_frame(pFormatCtx, packet) >= 0) {
        // Is this a packet from the video stream?
        if (packet->stream_index == streamIdx) {
            //decoder allocate frame to pFrame,new api
        //    LOGI("%s","Got Video Packet Succeed");
            int getPacketCode = avcodec_send_packet(pCodecCtx, packet);
            if(getPacketCode == 0) {
                int getFrameCode = avcodec_receive_frame(pCodecCtx, pFrame);
              //  LOGI("%d", getFrameCode);
                // Did we get a video frame?
                if (getFrameCode == 0) {
                //    LOGI("%s", "Got Video Frame Succeed");

                    //scale Frame
                    sws_scale(img_convert_ctx, (const uint8_t *const *) pFrame->data,
                              pFrame->linesize, 0, pFrame->height,
                              pFrame_out->data, pFrame_out->linesize);

                    #if (BUFFER_FMT_YUV == 1)

                    SDL_UpdateYUVTexture(sdlTexture, NULL, pFrame_out->data[0], pFrame_out->linesize[0],
                                         pFrame_out->data[1],pFrame_out->linesize[1],
                                         pFrame_out->data[2],pFrame_out->linesize[2]);
                    #else
                     SDL_UpdateTexture(sdlTexture,NULL,pFrame_out->data[0],pCodecCtx->width*4);  //4通道：pitch = width×4
                    #endif
                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, sdlTexture, NULL, &sdlRect);
                    SDL_RenderPresent(renderer);
                    //设置每秒25帧，1000/25 = 40
                    SDL_Delay(0);
                    while (SDL_PollEvent(&event)){
                    switch (event.type) {

                        case SDL_WINDOWEVENT_SIZE_CHANGED:
                         LOGE("%s", "大小改变");
                         resize_sdl_rect(&sdlRect,vwidth,vheight);
                     //   int winSizeW,winSizeH;
                       // SDL_GetWindowSize(sdlWindow,&winSizeW,&winSizeH);
                           // SDL_Quit();
                           // exit(0);
                        case SDL_KEYDOWN:
                           LOGE("%s", "退出");
                              SDL_Quit();
                              exit(0);
                        default:
                            break;
                    }
                    }
                }
                else if (getFrameCode == AVERROR(EAGAIN)) {
                  LOGE("%s", "Frame is not available right now,please try another input");
                }
                else if (getFrameCode == AVERROR_EOF) {
                   LOGE("%s", "the decoder has been fully flushed");
                }
                else if (getFrameCode == AVERROR(EINVAL)) {
                    LOGE("%s", "codec not opened, or it is an encoder");
                }
                else {
                    LOGI("%s", "legitimate decoding errors");
                }
            }
        }
        // Free the packet that was allocated by av_read_frame
        av_packet_unref(packet);
    }
    sws_freeContext(img_convert_ctx);
    av_frame_free(&pFrame_out);
    av_frame_free(&pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
 //   SDL_Quit();
    return 0;
}


