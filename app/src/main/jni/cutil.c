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
const char * className="com/allenliu/ffmepgdemo/PlayActivity";
/**回调java方法重新测量Surface的高度*/
float getScreenW(){
  // retrieve the JNI environment.
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    // find the Java class of the activity. It should be SDLActivity or a subclass of it.
        jobject activity = (jobject)SDL_AndroidGetActivity();
       jclass clazz = (*env)->FindClass(env,className);
    // find the identifier of the method to call
      jmethodID method_id =  (*env)->GetMethodID(env,clazz, "getScreenW", "()F");

    // effectively call the Java method
     float scrrenW=(*env)->CallFloatMethod(env,activity, method_id);
     (*env)->DeleteLocalRef(env, activity);
     (*env)->DeleteLocalRef(env, clazz);
return scrrenW;
    // clean up the local references.
    // (*env)->DeleteLocalRef(activity);

}
float getScreenH(){
  // retrieve the JNI environment.
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    jobject activity = (jobject)SDL_AndroidGetActivity();
    jclass clazz = (*env)->FindClass(env,className);
    jmethodID method_id =  (*env)->GetMethodID(env,clazz, "getScreenH", "()F");
    float scrrenH=(*env)->CallFloatMethod(env,activity, method_id);
     (*env)->DeleteLocalRef(env, activity);
     (*env)->DeleteLocalRef(env, clazz);
     return scrrenH;
}
float getPlaySpeed(){
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    jobject activity = (jobject)SDL_AndroidGetActivity();
    jclass clazz = (*env)->FindClass(env,className);
    jmethodID method_id =  (*env)->GetMethodID(env,clazz, "getPlaySpeed", "()F");
    float playSpeed=(*env)->CallFloatMethod(env,activity, method_id);
     (*env)->DeleteLocalRef(env, activity);
     (*env)->DeleteLocalRef(env, clazz);

     return playSpeed;
}
  char* getVideoFilter(){
      JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
      jobject activity = (jobject)SDL_AndroidGetActivity();
      jclass clazz = (*env)->FindClass(env,className);
      jmethodID method_id =  (*env)->GetMethodID(env,clazz, "getFilter", "()Ljava/lang/String;");
      jstring sfilter=(jstring)((*env)->CallObjectMethod(env,activity, method_id));
      (*env)->DeleteLocalRef(env, activity);
      (*env)->DeleteLocalRef(env, clazz);
   //   (*env)->DeleteLocalRef(env, method_id);
      if(sfilter !=NULL){
       char *filter=(char*)(*env)->GetStringUTFChars(env,sfilter,0);
          (*env)->DeleteLocalRef(env, sfilter);
       return filter;
       }else{
         (*env)->DeleteLocalRef(env, sfilter);
       return NULL;
       }
       }
  _Bool isNeedLoop(){
         JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
             jobject activity = (jobject)SDL_AndroidGetActivity();
             jclass clazz = (*env)->FindClass(env,className);
             jmethodID method_id =  (*env)->GetMethodID(env,clazz, "isNeedLoop", "()Z");
             _Bool isLoop=(*env)->CallBooleanMethod(env,activity, method_id);
              (*env)->DeleteLocalRef(env, activity);
              (*env)->DeleteLocalRef(env, clazz);
             return isLoop;
       }
