package com.allenliu.ffmepgdemo;

/**
 * Created by Allen Liu on 2017/4/7.
 */

public class FFmpegNative {
    static{
       System.loadLibrary("avutil-55");
        System.loadLibrary("avcodec-57");
        System.loadLibrary("swresample-2");
        System.loadLibrary("avformat-57");
        System.loadLibrary("swscale-4");
        System.loadLibrary("avfilter-6");
        System.loadLibrary("avdevice-57");
        System.loadLibrary("ffmpeg_jni");
    }
    public native String helloJNI();
    public native String helloFFMPEG();
    public native int  toYUV(String inputStr,String outputStr);
    public native void exit();
}
