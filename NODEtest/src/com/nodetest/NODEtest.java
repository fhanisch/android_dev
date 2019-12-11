package com.nodetest;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;

public class NODEtest extends Activity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("node");
    }
	
	/** Called when the activity is first created. */
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        /* Create a TextView and set its text to "Hello world" */
        TextView  tv = new TextView(this);
        tv.setText(nodeStart());
        setContentView(tv);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String nodeStart();
}