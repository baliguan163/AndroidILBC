package com.audio.demo;

import com.audio.lib.AudioWrapper;
import com.audio.lib.NetConfig;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;


//dlq
public class MainActivity extends Activity {

	String LOG = "MainActivity"; //dsdfdsf
	private AudioWrapper audioWrapper;
	// View
	private Button btnStartRecord;
	private Button btnStopRecord;
	private Button btnStartListen;
	private Button btnStopListen;
	private Button btnExit;
	private EditText ipEditText;
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        audioWrapper = AudioWrapper.getInstance();
		initView();
    }

    private void initView() {
		btnStartRecord = (Button) findViewById(R.id.startRecord);
		btnStartListen = (Button) findViewById(R.id.startListen);
		btnStopRecord = (Button) findViewById(R.id.stopRecord);
		btnStopListen = (Button) findViewById(R.id.stopListen);
		ipEditText = (EditText) findViewById(R.id.edittext_ip);

		btnStopRecord.setEnabled(false);
		btnStopListen.setEnabled(false);

		btnExit = (Button) findViewById(R.id.btnExit);
		btnStartRecord.setOnClickListener(new View.OnClickListener() {
			public void onClick(View arg0) {
				String ipString = ipEditText.getText().toString().trim();
				NetConfig.setServerHost(ipString);
				btnStartRecord.setEnabled(false);
				btnStopRecord.setEnabled(true);
				audioWrapper.startRecord();
			}
		});

		btnStopRecord.setOnClickListener(new View.OnClickListener() {
			public void onClick(View arg0) {
				btnStartRecord.setEnabled(true);
				btnStopRecord.setEnabled(false);
				audioWrapper.stopRecord();
			}
		});
		btnStartListen.setOnClickListener(new View.OnClickListener() {

			public void onClick(View arg0) {
				btnStartListen.setEnabled(false);
				btnStopListen.setEnabled(true);
				audioWrapper.startListen();
			}
		});
		btnStopListen.setOnClickListener(new View.OnClickListener() {
			public void onClick(View arg0) {
				btnStartListen.setEnabled(true);
				btnStopListen.setEnabled(false);
				audioWrapper.stopListen();
			}
		});
		btnExit.setOnClickListener(new View.OnClickListener() {
			public void onClick(View arg0) {
				audioWrapper.stopListen();
				audioWrapper.stopRecord();
				System.exit(0);
			}
		});
	}
    
    
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

//    @Override
//    public boolean onOptionsItemSelected(MenuItem item) {
//        // Handle action bar item clicks here. The action bar will
//        // automatically handle clicks on the Home/Up button, so long
//        // as you specify a parent activity in AndroidManifest.xml.
//        int id = item.getItemId();
//        if (id == R.id.action_settings) {
//            return true;
//        }
//        return super.onOptionsItemSelected(item);
//    }
    
}
