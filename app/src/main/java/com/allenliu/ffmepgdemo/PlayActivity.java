package com.allenliu.ffmepgdemo;

import android.app.Activity;
import android.content.Context;
import android.content.res.Configuration;
import android.graphics.Point;
import android.os.Build;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Display;
import android.view.LayoutInflater;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.Toast;

import org.libsdl.app.SDLActivity;

public class PlayActivity extends SDLActivity implements View.OnClickListener{
protected View guiContainer;
    Button btn1;
    Button btn2;
    Button btn3;
    Button btn4;
    Button btn5;
    /**
     * 播放速度
     */
    private float playSpeed=1f;
    private boolean isNeedLoop=false;
    /**
     * 视频滤镜
     *         //过滤器命令
     //   const char*filter_descr=NULL;
     //const char *filter_descr = "lutyuv='u=128:v=128'";
     //const char *filter_descr = "boxblur";
     //const char *filter_descr = "hflip";
     //    const char *filter_descr = "hue='h=60:s=-3'";
     //const char *filter_descr = "crop=2/3*in_w:2/3*in_h";
     //const char *filter_descr = "drawbox=x=100:y=100:w=100:h=100:color=pink@0.5";
     //const char *filter_descr = "drawtext=fontfile=arial.ttf:fontcolor=green:fontsize=30:text='水印文字'";
     */
    private String filter;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //setContentView(R.layout.gui_container);
        guiContainer= LayoutInflater.from(this).inflate(R.layout.gui_container,null);
        btn1= (Button) guiContainer.findViewById(R.id.btn1);
        btn2= (Button) guiContainer.findViewById(R.id.btn2);
        btn3= (Button) guiContainer.findViewById(R.id.btn3);
        btn4= (Button) guiContainer.findViewById(R.id.btn4);
        btn5= (Button) guiContainer.findViewById(R.id.btn5);
        btn1.setOnClickListener(this);
        btn2.setOnClickListener(this);
        btn3.setOnClickListener(this);
        btn4.setOnClickListener(this);
        btn5.setOnClickListener(this);
        mLayout.addView(guiContainer);
    }
   int index=0;
    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.btn1:
                setPlaySpeed(2f);
                break;
            case R.id.btn2:
                setPlaySpeed(1f);
                 break;
            case R.id.btn3:
                setPlaySpeed(0.5f);
                break;
            case R.id.btn4:
                if(index==0)
                setFilter("hue='h=60:s=-3'");
                else if(index==1)
                    setFilter("hflip");
                else if(index==2){
                    setFilter("lutyuv='u=128:v=128");
                }else if(index==3){
                    setFilter("edgedetect=mode=colormix:high=0");
                }else if(index==4){
                    setFilter("lutyuv='y=2*val'");
                }else if(index==5){
                    setFilter(null);
                }
                index++;
                if(index==6){
                    index=0;
                }
                break;
            case R.id.btn5:
                setNeedLoop(true);
                break;
        }
    }
    public Point getScreenSize(){
        WindowManager windowManager = (WindowManager) getSystemService(Context.WINDOW_SERVICE);
        Display display = windowManager.getDefaultDisplay();
        if(Build.VERSION.SDK_INT < Build.VERSION_CODES.HONEYCOMB_MR2){
            return new Point(display.getWidth(), display.getHeight());
        }else{
            Point point = new Point();
            display.getSize(point);
            return point;
        }
    }
    @Override
    public void onConfigurationChanged(Configuration newConfig) {

        if(this.getResources().getConfiguration().orientation == Configuration.ORIENTATION_PORTRAIT){
            getWindow().clearFlags(WindowManager.LayoutParams. FLAG_FULLSCREEN);
        }else if(this.getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE){
            getWindow().setFlags(WindowManager.LayoutParams. FLAG_FULLSCREEN , WindowManager.LayoutParams. FLAG_FULLSCREEN);
        }
        super.onConfigurationChanged(newConfig);
    }



    public void setPlaySpeed(float playSpeed) {
        this.playSpeed = playSpeed;
        SDLActivity.onNativeSurfaceChanged();
    }

    public void setFilter(String filter) {
        this.filter = filter;
        SDLActivity.onNativeSurfaceChanged();
    }



    public void setNeedLoop(boolean needLoop) {
        isNeedLoop = needLoop;
    }

    /**
     * C调用JAVA
     * @return
     */

    public float getScreenW(){
        return  getScreenSize().x;
    }
    public float getScreenH(){
        return  getScreenSize().y;
    }
    public float getPlaySpeed() {
        return playSpeed;
    }
    public boolean isNeedLoop() {
        return isNeedLoop;
    }

    public String getFilter() {
        return filter;
    }

//
}
