package com.helloandroid;

import android.app.Activity;
import android.widget.TextView;
import android.os.Bundle;

public class HelloAndroid2 extends Activity
{
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        /* Create a TextView and set its text to "Hello world" */
        TextView  tv = new TextView(this);
        tv.setText("*** Hello Android !!! ***");
        setContentView(tv);
    }
}