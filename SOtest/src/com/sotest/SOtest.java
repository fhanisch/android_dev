package com.sotest;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;
import android.content.pm.ApplicationInfo;

public class SOtest extends Activity {

    // Used to load the 'native-lib' library on application startup.
    static {
		System.loadLibrary("native-lib");
    }
	
	/** Called when the activity is first created. */
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        /* Create a TextView and set its text to "Hello world" */
		TextView  tv = new TextView(this);
		
		String libPath = getApplicationInfo().nativeLibraryDir;
		tv.setText(stringFromJNI(libPath));
		
		//String property = System.getProperty("java.library.path");
		//tv.setText(property);
		
        setContentView(tv);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI(String str);
}