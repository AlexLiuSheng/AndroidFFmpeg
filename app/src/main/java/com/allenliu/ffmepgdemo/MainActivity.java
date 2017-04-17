package com.allenliu.ffmepgdemo;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import org.libsdl.app.SDLActivity;

import java.io.File;

public class MainActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        TextView tv = (TextView)this.findViewById(R.id.tv);
        FFmpegNative ffmpeg = new FFmpegNative();
        int codecID = 28; //28 is the H264 Codec ID

        String res = ffmpeg.helloFFMPEG();

        if(!TextUtils.isEmpty(res)) {
            tv.setText(res);
        }
        else{
            tv.setText("Failed!");
        }
        String folderurl= Environment.getExternalStorageDirectory().getPath();
        String inputStr=folderurl+"/1490338208767.mp4";
        String outputStr=folderurl+"/YUV.yuv";
        //ffmpeg.toYUV(inputStr,outputStr);
        Button btn_Start= (Button) findViewById(R.id.btn_Start);
        btn_Start.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent=new Intent(MainActivity.this,PlayActivity.class);
                intent.setData(Uri.fromFile(new File("/storage/emulated/0/test.mp4")));
                startActivity(intent);

            }
        });

    }

}
