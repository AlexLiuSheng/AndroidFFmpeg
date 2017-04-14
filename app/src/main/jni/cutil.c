#include "SDL.h"
#include "stdio.h"
#ifdef ANDROID
#include <jni.h>
#include <android/log.h>
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, "(>_<)", format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  "(^_^)", format, ##__VA_ARGS__)
#else
#define LOGE(format, ...)  printf("(>_<) " format "\n", ##__VA_ARGS__)
#define LOGI(format, ...)  printf("(^_^) " format "\n", ##__VA_ARGS__)
#endif
/**回调java方法重新测量Surface的高度*/
float getScreenW(){
  // retrieve the JNI environment.
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();

    // retrieve the Java instance of the SDLActivity
    jobject activity = (jobject)SDL_AndroidGetActivity();

    // find the Java class of the activity. It should be SDLActivity or a subclass of it.
       jclass clazz = (*env)->FindClass(env,"org/libsdl/app/SDLActivity");
 //  jclass clazz(env->GetObjectClass(activity));

    // find the identifier of the method to call
      jmethodID method_id =  (*env)->GetMethodID(env,clazz, "getScreenW", "()F");

    // effectively call the Java method
     float scrrenW=(*env)->CallFloatMethod(env,activity, method_id);
return scrrenW;
    // clean up the local references.
    // (*env)->DeleteLocalRef(activity);
 //    (*env)->DeleteLocalRef(clazz);
}
float getScreenH(){
  // retrieve the JNI environment.
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();

    // retrieve the Java instance of the SDLActivity
    jobject activity = (jobject)SDL_AndroidGetActivity();

    // find the Java class of the activity. It should be SDLActivity or a subclass of it.
       jclass clazz = (*env)->FindClass(env,"org/libsdl/app/SDLActivity");
 //  jclass clazz(env->GetObjectClass(activity));

    // find the identifier of the method to call
    jmethodID method_id =  (*env)->GetMethodID(env,clazz, "getScreenH", "()F");

    // effectively call the Java method
    float scrrenH=(*env)->CallFloatMethod(env,activity, method_id);
return scrrenH;
}
